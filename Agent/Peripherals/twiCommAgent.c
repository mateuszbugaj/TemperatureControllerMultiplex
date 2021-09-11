#include <Peripherals/twiCommAgent.h>

volatile uint8_t lastReceived;
volatile uint8_t toTransmit;
volatile uint8_t isTransmited;
volatile enum status comMode = none;
uint8_t slaveAddress;

void releaseSCL(void){
    USI_PORT |= (1 << USI_SCL);
}

void releaseSDA(void){
    USI_PORT |= (1 << USI_SDA);
}

void pullSDA(){
    USI_PORT &= ~(1 << USI_SDA);
}

void pullSCL(void){
    USI_PORT &= ~(1 << USI_SCL);
}

void initStartConditionMode(void){
    // Start Condition Interrupt Enable, External, positive edge; shift data by External, Both Edges
    USICR = (1 << USISIE) | (1 << USIWM1) | (1 << USIWM0) | (1 << USICS1);

    // clear Interrupt Flags and Counter
    USISR = (1 << USIOIF) | (1 << USIPF) | (1 << USIDC);
}

void sendACK(){
    // pull SDA to LOW
    USI_DDR |= (1 << USI_SDA);
    pullSDA();

    // 1-bit counter for ACK
    USISR = (1 << USIOIF) | (1 << USIPF) | (1 << USIDC) | 0x0E;
}

void sendNACK(){
    // release SDA
    USI_DDR |= (1 << USI_SDA);
    releaseSDA();

    // 1-bit counter for ACK
    USISR = (1 << USIOIF) | (1 << USIPF) | (1 << USIDC) | 0x0E; 
}

void receiveACK(){
    USI_DDR &= ~(1 << USI_SDA);
    USIDR = 0x00;

    // 1-bit counter for ACK
    USISR = (1 << USIOIF) | (1 << USIPF) | (1 << USIDC) | 0x0E;
}

void initDataReception(){
    USI_DDR &= ~(1 << USI_SDA);
    USISR = (1 << USIOIF) | (1 << USIPF) | (1 << USIDC);
}

void initDataTransmission(){
    USI_DDR |= (1 << USI_SDA);
    releaseSDA(); // what the fuck 

    USISR = (1 << USIOIF) | (1 << USIPF) | (1 << USIDC);
}

void USI_I2C_slave_init(uint8_t address) {
	slaveAddress = address;
	// set SDA as INPUT and SCL as OUTPUT
	USI_DDR &= ~(1 << USI_SDA);
	USI_DDR |= (1 << USI_SCL);
	USI_PORT |= (1 << USI_SCL) | (1 << USI_SDA);
	
	// Start Condition Interrupt Enable, External, positive edge; shift data by External, Both Edges
	USICR = (1 << USISIE) | (1 << USIWM1) | (0 << USIWM0) | (1 << USICS1);

	// clear Interrupt Flags and Counter.
	USISR = (1 << USISIF) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC);
	
	comMode = none;
}

ISR(USI_START_vect) {
	// set SDA as INPUT
	USI_DDR &= ~(1 << USI_SDA);
	
	comMode = addressMode;
	// wait until START condition completed
	while ((USI_PIN & (1 << USI_SCL)) && !(USI_PIN & (1 << USI_SDA)));
	
	// if STOP condition occurred
	if (USI_PIN & (1 << USI_SCL)) {
		USICR = (1 << USISIE) | (1 << USIWM1) | (1 << USICS1);
	}
	else {
		USICR = (1 << USISIE) | (1 << USIOIE) | (1 << USIWM1) | (1 << USIWM0) | (1 << USICS1);
	}
	// clear Flags and set 4-bit Counter for 8-bit SLA+R/W
	USISR = (1 << USISIF) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC);
	USIDR = 0x00;
}

ISR( USI_OVF_vect) {
	switch (comMode) {
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