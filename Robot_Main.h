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


/* Robot_Main.c ------------------------------------*/
void IRSensorCog();
void pwmEyeCog();

// Sensor and Control Functions
void setServo(int leftSpeed, int rightSpeed);
void setEyebrowAngle(int leftEye, int rightEye);
void setEyeColors(int r, int g, int b);


/* Emotion_Anger.c ---------------------------------*/
void AngerFSM();
void FearFSM();
void SadnessFSM();
void LoveFSM();

#endif