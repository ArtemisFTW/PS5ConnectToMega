# PS5ConnectToMega
This library is useful to transmit PS5Controller Joystick, Pot and Button values over to an Arduino Mega 2560 via an esp32 uart.

The esp32 will read the PS5 Data via PS5Controller library available on github. It will send the received data to Arduino Mega using Serial1 of esp32.
The Mega will receive the data on Serial3 using ISR.

Wiring
ESP32 TX2 -> Mega RX3
ESP32 GND -> Mega GND
