/**
 * USART communication for sending and receiving data
 * from controller about temperature control or agents
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#ifndef BAUD
   #define BAUD 9600
#endif
#include <util/setbaud.h>
#include <avr/interrupt.h>

#define NUMBER_BUFFER_SIZE 8
#define COMMAND_MAX_SIZE 3
enum NUMBER_FORMAT {DECIMAL, BINARY, HEX};

char command[COMMAND_MAX_SIZE + 1];
uint16_t commandNumber;

void initUSART(void);
void printString(const char chars[]);
void printLogNum(const char message[], int data, enum NUMBER_FORMAT type);
uint8_t isNewCommand(void);