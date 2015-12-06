#include "th7_motion.h"

#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

#include "th6_timers.h"
#include "loadconfig.h"
#include "manager.h"
#include "th1_hwctrl.h"

pthread_t th_motion;

unsigned char currentMotion=0;
unsigned char currentMotionSpeed=2;
unsigned char runMotion=0;
unsigned char runMotionInLoop=0;
unsigned int motionTime=25;

// Variable de correction d'angles pour les moteur des pattes
signed char motorLegsCorrection[24];
unsigned char enableMotorCorrection = 1;


void loadHexapodMotionSeq(void);
unsigned char th7_displayMotionSeq(unsigned char SelectedSeq);

void makeMotion(void);
void setMotion(unsigned char motionNb);
void setMotionLoop(unsigned char motionLoopON);
void setMotionSpeed(unsigned char motionSpeed);


// ***************************************************************************
// ---------------------------------------------------------------------------
// THREAD HEXAPOD CORE: Gestion principale
// ---------------------------------------------------------------------------
// ***************************************************************************

void *motionTask (void * arg)
{
	RunningTask += TH7_SOA;

	printf ("# Demarrage tache MOTION: OK\n");

	// Chargement des s�quences de mouvement (hexapodRemote.cfg)
	loadHexapodMotionSeq();

	usleep(100000);

	th6_timerMotionIntervalInit(motionTime);


	while(!EndOfApp){
		//	  pthread_mutex_lock (&my_mutex);
		if(runMotion)th6_timerMotionIntervalStart();
		else th6_timerMotionIntervalStop();

		if(th6_timerMotionIntervalReadyFlag){
			th6_timerMotionIntervalInit(motionTime);
			makeMotion();
			th6_timerMotionIntervalReadyFlag=0;
		}

			//	pthread_mutex_unlock (&my_mutex);
		usleep(25000);
	}

	printf( "# ARRET tache MOTION\n");

	usleep(10000);
	RunningTask -= TH7_SOA;
	pthread_exit (0);
}


// ---------------------------------------------------------------------------
// CHARGEMENT DES SEQUENCES DE MOUVEMENTS
// ---------------------------------------------------------------------------
void loadHexapodMotionSeq(void){
	unsigned char loadingResult;								// Resultat du chargement des mouvements

	loadingResult=loadConfig();
	if(loadingResult==NOERR){
		printf(" - Chargement mouvements octopodRemote.cfg: OK (%d Seq & %d Cmd)\n",getNbOfSequences(),getNbOfCommands());
	}else printf(" - Chargement des mouvements octopodRemote.cfg: ERREUR (code: %d)\n",loadingResult);
}

// ---------------------------------------------------------------------------
// AFFICHAGE DE LA SEQUENCE DONNE DANS LA CONSOLE
// Entr�e: Num�ro de s�quence � afficher
// Sortie: OK, ERROR (S�quence invalid)
// ---------------------------------------------------------------------------

unsigned char th7_displayMotionSeq(unsigned char SelectedSeq){

	unsigned char j,k;

	if(SelectedSeq >= 0 && SelectedSeq <=getNbOfSequences()){
		printf("\nSequence: %d\n", SelectedSeq);
		for(j=0;j<getNbOfCmdForSeq(SelectedSeq)/2;j++){
			for(k=0;k<myMovementsTab[SelectedSeq][j][1]+2;k++){
				printf("%d ",myMovementsTab[SelectedSeq][j][k]);
			};
			printf("\n");
		}
		return(NOERR);
	}
	else{
		printf("\nSequence: %d invalid !\n", SelectedSeq);
		return(ERR);
	}
}

// ***************************************************************************
// ---------------------------------------------------------------------------
// Applique un mouvement contenu dans le fichier config
// ---------------------------------------------------------------------------
// ***************************************************************************

void makeMotion(void){
	unsigned char i;
	unsigned char nbMotorToModify;
	unsigned char offsetMotorToModify;
	signed char currentMotorAngle;

	static unsigned char j;
	unsigned char k;
	unsigned char myBufferOut[50];

	if(motorsPositionValid | !motorsInterruptEnable){

		if(!motorsInterruptEnable) usleep(150000);

		//for(j=0;j<getNbOfCmdForSeq(currentMotion)/2;j++){
		if(j<getNbOfCmdForSeq(currentMotion)/2){
			for(k=0;k<myMovementsTab[currentMotion][j][1]+2;k++){
				myBufferOut[k]=myMovementsTab[currentMotion][j][k];
			};

			// Mofifie Commande vitesse du controleur ATTENTION, MODIFICATION DE  LA TRAME INITIALE
			if(myBufferOut[2]>0 && myBufferOut[2]<9){				// Commande d'angle moteur ?
				if(currentMotionSpeed>0 && currentMotionSpeed<9){	// !=0 ou <10 = vitesse valide
					myBufferOut[2]=currentMotionSpeed;
				}

			// Correction de l'offset des moteurs
				if(enableMotorCorrection){
					nbMotorToModify=myBufferOut[4]-myBufferOut[3];
					offsetMotorToModify=myBufferOut[3];
					for(i=0; i<nbMotorToModify;i++){
						currentMotorAngle=myBufferOut[i+5] + motorLegsCorrection[i+offsetMotorToModify];
						if(currentMotorAngle<0)currentMotorAngle=0;
						if(currentMotorAngle>120)currentMotorAngle=120;
						myBufferOut[i+5]=currentMotorAngle;
					}
				}
			}
			sendUartFrame(myBufferOut,myBufferOut[1]+2);

			motorsPositionValid=0;
			if(currentMotionSpeed==1) usleep(150000);
			j++;

		}else {
			if(!runMotionInLoop)runMotion=0;										// Stop la boucle
			j=0;
			printf("\n----FIN DE SEQUENCE----\n");
		}
	}
}


// ***************************************************************************
// ---------------------------------------------------------------------------
// S�l�ction un mouvement � appliquer
// ---------------------------------------------------------------------------
// ***************************************************************************

void setMotion(unsigned char motionNb){
	currentMotion=motionNb;
	runMotion=1;
}

// ***************************************************************************
// ---------------------------------------------------------------------------
// mofifiction loop sur mouvement
// ---------------------------------------------------------------------------
// ***************************************************************************

void setMotionLoop(unsigned char motionLoopON){
	if(!motionLoopON)runMotionInLoop=0;
	else runMotionInLoop=1;
	runMotion=1;
}

// ***************************************************************************
// ---------------------------------------------------------------------------
// mofifiction speed sur mouvement
// ---------------------------------------------------------------------------
// ***************************************************************************

void setMotionSpeed(unsigned char motionSpeed){
	printf("\n speed select: %d \n",motionSpeed);
	if(motionSpeed>0 && motionSpeed<9){					// !=0 ou <10 = vitesse valide
		currentMotionSpeed=motionSpeed;
		motionTime=25;
		th6_timerMotionIntervalInit(motionTime);
	}
}


// ---------------------------------------------------------------------------
// CREATION THREAD MOTION
// ---------------------------------------------------------------------------
int createMotionTask(void)
{
	return (pthread_create (&th_motion, NULL, motionTask, NULL));
}

// ---------------------------------------------------------------------------
// DESTRUCTION THREAD UART
// ---------------------------------------------------------------------------
int killMotionTask(void){
	return (pthread_cancel(th_motion));
}

