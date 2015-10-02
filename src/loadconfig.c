#include <stdlib.h>
#include <stdio.h>


int readMyValue(unsigned char myChar);
int loadConfigFile(unsigned char mode);
unsigned char createmyMovementsTab(unsigned char seqCnt,unsigned char cmdCnt, unsigned char byteCnt);
void cleanmyMovementsTab(unsigned char seqCnt,unsigned char cmdCnt, unsigned char byteCnt);

unsigned char getNbOfSequences(void);				// Retourne le nombre de séquence total lue
unsigned char getNbOfCommands(void);					// Retourne le nombre de commandes total lue
unsigned char getNbOfCmdForSeq(unsigned char seqNb);	// Retourne le nombre de commandes dans une séquence donnée

unsigned char i;

// VARIABLE D'ECHANGE CHARGEMENT DU FICHIER CONFIG
unsigned char waitNewCmd=1;
unsigned char startNewSeq=1;
unsigned char configFileResult[3];
unsigned char nbCmdPerSeq[25];				// Maximum 25 sequences

//unsigned char myMovementsTab[];
unsigned char mode=0;
unsigned char dataCommandReady=0;

// VARIABLE POUR CREATION TABLEAU DE MOUVEMENT
int  seqenceNr,commandNr,byteNr;

unsigned char ***myMovementsTab; // myMovementsTab est un tableau 3D de taille axbxc


unsigned char loadConfig() {

// Ouverture fichier config SANS chargement dans le tableau (Comptage sequences et commande)
	if(loadConfigFile(0)){
//	   	printf(" - ouverture fichier cfg : OK\n");

	   	//Tentative de creation du Tableau
	    if(createmyMovementsTab(configFileResult[0],configFileResult[1],25)){

	    	// Ouverture fichier config avec chargement dans le tablea
	    	if(loadConfigFile(1)){
//	    		printf(" - Chargement des sequences : OK\n");
	    		dataCommandReady=1;
	    	}
	    	else {
//	    		printf(" - Chargement des sequences. : ERREUR\n");
	    		dataCommandReady=2;
	    	}

	    }
	    else
//	    	printf(" - Creation tableau mouv. :   ERREUR");
	    	dataCommandReady=3;
	}
	else{
    	printf(" - Lecture fichier cfg :      ERREUR");
	}

	// END TEST
	return(dataCommandReady);
}



//------------------------------------------------------------------
// EXTRACTION DES DONNEES DU FICHIER
//------------------------------------------------------------------

int loadConfigFile(unsigned char mode){

	int totalCmdCount=0; // Comptage du nombre total de commandes
	int totalSeqCount=0;

	int ptrCmdInSeq=0; // numero de commande dans la sequence
	int ptrByteInCmd=0;
	int myCharFile;
	int decodedValue;

    FILE *fichier = NULL;

  //  fichier = fopen("test.txt", "r+");
    fichier = fopen("hexapodRemote.cfg", "r+");

    if (fichier != NULL)
    {
    	    	while(myCharFile!=EOF){

    	    		myCharFile=fgetc(fichier);
    	    		decodedValue=readMyValue(myCharFile);

    	    		if(decodedValue>=0 && decodedValue<256){

    	    			// Reception nouvelle commande
    	    			if(waitNewCmd==1){
    	    				totalCmdCount++;
    	    				ptrCmdInSeq++;

    	    				waitNewCmd=0;
    	    				ptrByteInCmd=0;

    	    				// Attribue le nombre de commandes presente dans la sequence
    	    				nbCmdPerSeq[totalSeqCount-1]++;
    	    			}

    	    			// Reception nouvelle sequence
    	    			if(startNewSeq==1){
    	    				totalSeqCount++;
    	    				ptrCmdInSeq=0;
    	    				startNewSeq=0;

//    	    				printf("\n-- New sequence | Seq:%d Cmd:%d = \n",totalSeqCount,ptrCmdInSeq);

    	    			}

    	    		//
    	    			if(mode!=0){
    	    				myMovementsTab[totalSeqCount-1][ptrCmdInSeq][ptrByteInCmd]=decodedValue;
    	    			}
//    	    			printf("Seq: %d Cmd: %d Byte: %d = %d\n",totalSeqCount,ptrCmdInSeq,ptrByteInCmd,decodedValue);
    	    			ptrByteInCmd++;
//   	    			printf(" %d",decodedValue);


    	    		}

    	    		if ((decodedValue<-100) || (decodedValue>255)){
    	    			printf("ERREUR: %d\n",decodedValue);
    	    			//break;
    	    		}
    	    	}

    	    	nbCmdPerSeq[totalSeqCount-1]++;

//printf("Nombre de commandes: %d\n",totalCmdCount);
    	    	configFileResult[0]=totalSeqCount;
    	    	configFileResult[1]=totalCmdCount;

    	    	rewind(fichier);
    	    	fclose(fichier); // On ferme le fichier qui a ete ouvert
    } else {
    	return(-101);
    }
    return(1);
}

