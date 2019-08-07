# README

```bash
mosquitto_pub -h rpi.fritz.box -d --cafile ./root_ca_cert.pem -p 8883 -t "tor/relais" -m 0
mosquitto_pub -h rpi.fritz.box -d --cafile ./root_ca_cert.pem -p 8883 -t "tor/relais" -m 1
```

```bash
mosquitto_sub -h rpi.fritz.box -d --cafile ./root_ca_cert.pem -p 8883 -t "tor/#"
```

5500 mm / 25 s = 220 mm/s
