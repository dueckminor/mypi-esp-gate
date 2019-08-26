package main

import (
	"fmt"
	"os"
	"os/signal"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

type Tor struct {
	pos int8
	dir bool
}

var tor Tor

func mqttHandler(client mqtt.Client, msg mqtt.Message) {
	fmt.Println(msg.Topic(), ":", string(msg.Payload()))
}

func main() {
	mqttOpts := mqtt.NewClientOptions().AddBroker("tcps://rpi.fritz.box:8883")
	mqttOpts.SetClientID("tor-dumper")
	mqttOpts.SetDefaultPublishHandler(mqttHandler)

	mqttClient := mqtt.NewClient(mqttOpts)

	if token := mqttClient.Connect(); token.Wait() && token.Error() != nil {
		panic(token.Error())
	}

	mqttClient.Subscribe("tor/#", 2, mqttHandler)
	mqttClient.Subscribe("tor2/#", 2, mqttHandler)

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
