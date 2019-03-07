#ifndef Robot_Main_h
#define Robot_Main_h

#define ANGER 1
#define SADNESS 2
#define FEAR 3
#define LOVE 4

/* Robot_Main.c ------------------------------------*/
void enableIRSensorCog();

// Sensor and Control Functions
void setServo(int leftSpeed, int rightSpeed);
void setEyebrowAngle(int leftEye, int rightEye);
void setLEDColors(int r, int g, int b);


/* Emotion_Anger.c ---------------------------------*/
void AngerFSM();
void SadnessFSM();

#endif