//------------------------------------------------------------------
// TENTATIVE DE DECODAGE DE LA VALEUR
//------------------------------------------------------------------

int readMyValue(unsigned char myChar){
	static unsigned char flagCommentChar=0;
	static unsigned char myCharCount;
	static char myReadValue[3];

	static int myDecValue;
	unsigned char i;

	if(myChar=='\n') {
			if(flagCommentChar){
				flagCommentChar=0;
				startNewSeq=1;
			}

			for(i=0;i<3;i++)myReadValue[i]=0;
			myCharCount=0;
			waitNewCmd=1;
			return(-2);		// Detection nouvelle ligne
	}

	if(myChar=='/') {

			for(i=0;i<3;i++)myReadValue[i]=0;
			myCharCount=0;
			flagCommentChar=1;
			return(-3);		// Detection commentaire
	}

    if(myChar!=';'){
    	myReadValue[myCharCount]=myChar;
    	myCharCount++;

    	if(myCharCount>3){
			myCharCount=0;
//printf("er: %d-%d \n",myChar,myCharCount);
			return(-100);	// Probleme de codage > 3 digit
    	}
    	else{
    		return(-1);    // RAS, ne rien faire
    		}
    }
    else{
    	myDecValue=atoi(myReadValue);
    	for(i=0;i<3;i++)myReadValue[i]=0;
    	myCharCount=0;
    	return(myDecValue);
    }
}

//------------------------------------------------------------------
// Creation d'un tableau dynamique pour l'enregistrament des sequences
//------------------------------------------------------------------
unsigned char createmyMovementsTab(unsigned char seqCnt,unsigned char cmdCnt, unsigned char byteCnt){

	//printf("seqCnt = %i; cmdCnt = %i; byteCnt = %i\n",seqCnt,cmdCnt,byteCnt); // affichage des valeurs de seqCnt, b ,c

	    /* construction du tableau 3D myMovementsTab */
	    myMovementsTab = (unsigned char***)malloc(sizeof(unsigned char**) *seqCnt);

	     if ( myMovementsTab == NULL )
	{
	     return(0);
	}

	  for (seqenceNr=0;seqenceNr<seqCnt;seqenceNr++)
	  {
	      myMovementsTab[seqenceNr] = (unsigned char**)malloc(sizeof(unsigned char*) *cmdCnt);
	      for (commandNr=0;commandNr<cmdCnt;commandNr++)
	      {
	          myMovementsTab[seqenceNr][commandNr] = (unsigned char*)malloc(sizeof(unsigned char)*byteCnt);
	      }
	  }


		// Initialisation de tous les elements de myMovementsTab à 0 ensuite remplissage de myMovementsTab et affichage
		for (seqenceNr=0;seqenceNr<seqCnt;seqenceNr++)
		  {
		      for (commandNr=0;commandNr<cmdCnt;commandNr++)
		      {
		          for (byteNr=0;byteNr<byteCnt;byteNr++)
		          {

		               myMovementsTab[seqenceNr][commandNr][byteNr]=0; // initialisation de tous les elements de myMovementsTab
		          }

		      }
		  }
		//Fin tableau
	  return(1);
}

//------------------------------------------------------------------
// Retourne le nombre de séquence total lue
//------------------------------------------------------------------
unsigned char getNbOfSequences(void){
	return (configFileResult[0]);
}

//------------------------------------------------------------------
// Retourne le nombre de commandes total total lue
//------------------------------------------------------------------
unsigned char getNbOfCommands(void){
	return (configFileResult[1]);
}

//------------------------------------------------------------------
// Retourne le nombre de commande contenue dans la séquence donnée
//------------------------------------------------------------------
unsigned char getNbOfCmdForSeq(unsigned char seqNb){
	return (nbCmdPerSeq[seqNb]);
}


//------------------------------------------------------------------
// Liberation de l'espace memoire du tableau.
//------------------------------------------------------------------
void cleanmyMovementsTab(unsigned char seqCnt,unsigned char cmdCnt, unsigned char byteCnt){
	// Liberation memoire

		for (seqenceNr=0;seqenceNr<seqCnt;seqenceNr++)
		  {
		      for (commandNr=0;commandNr<cmdCnt;commandNr++)
		          free(myMovementsTab[seqenceNr][commandNr]);
		  free(myMovementsTab[seqenceNr]);
		  }
		free(myMovementsTab);
}
