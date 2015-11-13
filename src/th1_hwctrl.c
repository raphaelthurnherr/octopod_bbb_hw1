
// GPIO PIN MUX FOR BBB

#define GPIO_BATT_MOTOR	12
#define GPIO_BATT_BRAIN	15
#define GPIO_BUZZER	26
#define GPIO_ERR 1
#define NOERR 0

#define FRM_DATACOUNT_SLOT 1
#define FRM_COMMAND_SLOT 2

#define FRM_ANGLE_OFFSET 3
#define FRM_ANGLE_COUNT 4
#define FRM_ANGLE_DATA0 5

//#define FRM_ACK_MOTORS 0x10
//#define FRM_ACK_MOTORS_VALID 0xE0

#define FRM_USONIC_DATA_VALID 3
#define FRM_USONIC_DATA_MEASURE_H 4
#define FRM_USONIC_DATA_MEASURE_L 5

#define FRM_IR_DATA_VALID 3
#define FRM_IR0_DATAVALUE 3
#define FRM_IR1_DATAVALUE 4
#define FRM_IR2_DATAVALUE 5

#define FRM_COMPASS_DATA_VALID 3
#define FRM_COMPASS_DATA_MEASURE_H 4
#define FRM_COMPASS_DATA_MEASURE_L 5

// D�finition pour port s�rie
#define FRM_DATACOUNT_SLOT 1

#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

#include "th1_hwctrl.h"
#include "manager.h"

#include "serial.h"
#include "BBBiolib.h"

#include "th6_timers.h"



pthread_t th_hwctrl;


// initialisation des GPIO du system
void GpioSetup(void);

// VARIABLES PORTS SERIE
unsigned char hwctrl_uartAckDisplayStatus=0;

//Variable port série communication controleur bas niveau
unsigned char hwctrl_uartDataInReady=0;
unsigned char hwctrl_uartFrameIn[100];
unsigned char myBufferIn[100];

//Variable port série communication afficheur LCD
unsigned char hwctrl_uartLCDDataInReady=0;
unsigned char myLCDdataIn[50];

unsigned char smoothValue=0;
unsigned char motorsPositionValid;
// Demarre une commande heartbit pour connaitre l'�tat du controller;
void getControllerHeartBit(void);

// Affichage de la trame recue d�cod�e ou non
void HWctrl_displayAckToggle(unsigned char ackType);

// Retourne La distance � un angle donn�
void getUltrasonicDistance(unsigned char angle);

// Envoie un commande de mesure de distance
void StartUltrasonicMeasure(void);
void ReadStartUltrasonicSensor(void); // Start l'quisition de distance par le controleur
void StopUltrasonicSensor(void); // Stop l'quisition de distance par le controleur

// Envoie un commande de mesure d'angle d'orientation
void StartCompassMeasure(void);
void ReadStartCompassSensor(void); // Start l'quisition de distance par le controleur
void StopCompassSensor(void); 		// Stop l'quisition de distance par le controleur
void SetCompassInterruptsRange(unsigned int angleMin, unsigned int angleMax);
void SetCompassInterruptEnable(unsigned char OnOff);
// Valeur de distance lue [0] � l'angle [1]
unsigned int ultrasonicDistance[2]={0,0};

// Anlge d'orientation actuel
unsigned int compassAngle;
//
unsigned char USonicSensorBusy=0;
unsigned char CompassSensorBusy=0;

// Angles actuelle des Moteurs (Apr�s quittance du controlleur)
unsigned char motorsActualAngle[NB_TOTAL_OF_MOTORS];
// Etat actuelle des Moteurs (Apr�s quittance du controlleur)
unsigned char motorsActualState[NB_TOTAL_OF_MOTORS];

// Etat de la connexion du controleur bas niveau
unsigned char controllerConnected=0;
unsigned char controllerUSonicConnected=0;
unsigned char controllerCompassConnected=0;

