/*
Author- Hrishikesh Samant
Last modified - 20/4/2026

To use this library do the following:

1. Connect esp32 to Serial3 on Mega.
2. Set esp32 UART BAUD to 115200
3. Send PS5 data in the following format:

CRC index     uint8_t Data

0             0xAA
1             LeftJoystickX
2             LeftJoystickY
3             RightJoystickX
4             RightJoystickY
5             R2
6             L2
7             buttons & 0xFF            {bitmask the button values and store them in a uint16_t buttons variable..}
8             (buttons>>8) & 0xFF
9             (uint8_t)(0 - checksum)   {run a for loop from index 0 to 8 and store it inside uint8_t checksum}
10            0x55     

*/

#include "PS5Serial3.h"  //include library

PS5Serial3 controller; //class obj

ISR(USART3_RX_vect){
  /*data stored inside UDR3 will go to handle byte which is processing CRC..
  this is the main function where data is being received, wont work without this
  */
  controller.handleByte(UDR3); 
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  controller.psSetup(); //this will initialize the interrupt speed settings for UART3. It will set receive data in 8N1 format.
  Serial.println("Ready");
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //processPacket function will read the crc and update the joystick and pot values.. necessary to call once per loop
  controller.processPacket(); 

  //to read the buttons and assign them functions
  if(controller.available()){
    uint16_t event = controller.getEvent();
    if(event & 1<<0) Serial.println("Cross");
    if(event & 1<<1) Serial.println("Circle");
    if(event & 1<<2) Serial.println("Triangle");
    if(event & 1<<3) Serial.println("Square");
    if(event & 1<<4) Serial.println("L1");
    if(event & 1<<5) Serial.println("R1");
    if(event & 1<<6) Serial.println("Up");
    if(event & 1<<7) Serial.println("Down");
    if(event & 1<<8) Serial.println("Cross");
    if(event & 1<<9) Serial.println("Right");
  }

  //to read and store joystick and pot values use the get functions
  int8_t LX = controller.getLX();
  int8_t LY = controller.getLY();
  int8_t RX = controller.getRX();
  int8_t RY = controller.getRY();
  int8_t L2 = controller.getL2();
  int8_t R2 = controller.getR2();
}
