
// premier com port
extern unsigned char serialSetup(void);
extern unsigned char serialWrite(unsigned char *mySerialData, unsigned char count);
extern unsigned char serialRead(unsigned char *mySerialData);

// Seoncond com prt
extern unsigned char serial1Setup(void);
extern unsigned char serial1Write(unsigned char *mySerialData, unsigned char count);
extern unsigned char serial1Read(unsigned char *mySerialData);


extern unsigned char flag_uartOutBusy;
extern unsigned char flag_uart1OutBusy;