unsigned char BuzzerFreq_mS;

unsigned char waitForUltrasonicMotorMove=0;
unsigned char th1_LowBatteryCheck(void);	// Contr�le etat des batteries

void buzzerCtrl(unsigned char bipCount);
void buzzerConfig(unsigned char On, unsigned int t1, unsigned int t2, unsigned char tHF);

void setMotorAngle(unsigned char motor, unsigned char angle, unsigned char speed);	// Attribue un angle au servo moteur
void setMotorState(unsigned char motor, unsigned char state); 						// d�fini l'�tat ON/OFF du moteur
void setMotorsInterrupt(unsigned char Enable); // Active/d�sactive l'interruption lorsques les moteur sont en position
void sendUartFrame(unsigned char *buffToSend, unsigned char nbByte);		// Envoie d'une trame vers le contrôleur bas niveau


void sendLCDUartFrame(unsigned char *buffToSend, unsigned char nbByte); // Envoie d'une trame vers l'afficheur LCD


void checkUartData(void);			// Attente d'une trame UART valide recue du contrôleur bas niveau
void checkUartLCDData(void);		// Attente d'une trame UART recu du LCD

void processDataUart(void);			// Traitement de la trame serie recue

// Effectue  la mise a jour des angles et Etats des moteurs selon ack recu
void updateMotorAngle(void);
void updateMotorState(void);
void updateUltrasonicDistance(void);

// Mise � jour de l'angle d'orientation
void updateCompassAngle(void);
unsigned char compassIsCalibrate;

// Calibration de la boussole
void CompassCalibrate(void);

// active les interruptions des capteurs infrarouge
void SetIRinterrupts(unsigned char Enable);
void updateIRstate(void);
unsigned char IRdetectValid;
unsigned char IRdetectValue[3]={255, 255, 255};


// ***************************************************************************
// ---------------------------------------------------------------------------
// THREAD GESTION UARTIRdetectValid
// ---------------------------------------------------------------------------
// ***************************************************************************

void *hwctrlTask (void * arg)
{
	printf ("# Demarrage tache HW CTRL: OK\n");

	// OUVERTURE PORT COM POUR COMMUNICATION CONTROLEUR BAS NIVEAU
	serialSetup();

	// OUVERTURE PORT COM POUR LCD
	serial1Setup();


	// Initialisation GPIO du syst�me
	GpioSetup();

	// Initialisation du buzzer
	buzzerConfig(1,300,100,1);		// Buzzer ON, 300ms, 100ms, 1khz (1mS)

	usleep(10000);


	//SetIRinterrupts(1);

	buzzerCtrl(0);

	while(!killAllThread){
//	  pthread_mutex_lock (&my_mutex);



	  // Trame UART recue du controleur bas niveau ?
	  checkUartData();

	  // Trame UART recue de l' afficheur LCD
	  checkUartLCDData();

	  if(hwctrl_uartDataInReady)	processDataUart(); // Traitement de la trame recue


	  // Fin de d�placement du servomoteur EYES, D�marre la mesure
	  if(th6_timerMotorEyesMoveReadyFlag){
		  //if(!USonicSensorBusy)StartUltrasonicMeasure();
		  StartUltrasonicMeasure();
	  }

	  if(th6_timerCompassReadyFlag){

		  ReadStartCompassSensor();
		  th6_timerCompassReadyFlag=0;
	  }



	  usleep(100);
//	pthread_mutex_unlock (&my_mutex);
  }


  pin_low(8,GPIO_BUZZER);								// D�sactive gpio buzzer
  iolib_free();											// Lib�re l'acc�s aux gpios

  printf( "# ARRET tache HW CTRL\n");

  usleep(10000);

  killAllThread+=1;
  pthread_exit (0);
}


