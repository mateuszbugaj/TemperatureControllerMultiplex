#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
#include <Peripherals/desktopComm.h>
#include <Peripherals/twiCommController.h>
#include <avr/interrupt.h>

#define SLAVE_ADDRESS 0x0f

int main(void){
    clock_prescale_set(clock_div_2);
    DDRB |= (1 << PB0);
    initUSART();
    initTWI();
    sei();
    while(1){
        if(isNewCommand()){
            printString("Got command");

            // Write number 4 to the slave to check transmission
            printString(". Writing data to the slave");
            TWIStart();
            TWISendAddress(SLAVE_ADDRESS, WRITE_BYTE);
            TWISendData(4);
            TWIStop();

            // Reading data from slave to check reception
            printString(". Reading data from slave");
            TWIStart();
            TWISendAddress(SLAVE_ADDRESS, READ_BYTE);

            char log[100] = { 0 };
            snprintf(log, 100, "Data from slave: %d", TWIGetData(SEND_NACK));
            printString(log);

            TWIStop();
        }

        if(isError()){
            printString("TWI Error");
        }
    }

    return 0;
}