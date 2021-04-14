# README

LED signals

| LED pattern (3 sec)               | Status                              |
| --------------------------------- | ----------------------------------- |
| `XXXXX_____XXXXX_____XXXXX_____`  | Trying to connect to WLAN           |
| `X_X_X__XX_XX_XX__X_X_X________`  | Failed to connect to WLAN           |
| `XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX`  | Trying to connect to WLAN using WPS |
| `X_____________________________`  | OK                                  |



```bash
mosquitto_pub -h mypi.fritz.box -d --cafile ./root_ca_cert.pem --insecure -p 8883 -t "esp-gate-dev/relais" -m 0
mosquitto_pub -h mypi.fritz.box -d --cafile ./root_ca_cert.pem --insecure -p 8883 -t "esp-gate-dev/relais" -m 1
```

```bash
mosquitto_sub -h mypi.fritz.box -d --cafile ./root_ca_cert.pem --insecure -p 8883 -t "esp-gate-dev/#"
```

5500 mm / 25 s = 220 mm/s