// ----------------------------------------------
void getControllerHeartBit(void){
	unsigned char CmdGetHearthBit[3]={170,1,200};

	// ENVOIE LES COMMANDES DE DEMANDE DE DISTANCE AU CONTROLEUR
		while(flag_uartOutBusy);
		flag_uartOutBusy=1;
		serialWrite(CmdGetHearthBit, 3);
		flag_uartOutBusy=0;
}

// Retourne La distance � un angle donn�, moteur vitesse max !
void getUltrasonicDistance(unsigned char angle){
		setMotorAngle(EYES_MOTOR_X,angle, 1);
		th6_timerMotorEyesMoveStart(90);
}

// ----------------------------------------------
// Envoie la commande pour aquisition de la distance
// ----------------------------------------------
void StartUltrasonicMeasure(void){
	USonicSensorBusy=1;
		th6_timerMotorEyesMoveStop();
		ReadStartUltrasonicSensor();
		th6_timerUSonicSampleStart(100);		// Activation du timer echantillonage ultrasonic mesure 100ms
}

// ----------------------------------------------
// Envoie la commande pour Stopper la mesure de distance par le capteur
// ----------------------------------------------
void StopUltrasonicSensor(void){
	unsigned char CmdStopDistanceMeas[3]={170,1,11};
		USonicSensorBusy=0;
	// ENVOIE LES COMMANDES DE DEMANDE DE DISTANCE AU CONTROLEUR
		while(flag_uartOutBusy);
		flag_uartOutBusy=1;
		serialWrite(CmdStopDistanceMeas, 3);
		flag_uartOutBusy=0;
}

// ----------------------------------------------
// Envoie la commande pour Starter la mesure de distance
// ----------------------------------------------
void ReadStartUltrasonicSensor(void){
	unsigned char CmdStartUsonicMeas[3]={170,1,10};

	// ENVOIE LES COMMANDES DE DEMANDE DE DISTANCE AU CONTROLEUR
		while(flag_uartOutBusy);
		flag_uartOutBusy=1;
		serialWrite(CmdStartUsonicMeas, 3);
		flag_uartOutBusy=0;
}


// ----------------------------------------------
// Envoie la commande pour aquisition de l'angle d'orientation
// ----------------------------------------------
void StartCompassMeasure(void){
	th6_timerCompassStart(300);		// Activation du timer 80ms
}

// ----------------------------------------------
// Envoie la commande pour Stopper la mesure d'angle d'orientation
// ----------------------------------------------
void StopCompassSensor(void){
	unsigned char CmdStopCompassMeas[3]={170,1,21};
		CompassSensorBusy=0;
	// ENVOIE LES COMMANDES DE DEMANDE DE DISTANCE AU CONTROLEUR
		while(flag_uartOutBusy);
		flag_uartOutBusy=1;
		serialWrite(CmdStopCompassMeas, 3);
		flag_uartOutBusy=0;
}

// ----------------------------------------------
// Envoie la commande pour Starter la mesure d'angle d'orientation
// ----------------------------------------------
void ReadStartCompassSensor(void){
	unsigned char CmdStartCompassMeas[3]={170,1,20};
	// ENVOIE LES COMMANDES DE DEMANDE DE DISTANCE AU CONTROLEUR
		while(flag_uartOutBusy);
		flag_uartOutBusy=1;
		serialWrite(CmdStartCompassMeas, 3);
		flag_uartOutBusy=0;
}

// ----------------------------------------------
// Envoie la commande pour lancer la calibration de la boussole
// ----------------------------------------------
void CompassCalibrate(void){
	unsigned char CmdCalibrateCompasse[3]={170,1,25};

	// ENVOIE LES COMMANDES DE DEMANDE DE DISTANCE AU CONTROLEUR
		while(flag_uartOutBusy);
		flag_uartOutBusy=1;
		serialWrite(CmdCalibrateCompasse, 3);
		flag_uartOutBusy=0;
}


