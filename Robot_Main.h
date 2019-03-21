#ifndef Robot_Main_h
#define Robot_Main_h

/* Emotional States --------------------------------*/
#define DEFAULT_EMOTION 0
#define ANGER 1
#define SADNESS 2
#define FEAR 3
#define LOVE 4

/* Pin Definitions ---------------------------------*/
#define PIN_EYE_R 2
#define PIN_EYE_G 1
#define PIN_EYE_B 0

#define SERVO_DRIVE_L 12
#define SERVO_DRIVE_R 14
#define SERVO_EYEBROW_L 16
#define SERVO_EYEBROW_R 17

#define PIN_PROX_TRIG 3
#define PIN_PROX_ECHO 4

#define PIN_GYRO_TX 5 // connect to Gyro's RX Pin
#define PIN_GYRO_RX 6 // connect to Gyro's TX Pin (Yellow)
// Gyro: Green to +3.3v, Blue to GND
// Gyro must be connected for program to function

#define PIN_BUZZER 10


/* Other Macros ------------------------------------*/
#define GYRO_HISTORY_COUNT 80

/* Robot_Main.c ------------------------------------*/
void IRSensorCog();
void pwmEyeCog();
void gyroLoggingCog();

// Sensor and Control Functions
void setServo(int leftSpeed, int rightSpeed);     // speeds are roughly from 0 (off) to 100 (full speed)
void setEyebrowAngle(int leftEye, int rightEye);  // angles are tenths of a degree
void setEyeColors(int r, int g, int b);           // rgb are ints from 0 (off) to 255 (fully on)
float getProxDistance();                          // Returns the distance in cm to the nearest object, or -1 for no object
int getTiltStatus();                              // Returns true if tilted, false if not tilted

/* Emotion_Anger.c ---------------------------------*/
void DefaultFSM();
void AngerFSM();
void FearFSM();
void SadnessFSM();
void LoveFSM();

#endif