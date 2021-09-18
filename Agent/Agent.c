#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <Peripherals/twiCommAgent.h>
#include <avr/power.h>

#define LED_1 PB4
#define TWI_slaveAddress (0x0f)

#define SET_REFRENCE_TEMP_CODE 0x01
#define GET_AGENT_TEMP_CODE 0x02

volatile uint8_t currentAction = 0;
volatile uint8_t referenceTemp = 0;
volatile uint8_t tempReading = 9;

void action(uint8_t code);

int main(void){
    clock_prescale_set(clock_div_1); // CPU Clock at 8MHz
    DDRB |= (1 << LED_1);

    initTWI(0x0f);
    sei();
    while(1){
        toTransmit = tempReading;

        if(isNewReceived()){
            action(lastReceived);

            if(currentAction == 0 && referenceTemp == 4){
                PORTB ^= (1 << LED_1);
            }
        }
    }

    return 0;
}

void action(uint8_t code){
    if(currentAction == 0){
        currentAction = code;
        return;
    }

    switch (currentAction){
        case SET_REFRENCE_TEMP_CODE:
            referenceTemp = code;
            break;

        default:
            currentAction = 0;
            break;
    }

    currentAction = 0;
}