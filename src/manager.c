/*
 ============================================================================
 Name        : hexaPod.c
 Author      : rth
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 01/07/2014 - Modification baudrate UART

 */

#include "manager.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>

#include <unistd.h>

#include "th1_hwctrl.h"
#include "th2_core.h"
#include "th3_console.h"
#include "th4_tcp.h"
#include "th5_ai.h"
#include "th6_timers.h"
#include "th7_motion.h"
#include "tools.h"

int InitMultiTasking (void);
unsigned char killAllThread=0;


// ***************************************************************************
// ---------------------------------------------------------------------------
// STARTUP
// ---------------------------------------------------------------------------
// ***************************************************************************

int main(void) {
// LECTURE IP ADRESSE BBB
	int fd;
	struct ifreq ifr;

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);
	strcpy(OctopodIPlan, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));

// FIN LECTURE IP ADRESSE BBB

	system("clear");
	printf("\nOctopod V1.3.3  - 09/11/2015          \n");
	printf("---------------------------------------\n\n");
	printf("# Octopod IP ETH0: %s \n", OctopodIPlan);

	printf ("# Demarrage du gestionnaire des taches...\n");
	// DEMARRAGE DES T�CHES
	InitMultiTasking();

	usleep(100000);
	th6_timer30sManagerStart(30000);		// D�marrage Timer 30s

	// ---------------------------------------------------------------------------
	// MAIN LOOP
	// ---------------------------------------------------------------------------
	while(!killAllThread){
		if(killAllThread>=5){
			if(!killTcpTask())  printf( "# ARRET* tache TCP\n"); // TACHE TCP
			if(!killConsoleTask())  printf( "# ARRET* tache console\n"); // TACHE CONSOLE
		}

// Contr�le chaques 30seconds
		if(th6_timer30sManagerReadyFlag){

			// Contr�le des l'�tat des batteries, alarme+affichage ON si warning
			if(BatteryCheck(1)){
				buzzerCtrl(3);		// Test beep
			}

			th6_timer30sManagerReadyFlag=0;
		}
		usleep(100000);		/* Ralentit le thread d'ecriture... */
	}
		usleep(100000);		/* Ralentit le thread d'ecriture... */
		printf("\n# Fin d' application, bye bye !\n");
		return (0);
}



// ***************************************************************************
// ---------------------------------------------------------------------------
// INIT MULTITASKING
// ---------------------------------------------------------------------------
// ***************************************************************************

int InitMultiTasking (void)
{
  //void *ret;

	// CREATION TACHE HWCTRL
	  if (createHwctrlTask() < 0) {
		printf ("# Creation tache HW CTRL : ERREUR\n");
		exit (1);
	  }

	// CREATION TACHE CORE
	if (createCoreTask() < 0) {
		printf ("# Creation tache HW CTRL : ERREUR\n");
		exit (1);
	}

	// CREATION TACHE TCP
	if (createTcpTask() < 0) {
		printf ("# Creation tache TCP : ERREUR\n");
		exit (1);
	}

	// CREATION TACHE AI
	if (createAITask() < 0) {
		printf ("# Creation tache AI : ERREUR\n");
		exit (1);
	}

	// CREATION TACHE TIMERS
	if (createTimersTask() < 0) {
		printf ("# Creation tache TIMERS : ERREUR\n");
		exit (1);
	}

	// CREATION TACHE MOTION
	if (createMotionTask() < 0) {
		printf ("# Creation tache MOTION : ERREUR\n");
		exit (1);
	}
	usleep(100000);
	// CREATION TACHE CONSOLE
	if (createConsoleTask() < 0) {
		printf ("# Creation tache CONSOLE : ERREUR\n");
		exit (1);
	}

/*
(void)pthread_join (th_hwctrl, &ret);		// TACHE UART
*/

return(1);
}
