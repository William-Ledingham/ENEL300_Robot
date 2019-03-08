#ifndef Robot_Main_h
#define Robot_Main_h

/* Emotional States --------------------------------*/
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


/* Robot_Main.c ------------------------------------*/
void IRSensorCog();
void pwmEyeCog();

// Sensor and Control Functions
void setServo(int leftSpeed, int rightSpeed);     // speeds are roughly from 0 (off) to 100 (full speed)
void setEyebrowAngle(int leftEye, int rightEye);  // angles are tenths of a degree
void setEyeColors(int r, int g, int b);           // rgb are ints from 0 (off) to 255 (fully on)
float getProxDistance();                          // Returns the distance in cm to the nearest object, or -1 for no object

/* Emotion_Anger.c ---------------------------------*/
void AngerFSM();
void FearFSM();
void SadnessFSM();
void LoveFSM();

#endif