#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* definitions for ATtiny25, ATtiny45 and ATtiny85  */
#include <avr/iotnx5.h>

/* Hardware defines */
#define USI_DDR	DDRB
#define USI_PORT PORTB
#define USI_PIN	PINB
#define USI_SDA	PB0
#define USI_SCL	PB2

/* Comunication status */
enum status {none,
            addressMode, 
            masterReadData, 
            masterWriteData, 
            slaveReceiveData, 
            slaveTransmitData};

#define BUFFERSIZE 8

extern volatile uint8_t comState;

void USI_I2C_slave_init(uint8_t);

#define LED_1 PB3
#define LED_2 PB4