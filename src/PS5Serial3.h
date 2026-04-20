#pragma once
#include <Arduino.h>

class PS5Serial3 {

public:
  PS5Serial3();
  bool psSetup();
  void handleByte(uint8_t byte);
  bool available();
  uint16_t getEvent();
  void processPacket();
  void printPots();

  int8_t getLX();
  int8_t getLY();
  int8_t getRX();
  int8_t getRY();
  int8_t getL2();
  int8_t getR2();


private:
#define EVENT_QUEUE_SIZE 8
#define BAUD 115200
#define BAUDRATE ((F_CPU / (BAUD * 8UL) - 1))

  volatile uint8_t event_head = 0;
  volatile uint8_t event_tail = 0;
  volatile uint16_t prev_buttons_isr = 0;


  bool incoming;
  uint8_t RXIncomingByte = 0;
  volatile uint8_t bufferCounter = 0;
  uint8_t packet_buffer[11];

  uint16_t buttons = 0, pressed = 0;
  bool ready = false;

  // PS5 values
  int8_t LX = 0, LY = 0, RX = 0, RY = 0, L2 = 0, R2 = 0;


  bool checkCRC(uint8_t *buf);
  void enqueue_button_event(uint16_t pressed);
  volatile uint16_t event_queue[EVENT_QUEUE_SIZE];
};