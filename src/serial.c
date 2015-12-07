#include "serial.h"
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART

//At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively
int uart0_filestream = -1;

// pour 2eme  port COM
int uart1_filestream = -1;

unsigned char flag_uartOutBusy=0;
unsigned char flag_uart1OutBusy=0;


unsigned char serialSetup(void){

	//-------------------------
	//----- SETUP USART 0 -----
	//-------------------------


	//OPEN THE UART
	//The flags (defined in fcntl.h):
	//	Access modes (use 1 of these):
	//		O_RDONLY - Open for reading only.
	//		O_RDWR - Open for reading and writing.
	//		O_WRONLY - Open for writing only.
	//
	//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
	//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
	//											immediately with a failure status if the output can't be written immediately.
	//
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
	uart0_filestream = open("/dev/ttyO4", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (uart0_filestream == -1)
	{
		return(0);
	}

	//CONFIGURE THE UART
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;		//<Set baud rate
//	options.c_cflag = B230400 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);

	return(1);
	// END SERIAL
}


// Fonction d'écriture sur port COM Uart0
unsigned char serialWrite(unsigned char *mySerialData, unsigned char count){
	if (uart0_filestream != -1)
	{
		write(uart0_filestream, mySerialData, count);		//Filestream, bytes to write, number of bytes to write
		return(1);
	}else return(0);

}



// Fonction de leccture sur port COM Uart0
unsigned char serialRead(unsigned char *mySerialData){
	unsigned char myChar;
	static unsigned char bufferInput[100];
	static unsigned char cntChar=0;
	static unsigned char waitForData=0;
	static unsigned char dataLenght=0;
	static unsigned char flagAckReceive=0;
	unsigned char i;


	if (uart0_filestream != -1)
	{
	//bufferInput[cntChar] = serialPort1.ReadByte();

	if(read(uart0_filestream, &myChar,1)!=-1){
		bufferInput[cntChar]=myChar;
		cntChar++;

		if (waitForData == 0)
		{
			if (cntChar >= 2)
			{
				if (bufferInput[0] == 0xaa)
				{

					dataLenght = bufferInput[1];
					waitForData = 1;
				}
				else
				{
					tcflush(uart0_filestream,TCIOFLUSH);
					cntChar = 0;
					waitForData = 0;
				}
			}
		}
		else
		{
			if (cntChar >= dataLenght + 2)
			{
				flagAckReceive = 1;
				cntChar = 0;
				waitForData = 0;

				tcflush(uart0_filestream,TCIOFLUSH);
			}
		}
	}
}

	if(flagAckReceive){
		for(i=0;i<bufferInput[1]+2;i++){
			mySerialData[i]=bufferInput[i];
		}
		flagAckReceive=0;
		return(1);
	}

	return(0);
}


// ******************************************************************3
// * FONCTION SERIAL POUR SECOND PORT COM (LCD)
// *
// ******************************************************************3
// SETUP SECOND PORT COM
unsigned char serial1Setup(void){

	//-------------------------
	//----- SETUP USART 1 -----
	//-------------------------


	//OPEN THE UART
	//The flags (defined in fcntl.h):
	//	Access modes (use 1 of these):
	//		O_RDONLY - Open for reading only.
	//		O_RDWR - Open for reading and writing.
	//		O_WRONLY - Open for writing only.
	//
	//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
	//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
	//											immediately with a failure status if the output can't be written immediately.
	//
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
	uart1_filestream = open("/dev/ttyO2", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
	if (uart1_filestream == -1)
	{
		return(0);
	}

	//CONFIGURE THE UART
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)
	struct termios options;
	tcgetattr(uart1_filestream, &options);
	//options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
		options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;		//<Set baud rate
//	options.c_cflag = B230400 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart1_filestream, TCIFLUSH);
	tcsetattr(uart1_filestream, TCSANOW, &options);

	return(1);
	// END SERIAL
}



// Fonction d'écriture sur port COM Uart1
unsigned char serial1Write(unsigned char *mySerialData, unsigned char count){
	if (uart1_filestream != -1)
	{
		write(uart1_filestream, mySerialData, count);		//Filestream, bytes to write, number of bytes to write
		return(1);
	}else return(0);

}



// Fonction de lecture sur second  port COM Uart1
unsigned char serial1Read(unsigned char *mySerialData){
	unsigned char myChar;
	static unsigned char bufferInput[50];
	static unsigned char cntChar=0;
	static unsigned char sof=0;

	unsigned char i;


	if (uart1_filestream != -1)
	{
	if(read(uart1_filestream, &myChar,1)!=-1){

		if (sof == 0)
		{
			// Detection d'un nouvelle trame
				if (myChar == 0xAA){
					cntChar=0;						// D�marre le comptage du nombre d'octet utile
					sof = 1;
				}
				else
				{
					tcflush(uart1_filestream,TCIOFLUSH);	// Reset la récéption
					sof = 0;
				}
		}
		else
		{
			// Donneee en cours de reception
			if(myChar == 0xFE){
				// Attente fin de trame
				for(i=0;i<cntChar;i++){
					mySerialData[i]=bufferInput[i];
				}

				tcflush(uart1_filestream,TCIOFLUSH);	// Reset la récéption
				sof = 0;										// Attente nouvelle trame
				return(1);
			}
			else{
					if(cntChar>=35){							// mauvaise trame > 35 data max
						tcflush(uart1_filestream,TCIOFLUSH);	// Reset la récéption
						cntChar = 0;
						sof = 0;
					}else{
						bufferInput[cntChar]=myChar;		// Enregistrement des données
						cntChar++;
					}
			}
		}
	}
}
	return(0);
}



