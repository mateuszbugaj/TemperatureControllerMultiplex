#include <Peripherals/twiCommAgent.h>

volatile uint8_t lastReceived;
volatile uint8_t toTransmit;
volatile uint8_t isTransmited;
volatile enum status comMode = none;
uint8_t slaveAddress;

/*
    USICR
        Start Condition Interrupt Enable
        External clock source, positive edge
        Counter toggle on both edges

    USISR
        Clear all flags
        Clear counter

    Clear USIDR
*/
void initStartConditionMode(void){
    USICR = (1 << USISIE) | (1 << USIWM1) | (1 << USICS1) | (1 << USIWM0);
    USISR = (1 << USIOIF) | (1 << USIPF) | (1 << USIDC);
    USIDR = 0x00;
}

/*
    Set SDA as output
    Pull SDA low
    Set 4-bit counter to count 1 and overflow
*/
void sendACK(){
    USI_DDR |= (1 << USI_SDA);
    USI_PORT &= ~(1 << USI_SDA);
    USISR = (1 << USIOIF) | (1 << USIPF) | (1 << USIDC) | 0x0E;
}

/*
    Set SDA as output
    Release SDA
    Set 4-bit counter to count 1 and overflow
*/
void sendNACK(){
    USI_DDR |= (1 << USI_SDA);
    USI_PORT |= (1 << USI_SDA);
    USISR = (1 << USIOIF) | (1 << USIPF) | (1 << USIDC) | 0x0E; 
}

/*
    Set SDA as input
    Clear USI Data Register
    Set 4-bit counter to count 1 and overflow
*/
void receiveACK(){
    USI_DDR &= ~(1 << USI_SDA);
    USIDR = 0x00;
    USISR = (1 << USIOIF) | (1 << USIPF) | (1 << USIDC) | 0x0E;
}

/*
    Set SDA as input
    Clear 4-bit counter
*/
void initDataReception(){
    USI_DDR &= ~(1 << USI_SDA);
    USISR = (1 << USIOIF) | (1 << USIPF) | (1 << USIDC);
}

/*
    Set SDA as output
    Release SDA
    Clear 4-bit counter
*/
void initDataTransmission(){
    USI_DDR |= (1 << USI_SDA);
    USI_PORT |= (1 << USI_SDA);
    USISR = (1 << USIOIF) | (1 << USIPF) | (1 << USIDC);
}

/*
    Set slave address
    Set SDA as INPUT and SCL as OUTPUT
    Activate internall pull-up resistors for SDA ans SCL
    Initialize Start Condition Mode
*/
void USI_I2C_slave_init(uint8_t address) {
	slaveAddress = address;
	USI_DDR &= ~(1 << USI_SDA);
	USI_DDR |= (1 << USI_SCL);
	USI_PORT |= (1 << USI_SCL) | (1 << USI_SDA);
    initStartConditionMode();
	comMode = none;
}

/*
    Start Condition Detector Intrerrupt
    When start condition is detected:
    Set SDA as INPUT
    Set comMode to addressMode
    Clear Flags and set 4-bit Counter for 8-bit SLA+R/W

*/
ISR(USI_START_vect) {
	USI_DDR &= ~(1 << USI_SDA);
	comMode = addressMode;

	// wait until START condition completed
	while ((USI_PIN & (1 << USI_SCL)) && !(USI_PIN & (1 << USI_SDA)));

    initStartConditionMode();
	
	/* 
        If STOP condition didn't occurred, enable Counter Overflow Interrupt and
        clear Start Condition Interrupt flag
    */
	if (!(USI_PIN & (1 << USI_SCL))) {
	    USICR |= (1 << USIOIE);
        USISR |= (1 << USISIF);
	}

}

/*
    4-bit Counter Overflow Interrupt routine
    Act on the previously selected action
*/
ISR( USI_OVF_vect) {
	switch (comMode) {

        /*
            addressMode:
            Check if 7 most significant bits are equal to the agentAddress or initialize start condition
            Check for read or write bit in least significant place of SLA_X, set proper comMode and send ack
            Clear Data Register
        */
		case addressMode:	
            if ((USIDR >> 1) == slaveAddress) {
                if (USIDR & 0x01){
                    comMode = masterReadData;
                } else{
                    comMode = masterWriteData;
                }
                
                sendACK();
            } else {
                initStartConditionMode();
            }
            USIDR = 0x00;
            break;
		
		case masterWriteData:	
            comMode = slaveReceiveData;
            initDataReception();
            // STOP condition detector
            // wait for SDA HIGH
            while (!(USI_PIN & (1 << USI_SDA)));
            // if STOP condition
            if (USI_PIN & (1 << USI_SCL)) {
                initStartConditionMode();
            }
            break;
							
		case slaveReceiveData:
            comMode = masterWriteData;
            lastReceived = USIDR;
            sendACK();

            if(lastReceived == 4){
                PORTB ^= (1 << LED_2);
            }

            break;
							
		case masterReadData:
            // if received NACK
            if (USIDR) {
                initStartConditionMode();
            }
            else {
                comMode = slaveTransmitData;
                USIDR = 53;
                initDataTransmission();
            }
            break;
							
		case slaveTransmitData:
            comMode = masterReadData;
            receiveACK();
            break;
	}
}