#ifndef Robot_Main_h
#define Robot_Main_h

/* Emotional States --------------------------------*/
#define DEFAULT_EMOTION 0
#define ANGER 1
#define SADNESS 2
#define FEAR 3
#define LOVE 4
#define TEST_MODE 5

/* Pin Definitions ---------------------------------*/
#define PIN_EYE_R 2
#define PIN_EYE_G 1
#define PIN_EYE_B 0

#define SERVO_DRIVE_L 12
#define SERVO_DRIVE_R 14
#define SERVO_EYEBROW_L 17
#define SERVO_EYEBROW_R 16

#define PIN_PROX_TRIG 3
#define PIN_PROX_ECHO 4

#define PIN_GYRO_TX 5 // connect to Gyro's RX Pin
#define PIN_GYRO_RX 6 // connect to Gyro's TX Pin (Yellow)
// Gyro: Green to +3.3v, Blue to GND
// Gyro must be connected for program to function

#define PIN_BUTTON 8 
// 

#define PIN_BUZZER 10

#define PIN_MIC_AD 2
// Connect AUD to PIN_MIC_AD, Vcc to 3.3v, GND to GND

#define RAND_MAX 500

/* Other Macros ------------------------------------*/
#define GYRO_HISTORY_COUNT 20 // stores 20 values / 4 values per sec = 5 seconds

/* Robot_Main.c ------------------------------------*/
void IRSensorCog();
void pwmEyeCog();
void gyroLoggingCog();
void micCog();

// Sensor and Control Functions
void setServo(int leftSpeed, int rightSpeed);     // speeds are roughly from 0 (off) to 100 (full speed)
void setEyebrowAngle(int leftEye, int rightEye);  // angles are tenths of a degree
void setEyeColors(int r, int g, int b);           // rgb are ints from 0 (off) to 255 (fully on)
float getProxDistance();                          // Returns the distance in cm to the nearest object, or -1 for no object
int getTiltStatus();                              // Returns true if tilted, false if not tilted
float getTimeSinceMic();                          // Returns number of seconds since microphone last triggered
void resetOutputs();								          // zero all LED/Servo outputs for reset of states

/* Emotion_Anger.c ---------------------------------*/
void DefaultFSM();
void AngerFSM();
void FearFSM();
void SadnessFSM();
void LoveFSM();
void TestFSM();

#endif