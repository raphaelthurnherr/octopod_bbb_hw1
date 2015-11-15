#define TRUE	1
#define FALSE	0

#define SYS_STATUS	0
#define SYS_IP_LAN	1
#define SYS_IP_WLAN	2
#define APP_STATUS	3
#define LLHW_STATUS 4
#define APP_MOT_STATE 5
#define APP_MOT_ANGLE 6
#define COMPASS		7
#define USONIC		8


#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

#include "th8_lcdDisplay.h"

#include "manager.h"
#include "th2_core.h"
#include "th1_hwctrl.h"

pthread_t th_lcd;

unsigned char lcdBufferOut[50];

void loadSysStatus();
void loadSysIpLan();
void loadSysIpWlan();
void loadAppStatus();
void loadLLHwStatus();
void loadMotorState();
void loadMotorAngle();
void loadCompassMeas();
void loadUSonicMeas();

void Lcd_DisplayData(unsigned char CMD);

// ***************************************************************************
// ---------------------------------------------------------------------------
// THREAD LCD: Gestion de l'interface utilisateur LCD
// ---------------------------------------------------------------------------
// ***************************************************************************

void *lcdTask (void * arg)
{
	printf ("# Demarrage tache HMI LCD: OK\n");

	if(SystemStatus.AppStarted)SystemStatus.AppStarted=0;


	 usleep(1000000);

	 RunningTask += TH8_SOA;
	 while(!EndOfApp){
//	  pthread_mutex_lock (&my_mutex);

		 Lcd_DisplayData(0);
		 usleep(100000);

	  if(hwctrl_uartLCDDataInReady){
		  hwctrl_uartLCDDataInReady=0;
	  }

//	pthread_mutex_unlock (&my_mutex);
  }
	 usleep(1000000);
	 Lcd_DisplayData(0);
  printf( "# ARRET tache HMI LCD\n");

  RunningTask -= TH8_SOA;
  pthread_exit (0);
}

// ---------------------------------------------------------------------------
// LCD_SENDCMD Envoie de data défini à l'afficheur
// ---------------------------------------------------------------------------

void Lcd_DisplayData(unsigned char CMD)
{
	switch(CMD){
		case SYS_STATUS : loadSysStatus(); break;
		case SYS_IP_LAN : loadSysIpLan(); break;
		case SYS_IP_WLAN : loadSysIpWlan(); break;
		case APP_STATUS : loadAppStatus(); break;
		case LLHW_STATUS : loadLLHwStatus(); break;
		case APP_MOT_STATE : loadMotorState(); break;
		case APP_MOT_ANGLE : loadMotorAngle(); break;
		case COMPASS : loadCompassMeas(); break;
		case USONIC : loadUSonicMeas(); break;
		default: break;
	}
}


// ---------------------------------------------------------------------------
// Charge le status du system dans le buffer de sortie
// ---------------------------------------------------------------------------
void loadSysStatus(){

	lcdBufferOut[0]=0xAA;
	lcdBufferOut[1]=SYS_STATUS;
	lcdBufferOut[2]=SystemStatus.AppStarted;
	lcdBufferOut[3]=SystemStatus.LLHwConnected;
	lcdBufferOut[4]=SystemStatus.LanWifiReady;
	lcdBufferOut[5]=SystemStatus.LanEthReady;
	lcdBufferOut[6]=SystemStatus.Batt1Warning;
	lcdBufferOut[7]=SystemStatus.Batt2Warning;
	lcdBufferOut[8]=0xEE;

	sendLCDUartFrame(lcdBufferOut, 9);
};

// ---------------------------------------------------------------------------
// Charge l'adresse IP LAN dans le buffer de sortie
// ---------------------------------------------------------------------------
void loadSysIpLan(){

	lcdBufferOut[0]=0xAA;
	lcdBufferOut[1]=SYS_IP_LAN;
	lcdBufferOut[2]=SystemLan.bIP_lan[0];
	lcdBufferOut[3]=SystemLan.bIP_lan[1];
	lcdBufferOut[4]=SystemLan.bIP_lan[2];
	lcdBufferOut[5]=SystemLan.bIP_lan[3];
	lcdBufferOut[6]=0xEE;

	sendLCDUartFrame(lcdBufferOut, 7);
};

// ---------------------------------------------------------------------------
// Charge l'adresse IP WLAN dans le buffer de sortie
// ---------------------------------------------------------------------------
void loadSysIpWlan(){

	lcdBufferOut[0]=0xAA;
	lcdBufferOut[1]=SYS_IP_LAN;
	lcdBufferOut[2]=SystemLan.bIP_wlan[0];
	lcdBufferOut[3]=SystemLan.bIP_wlan[1];
	lcdBufferOut[4]=SystemLan.bIP_wlan[2];
	lcdBufferOut[5]=SystemLan.bIP_wlan[3];
	lcdBufferOut[6]=0xEE;

	sendLCDUartFrame(lcdBufferOut, 7);
};

