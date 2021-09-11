#include <Peripherals/twiCommController.h>

volatile uint8_t statusCode;
volatile bool TWIError;

void decodeStatus(uint8_t code){
    statusCode = code;
    if(code == 0x30 || 
       code == 0x38 ||
       code == 0x48 ||
       code == 0x58){
           TWIError = true;
       }
}

void initTWI(){
    // Set SCL frequency to 100kHz for 8MHz CPU internal clock
    TWBR = 32;
    TWCR |= (1 << TWEN); // Enable TWI

    statusCode = 0;
    TWIError = false;
}

void waitForComplete(void){
    while(!(TWCR & (1<<TWINT)));
}

void TWIStart(void){
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    waitForComplete();
    statusCode = TWSR & 0xF8;
}

uint8_t generateAddress(uint8_t address, uint8_t direction){
    return address * 2 + direction;
}

void TWISendAddress(uint8_t address, uint8_t direction){
    uint8_t SLA_X = generateAddress(address, direction);
    TWDR = SLA_X;
    TWCR = (1<<TWINT) | (1<<TWEN);
    waitForComplete();
    statusCode = TWSR & 0xF8;
}


void TWISendData(uint8_t data){
    TWDR = data;
    TWCR = (1<<TWINT) | (1<<TWEN);
    waitForComplete();
    statusCode = TWSR & 0xF8;
}

uint8_t TWIGetData(bool ack){
    if(ack){
        TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
    }
     else{
        TWCR = (1<<TWINT) | (1<<TWEN);
     }

    waitForComplete();
    statusCode = TWSR & 0xF8;
    return (TWDR);
}

void TWIStop(void){
    TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

uint8_t TWIStatusCode(void){
    return statusCode;
}

bool isError(void){
    return TWIError;
}