// ---------------------------------------------------------------------------
// Contr�le de la reception d'une trame valide recue du controleur bas niveau
// ---------------------------------------------------------------------------
void checkUartData(void){
	unsigned char i;

	if(serialRead(myBufferIn)){
		// COPIE LA TRAME D'ENTREE DANS LA TRAME PARTAGEE SI LIBRE (Core_uartDataInReady)
		if(hwctrl_uartDataInReady==0){
			for(i=0;i<myBufferIn[FRM_DATACOUNT_SLOT]+2;i++){
				hwctrl_uartFrameIn[i]=myBufferIn[i];
			}
			hwctrl_uartDataInReady=1;
		}
	}
}


// ***************************************************************************
// ---------------------------------------------------------------------------
// TRAITEMENT DES DONNEES UART RECUE
// ---------------------------------------------------------------------------
// ***************************************************************************
void processDataUart(void){
int i;
unsigned char CommandReadback;

	// AFFICHE LA QUITTANCE RECU SUR UART
	  if(hwctrl_uartAckDisplayStatus&UART_ACK_ALL_UNDECODED){
			printf("my serial data read: " );
			for(i=0;i<hwctrl_uartFrameIn[FRM_DATACOUNT_SLOT]+2;i++){
				printf(" %d",hwctrl_uartFrameIn[i]);
			}
			printf("\n");
	  }

		CommandReadback=hwctrl_uartFrameIn[FRM_COMMAND_SLOT];

	  // TRAITENENT DE LA DONNE
	 switch(CommandReadback){

// READ BACK ULTRASON APRES ENVOIE DE COMMANDES
 	 	 case 0	:	updateMotorState();						// Echo commande moteur (ON/OFF)
 	 	 	 	 	motorsPositionValid=1;

					break;
	 	 case 1	:	updateMotorAngle();						// Echo commande positionnement moteur pleine vitesse(0..90 degr�s)
	 	 	 	 	motorsPositionValid=1;
					break;
	 	 case 2	:											// Echo commande positionnement moteur vitesses r�duite(0..90 degr�s)
					break;

	 	 case 9	:	motorsPositionValid=1;					// Echo commande interruption moteurs ON/OFF
					break;

// READ BACK ULTRASON APRES ENVOIE DE COMMANDES
	 	 case 10:	updateUltrasonicDistance();				// Echo demande de lecture du capteur ultrason
					break;									// +Datas: validit� de la mesure(8bit bool), mesure en mm (16bit)
		 case 11:											// Echo stop mesure sur capteur ultrasons
					break;

// READ BACK BOUSSOLE APRES ENVOIE DE COMMANDES
	 	 case 20:	updateCompassAngle();					// Echo demande de lecture du capteur ultrason
					break;									// +Datas: validit� de la mesure(8bit bool), mesure en degrees avec une d�cimale (16bit)
		 case 21:											// Echo stop mesure sur boussole
					break;
		 case 22:											// Echo commande interruption boussole ON/OFF
					break;
		 case 23:											// Echo commande param�trage interruptions dans une plage donn�e Min(16bit)+Max(16bit)
					break;
	 	 case 25:											// Echo de commande de d�marrage calibration du capteur
					break;									// (Fonctionne en tournant � 360deg sur axes XYZ l'appareil)

// READ BACK INFRAROUGES APRES ENVOIE DE COMMANDES
		 case 30:											// Echo demande de lecture des capteur infrarouge (ON/OFF)
					break;									// +Datas: bool sur 3x8 bit (capteur1, capteur2, capteur3)
		 case 32:											// Echo commande interruption Infrarouge ON/OFF (sur changement d'�tat)
					break;

// INTERRUPTIONS READ BACK (MOTEURS, ULTRASONS, BOUSSOLE, INFRAROUGES)
// RECUE SI INTERRUPTIONS ACTIVEES
	 	 case 100:	motorsPositionValid=1;					// Interruption moteurs 0..17 en place + valeurs positions moteurs
					break;
		 case 101:											// Interruption moteur 18(eyes) en place + valeur position moteur
					break;
		 case 110:											// Interruption capteur ultrasons + valeur de mesure
					break;
		 case 120:	updateCompassAngle();					// Interruption boussole + valeur de mesure
					break;
		 case 130:	updateIRstate();						// Interruption capteurs IR + �tats
					break;


// READ BACK HEARTBIT (Version firmware du controleur au format ascii)
		 case 200	:	if(hwctrl_uartAckDisplayStatus&UART_ACK_HEARTBIT){
							printf("Heart bit: ");
							for(i=3;i<hwctrl_uartFrameIn[1]+1;i++){
								printf("%c", hwctrl_uartFrameIn[i]);
							}
							printf("\n");
						}
						controllerConnected=1;
						break;

		 default	:	break;
		 }
	 	 hwctrl_uartDataInReady=0;
}

