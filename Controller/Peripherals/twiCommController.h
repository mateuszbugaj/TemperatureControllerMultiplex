/**
 * Serial comunication implemented in two-wire interface
 * 
 */

#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <avr/interrupt.h>

#define WRITE_BYTE 0
#define READ_BYTE 1

#define SEND_ACK 1
#define SEND_NACK 0

#define LOG_ENABLED 1
#define LOG_DISABLED 0

#define TIME_OUT_MS 10

void initTWI(void);
void TWIStart(void);
void TWISendAddress(uint8_t address, uint8_t direction);
void TWISendData(uint8_t data);
uint8_t TWIGetData(bool ack);
void TWIStop(void);
uint8_t TWIStatusCode(void);
bool isError(void);
void clearTWIError(void);