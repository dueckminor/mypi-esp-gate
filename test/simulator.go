package main

import (
	"fmt"
	"os"
	"os/signal"
	"strconv"
	"time"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

var relais = false

func mqttHandler(client mqtt.Client, msg mqtt.Message) {
	fmt.Println("[MQTT] Received Message", msg.Topic(), string(msg.Payload()))
	if msg.Topic() == "tor2/debug/relais" {
		payload := string(msg.Payload())
		relais = payload == "1"
	}
}

func getSensorBits(position int, enter, leave bool) uint8 {
	sensorBits := uint8(0)
	if enter {
		sensorBits |= 1
	}
	if leave {
		sensorBits |= (1 << 4)
	}
	sensorBits |= uint8(position) << 1
	return ^sensorBits & 0x1f
}

func createSensorBitTable() []uint8 {
	sensorBitTable := make([]uint8, 101)

	for i := range sensorBitTable {
		sensorBitTable[i] = getSensorBits(0, false, false)
	}

	sensorBitTable[0] = getSensorBits(0, true, true)
	sensorBitTable[1] = getSensorBits(0, false, true)
	sensorBitTable[2] = getSensorBits(0, false, false)

	for pos := 1; pos < 7; pos++ {
		x := 14*pos - 2
		sensorBitTable[x+0] = getSensorBits(0, true, false)
		sensorBitTable[x+1] = getSensorBits(pos, true, false)
		sensorBitTable[x+2] = getSensorBits(pos, true, true)
		sensorBitTable[x+3] = getSensorBits(pos, false, true)
		sensorBitTable[x+4] = getSensorBits(pos, false, true)
		sensorBitTable[x+5] = getSensorBits(0, false, true)
	}

	sensorBitTable[97] = getSensorBits(0, false, false)
	sensorBitTable[98] = getSensorBits(0, true, false)
	sensorBitTable[99] = getSensorBits(7, true, false)
	sensorBitTable[100] = getSensorBits(7, true, true)

	return sensorBitTable
}

func main() {
	mqttOpts := mqtt.NewClientOptions().AddBroker("tcps://rpi.fritz.box:8883")
	mqttOpts.SetClientID("tor-simulator")
	mqttOpts.SetDefaultPublishHandler(mqttHandler)

	mqttClient := mqtt.NewClient(mqttOpts)

	if token := mqttClient.Connect(); token.Wait() && token.Error() != nil {
		panic(token.Error())
	}

	mqttClient.Subscribe("tor2/relais", 2, mqttHandler)
	mqttClient.Subscribe("tor2/debug/relais", 2, mqttHandler)

	go func() {
		sensorBitTable := createSensorBitTable()
		position := 100
		lastDirection := 1
		currentDirection := 0

		for true {
			mqttClient.Publish("tor2/simulator/sensor_bits", 2, false, strconv.FormatInt(int64(sensorBitTable[position]), 10))
			time.Sleep(time.Millisecond * 50)
			if relais {
				if currentDirection != 0 {
					fmt.Println("Stoping at position", position)
					currentDirection = 0
				} else {
					currentDirection = -lastDirection
					lastDirection = currentDirection
					fmt.Println("Moving from", position, "in direction", currentDirection)
				}
				relais = false
			}
			position += currentDirection
			if position < 0 {
				position = 0
				currentDirection = 0
				fmt.Println("Stoping at position", position)
			}
			if position > 100 {
				position = 100
				currentDirection = 0
				fmt.Println("Stoping at position", position)
			}

		}
	}()

	quit := make(chan struct{})
	c := make(chan os.Signal, 1)
	signal.Notify(c, os.Interrupt)
	go func() {
		<-c
		mqttClient.Disconnect(250)
		fmt.Println("[MQTT] Disconnected")

		quit <- struct{}{}
	}()
	<-quit

}
