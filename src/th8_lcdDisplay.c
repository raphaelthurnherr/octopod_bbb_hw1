#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

#include "th8_lcdDisplay.h"

#include "manager.h"
#include "th2_core.h"
#include "th1_hwctrl.h"

pthread_t th_lcd;


// ***************************************************************************
// ---------------------------------------------------------------------------
// THREAD LCD: Gestion de l'interface utilisateur LCD
// ---------------------------------------------------------------------------
// ***************************************************************************

void *lcdTask (void * arg)
{
	printf ("# Demarrage tache HMI LCD: OK\n");


	//SystemStatus.AppStarted

	 usleep(1000000);
  while(!killAllThread){
//	  pthread_mutex_lock (&my_mutex);



	  if(hwctrl_uartLCDDataInReady)hwctrl_uartLCDDataInReady=0;

//	pthread_mutex_unlock (&my_mutex);
  usleep(5000000);
  }

  printf( "# ARRET tache HMI LCD\n");

  usleep(10000);

  killAllThread+=1;
  pthread_exit (0);
}





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

