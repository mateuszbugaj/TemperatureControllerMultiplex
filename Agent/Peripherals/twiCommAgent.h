#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

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

extern volatile uint8_t tranCount;
extern volatile uint8_t recCount;
extern volatile uint8_t recBuffer[];
extern volatile uint8_t tranBuffer[];
extern volatile uint8_t comState;

void USI_I2C_slave_init(uint8_t);
void flushRecBuffer();

#define LED_1 PB3
#define LED_2 PB4