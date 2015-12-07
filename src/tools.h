#ifndef HEXAPOD_TOOLS_H_
#define HEXAPOD_TOOLS_H_
#endif


extern unsigned char BatteryCheck(unsigned char DisplayWarning);

// Scan la distance sur 90 degrés selon kun offset défini avec un pas et stock les valeurs à une adresse définie
extern void ScanZone90(unsigned char offsetAngle,unsigned char angleStep, unsigned char *Target);


extern void makeMapZone(unsigned char AngleScanDept, unsigned int *tabAngleDistance);
extern void displayMapZone(unsigned char AngleScanDept, unsigned char filtre);
extern void MapZoneFiltre1(unsigned char AngleScanDept);
extern void MapZoneFiltre2(unsigned char AngleScanDept);
extern unsigned char calcBestAngle(unsigned char AngleScanDept);
extern void hexapodCalibrateCompass(void);
