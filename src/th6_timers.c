#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

#include "th6_timers.h"

#include "manager.h"

pthread_t th_timers;

// Timers Synth�tiseur de frequence resolution 250uS
unsigned int 	tmr_synFreqTarget=1;
unsigned int 	tmr_synFreqCnt=0;
unsigned char 	ultraSonPeriodic=0;
unsigned char 	th6_timerSynFreqReadyFlag=1;
unsigned char 	tmr_synFreqRunFlag=0;
// Timers synt�tiseur de fr�quence
void th6_timerSynFreqStart(unsigned int xmS);
void th6_timerSynFreqStop(void);


// Timers 30sec pour gestionnaire de tache
void th6_timer30sManagerStart(unsigned int xmS);
void th6_timer30sManagerStop(void);
unsigned char th6_timer30sManagerReadyFlag=1;
unsigned int 	tmr_30sManagerTarget=1;
unsigned int 	tmr_30sManagerCnt=0;
unsigned char 	tmr_30sManagerRunFlag=0;

// Timers 100ms Pour mesure de distance ultrasons
void th6_timerUSonicSampleStart(unsigned int xmS);
void th6_timerUSonicSampleStop(void);
unsigned char th6_timerUSonicSampleReadyFlag=1;
unsigned int 	tmr_USonicSampleTarget=1;
unsigned int 	tmr_USonicSampleCnt=0;
unsigned char 	tmr_USonicSampleRunFlag=0;

// Timers 300ms Pour mesure de d'angle d'orientation
void th6_timerCompassStart(unsigned int xmS);
void th6_timerCompassStop(void);
unsigned char th6_timerCompassReadyFlag=0;
unsigned int 	tmr_CompassTarget=1;
unsigned int 	tmr_CompassCnt=0;
unsigned char 	tmr_CompassRunFlag=0;

// Timers 20ms pour attente de d�placement sevomoteur
void th6_timerMotorEyesMoveStart(unsigned int xmS);
void th6_timerMotorEyesMoveStop(void);
unsigned char th6_timerMotorEyesMoveReadyFlag=0;
unsigned int 	tmr_MotorEyesMoveTarget=1;
unsigned int 	tmr_MotorEyesMoveCnt=0;
unsigned char 	tmr_MotorEyesMoveRunFlag=0;

// Timers pour interval d'envoie des mouvements
void th6_timerMotionIntervalInit(unsigned int xmS);
void th6_timerMotionIntervalStop(void);
void th6_timerMotionIntervalStart(void);

unsigned char th6_timerMotionIntervalReadyFlag=1;
unsigned int 	tmr_MotionIntervalTarget=300;
unsigned int 	tmr_MotionIntervalCnt=0;
unsigned char 	tmr_MotionIntervalRunFlag=0;

// Timers pour PWM buzzer (dutycycle)
int tmr_BuzzDutyHighTarget = 1000;
int tmr_BuzzDutyLowTarget = 1000;
int tmr_BuzzDutyCnt=0;
unsigned char tmr_BuzzDutyRunFlag;
unsigned char th6_timerBuzzDutyHighReadyFlag;
unsigned char th6_timerBuzzDutyLowReadyFlag;
void th6_timerBuzzDutyStart(unsigned int mS_high,unsigned int mS_low);
void th6_timerBuzzDutyStop(void);

// ***************************************************************************
// ---------------------------------------------------------------------------
// THREAD HEXAPOD TIMERS: Divers timers pour les taches adjacentes
// ---------------------------------------------------------------------------
// ***************************************************************************

