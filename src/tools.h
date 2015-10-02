#ifndef HEXAPOD_TOOLS_H_
#define HEXAPOD_TOOLS_H_
#endif


extern unsigned char BatteryCheck(unsigned char DisplayWarning);

// Scan la distance d'un angle donn� � un autre avec un pas et stock les valeurs � une adresse d�finie
extern void ScanZone(unsigned char fromAngle, unsigned char toAngle, unsigned char angleStep, unsigned char *Target);


extern void makeMapZone(unsigned char AngleScanDept, unsigned char *tabAngleDistance);
extern void displayMapZone(unsigned char AngleScanDept, unsigned char filtre);
extern void MapZoneFiltre1(unsigned char AngleScanDept);
extern void MapZoneFiltre2(unsigned char AngleScanDept);
extern unsigned char calcBestAngle(unsigned char AngleScanDept);
extern void hexapodCalibrateCompass(void);
