#include <stdio.h>
#include <stdlib.h>
#include "th1_hwctrl.h"
#include <math.h>
#include "th7_motion.h"


unsigned int mapZone[50][90];   // zones de (200cm) chaque 10cm sur 90 degres
unsigned int mapZoneFiltre1[50][90];   // zones de (200cm) chaque 10cm sur 90 degres
unsigned int mapZoneFiltre2[50][90];   // zones de (200cm) chaque 10cm sur 90 degres

void makeMapZone(unsigned char AngleScanDept, unsigned int *tabAngleDistance);
void displayMapZone(unsigned char AngleScanDept, unsigned char filtre);
void MapZoneFiltre1(unsigned char AngleScanDept);
void MapZoneFiltre2(unsigned char AngleScanDept);
unsigned char calcBestAngle(unsigned char AngleScanDept);


// Contr�le de l'�tat des batteries affichage alarmes
unsigned char BatteryCheck(unsigned char DisplayWarning);

// Scan la distance d'un angle donn� � un autre avec un pas et stock les valeurs � une adresse d�finie
void ScanZone90(unsigned char offsetAngle, unsigned char angleStep, unsigned int *Target);

void hexapodCalibrateCompass(void);


// ***************************************************************************
// Contr�le de l'�tat des batteries affichage alarmes
// Entr�e: Affichage du status de batteries ON/OFF
// Sortie: Retour d'�tat warning [0:NO WARNING 1: BATT1, 2: BATT2, 3; BATT+2]
// ***************************************************************************

unsigned char BatteryCheck(unsigned char DisplayWarning){

	unsigned char BatteryWarning=0;

	BatteryWarning=th1_LowBatteryCheck();

	if(DisplayWarning){

		printf("\nBATTERY CHECK... ");
		if(BatteryWarning){
			printf("WARNING LOW BATTERY ");
			switch(BatteryWarning){
				case 1:	printf("BRAIN\n");	break;			// Batt A warning
				case 2:	printf("MOTORS\n");	break;			// Batt B warning
				case 3:	printf("MOTORS + BRAIN\n");	break;			// Batt A+B warning
				default:	break;
			}
		}else printf("OK\n");
	}
	return(BatteryWarning);
}


// ***************************************************************************
// ATTENTION: FONCTION BLOCANTE CORE
// Scan la distance d'un angle donn� � un autre avec un pas et stock les valeurs � une adresse d�finie
// Entr�e: fromAngle, Angle de d�part
// Entr�e: toAngle, Angle de d'arriv�e
// Entr�e: angleStep, nombre de degres entre les scans
// Entr�e: Target, addresse pour enregistrement des valeurs [0] si pas d'enregistrement];
// ***************************************************************************
void ScanZone90(unsigned char offsetAngle, unsigned char angleStep, unsigned int *Target){
	unsigned char i;
	unsigned char reverse=0;
	signed char angleActual=0;

	// SERVOMOTEUR ULTRASON ACTIF
	setMotorState(EYES_MOTOR_X,1);
	usleep(50000);
	ReadStartUltrasonicSensor();

	angleActual=offsetAngle;

	while(angleActual<=offsetAngle+90)
	{
		// DEMARRE LA MESURE DE DISTANCE A L'ANGLE DONNE
		getUltrasonicDistance(angleActual);
		usleep(50000);
		if(ultrasonicDistance[1]==angleActual){
			Target[angleActual-offsetAngle]=ultrasonicDistance[0]/10;
			angleActual+=angleStep;
		}
	}
	StopUltrasonicSensor();
}



// ***************************************************************************
// CREATION D'UN PLAN DE ZONE
// Entr�e: AngleScanDept, Tableau de valeur de distance pour chaque angle (0..90)
// Entr�e: R�solution angulaire � traiter [degres]
// ***************************************************************************

void makeMapZone(unsigned char AngleScanDept, unsigned int *tabAngleDistance)
{
	unsigned int i;
	unsigned int indexOnMap;

    float dist=0;


    // Efface de tableau de zone
    for(i=0;i<90;i+=AngleScanDept)
        for (indexOnMap = 0; indexOnMap < 50; indexOnMap++)
            mapZone[indexOnMap][i]=0;


    // Creation de la map
    for(i=0;i<90;i+=AngleScanDept){

        // Converti la mesure en un index de tableau
        dist = tabAngleDistance[i];
        indexOnMap = (int)round(dist / 5);
        if (indexOnMap > 49) indexOnMap = 49;

        mapZone[indexOnMap][i] = 1;
    }
}


