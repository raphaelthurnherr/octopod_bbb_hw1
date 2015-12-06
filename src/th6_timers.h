#ifndef TH6_TIMERS_H_
#define TH6_TIMERS_H_
#endif


extern int createTimersTask(void);
extern int killTimerTask(void);

// Timers syntétiseur de fréquence
extern unsigned char th6_timerSynFreqReadyFlag;
extern void th6_timerSynFreqStart(unsigned int xmS);
extern void th6_timerSynFreqStop(void);


// Timers 30sec pour gestionnaire de tache
extern unsigned char th6_timer30sManagerReadyFlag;
extern void th6_timer30sManagerStart(unsigned int xmS);
extern void th6_timer30sManagerStop(void);


// Timers 100ms Pour mesure de distance ultrasons
extern unsigned char th6_timerUSonicSampleReadyFlag;
extern void th6_timerUSonicSampleStart(unsigned int xmS);
extern void th6_timerUSonicSampleStop(void);

// Timers 300ms Pour mesure d'angle d'orientation
extern unsigned char th6_timerCompassReadyFlag;
extern void th6_timerCompassStart(unsigned int xmS);
extern void th6_timerCompassStop(void);

// Timers 20ms pour attente de déplacement sevomoteur
extern unsigned char th6_timerMotorEyesMoveReadyFlag;
extern void th6_timerMotorEyesMoveStart(unsigned int xmS);
extern void th6_timerMotorEyesMoveStop(void);

// Timers pour interval d'envoie des mouvements
extern unsigned char th6_timerMotionIntervalReadyFlag;
extern void th6_timerMotionIntervalInit(unsigned int xmS);
extern void th6_timerMotionIntervalStop(void);
extern void th6_timerMotionIntervalStart(void);
