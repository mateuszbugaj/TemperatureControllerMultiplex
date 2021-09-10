#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
#include <Peripherals/desktopComm.h>
#include <avr/interrupt.h>

int main(void){
    clock_prescale_set(clock_div_2);
    DDRB |= (1 << PB0);
    initUSART();
    sei();
    while(1){
        if(isNewCommand()){
            // PORTB ^= (1 << PB0);
        }
    }

    return 0;
}