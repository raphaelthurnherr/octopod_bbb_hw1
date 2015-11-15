#ifndef HEXAPOD_MANAGER_H
#define HEXAPOD_MANAGER_H
#endif

#define TH1_SOA	1		// Start of app mask for task
#define TH2_SOA	2		// Start of app mask for task
#define TH3_SOA	4		// Start of app mask for task
#define TH4_SOA	8		// Start of app mask for task
#define TH5_SOA	16		// Start of app mask for task
#define TH6_SOA	32		// Start of app mask for task
#define TH7_SOA	64		// Start of app mask for task
#define TH8_SOA	128		// Start of app mask for task

extern unsigned char RunningTask;
extern unsigned char EndOfApp;

extern unsigned char SystemBatteryWarning[2];

extern char SystemTaskReady;
