# Raspberry Pi Temperature and Humidity Reader

Reads humidity and temperature from dht22 sensor, then displays it on the screen using Qt framework

1. Setup
- Screen is connected to Raspberry Pi by I2C bus number 1.
- DHT22 sensor uses GPIO pin #4
- TSL2580 light sensor is connected to second I2C bus created using command:
``` markdown
sudo dtoverlay i2c-gpio bus=2 i2c_gpio_sda=22 i2c_gpio_scl=23
```
or by appending a line
``` markdown
dtoverlay=i2c-gpio,bus=2,i2c_gpio_sda=22,i2c_gpio_scl=23
```
at the end of the `/boot/config.txt` file.
