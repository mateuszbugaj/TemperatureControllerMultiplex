#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
#include <Peripherals/desktopComm.h>
#include <Peripherals/twiCommController.h>
#include <avr/interrupt.h>
#include <string.h>

#define AGENT_ADDRESS 0x0f
#define COMMAND_UNDEFINED_CODE 0x00
#define SET_REFRENCE_TEMP "srt"
#define SET_REFRENCE_TEMP_CODE 0x01
#define GET_AGENT_TEMP "gat"
#define GET_AGENT_TEMP_CODE 0x02

uint8_t commandCode;

uint8_t getCommandCode(char command[COMMAND_MAX_SIZE + 1]);
void action(uint8_t commandCode, uint16_t number);

int main(void){
    clock_prescale_set(clock_div_2);
    DDRB |= (1 << PB0) | (1 << PB1);
    initUSART();
    initTWI();
    sei();
    while(1){
        if(isNewCommand()){
            action(getCommandCode(command), commandNumber);
        }

        if(isError()){
            printString("TWI Error");
            clearTWIError();
        }
    }

    return 0;
}

uint8_t getCommandCode(char com[COMMAND_MAX_SIZE + 1]){
    if(strcmp(com, SET_REFRENCE_TEMP) == 0){
        return SET_REFRENCE_TEMP_CODE;
    }

    if(strcmp(com, GET_AGENT_TEMP) == 0){
        return GET_AGENT_TEMP_CODE;
    }

    return COMMAND_UNDEFINED_CODE;
}

void action(uint8_t commandCode, uint16_t number){
    switch (commandCode){
        case SET_REFRENCE_TEMP_CODE:
            printString(". Writing reference temperature value to the agent");
            TWIStart();
            TWISendAddress(AGENT_ADDRESS, WRITE_BYTE);
            TWISendData(number);
            TWIStop();
            break;

        case GET_AGENT_TEMP_CODE:
            printString(". Getting temperature reading from the agent");
            TWIStart();
            TWISendAddress(number, READ_BYTE);
            uint8_t data = TWIGetData(SEND_NACK);

            if(isError()){
                printString(". Error while receiving data from agent");
            } else {
                char log[100] = { 0 };
                snprintf(log, 100, "Data from agent %d: %d", number, data);
                printString(log);
                TWIStop();
            }
            break;

        case COMMAND_UNDEFINED_CODE:
            printString(". Command undefined");
            break;
        
        default:
            break;
    }
}