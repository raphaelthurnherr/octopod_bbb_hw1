#ifndef TH1_CORE_H_
#define TH1_CORE_H_
#endif

#define TRUE					1
#define FALSE					0

#define NO_COMMAND				0
#define TEST_SEARCH_SCAN		1
#define TEST_SEARCH_BEST_WAY	2
#define TEST_SEARCH_MOVING		3
#define TEST_BUZZER				4
#define TEST_LEGS				5

#define REMOTE_RUN_AI			6
#define RUN_MOTION				7
#define RUN_MOTION_LOOP			8
#define RUN_SPEED				9
#define RUN_STOP				10
#define CHANGEMOTION 			11

#define TEST_COMPASS			12
#define CALIB_COMPASS			13
#define INT_COMPASS				14
#define CALIB_HEXAPOD			15

#define MOTION_BODY_UP			0
#define MOTION_BODY_DW			1

#define MOTION_HEAD_UP			2
#define MOTION_HEAD_DW			3

#define MOTION_BODY_FORWARD		4
#define MOTION_BODY_BACK		5

#define MOTION_HAND_OPEN		6
#define MOTION_HAND_CLOSE		7

#define MOTION_RESET			9


extern struct SysStatus SystemStatus;
extern struct ApplicationStatus OctopodStatus;
extern struct hwStatus LLHwStatus;
extern struct MotStatus MotorsStatus;
extern struct SensStatus SensorsStatus;
extern struct WayFinder MeasureMap;

extern int createCoreTask(void);
extern int killCoreTask(void);


void UICommand(unsigned char Command);
