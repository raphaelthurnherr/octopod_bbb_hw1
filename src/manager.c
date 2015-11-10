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
void ethernetCheck(void);

unsigned char killAllThread=0;

struct EthernetInfo{
	char sIP_lan[16];
	char sIP_wlan[16];
};

struct EthernetInfo OctopodEthernet;

// ***************************************************************************
// ---------------------------------------------------------------------------
// STARTUP
// ---------------------------------------------------------------------------
// ***************************************************************************

int main(void) {

	ethernetCheck();

	system("clear");
	printf("\nOctopod V1.3.4  - 10/11/2015          \n");
	printf("---------------------------------------\n\n");
	printf("# Octopod IP ETH0: %s \n", OctopodEthernet.sIP_lan);
	printf("# Octopod IP WLAN0: %s \n", OctopodEthernet.sIP_wlan);

	printf ("# Demarrage du gestionnaire des taches...\n");
	// DEMARRAGE DES T�CHES
	InitMultiTasking();

	usleep(100000);
	th6_timer30sManagerStart(30000);		// D�marrage Timer 30s

	// Contrôle de la disponibilité du controller bas niveau et des capteur
	checkLLcontrollerDevices();
	usleep(100000);
	checkLLcontrollerDevices();
	usleep(100000);
	// DEBUG;
	unsigned char temp[4]={0xAA, 0x01, 0x01, 0xEE};
	sendLCDUartFrame(temp, 4);

	// ---------------------------------------------------------------------------
	// MAIN LOOP
	// ---------------------------------------------------------------------------
	while(!killAllThread){
		if(killAllThread>=5){
			if(!killTcpTask())  printf( "# ARRET* tache TCP\n"); // TACHE TCP
			if(!killConsoleTask())  printf( "# ARRET* tache console\n"); // TACHE CONSOLE
		}

// Controle chaques 30seconds
		if(th6_timer30sManagerReadyFlag){

			// Contr�le des l'�tat des batteries, alarme+affichage ON si warning
			if(BatteryCheck(1)){
				buzzerCtrl(3);		// Test beep
			}

			// Contrôle des connexion ethernet
			ethernetCheck();

			printf("CHECK IP ETH0: %s \n", OctopodEthernet.sIP_lan);
			printf("CHECK IP WLAN0: %s \n", OctopodEthernet.sIP_wlan);

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


void ethernetCheck(void){
	// LECTURE IP ADRESSE BBB
		int fd;
		struct ifreq ifr;
		struct ifreq ifrw;

		fd = socket(AF_INET, SOCK_DGRAM, 0);
		ifr.ifr_addr.sa_family = AF_INET;
		strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
		ioctl(fd, SIOCGIFADDR, &ifr);
		close(fd);
		strcpy(OctopodEthernet.sIP_lan, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));
		fd = socket(AF_INET, SOCK_DGRAM, 0);
		ifrw.ifr_addr.sa_family = AF_INET;
		strncpy(ifrw.ifr_name, "wlan1", IFNAMSIZ-1);
		ioctl(fd, SIOCGIFADDR, &ifrw);
		close(fd);
		strcpy(OctopodEthernet.sIP_wlan, inet_ntoa(((struct sockaddr_in*)&ifrw.ifr_addr)->sin_addr));

	// FIN LECTURE IP ADRESSE BBB
}
