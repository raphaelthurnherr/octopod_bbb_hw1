#define TRUE	1
#define FALSE	0

#define TO_LCD	0
#define FROM_LCD 1

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
#include "th1_hwctrl.h"
#include "th2_core.h"

pthread_t th_lcd;

unsigned char lcdBufferOut[50];

// STATUS GENERAL DE L'APPLICATION
struct SysStatus LcdLB_SystemStatus;
struct ApplicationStatus LcdLB_OctopodStatus;
struct hwStatus LcdLB_LLHwStatus;
struct MotStatus LcdLB_MotorsStatus;
struct SensStatus LcdLB_SensorsStatus;
struct WayFinder LcdLB_MeasureMap;
struct EthernetInfo LcdLB_SystemLan;


void loadSysStatus();
void loadSysIpLan();
void loadSysIpWlan();
void loadAppStatus();
void loadLLHwStatus();
void loadMotorState();
void loadMotorAngle();
void loadCompassMeas();
void loadUSonicMeas();

void LcdReply(unsigned char CMD);
void ProcessLCDrequest(void);

// ***************************************************************************
// ---------------------------------------------------------------------------
// THREAD LCD: Gestion de l'interface utilisateur LCD
// ---------------------------------------------------------------------------
// ***************************************************************************

void *lcdTask (void * arg)
{
	RunningTask += TH8_SOA;
	printf ("# Demarrage tache HMI LCD: OK\n");

	 usleep(1000000);

	 LcdReply (SYS_STATUS);

	 while((!EndOfApp) | SystemStatus.AppStarted){
//	  pthread_mutex_lock (&my_mutex);

		// Controle si donnée recue de l'afficheur LCD
		 if(hwctrl_uartLCDDataInReady){
			 ProcessLCDrequest();
			 hwctrl_uartLCDDataInReady=0;
		 }
		// LcdReply (USONIC);

		 usleep(100000);
//	pthread_mutex_unlock (&my_mutex);
  }

  LcdReply(SYS_STATUS);						// Renvoie au LCD l'état de fermeture de l'application
  printf( "# ARRET tache HMI LCD\n");

  RunningTask -= TH8_SOA;
  pthread_exit (0);
}

// ---------------------------------------------------------------------------
// Traite la trame recu sur port série LCD
// ---------------------------------------------------------------------------

void ProcessLCDrequest(void){
	unsigned char RequestType;
	unsigned char RequestCommand;

	RequestType=myLCDdataIn[0];
	RequestCommand=myLCDdataIn[1];

	if(RequestType == 0){			// Requete de type STATUS...
		switch(RequestCommand){
			case SYS_STATUS : LcdReply (SYS_STATUS); break;
			case SYS_IP_LAN : LcdReply (SYS_IP_LAN);break;
			case SYS_IP_WLAN : LcdReply (SYS_IP_WLAN);break;
			case APP_STATUS : LcdReply (APP_STATUS);break;
			case LLHW_STATUS : LcdReply (LLHW_STATUS);break;
			case APP_MOT_STATE : LcdReply (APP_MOT_STATE);break;
			case APP_MOT_ANGLE : LcdReply (APP_MOT_ANGLE);break;
			case COMPASS : LcdReply (COMPASS);break;
			case USONIC : LcdReply (USONIC);break;
			default	:	break;
		}
	}
}

// ---------------------------------------------------------------------------
// LCD_SENDCMD Envoie de data défini à l'afficheur
// ---------------------------------------------------------------------------

void LcdReply(unsigned char CMD)
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
	for(i=0;i<=25;i++){
		lcdBufferOut[i+2]=MotorsStatus.State[i];
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
	for(i=0;i<=25;i++){
		lcdBufferOut[i+2]=MotorsStatus.Angle[i];
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
	lcdBufferOut[3]=((SensorsStatus.Heading)&0xFF00)>>8;
	lcdBufferOut[4]=(SensorsStatus.Heading)&0x00FF;

	lcdBufferOut[5]=0xEE;
	sendLCDUartFrame(lcdBufferOut, 6);
};

// ---------------------------------------------------------------------------
// Charge les données distance ultrasons dans le buffer de sortie
// ---------------------------------------------------------------------------
void loadUSonicMeas(){

	lcdBufferOut[0]=0xAA;
	lcdBufferOut[1]=USONIC;

	lcdBufferOut[2]=(SensorsStatus.Distance&0xFF00)>>8;
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

