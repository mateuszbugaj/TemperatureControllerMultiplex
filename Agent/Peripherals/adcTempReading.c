#include <Peripherals/adcTempReading.h>

/*
    Initialize ADC
    ADMUX
        Set VCC as voltage reference (REFS[2:0] = 0)
        Left adjust the results
        Set input channel for PB3 (MUX[3:0] = 3)

    ADCSRA
        Enable ADC
        Start the first conversion
        Enable auto trigger of ADC (Probably should set this to Timer/Counter with ADCSRB)
        Enable ADC interrupt
        Set clock prescaler to 128

*/
void initAdcTemp(){
    ADMUX = (1 << ADLAR) | (1 << MUX1) | (1 << MUX0);
    ADCSRA = (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint8_t getTempReading(){
    return ADCH;
}