// ***************************************************************************
// AFFICHAGE DU PLAN DE ZONE
// Entr�e: R�solution angulaire � traiter [degres]
// Entr�e: filtre � appliquer (0..2) 0 = pas de filtre
// ***************************************************************************

void displayMapZone(unsigned char AngleScanDept, unsigned char filtre){
	unsigned char i;
	unsigned char indexDist;

	printf("\n");

	for(indexDist=50;indexDist>0;indexDist--){
		printf("%03d cm- ",indexDist*5);
		switch(filtre){
		case 0 : 		for(i=0;i<90;i+=AngleScanDept){
							if (mapZone[indexDist][i] == 1) printf("o");
							else printf(" ");
						}break;

		case 1 : 		for(i=0;i<90;i+=AngleScanDept){
							if (mapZoneFiltre1[indexDist][i] == 1) printf("o");
							else printf(" ");
						}break;

		case 2 : 		for(i=0;i<90;i+=AngleScanDept){
							if (mapZoneFiltre2[indexDist][i] == 1) printf("o");
							else printf(" ");
						}break;

		default :		printf("Filtre invalide ! \n");
						break;
		}

		printf("\n");
	}
}


// ---------------------------------------------------------------------------
// Application d'un filtre 1 sur la mapZone
// ---------------------------------------------------------------------------
void MapZoneFiltre1(unsigned char AngleScanDept)
{
	unsigned int i;
	unsigned int massCnt = 0;
	unsigned int indexOnMap;

    // Efface de tableau de zone
    for (i = 0; i < 90; i+=AngleScanDept)
        for (indexOnMap = 0; indexOnMap < 50; indexOnMap++)
            mapZoneFiltre1[indexOnMap][i] = 0;


    // Creation de la map
    for (indexOnMap = 1; indexOnMap < 50; indexOnMap++)
    {
        for (i = 0; i < 90;i+=AngleScanDept)
            if (mapZone[indexOnMap][i] != 0)
            {
                mapZoneFiltre1[indexOnMap][i] = 1;
                massCnt++;
            }
            else
            {
                    if ((massCnt >= 1) & (mapZoneFiltre1[indexOnMap - 1][i] == 1))
                    {
                        mapZoneFiltre1[indexOnMap][i] = 1;
                        mapZoneFiltre1[indexOnMap - 1][i] = 0;
                        massCnt++;
                    }
                    else if ((massCnt >= 1) & (mapZoneFiltre1[indexOnMap + 1][i] == 1))
                    {
                        mapZoneFiltre1[indexOnMap][i] = 1;
                        mapZoneFiltre1[indexOnMap + 1][i] = 0;
                        massCnt++;
                    }
                    else massCnt = 0;
            }
    }
}



// ---------------------------------------------------------------------------
// Application d'un filtre 2 sur la mapZone
// ---------------------------------------------------------------------------
void MapZoneFiltre2(unsigned char AngleScanDept)
{
    unsigned int i;
    unsigned int indexOnMap;

    // Efface de tableau de zone
    for (i = 0; i < 90; i+=AngleScanDept)
        for (indexOnMap = 0; indexOnMap < 50; indexOnMap++)
            mapZoneFiltre2[indexOnMap][i] = 0;


    // Creation de la map
    for (indexOnMap = 1; indexOnMap < 50; indexOnMap++)
    {
        for (i = 0; i < 90; i+=AngleScanDept)
            if (mapZoneFiltre1[indexOnMap][i] ==1){
                if ((mapZoneFiltre1[indexOnMap-AngleScanDept][i] ==0) && (mapZoneFiltre1[indexOnMap+AngleScanDept][i] ==0)
                	&& (mapZoneFiltre1[indexOnMap][i-AngleScanDept] == 0) && (mapZoneFiltre1[indexOnMap][i+AngleScanDept] == 0)
                	&&  (mapZoneFiltre1[indexOnMap-AngleScanDept][i-AngleScanDept] == 0) && (mapZoneFiltre1[indexOnMap+AngleScanDept][i+AngleScanDept] == 0)
                    )
                {
                    // debug
                //    mapZoneFiltre2[indexOnMap][i] = 0;
                    mapZoneFiltre2[49][i] = 1;

                }
                else mapZoneFiltre2[indexOnMap][i] = 1;
            }
    }
}

