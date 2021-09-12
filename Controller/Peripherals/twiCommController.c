#include <Peripherals/twiCommController.h>

uint8_t statusCode;
bool TWIError;
volatile bool timeoutIsSet = false;

void decodeStatus(uint8_t code){
    statusCode = code;
    if(code == 0x30 || 
       code == 0x38 ||
       code == 0x48 ||
       code == 0x58){
           TWIError = true;
       }
}

/*
    Set compare register
    Enable output compare interrupt

    TCCR1B
        Enable Clear Timer on Compare Match (CTC) mode
        Set CPU clock prescaler to 1/8 (For 8MHz it gaves 1000000 tick per sec)
*/
void setTimeoutTimer(uint8_t ms){
    OCR1A = ms * 1000;
    TIMSK1 = (1 << OCIE1A);
    TCCR1B = (1 << WGM12) | (1 << CS11);
}

/*
    Timeout timer compare match interrupt
*/
ISR(TIMER1_COMPA_vect){
    TCNT0 = 0;
    TCCR1B = 0; // stop the timer
    timeoutIsSet = true;
}

/*
    Set SCL frequency to 100kHz for 8MHz CPU internal clock
    TWCR
        Enable TWI
*/
void initTWI(){
    TWBR = 32;
    TWCR |= (1 << TWEN);
    statusCode = 0;
    TWIError = false;
}

/*
    Start Timeout timer
    Wait until TWINT bit is set or Timeout timer goes off
    If Timeout timer breaks the while loop,
    raise an error and set TWINT bit manually
    Otherwise, stop and reset the timer
*/
void waitForComplete(void){
    setTimeoutTimer(TIME_OUT_MS);
    while(!(TWCR & (1<<TWINT))){
        if(timeoutIsSet){
            break;
        }
    }

    if(timeoutIsSet){
        timeoutIsSet = false;
        TWIError = true;
        statusCode = 0x99; // Temp status code for timeout
        TWCR |= (1 << TWINT);
    } else {
        TCNT0 = 0;
        TCCR1B = 0; // stop the timer
    }
}

void TWIStart(void){
    TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
    waitForComplete();
    statusCode = TWSR & 0xF8;
}

/*
    Combine agent address and direction bit
    Load SLA_X into Data Register
*/
void TWISendAddress(uint8_t address, uint8_t direction){
    uint8_t SLA_X = address * 2 + direction;
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

void clearTWIError(void){
    TWIError = false;
}