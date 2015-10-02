

extern unsigned char serialSetup(void);
extern unsigned char serialWrite(unsigned char *mySerialData, unsigned char count);
extern unsigned char serialRead(unsigned char *mySerialData);
extern unsigned char flag_uartOutBusy;