// ---------------------------------------------------------------------------
// Effectue  la mise a jour des angles des moteurs selon ack recu
// ---------------------------------------------------------------------------

void updateMotorAngle(void){
	int i;

	for(i=0; i<hwctrl_uartFrameIn[FRM_ANGLE_COUNT];i++){
		motorsActualAngle[hwctrl_uartFrameIn[FRM_ANGLE_OFFSET]+i]=hwctrl_uartFrameIn[FRM_ANGLE_DATA0+i];
	}

	// AFFICHAGE CONSOLE DU ACK
	if(hwctrl_uartAckDisplayStatus&UART_ACK_MOTORS){
		if(hwctrl_uartFrameIn[FRM_ANGLE_OFFSET])
			printf("Motor offset: %d / Count: %d / Angle(s): ",hwctrl_uartFrameIn[FRM_ANGLE_OFFSET],hwctrl_uartFrameIn[FRM_ANGLE_COUNT]);
		for(i=0; i<hwctrl_uartFrameIn[FRM_ANGLE_COUNT];i++){
			printf("%d ",hwctrl_uartFrameIn[FRM_ANGLE_DATA0+i]);
		}
		printf("\n");
	 }
}

// ---------------------------------------------------------------------------
// Effectue  la mise a jour des etat des moteurs selon ack recu
// ---------------------------------------------------------------------------

void updateMotorState(void){
	int i;

	for(i=0; i<hwctrl_uartFrameIn[FRM_ANGLE_COUNT];i++){
		// Mise � jour des angles moteur
		motorsActualState[hwctrl_uartFrameIn[FRM_ANGLE_OFFSET]+i]=hwctrl_uartFrameIn[FRM_ANGLE_DATA0+i];
	}

	// AFFICHAGE CONSOLE DU ACK
	if(hwctrl_uartDataInReady&& (hwctrl_uartAckDisplayStatus&UART_ACK_MOTORS)){
		if(hwctrl_uartFrameIn[FRM_ANGLE_OFFSET])
			printf("Motor offset: %d / Count: %d / State(s): ",hwctrl_uartFrameIn[FRM_ANGLE_OFFSET],hwctrl_uartFrameIn[FRM_ANGLE_COUNT]);
		for(i=0; i<hwctrl_uartFrameIn[FRM_ANGLE_COUNT];i++){
			printf("%d ",hwctrl_uartFrameIn[FRM_ANGLE_DATA0+i]);
		}
		printf("\n");
	 }
}

// ---------------------------------------------------------------------------
// Effectue  la mise a jour de la distance utltrason selon ack recu
// ---------------------------------------------------------------------------

