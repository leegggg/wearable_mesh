# wearable mesh

A sensor and actuator network for wearable devices based on esp mcu and mqtt.

## network set up example

send the following commands to the esp32 via serial to set up the network. Use LF as line ending.

```
AT+UCI=SET,cli.echo,1

AT+UCI=SET,wireless.sat[0].enabled,1
AT+UCI=SET,wireless.sat[0].ssid,ssid0
AT+UCI=SET,wireless.sat[0].key,key0key0

AT+UCI=SET,wireless.sat[1].enabled,1
AT+UCI=SET,wireless.sat[1].ssid,ssid1
AT+UCI=SET,wireless.sat[1].key,key1key1

AT+UCI=SET,wireless.sat[2].enabled,1
AT+UCI=SET,wireless.sat[2].ssid,ssid2
AT+UCI=SET,wireless.sat[2].key,key2key2

AT+UCI=SET,wireless.sat[3].enabled,1
AT+UCI=SET,wireless.sat[3].ssid,ssid3ssid3
AT+UCI=SET,wireless.sat[3].key,ksy3ksy3

AT+UCI=SET,mqtt.mesh,wab-a1b2

AT+UCI=SET,direct_pwm.allowed_pins,0x0000000000000018

# For coyote and jiandan services
AT+UCI=SET,coyote.enabled,1
AT+UCI=SET,jiandan.enabled,1

AT+RST
```