/*
 * loadConfig.h
 *
 *  Created on: 10 Jun 2014
 *      Author: nav
 */

#ifndef LOADCONFIG_H_
#define LOADCONFIG_H_

#define ERR_OPENFILE 0
#define NOERR 1
#define ERR_EXTRACT 2
#define ERR_CREATE_TAB 3

#include "loadconfig.h"

extern unsigned char ***myMovementsTab; 		// myMovementsTab est un tableau 3D de taille axbxc

extern unsigned char loadConfig(void);

extern unsigned char getNbOfSequences(void);				// Retourne le nombre de séquence total lue
extern unsigned char getNbOfCommands(void);					// Retourne le nombre de commandes total lue
extern unsigned char getNbOfCmdForSeq(unsigned char seqNb);	// Retourne le nombre de commandes dans une séquence donnée

extern void cleanmyMovementsTab(unsigned char seqCnt,unsigned char cmdCnt, unsigned char byteCnt);
#endif /* LOADCONFIG_H_ */
