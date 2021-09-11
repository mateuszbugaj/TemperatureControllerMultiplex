#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <Peripherals/twiCommAgent.h>
#include <avr/power.h>

#define SDA PB0
#define SCL PB2
#define LED_1 PB3
#define LED_2 PB4
#define TWI_slaveAddress (0x0f)

int main(void){
    clock_prescale_set(clock_div_1); // CPU Clock at 8MHz
    DDRB |= (1 << LED_1) | (1 << LED_2);

    USI_I2C_slave_init(0x0f);
    sei();
    while(1){
        PORTB ^= (1 << LED_1);

        _delay_ms(500);
    }

    return 0;
}