void *timersTask (void * arg)
{
	printf ("# Demarrage tache TIMERS: OK\n");

  while(!killAllThread){
//	  pthread_mutex_lock (&my_mutex);

	 // Evaluation timer synth�tiseur de fr�quence
	 if(tmr_synFreqCnt>=tmr_synFreqTarget){
		 th6_timerSynFreqReadyFlag=1;
		 tmr_synFreqCnt=0;
	 }
	 if(tmr_synFreqRunFlag)tmr_synFreqCnt++;

	 // Evaluation timer pour g�n�ration dutycycle HIGH
	 if(tmr_BuzzDutyCnt>=tmr_BuzzDutyHighTarget){
		 th6_timerBuzzDutyHighReadyFlag=1;
	 }
	 // Evaluation timer pour g�n�ration dutycycle LOW
	 if(tmr_BuzzDutyCnt>=tmr_BuzzDutyLowTarget){
		 th6_timerBuzzDutyLowReadyFlag=1;
		 tmr_BuzzDutyCnt=0;
	 }
	 if(tmr_BuzzDutyRunFlag)tmr_BuzzDutyCnt++;

	 // Evaluation timer 30sec pour gestionnaire de tache
	 if(tmr_30sManagerCnt>=tmr_30sManagerTarget){
		 th6_timer30sManagerReadyFlag=1;
		 tmr_30sManagerCnt=0;
	 }
	 if(tmr_30sManagerRunFlag)tmr_30sManagerCnt++;

	 // Timers 100ms Pour mesure de distance ultrasons
	 if(tmr_USonicSampleCnt>=tmr_USonicSampleTarget){
		 th6_timerUSonicSampleReadyFlag=1;
		 tmr_USonicSampleCnt=0;
	 }
	 if(tmr_USonicSampleRunFlag)tmr_USonicSampleCnt++;

	 // Timers 300ms Pour mesure de l'angle d'orientation
	 if(tmr_CompassCnt>=tmr_CompassTarget){
		 th6_timerCompassReadyFlag=1;
		 tmr_CompassCnt=0;
	 }
	 if(tmr_CompassRunFlag)tmr_CompassCnt++;

	 // Timers 20ms pour attente de d�placement sevomoteur
	 if(tmr_MotorEyesMoveCnt>=tmr_MotorEyesMoveTarget){
		 th6_timerMotorEyesMoveReadyFlag=1;
		 tmr_MotorEyesMoveCnt=0;
	 }
	 if(tmr_MotorEyesMoveRunFlag)tmr_MotorEyesMoveCnt++;

	 // Timers pour interval d'envoie des mouvements
	 if(tmr_MotionIntervalCnt>=tmr_MotionIntervalTarget){
		 th6_timerMotionIntervalReadyFlag=1;
		 tmr_MotionIntervalCnt=0;
	 }
	 if(tmr_MotionIntervalRunFlag)tmr_MotionIntervalCnt++;

//	pthread_mutex_unlock (&my_mutex);

	 usleep(1000);		//1mS
  }

  printf( "# ARRET tache TIMERS\n");

  usleep(10000);

  killAllThread+=1;
  pthread_exit (0);
}

// ---------------------------------------------------------------------------
// FONCTION TIMER SYNTHETISEUR DE FREQUENCE
// ---------------------------------------------------------------------------

// Timer START
void th6_timerSynFreqStart(unsigned int xmS){
	tmr_synFreqTarget = xmS;
	tmr_synFreqCnt=0;
	th6_timerSynFreqReadyFlag=0;
	tmr_synFreqRunFlag=1;
}

// Timer STOP
void th6_timerSynFreqStop(void){
	tmr_synFreqRunFlag=0;
	th6_timerSynFreqReadyFlag=0;
}

// ---------------------------------------------------------------------------
// FONCTION TIMER 30Secondes pour gestionnaire de tache
// ---------------------------------------------------------------------------

// Timer START
void th6_timer30sManagerStart(unsigned int xmS){
	tmr_30sManagerTarget = xmS;
	tmr_30sManagerCnt=0;
	th6_timer30sManagerReadyFlag=0;
	tmr_30sManagerRunFlag=1;
}

// Timer STOP
void th6_timer30sManagerStop(void){
	tmr_30sManagerRunFlag=0;
	th6_timer30sManagerReadyFlag=0;
}

