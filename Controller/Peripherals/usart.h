/**
 * USART communication for sending and receiving data
 * from controller about temperature control or agents
 */

#define NUMBER_BUFFER_SIZE 8
enum NUMBER_FORMAT {DECIMAL, BINARY, HEX};

void initUSART(void);
void printString(const char chars[]);
void printLogNum(const char message[], int data, enum NUMBER_FORMAT type);
uint8_t getRecentNumber(void);