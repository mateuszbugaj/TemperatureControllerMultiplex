#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

/* definitions for ATtiny25, ATtiny45 and ATtiny85  */
#include <avr/iotnx5.h>

/* Hardware defines */
#define USI_DDR	DDRB
#define USI_PORT PORTB
#define USI_PIN	PINB
#define USI_SDA	PB0
#define USI_SCL	PB2

/* Comunication modes */
enum modes {addressMode, 
            masterReadData, 
            masterWriteData, 
            slaveReceiveData, 
            slaveTransmitData};

/* Data transmision variables */
volatile uint8_t lastReceived;
volatile bool newReceived;
volatile uint8_t toTransmit;
volatile bool isTransmited;
volatile enum modes comMode;
uint8_t slaveAddress;

void initTWI(uint8_t);
bool isNewReceived(void);