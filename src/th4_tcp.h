#ifndef TH2_TCP_H_
#define TH2_TCP_H_
#endif

#define STANDUP 0
#define FORWARD 1
#define ATTACK 	2
#define LATLEFT  3
#define ROTLEFT 4
#define ROTRIGHT 5
#define LATRIGHT 6
#define SLEEPOFF 7
#define BACK	 8
#define DANCE 	9
#define STRECHUPDW 10



extern int createTcpTask(void);
extern int killTcpTask(void);

