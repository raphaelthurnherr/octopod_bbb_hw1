#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

#include "th4_tcp.h"

#include "manager.h"
#include "th2_core.h"
#include "tcpserver.h"

pthread_t th_tcp;


// ***************************************************************************
// ---------------------------------------------------------------------------
// THREAD HEXAPOD CONSOLE: Gestion "interface UI" via TCP
// ---------------------------------------------------------------------------
// ***************************************************************************

void *tcpTask (void * arg)
{
	if(tcpOpen(TCP_PORT)){
		printf ("# Demarrage tache TCP sur port %d: OK\n",TCP_PORT);
	}
	else{
		printf( "# Demarrage tache TCP sur port %d: ERREUR\n",TCP_PORT );
	}

	RunningTask += TH4_SOA;

  while(!EndOfApp){
//	  pthread_mutex_lock (&my_mutex);

		printf(" - Attente de client TCP...\n");
		if(waitTcpClient()){ // BLOCANT
			printf("Client TCP connect�...\n");

		}else
			printf("Client TCP refus� !\n");

		int tcpData=0;


		while((tcpData=tcpCheck())>=0){
// TRAITEMENT DE LA COMMANDE RECUE

			//AI_enable=0;

			printf("R�ception commande TCP: %d\n",tcpData);

			tcpWriteString("S�quence: ",10);
			tcpWrite(tcpData);

			switch(tcpData){

			// MOUVEMENTS
			case 0 :	UICommand((ROTLEFT<<4)|RUN_MOTION);
						setMotionLoop(1);
					 	break;

			case 1 :	UICommand((FORWARD<<4)|RUN_MOTION);
						setMotionLoop(1);
					 	break;

			case 2 : 	UICommand((ROTRIGHT<<4)|RUN_MOTION);
						setMotionLoop(1);
						break;

			case 3 :	UICommand((LATLEFT<<4)|RUN_MOTION);
						setMotionLoop(1);
						break;

			case 4 :	UICommand((STANDUP<<4)|RUN_MOTION);
						setMotionLoop(0);
						break;

			case 5 :	UICommand((LATRIGHT<<4)|RUN_MOTION);
						setMotionLoop(1);
						break;

			case 6 :	UICommand((BACK<<4)|RUN_MOTION);
						setMotionLoop(1);
						break;

			case 7 :	UICommand((ATTACK<<4)|RUN_MOTION);
						setMotionLoop(1);
						break;

			case 8 :	UICommand((DANCE<<4)|RUN_MOTION);
						setMotionLoop(1);
						break;

			case 9 :	UICommand((STRECHUPDW<<4)|RUN_MOTION);
						setMotionLoop(1);
						break;

			case 10 :	UICommand((SLEEPOFF<<4)|RUN_MOTION);
						setMotionLoop(0);
						break;

			// DIVERS

			case 20	 : 	setMotionLoop(0);
						UICommand(TEST_SEARCH_BEST_WAY);
						break;

			case 21  :	UICommand((5<<4)|TEST_BUZZER);
						break;

			// VITESSES
			case 51 :	UICommand(((tcpData-50)<<4)|RUN_SPEED);
						break;
			case 52 :	UICommand(((tcpData-50)<<4)|RUN_SPEED);
						break;
			case 53 :	UICommand(((tcpData-50)<<4)|RUN_SPEED);
						break;
			case 54 :	UICommand(((tcpData-50)<<4)|RUN_SPEED);
						break;
			case 55 :	UICommand(((tcpData-50)<<4)|RUN_SPEED);
						break;
			case 56 :	UICommand(((tcpData-50)<<4)|RUN_SPEED);
						break;
			case 57 :	UICommand(((tcpData-50)<<4)|RUN_SPEED);
						break;
			case 58 :	UICommand(((tcpData-50)<<4)|RUN_SPEED);
						break;
			case 59 :	UICommand(((tcpData-50)<<4)|RUN_SPEED);
						break;

			case 60	 : 	UICommand(((tcpData-60)<<4)|CHANGEMOTION);
						break;

			case 61  :	UICommand(((tcpData-60)<<4)|CHANGEMOTION);
						break;

			case 62	 : 	UICommand(((tcpData-60)<<4)|CHANGEMOTION);
						break;

			case 63  :	UICommand(((tcpData-60)<<4)|CHANGEMOTION);
						break;

			case 64	 : 	UICommand(((tcpData-60)<<4)|CHANGEMOTION);
						break;

			case 65  :	UICommand(((tcpData-60)<<4)|CHANGEMOTION);
						break;

			case 66	 : 	UICommand(((tcpData-60)<<4)|CHANGEMOTION);
						break;

			case 67  :	UICommand(((tcpData-60)<<4)|CHANGEMOTION);
						break;

			case 68	 : 	UICommand(((tcpData-60)<<4)|CHANGEMOTION);
						break;

			case 69  :	UICommand(((tcpData-60)<<4)|CHANGEMOTION);
						break;


			case 80  :	UICommand(CALIB_HEXAPOD);
						break;

			case 81  :	UICommand(REMOTE_RUN_AI);				// DEMARRAGE HEXAPOD EN AUTONOMIE
						break;

			case 99 :
/*						if(AI_enable){
							AI_enable=0;
							hexapod_currentSeq=STANDBY_ON;
							hexapod_SetSeqloop=0;
							sendFullSequence();
						}else{
							AI_enable=1;
						}*/
						break;

			default :   tcpWriteString("S�quence: INCONNUE",18);
						break;
			}



//			displayHexapodState();

// FIN DE TRAITEMENT
		}

		if(tcpData==-1) printf("Client TCP d�connect�...");
		if(tcpData==-2) printf("Erreur de reception TCP...");


//	pthread_mutex_unlock (&my_mutex);
  usleep(10000);
  }

  printf( "# ARRET tache TCP\n");

  usleep(10000);
  RunningTask -= TH4_SOA;
  pthread_exit (0);
}


// ---------------------------------------------------------------------------
// CREATION THREAD TCP
// ---------------------------------------------------------------------------
int createTcpTask(void)
{
	return (pthread_create (&th_tcp, NULL, tcpTask, NULL));
}

// ---------------------------------------------------------------------------
// DESTRUCTION THREAD TCP
// ---------------------------------------------------------------------------
int killTcpTask(void){
	return (pthread_cancel(th_tcp));
}