// ---------------------------------------------------------------------------
// Charge le status de l'application dans le buffer de sortie
// ---------------------------------------------------------------------------
void loadAppStatus(){

	lcdBufferOut[0]=0xAA;
	lcdBufferOut[1]=APP_STATUS;
	lcdBufferOut[2]=OctopodStatus.AutoMode;
	lcdBufferOut[3]=OctopodStatus.MotionRun;
	lcdBufferOut[4]=OctopodStatus.MotionSpeed;
	lcdBufferOut[5]=OctopodStatus.ScanZone;
	lcdBufferOut[6]=0xEE;
	sendLCDUartFrame(lcdBufferOut, 7);
};

// ---------------------------------------------------------------------------
// Charge le status de la carte Hardware bas niveau dans le buffer de sortie
// ---------------------------------------------------------------------------
void loadLLHwStatus(){
	lcdBufferOut[0]=0xAA;
	lcdBufferOut[1]=LLHW_STATUS;
	lcdBufferOut[2]=LLHwStatus.USonicReady;
	lcdBufferOut[3]=LLHwStatus.CompassReady;
	lcdBufferOut[4]=LLHwStatus.IR0State;
	lcdBufferOut[5]=LLHwStatus.IR1State;
	lcdBufferOut[6]=LLHwStatus.IR2State;
	lcdBufferOut[7]=0xEE;

	sendLCDUartFrame(lcdBufferOut, 8);
};

// ---------------------------------------------------------------------------
// Charge l'état des moteurs ON/OFF dans le buffer de sortie
// ---------------------------------------------------------------------------
void loadMotorState(){
	unsigned char i;

	lcdBufferOut[0]=0xAA;
	lcdBufferOut[1]=APP_MOT_STATE;
	for(i=1;i<=26;i++){
		lcdBufferOut[i]=MotorsStatus.State[i];
	}
		lcdBufferOut[28]=0xEE;
	sendLCDUartFrame(lcdBufferOut, 29);
};

// ---------------------------------------------------------------------------
// Charge l'état des moteurs ANGLE dans le buffer de sortie
// ---------------------------------------------------------------------------
void loadMotorAngle(){
	unsigned char i;

	lcdBufferOut[0]=0xAA;

	lcdBufferOut[1]=APP_MOT_ANGLE;
	for(i=1;i<=26;i++){
		lcdBufferOut[i]=MotorsStatus.Angle[i];
	}
	lcdBufferOut[28]=0xEE;

	sendLCDUartFrame(lcdBufferOut, 29);
};

// ---------------------------------------------------------------------------
// Charge les données boussole dans le buffer de sortie
// ---------------------------------------------------------------------------
void loadCompassMeas(){
	lcdBufferOut[0]=0xAA;
	lcdBufferOut[1]=COMPASS;

	lcdBufferOut[2]=SensorsStatus.CompassIsCalibrate;
	lcdBufferOut[3]=SensorsStatus.Heading&0xFF00>>8;
	lcdBufferOut[4]=SensorsStatus.Heading&0x00FF;

	lcdBufferOut[5]=0xEE;
	sendLCDUartFrame(lcdBufferOut, 6);
};

// ---------------------------------------------------------------------------
// Charge les données distance ultrasons dans le buffer de sortie
// ---------------------------------------------------------------------------
void loadUSonicMeas(){

	lcdBufferOut[0]=0xAA;
	lcdBufferOut[1]=USONIC;

	lcdBufferOut[2]=SensorsStatus.Distance&0xFF00>>8;
	lcdBufferOut[3]=SensorsStatus.Distance&0x00FF;
	lcdBufferOut[4]=SensorsStatus.DistanceMotorX;
	lcdBufferOut[5]=SensorsStatus.DistanceMotorY;

	lcdBufferOut[6]=0xEE;
	sendLCDUartFrame(lcdBufferOut, 7);
};

// ---------------------------------------------------------------------------
// CREATION THREAD LCD TASK
// ---------------------------------------------------------------------------
int createLcdTask(void)
{
	return (pthread_create (&th_lcd, NULL, lcdTask, NULL));
}

// ---------------------------------------------------------------------------
// DESTRUCTION THREAD LCD TASK
// ---------------------------------------------------------------------------
int killLcdTask(void){
	return (pthread_cancel(th_lcd));
}

