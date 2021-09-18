#include <avr/io.h>
#include <avr/interrupt.h>

/* Hardware defines */
#define TEMP_DDR DDRB
#define TEMP_PIN PINB
#define TEMP_PORT PORTB
#define TEMP_INPUT PB3

void initAdcTemp(void);
uint8_t getTempReading(void);