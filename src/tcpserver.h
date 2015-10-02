#define TCP_PORT 51718

extern unsigned char tcpOpen(unsigned int portno);
extern unsigned char waitTcpClient(); // BLOCANT
extern int tcpCheck();
extern void tcpWrite(int x);
extern void tcpWriteString(char *myStringTcp,int count);
extern void tcpClose();