void updateUltrasonicDistance(void){ //Data non valide

	if(hwctrl_uartFrameIn[FRM_USONIC_DATA_VALID]){
		 ultrasonicDistance[0]=hwctrl_uartFrameIn[FRM_USONIC_DATA_MEASURE_L]+(hwctrl_uartFrameIn[FRM_USONIC_DATA_MEASURE_H]*256);
		 ultrasonicDistance[1]=motorsActualAngle[EYES_MOTOR_X];
			if(hwctrl_uartDataInReady&&(hwctrl_uartAckDisplayStatus&UART_ACK_ULTRASONIC)){
				printf("Distance ultrasons a %d [deg]: %.1f [cm]\n",ultrasonicDistance[1],(float)ultrasonicDistance[0]/10);
			}
			controllerUSonicConnected=1;			// Controller considéré comme disponible
	}
	else //Data non valide
		if(hwctrl_uartDataInReady&&(hwctrl_uartAckDisplayStatus&UART_ACK_ULTRASONIC)){
			printf("Distance ultrasons a %d [deg]: INVALID\n",ultrasonicDistance[1]);
			controllerUSonicConnected=0;			// Controller considéré comme indisponible
		}

	USonicSensorBusy=0;
	th6_timerUSonicSampleStop();
}


// ---------------------------------------------------------------------------
// Effectue  la mise a jour de l'angle d'orientation selon AK recu
// ---------------------------------------------------------------------------

void updateCompassAngle(void){ //Data non valide

	if(hwctrl_uartFrameIn[FRM_COMPASS_DATA_VALID]){
		compassAngle=hwctrl_uartFrameIn[FRM_COMPASS_DATA_MEASURE_L]+(hwctrl_uartFrameIn[FRM_COMPASS_DATA_MEASURE_H]*256);
			if(hwctrl_uartDataInReady&&(hwctrl_uartAckDisplayStatus&UART_ACK_COMPASS)){
				printf("Angle d'orientation [deg]: %.1f \n",(float)compassAngle/10);
			}
			compassIsCalibrate=1;
			controllerCompassConnected=1;		// Boussole considérée comme disponible
	}
	else{ 		//Data non valide
		if(hwctrl_uartDataInReady&&(hwctrl_uartAckDisplayStatus&UART_ACK_COMPASS)){
			printf("Angle d'orientation [deg]: a %.2f INVALID\n",(float)compassAngle/10);
			compassIsCalibrate=0;
			controllerCompassConnected=1;		// Boussole considérée comme non disponible
		}
	}

	CompassSensorBusy=0;
}

// ---------------------------------------------------------------------------
// INITIALISATION DES ENTREES/SORTIES DU SYSTEM
// ---------------------------------------------------------------------------
void GpioSetup(void){
	iolib_init();	// Ouverture des GPIO

	iolib_setdir(8,GPIO_BATT_MOTOR, BBBIO_DIR_IN);		// P8.x en entr�e pour warning batterie
	iolib_setdir(8,GPIO_BATT_BRAIN, BBBIO_DIR_IN);		// P8.x en entr�e pour warning batterie
	iolib_setdir(8,GPIO_BUZZER, BBBIO_DIR_OUT);			// P8.x en sortie pour buzzer
	pin_low(8,GPIO_BUZZER);								// D�sactive gpio buzzer
}

// ---------------------------------------------------------------------------
// RETOURNE WARNING BATTERIE (0:no warning, 1:batt A low, 2:batt B low, 3, Both Low)
// ---------------------------------------------------------------------------
unsigned char th1_LowBatteryCheck(void){
	unsigned char batteryState=0;

  // CONTROLE DE L'ETAT DES BATTERIES
	 if(is_high(8,GPIO_BATT_BRAIN)){
		 batteryState|=0x01;
	 }else  batteryState&=0xFE;

	 if(is_high(8,GPIO_BATT_MOTOR)){
		 batteryState|=0x02;
	 } else batteryState&=0xFD;
	 return(batteryState);
}

