#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

#include "th5_ai.h"

#include "manager.h"
#include "th1_hwctrl.h"
pthread_t th_ai;

unsigned char USMeasureStarted;		// Flag nesure ultrasons actif/inactif
unsigned char CompassCalibStarted;	// Flag calibration boussole en cours

// ***************************************************************************
// ---------------------------------------------------------------------------
// THREAD HEXAPOD AI: Gestion INTELLIGEANCE ARTIFICIELLE
// ---------------------------------------------------------------------------
// ***************************************************************************

void *aiTask (void * arg)
{
	printf ("# Demarrage tache AI: OK\n");

  while(!killAllThread){
//	  pthread_mutex_lock (&my_mutex);

	  // D�marrage de la s�quence en autonomie
	  if(AIenable){
		  if(!compassIsCalibrate){
				  if(!CompassCalibStarted){
				  buzzerCtrl(2);			// Beeb de calibration
				  StopUltrasonicSensor(); 	// Stop la mesure ultrasons durant la calibration
				  USMeasureStarted=0;		// Flag mesure distance OFF (pour AI)
				  hexapodCalibrateCompass();		// D�marre la calibration boussole en mouvement
				  CompassCalibStarted=1;
				  StartCompassMeasure();
			  }

		  }
		  else{
			  	  CompassCalibStarted=0;
			  if (!USMeasureStarted){
				  StartUltrasonicMeasure();		// D�marre une mesure permanente de distance;
				  USMeasureStarted=1;			// Flag mesure distance ON (pour AI)
			  }
		  }
	  }


//	pthread_mutex_unlock (&my_mutex);
  usleep(10000);
  }

  printf( "# ARRET tache AI\n");

  usleep(10000);

  killAllThread+=1;
  pthread_exit (0);
}


// ---------------------------------------------------------------------------
// CREATION THREAD AI
// ---------------------------------------------------------------------------
int createAITask(void)
{
	return (pthread_create (&th_ai, NULL, aiTask, NULL));
}

// ---------------------------------------------------------------------------
// DESTRUCTION THREAD AI
// ---------------------------------------------------------------------------
int killAITask(void){
	return (pthread_cancel(th_ai));
}

