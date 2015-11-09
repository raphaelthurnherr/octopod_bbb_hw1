
#define ANGLE_SLOT	0
#define DIST_SLOT	1

#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

#include "th2_core.h"
#include "manager.h"

#include "th1_hwctrl.h"
#include "th7_motion.h"

#include "tools.h"

pthread_t th_core;

unsigned char remoteCommandReadyData=0;
unsigned char motionSpeed=3;
unsigned char motionLoop=0;
unsigned char AIenable=0;
unsigned char distanceAtAngle[90];
unsigned char i;

unsigned char SearchMyWay(unsigned char *myScan, unsigned char depth, unsigned char DisplayResult);

void processUICommand(void);

void UICommand(unsigned char Command);

void changeMotion (unsigned char motionType);


// ***************************************************************************
// ---------------------------------------------------------------------------
// THREAD HEXAPOD CORE: Gestion principale
// ---------------------------------------------------------------------------
// ***************************************************************************

void *coreTask (void * arg){
	printf ("# Demarrage tache CORE: OK\n");

	while(!killAllThread){

	  if(remoteCommandReadyData) processUICommand();

  usleep(5000);
  }

  printf( "# ARRET tache CORE\n");

  usleep(10000);

  killAllThread+=1;
  pthread_exit (0);
}

// ---------------------------------------------------------------------------
// CREATION THREAD UART
// ---------------------------------------------------------------------------
unsigned char SearchMyWay(unsigned char *myScan, unsigned char depth, unsigned char DisplayResult){
	unsigned char bestWay;
	unsigned char i;
	  ScanZone(0,90,depth,myScan);
	  makeMapZone(depth, myScan);
	  MapZoneFiltre1(depth);
	  MapZoneFiltre2(depth);

	  bestWay=calcBestAngle(depth);

	  if(DisplayResult){
//		  displayMapZone(depth,0);
		  displayMapZone(depth,2);
		  printf("      - ");
		  for(i=0;i<bestWay/depth;i++)printf(" ");
		  printf("| %d \n\n",bestWay);

	  }
	  setMotorAngle(18,45,1);	// Attribue un angle au servo moteur
	  return(bestWay);
}

void UICommand(unsigned char Command){
	remoteCommandReadyData=Command;
}

void processUICommand(void){

	unsigned char myCommand;
	unsigned char myValue;

	myCommand=remoteCommandReadyData&0x0F;
	myValue=(remoteCommandReadyData>>4)&0x0F;

	printf("MY PROCESS UI CMD: %d - %d", myCommand, myValue);
	usleep(10000);
	switch(myCommand){
		case TEST_SEARCH_SCAN : break;
		case TEST_SEARCH_BEST_WAY : SearchMyWay(distanceAtAngle, 1, 1); break;
		case TEST_BUZZER : buzzerCtrl(myValue); break;
		case TEST_LEGS : break;
		case REMOTE_RUN_AI : AIenable=1; break;
		case RUN_MOTION : 	AIenable=0;						// D�sactive l'intelligence artificielle.
							setMotorsInterrupt(1);			// Active les interruption info moteur en place
							usleep(100000);
							setMotion(myValue); break;
		case RUN_MOTION_LOOP : setMotionLoop(myValue); break;
		case RUN_SPEED : setMotionSpeed(myValue); break;
		case RUN_STOP : break;
		case CHANGEMOTION : changeMotion(myValue); break;
		case TEST_COMPASS : StartCompassMeasure(); break;

		case CALIB_COMPASS: compassIsCalibrate=0;
							CompassCalibrate();
							buzzerCtrl(2);		// Test beep
							break;

		case INT_COMPASS : 	SetCompassInterruptsRange(175, 185);
							usleep(10000);
							SetCompassInterruptEnable(1);
						break;

		case CALIB_HEXAPOD : hexapodCalibrateCompass(); break;				// Calibration automatique de la boussole (rotation sur place)

		default : break;
	}
	remoteCommandReadyData=0;
}