// ---------------------------------------------------------------------------
// RECHERCHE DANS LE PLAN DE ZONE FILTREE LE MEILLEUR PASSAGE
// ---------------------------------------------------------------------------
unsigned char calcBestAngle(unsigned char AngleScanDept)
{
   // DEFINE

   unsigned int totalDegree=90;
   unsigned int splitSector = 3;

   // END DEFINE
   unsigned int i;
   unsigned int indexOnMap;
   unsigned int maxDegreePerLevel[3];

   unsigned int approxSuccMean[3];
   unsigned int slotPerSector = 0;
   unsigned int approxLevel = 0;
   unsigned int sectorScanCnt = 0;
   unsigned int startSectorSlot = 0;
   unsigned int endSectorSlot = 0;
   unsigned int maxBeamLevel = 0;
   unsigned int sectorOffset[3];
   unsigned int totalSectorOffset = 0;
   unsigned int myWays[90];
   unsigned char myOptimalAngle;
   unsigned int myOptimalDistance;

      // Efface de tableau de zone
   for (i = 0; i < 90; i+=AngleScanDept)
       for (indexOnMap = 0; indexOnMap < 50; indexOnMap++)
           myWays[i] = 0;


   // Creation de la map
   for (i = 0; i < 90; i+=AngleScanDept)
   {
       for (indexOnMap = 0; indexOnMap < 50; indexOnMap++)
       {
           if (mapZoneFiltre2[indexOnMap][i] == 0) myWays[i] += 1;
           else indexOnMap = 49;
       }
   }


   ////------------------------------------------------------------

   maxDegreePerLevel[0] = totalDegree / splitSector;
   maxDegreePerLevel[1] = maxDegreePerLevel[0] / splitSector;
   maxDegreePerLevel[2] = maxDegreePerLevel[1] / splitSector;

    for (i = 0; i < 3; i++) sectorOffset[i]=0;
   for (approxLevel = 0; approxLevel < 3; approxLevel++)
   {
       slotPerSector = maxDegreePerLevel[approxLevel];

       for (sectorScanCnt = 0; sectorScanCnt < 3; sectorScanCnt++)
       {
           startSectorSlot = sectorScanCnt*slotPerSector+ totalSectorOffset;
           endSectorSlot = (sectorScanCnt * slotPerSector)+ slotPerSector + totalSectorOffset;

           approxSuccMean[sectorScanCnt] = 0;

           for (i = startSectorSlot; i < endSectorSlot; i+=AngleScanDept)
           {
               approxSuccMean[sectorScanCnt] += myWays[i];
           }
       }

       maxBeamLevel = 0;
       // Recherche du meilleur secteur
       for (i = 0; i < splitSector; i++) if (approxSuccMean[i] > maxBeamLevel)
           {
               sectorOffset[approxLevel] = i * maxDegreePerLevel[approxLevel];
               maxBeamLevel = approxSuccMean[i];
           }

       totalSectorOffset += sectorOffset[approxLevel];

           if (maxBeamLevel == 0)
               sectorOffset[approxLevel] = 0;
   }


   ////------------------------------------------------------------



   myOptimalAngle = (startSectorSlot + endSectorSlot) / 2;
/*
   printf("------");
   for(i=0;i<90;i+=ANGLE_SCAN_DEPTH){
   							if (i == myOptimalAngle) printf("o");
   							else printf(" ");
   						}
   printf("\n");
*/
   myOptimalDistance=mapZoneFiltre2[myOptimalAngle];
   return(myOptimalAngle);

}


void hexapodCalibrateCompass(void){

	StopUltrasonicSensor();
	usleep(10000);

	// R�initialise la calibration
	compassIsCalibrate=0;

	usleep(1000000);
	setMotorsInterrupt(1);			// Active les interruption moteur en place

	printf("VITESSE 2 \n");
	// D�fini la vitesse (moyenne)
	setMotionSpeed(2);
	usleep(1000000);

	// S�quence activation des moteurs
	printf("ACTIVATION MOTEUR \n");
	setMotion(0);
	usleep(1000000);

	printf("CALIBRATION BOUSSOLE \n");
	// D�marre la calibration de la boussole
	CompassCalibrate();
	usleep(1000000);

	printf("ROTATION... \n");

	// S�quence rotation gauche
	setMotion(5);
	usleep(1000000);

	// Mise en boucle du mouvement
	setMotionLoop(1);

	sleep(29);

	// Sequence rotation droite
	setMotion(4);

	sleep(29);

	// Sequance stand-up
	setMotion(0);

	sleep(1);

	// Mise en boucle du mouvement
	setMotionLoop(0);
}
