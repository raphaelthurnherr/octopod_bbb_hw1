#ifndef TH1_HWCTRL_H_
#define TH1_HWCTRL_H_
#endif

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

extern void buzzerCtrl(unsigned char bipCount);	// Séquence de bip à produire

// Configurtion buzzer (ON:1/OFF:0, timeON [ms], timeOFF [ms], Frequence BIP [ms])
extern void buzzerConfig(unsigned char On, unsigned int t1, unsigned int t2, unsigned char tHF);

extern void setMotorAngle(unsigned char motor, unsigned char angle, unsigned char speed);	// Attribue un angle au servo moteur
extern void setMotorState(unsigned char motor, unsigned char state); 						// défini l'état ON/OFF du moteur
extern void setMotorsInterrupt(unsigned char Enable); // Active/désactive l'interruption lorsques les moteur sont en position

// Envoie une trame compète prédéfinie
extern void sendUartFrame(unsigned char *buffToSend, unsigned char nbByte);

// Retourne La distance à un angle donné
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

// Valeur de distance lue [0] à l'angle [1]
extern unsigned int ultrasonicDistance[2];

// Etat de la connexion avec le controlleur de moteurs
extern unsigned char controllerConnected;

// Angles actuelle des Moteurs (Après quittance du controlleur)
extern unsigned char motorsActualAngle[20];
// Etat actuelle des Moteurs (Après quittance du controlleur)
extern unsigned char motorsActualState[20];

extern unsigned char motorsPositionValid;

extern unsigned char smoothValue;
// Demarre une commande heartbit pour connaitre l'état du controller;
extern void getControllerHeartBit(void);

extern int createHwctrlTask(void);
extern int killHwctrlTask(void);

