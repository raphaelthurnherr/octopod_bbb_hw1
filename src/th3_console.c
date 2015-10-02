#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

#include "th3_console.h"

#include "manager.h"
#include "th1_hwctrl.h"
#include "th2_core.h"
#include "th7_motion.h"

pthread_t th_console;

unsigned char motionInLoop=0;
// ***************************************************************************
// ---------------------------------------------------------------------------
// THREAD HEXAPOD CONSOLE: Gestion interface UI via console
// ---------------------------------------------------------------------------
// ***************************************************************************

void *consoleTask (void * arg)
{
	int myConsoleCommand;					// Commande utilisateur recue
	unsigned int myValue;					// Valeur utilisateur recue

	printf ("# Demarrage tache CONSOLE: OK\n");

	while(!killAllThread){
	//	  pthread_mutex_lock (&my_mutex);

		// AFFICHAGE DU MENU DANS LA CONSOLE
		printf("\nCommandes:\n\n");
		printf(" 0..Debug : Execute un mouvement, affiche la s�quence\n");
		printf(" 100  : Sequence en boucle ON/OFF\n");
		printf(" 104  : Vitesse servo 1..8\n");
		//printf(" 105  : Look around ON/OFF\n");
		printf(" 300  : Affichage UART RX BRUT ON/OFF\n");
		printf(" 301  : Affichage ACK Distance decodees ON/OFF\n");
		printf(" 302  : Affichage ACK Motor decodees ON/OFF\n");
		printf(" 303  : Affichage ACK Heartbit decode ON/OFF\n");
		printf(" 304  : Affichage ACK angle orientation decodees ON/OFF\n");
		printf(" 305  : Affichage ACK interruptions IR ON/OFF\n");
		//printf(" 400  : Intelligence artificielle ON/OFF\n");
		//printf(" 500  : Test Servo & Ultrasons\n");
		printf(" 501  : Test Scan Zone\n");
		printf(" 504  : Test boussole \n");
		printf(" 505  : Calibration boussole \n");
		printf(" 506  : Test interrupt boussole 170-190 deg\n");
		printf(" 600  : Test beep\n");
		printf(" 601  : Autocalibration hexapod\n");
		printf(" 666  : Quitter\n");

		printf(" -> ");

		scanf("%d",&myConsoleCommand);

		// COMMANDE FIN D'APPLICATION
		if(myConsoleCommand ==666){
			//cleanmyMovementsTab(configFileResult[0],configFileResult[1],25);
			break;
		}

		if(myConsoleCommand<100){
			th7_displayMotionSeq(myConsoleCommand);
			UICommand((myConsoleCommand<<4)|RUN_MOTION);
		}
		else{
			// AUTRES COMMANDES
			switch(myConsoleCommand){

				// Repetition de la s�quence
				case	100	:	motionInLoop=~motionInLoop;
								UICommand((motionInLoop<<4)|RUN_MOTION_LOOP);
								break;


				// Modification du temps d'envoie entre les commandes (ms)
				case	104	:	printf("\nVitesse 1..8: ");
								scanf("%d",&myValue);
								UICommand((myValue<<4)|RUN_SPEED);
								printf("\n ->");
								break;

				//
				case	105	:	//flag_lookAround=!flag_lookAround;
								break;

				// Affichage des trames UART recue
				case	300	:	HWctrl_displayAckToggle(UART_ACK_ALL_UNDECODED);
								break;

				// Affichage de la distance si disponible
				case	301	:	HWctrl_displayAckToggle(UART_ACK_ULTRASONIC);
								break;

				// Affichage de la distance si disponible
				case	302	:	HWctrl_displayAckToggle(UART_ACK_MOTORS);
								break;

				// Modification du temps d'envoie entre les commandes (ms)
				case	303	:	HWctrl_displayAckToggle(UART_ACK_HEARTBIT);
								getControllerHeartBit();
								break;

				// Modification du temps d'envoie entre les commandes (ms)
				case	304	:	HWctrl_displayAckToggle(UART_ACK_COMPASS);
								break;

				// Activation de l'affichage des interruption IR0..IR2
				case	305	:	HWctrl_displayAckToggle(UART_ACK_IR);
								break;

				// Affichage de la distance si disponible
				case	400	:	///AI_enable=!AI_enable;
								break;

				// Affichage de la distance si disponible
				case	500	:	//EyesServoTest();
								break;

				// Affichage de la distance si disponible
				case	501	:	UICommand(TEST_SEARCH_BEST_WAY);
								break;

				// Affichage de l'angle d'orientation si disponible
				case	504	:	UICommand(TEST_COMPASS);
								break;

				// Affichage de l'angle d'orientation si disponible
				case	505	:	UICommand(CALIB_COMPASS);

								break;

				// Modification du temps d'envoie entre les commandes (ms)
				case	506	:	UICommand(INT_COMPASS);
								break;

				// Envoie une s�quence de beep
				case	600	:	printf("\n Nombre de beep: ");
								scanf("%d",&myValue);
								UICommand((myValue<<4)|TEST_BUZZER);
								break;

				// D�marre une proc�dure d'autocalibration
				case	601	:	UICommand(CALIB_HEXAPOD);
								break;

				default		:	printf("Commande inconnue !\n->");
			}
		}


//	pthread_mutex_unlock (&my_mutex);
  usleep(10000);
  }

  printf( "# ARRET tache CONSOLE\n");

  usleep(10000);

  killAllThread+=1;
  pthread_exit (0);
}


// ---------------------------------------------------------------------------
// CREATION THREAD UART
// ---------------------------------------------------------------------------
int createConsoleTask(void)
{
	return (pthread_create (&th_console, NULL, consoleTask, NULL));
}

// ---------------------------------------------------------------------------
// DESTRUCTION THREAD UART
// ---------------------------------------------------------------------------
int killConsoleTask(void){
	return (pthread_cancel(th_console));
}