// ---------------------------------------------------------------------------
// RETOURNE WARNING BATTERIE (0:no warning, 1:batt A low, 2:batt B low, 3, Both Low)
// ---------------------------------------------------------------------------
void buzzerCtrl(unsigned char bipCount){
	static unsigned char BuzzOn=0;
	static unsigned int nbBipToDo=0;

	if(nbBipToDo>0){
		  if(!th6_timerBuzzDutyHighReadyFlag){
			  if(th6_timerSynFreqReadyFlag){
				  BuzzOn=~BuzzOn;
				  if(BuzzOn)pin_low(8,GPIO_BUZZER);
				  else pin_high(8,GPIO_BUZZER);
				  th6_timerSynFreqReadyFlag=0;
			  }
		  }else{

			  th6_timerSynFreqStop();
			  pin_low(8,GPIO_BUZZER);
		  }
		  if(th6_timerBuzzDutyLowReadyFlag){
			  th6_timerSynFreqStart(BuzzerFreq_mS);						// Horloge 1khz
			  th6_timerBuzzDutyLowReadyFlag=0;
			  th6_timerBuzzDutyHighReadyFlag=0;
			  if(nbBipToDo>0)nbBipToDo--;
		  }
	 }else
	 {
		 // reset compteur
		 th6_timerBuzzDutyLowReadyFlag=0;
		 th6_timerBuzzDutyHighReadyFlag=0;
		 th6_timerSynFreqStop();
	 }
	if(bipCount>0){
		nbBipToDo=bipCount;				// Bip demand� par utilisateur
	}
}

// ---------------------------------------------------------------------------
// Configuration BUzzer
// ON/OFF, timeON [ms], timeOFF [ms], Frequence BIP [ms]
// ---------------------------------------------------------------------------

void buzzerConfig(unsigned char On, unsigned int t1, unsigned int t2, unsigned char tHF){

	if(On){
		BuzzerFreq_mS=tHF;
		th6_timerSynFreqStart(BuzzerFreq_mS);						// Horloge 1khz
		th6_timerBuzzDutyStart(t1, t2);					// 1Hz, dutycycle 50%
	}else th6_timerSynFreqStop();						// Horloge 1khz
}

// ----------------------------------------------
// D�finit un angle sur un servomoteur
// ----------------------------------------------
void setMotorAngle(unsigned char motor, unsigned char angle, unsigned char speed){

	if(speed<1) speed=1;
	if(speed>8) speed=8;

	unsigned char BufferOut[6]={170,4,speed,motor,1,angle};

	while(flag_uartOutBusy);
		flag_uartOutBusy=1;
		serialWrite(BufferOut, 6);
		flag_uartOutBusy=0;
		usleep(100000);	// Sleep pour vitesse MAX Mecanique. (sous-evalu�...)
}


// ----------------------------------------------
// D�finit l'�tat du servomoteur
// ----------------------------------------------
void setMotorState(unsigned char motor, unsigned char state){

	while(flag_uartOutBusy);

	flag_uartOutBusy=1;
	unsigned char BufferOut[6]={170,4,0,motor,1,state};
	serialWrite(BufferOut, 6);
	flag_uartOutBusy=0;
}


// ----------------------------------------------
// Active/d�sactive l'interruption lorsques les moteur sont en position
// ----------------------------------------------
void setMotorsInterrupt(unsigned char Enable){
	while(flag_uartOutBusy);
	flag_uartOutBusy=1;
	unsigned char BufferOut[4]={170,2,9,Enable};
	serialWrite(BufferOut, 4);
	flag_uartOutBusy=0;

	motorsPositionValid=1;
}


// ----------------------------------------------
// Active/d�sactive les interruptions d'evenement de capteurs IR
// ----------------------------------------------
void SetIRinterrupts(unsigned char Enable){
	while(flag_uartOutBusy);
	flag_uartOutBusy=1;
	unsigned char BufferOut[6]={170,4,32,Enable,Enable,Enable};
	serialWrite(BufferOut, 6);
	flag_uartOutBusy=0;
}


void SetCompassInterruptEnable(unsigned char OnOff){
	unsigned char CompassIntEnableCmd[4]={170,2,22,OnOff};

	while(flag_uartOutBusy);
	flag_uartOutBusy=1;
	serialWrite(CompassIntEnableCmd, 4);
	flag_uartOutBusy=0;
}


