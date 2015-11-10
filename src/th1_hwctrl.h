#ifndef TH1_HWCTRL_H_
#define TH1_HWCTRL_H_
#endif

#define NB_TOTAL_OF_MOTORS	26

#define EYES_MOTOR 18


// Masque d' affichage des ACK UART
#define UART_ACK_ALL_UNDECODED 1
#define UART_ACK_MOTORS 2
#define UART_ACK_ULTRASONIC 4
#define UART_ACK_HEARTBIT 8
#define UART_ACK_COMPASS 16
#define UART_ACK_IR 32

extern void HWctrl_displayAckToggle(unsigned char ackType);

extern unsigned char th1_LowBatteryCheck(void); // Retourne (0:no warning, 1:batt A low, 2:batt B low, 3, Both Low)

extern void buzzerCtrl(unsigned char bipCount);	// S�quence de bip � produire

// Configurtion buzzer (ON:1/OFF:0, timeON [ms], timeOFF [ms], Frequence BIP [ms])
extern void buzzerConfig(unsigned char On, unsigned int t1, unsigned int t2, unsigned char tHF);

extern void setMotorAngle(unsigned char motor, unsigned char angle, unsigned char speed);	// Attribue un angle au servo moteur
extern void setMotorState(unsigned char motor, unsigned char state); 						// d�fini l'�tat ON/OFF du moteur
extern void setMotorsInterrupt(unsigned char Enable); // Active/d�sactive l'interruption lorsques les moteur sont en position

// Envoie une trame comp�te pr�d�finie
extern void sendUartFrame(unsigned char *buffToSend, unsigned char nbByte);

// Envoie d'une trame vers l'afficheur LCD
void sendLCDUartFrame(unsigned char *buffToSend, unsigned char nbByte);

// Retourne La distance � un angle donn�
extern void getUltrasonicDistance(unsigned char angle);
extern void ReadStartUltrasonicSensor(void);
extern void StopUltrasonicSensor(void);

// Retourne l'angle d'orientation
extern void ReadStartCompassSensor(void);
extern void StopCompassSensor(void);
extern void StartCompassMeasure(void);
// Calibration de la boussole
extern void CompassCalibrate(void);
extern void SetCompassInterruptsRange(unsigned int angleMin, unsigned int angleMax);
extern void SetCompassInterruptEnable(unsigned char OnOff);

extern unsigned char compassIsCalibrate;


extern void SetIRinterrupts(unsigned char Enable);
extern unsigned char IRdetectValid;
extern unsigned char IRdetectValue[3];

// Valeur de distance lue [0] � l'angle [1]
extern unsigned int ultrasonicDistance[2];

// Etat de la connexion avec le controlleur de moteurs
extern unsigned char controllerConnected;

// Angles actuelle des Moteurs (Apr�s quittance du controlleur)
extern unsigned char motorsActualAngle[NB_TOTAL_OF_MOTORS];
// Etat actuelle des Moteurs (Apr�s quittance du controlleur)
extern unsigned char motorsActualState[NB_TOTAL_OF_MOTORS];

extern unsigned char motorsPositionValid;

extern unsigned char smoothValue;
// Demarre une commande heartbit pour connaitre l'�tat du controller;
extern void getControllerHeartBit(void);

extern int createHwctrlTask(void);
extern int killHwctrlTask(void);

