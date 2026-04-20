#include "PS5Serial3.h"
#include <util/atomic.h>

PS5Serial3::PS5Serial3() {
  bufferCounter = 0;
  event_head = 0;
  event_tail = 0;
  prev_buttons_isr = 0;
}

bool PS5Serial3::psSetup() {
  UBRR3H = BAUDRATE >> 8;
  UBRR3L = BAUDRATE;
  UCSR3A = (1 << U2X3);
  UCSR3B = (1 << RXCIE3) | (1 << RXEN3) | (1 << TXEN3);
  UCSR3B &= ~(1 << UCSZ32);
  UCSR3C = (1 << UCSZ31) | (1 << UCSZ30);
}

bool PS5Serial3::checkCRC(uint8_t *buf){
  if (buf[0] != 0XAA || buf[10] != 0x55) return false;
  uint8_t sum = 0;
  for (int i = 0; i <= 8; i++) sum += buf[i];
  return ((uint8_t)(0 - sum) == buf[9]);
}

bool PS5Serial3::available() {
  return (event_head != event_tail);
}

uint16_t PS5Serial3::getEvent() {
  uint16_t evt = 0;

  if (event_head == event_tail) return 0;

  evt = event_queue[event_tail];
  event_tail = (event_tail + 1) % EVENT_QUEUE_SIZE;

  return evt;
}

void PS5Serial3::enqueue_button_event(uint16_t pressed){
  uint8_t next = (event_head + 1) % EVENT_QUEUE_SIZE;

	if (next != event_tail) {   // queue not full
		event_queue[event_head] = pressed;
		event_head = next;
	}
}

void PS5Serial3::processPacket(){
  if(ready){
    uint8_t localbuf[11];
    cli();
    memcpy(localbuf,packet_buffer,11);
    sei();

    if(checkCRC(localbuf)){
      LX = (int8_t)localbuf[1];
      LY = (int8_t)localbuf[2];
      RX = (int8_t)localbuf[3];
      RY = (int8_t)localbuf[4];
      R2 = localbuf[5];
      L2 = localbuf[6];
    }
  }
}

void PS5Serial3::printPots(){
  Serial.print(LX);
  Serial.print("\t");
  Serial.print(LY);
  Serial.print("\t");
  Serial.print(RX);
  Serial.print("\t");
  Serial.print(RY);
  Serial.print("\t");
  Serial.print(L2);
  Serial.print("\t");
  Serial.print(R2);
  Serial.println();
}


void PS5Serial3::handleByte(uint8_t byte){
  RXIncomingByte = byte;
  if (RXIncomingByte == 0xAA) {
    bufferCounter = 0;
    packet_buffer[bufferCounter++] = RXIncomingByte;
    return;
  }

  if (bufferCounter > 0) {
    packet_buffer[bufferCounter++] = RXIncomingByte;

    if (bufferCounter == 11) {
      if (checkCRC(packet_buffer)) {
        ready=true;
        buttons = packet_buffer[7] | (packet_buffer[8]<<8);
        pressed = buttons & ~prev_buttons_isr;
        if(pressed) enqueue_button_event(pressed);
        prev_buttons_isr = buttons;
      }
      bufferCounter = 0;
    }
  }
}

int8_t PS5Serial3::getLX(){
  return LX;
}
int8_t PS5Serial3::getLY(){
  return LY;
}
int8_t PS5Serial3::getRX(){
  return RX;
}
int8_t PS5Serial3::getRY(){
  return RY;
}
int8_t PS5Serial3::getL2(){
  return L2;
}
int8_t PS5Serial3::getR2(){
  return R2;
}