void SetCompassInterruptsRange(unsigned int angleMin, unsigned int angleMax){

	unsigned char CompassIntSetCmd[7]={170,5,23,0,0,0,0};

	unsigned char MinLow, MinHigh, MaxLow, MaxHigh;
	// Converti en dixi�me de degr�s
	angleMin*=10;
	angleMax*=10;

	MinLow=angleMin&0x00FF;
	MinHigh=(angleMin&0xFF00) >> 8;

	MaxLow=angleMax&0x00FF;
	MaxHigh=(angleMax&0xFF00) >> 8;

	CompassIntSetCmd[3]=MinHigh;
	CompassIntSetCmd[4]=MinLow;
	CompassIntSetCmd[5]=MaxHigh;
	CompassIntSetCmd[6]=MaxLow;

	while(flag_uartOutBusy);
	flag_uartOutBusy=1;
	serialWrite(CompassIntSetCmd, 7);
	flag_uartOutBusy=0;



}


// ----------------------------------------------
// mise a jour des valeurs capteurs
// ----------------------------------------------

void updateIRstate(void){

	//if(hwctrl_uartFrameIn[FRM_IR_DATA_VALID]){
		IRdetectValue[0]=hwctrl_uartFrameIn[FRM_IR0_DATAVALUE];
		IRdetectValue[1]=hwctrl_uartFrameIn[FRM_IR1_DATAVALUE];
		IRdetectValue[2]=hwctrl_uartFrameIn[FRM_IR2_DATAVALUE];
			if(hwctrl_uartDataInReady&&(hwctrl_uartAckDisplayStatus&UART_ACK_IR)){
				printf("Interruption capteurs: IR0 %d IR1 %d IR2 %d \n",IRdetectValue[0],IRdetectValue[1],IRdetectValue[2]);
			}
			IRdetectValid=1;
}

// ----------------------------------------------
// Envoie une trame compl�te pr�d�finie
// ----------------------------------------------
void sendUartFrame(unsigned char *buffToSend, unsigned char nbByte){

	while(flag_uartOutBusy);
		flag_uartOutBusy=1;
	serialWrite(buffToSend, nbByte);
	flag_uartOutBusy=0;
}


// Affichage de la trame recue d�cod�e ou non
void HWctrl_displayAckToggle(unsigned char ackType){
	if(hwctrl_uartAckDisplayStatus&ackType)hwctrl_uartAckDisplayStatus&=0xFF-ackType;
	else hwctrl_uartAckDisplayStatus|=ackType;
}


// *********************************************************ethernetCheck();
// FONCTIONS DE COMMUNICATION AVEC AFICHEUR LCD
// *********************************************************


// ---------------------------------------------------------------------------
// Contr�le de la reception d'une trame valide recue de l'afficheur LCD
// ---------------------------------------------------------------------------
void checkUartLCDData(void){

	if(serial1Read(myLCDdataIn)){
			hwctrl_uartLCDDataInReady=1;
	}
}

// ----------------------------------------------
// sendLCDUartFrame
// ----------------------------------------------
void sendLCDUartFrame(unsigned char *buffToSend, unsigned char nbByte){

	while(flag_uart1OutBusy);
		flag_uart1OutBusy=1;
	serial1Write(buffToSend, nbByte);
	flag_uart1OutBusy=0;
}


// ---------------------------------------------------------------------------
// CREATION THREAD UART
// ---------------------------------------------------------------------------
int createHwctrlTask(void)
{
	return (pthread_create (&th_hwctrl, NULL, hwctrlTask, NULL));
}

// ---------------------------------------------------------------------------
// DESTRUCTION THREAD UART
// ---------------------------------------------------------------------------
int killHwctrlTask(void){
	return (pthread_cancel(th_hwctrl));
}