void changeMotion (unsigned char motionType){
	unsigned char i;
	static signed int correction[24];

// ----- MODIFICATION DE LA HAUTEUR !
	// MODIFIE LES OFFSET x2, x3 de chaque patte A, B, C, D, E, F pour modifier la hauteur
	if(motionType==MOTION_BODY_DW){
			// A23, B23, C23 +
			correction[1]=correction[2]=correction[4]=correction[5]=correction[7]=correction[8]+=5;
			// D23, E23, F23 - (moteur a l'envers mecaniquement)
			correction[10]=correction[11]=correction[13]=correction[14]=correction[16]=correction[17]-=5;
		}
	if(motionType==MOTION_BODY_UP){
			// A23, B23, C23 -
			correction[1]=correction[2]=correction[4]=correction[5]=correction[7]=correction[8]-=5;
			// D23, E23, F23 + (moteur a l'envers mecaniquement)
			correction[10]=correction[11]=correction[13]=correction[14]=correction[16]=correction[17]+=5;
	}
// ----- MODIFICATION DE L ASSIETTE AVANT ARRIERE
	// MODIFIE LES OFFSET x2, x3 de chaque patte A, B, C, D, E, F pour mofifier la hauteur avant
	if(motionType==MOTION_HEAD_DW){
		correction[1]=correction[2]+=5; // A23
//			correction[4]=correction[5]+=5; // B12
		correction[7]=correction[8]-=5; // C23

		// ATENTION, DEF mont�s � l'envers
		correction[10]=correction[11]+=5; // D23
//			correction[13]=correction[14]-=5; // E23
		correction[16]=correction[17]-=5; // F23
	}

	if(motionType==MOTION_HEAD_UP){
		correction[1]=correction[2]-=5; // A23
//			correction[4]=correction[5]+=5; // B23
		correction[7]=correction[8]+=5; // C23

		// ATENTION, DEF mont�s � l'envers
		correction[10]=correction[11]-=5; // D23
//			correction[13]=correction[14]-=5; // E23
		correction[16]=correction[17]+=5; // F23
		}

// ----- MODIFICATION DES EPAULES VERS AVANT, VERS ARRIERE
	// MODIFIE LES OFFSET x1de chaque patte A, B, C, D, E, F pour mofifier la rotation
	if(motionType==MOTION_BODY_BACK){
		// A1, B1, C1 +
		correction[0]=correction[3]=correction[6]+=5;
		// D1, E1, E1 - (moteur a l'envers mecaniquement)
		correction[9]=correction[12]=correction[15]-=5;
		}

	if(motionType==MOTION_BODY_FORWARD){
		// A12, B12, C12 +
		correction[0]=correction[3]=correction[6]-=5;
		// D1, E1, E1 - (moteur a l'envers mecaniquement)
		correction[9]=correction[12]=correction[15]+=5;
		}

// ----- MODIFICATION DES PATTES EXTERIEURS
	if(motionType==MOTION_HAND_OPEN){
		correction[2]=correction[5]=correction[8]-=5;
		// D1, E1, E1 - (moteur a l'envers mecaniquement)
		correction[11]=correction[14]=correction[17]+=5;
		}

	if(motionType==MOTION_HAND_CLOSE){
		correction[2]=correction[5]=correction[8]+=5;
		// D1, E1, E1 - (moteur a l'envers mecaniquement)
		correction[11]=correction[14]=correction[17]-=5;
		}

	if(motionType==MOTION_RESET){
		for (i=0;i<18;i++)
			correction[i]=0;
		}



	for (i=0;i<18;i++){
		if(correction[i]<=-90) correction[i]=-90;
		if(correction[i]>=90) correction[i]=90;

		motorLegsCorrection[i]=correction[i];
	}

}

// ---------------------------------------------------------------------------
// CREATION THREAD UART
// ---------------------------------------------------------------------------
int createCoreTask(void)
{
	return (pthread_create (&th_core, NULL, coreTask, NULL));
}

// ---------------------------------------------------------------------------
// DESTRUCTION THREAD UART
// ---------------------------------------------------------------------------
int killCoreTask(void){
	return (pthread_cancel(th_core));
}