// ---------------------------------------------------------------------------
// // Timers 100ms Pour mesure de distance ultrasons
// ---------------------------------------------------------------------------

// Timer START
void th6_timerUSonicSampleStart(unsigned int xmS){
	tmr_USonicSampleTarget = xmS;
	tmr_USonicSampleCnt=0;
	th6_timerUSonicSampleReadyFlag=0;
	tmr_USonicSampleRunFlag=1;
}

// Timer STOP
void th6_timerUSonicSampleStop(void){
	tmr_USonicSampleRunFlag=0;
	th6_timerUSonicSampleReadyFlag=0;
}


// ---------------------------------------------------------------------------
// Timers 20ms pour attente de d�placement sevomoteur
// ---------------------------------------------------------------------------

// Timer START
void th6_timerMotorEyesMoveStart(unsigned int xmS){
	tmr_MotorEyesMoveTarget = xmS;
	tmr_MotorEyesMoveCnt=0;
	th6_timerMotorEyesMoveReadyFlag=0;
	tmr_MotorEyesMoveRunFlag=1;
}

// Timer STOP
void th6_timerMotorEyesMoveStop(void){
	tmr_MotorEyesMoveRunFlag=0;
	th6_timerMotorEyesMoveReadyFlag=0;
}

// ---------------------------------------------------------------------------
// Timers pour interval d'envoie des mouvements
// ---------------------------------------------------------------------------

// Timer START
void th6_timerMotionIntervalInit(unsigned int xmS){
	tmr_MotionIntervalTarget = xmS;
	tmr_MotionIntervalCnt=0;
	th6_timerMotionIntervalReadyFlag=0;
}

// Timer ON
void th6_timerMotionIntervalStart(void){
	tmr_MotionIntervalRunFlag=1;
}

// Timer STOP
void th6_timerMotionIntervalStop(void){
	tmr_MotionIntervalRunFlag=0;
	th6_timerMotionIntervalReadyFlag=0;
}


// ---------------------------------------------------------------------------
// FONCTION TIMER POUR GENERATION DUTYCYCLE
// ---------------------------------------------------------------------------

// Timer START
void th6_timerBuzzDutyStart(unsigned int mS_high,unsigned int mS_low){
	tmr_BuzzDutyHighTarget = mS_high;
	tmr_BuzzDutyLowTarget = mS_high+mS_low;
	tmr_BuzzDutyCnt=0;
	th6_timerBuzzDutyHighReadyFlag=0;
	th6_timerBuzzDutyLowReadyFlag=0;
	tmr_BuzzDutyRunFlag=1;
}

// Timer STOP
void th6_timerBuzzDutyStop(void){
	tmr_BuzzDutyRunFlag=0;
	th6_timerBuzzDutyHighReadyFlag=0;
	th6_timerBuzzDutyLowReadyFlag=0;
}


// ---------------------------------------------------------------------------
// // Timers 100ms Pour mesure d'angle d'orientation
// ---------------------------------------------------------------------------

// Timer START
void th6_timerCompassStart(unsigned int xmS){
	tmr_CompassTarget = xmS;
	tmr_CompassCnt=0;
	th6_timerCompassReadyFlag=0;
	tmr_CompassRunFlag=1;
}

// Timer STOP
void th6_timerCompassStop(void){
	tmr_CompassRunFlag=0;
	th6_timerCompassReadyFlag=0;
}



// ---------------------------------------------------------------------------
// CREATION THREAD TIMERS
// ---------------------------------------------------------------------------
int createTimersTask(void)
{
	return (pthread_create (&th_timers, NULL, timersTask, NULL));
}

// ---------------------------------------------------------------------------
// DESTRUCTION THREAD TIMERS
// ---------------------------------------------------------------------------
int killTimersTask(void){
	return (pthread_cancel(th_timers));
}

