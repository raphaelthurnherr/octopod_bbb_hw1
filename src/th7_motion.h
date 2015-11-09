#ifndef TH7_HEXAPOD_MOTION_H_
#define TH7_HEXAPOD_MOTION_H_
#endif

#define ERR	  0
#define NOERR 1

extern unsigned char th7_displayMotionSeq(unsigned char SelectedSeq);

extern void setMotion(unsigned char motionNb);
extern void setMotionLoop(unsigned char motionLoopON);
extern void setMotionSpeed(unsigned char motionSpeed);

extern signed char motorLegsCorrection[24];

extern unsigned char runMotion;

extern int createMotionTask(void);
extern int killMotionTask(void);

