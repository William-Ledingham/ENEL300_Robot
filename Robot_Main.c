#include "simpletools.h"
#include "servo.h"
#include "stdio.h"
#include "sirc.h"
#include "fdserial.h" // for serial communication with Arduino/gyro

#include <Robot_Main.h>

/* -- Global Variables -- */
// State Codes
volatile int lastIRCode = 0;
volatile int emotionalState = 0;
volatile int currentState = 0; // current state within each emotional state machine

// Output Globals
volatile int eyeR = 0, eyeG = 0, eyeB = 0;

// Input Globals
volatile float gyroX, gyroY, gyroZ, gyroT; // x, y, y, and total tilt as sum (updated 4 times per second), all in signed degrees
volatile float gyroXHistory[GYRO_HISTORY_COUNT], gyroYHistory[GYRO_HISTORY_COUNT], gyroTHistory[GYRO_HISTORY_COUNT]; // each stores 20 seconds/80 values, newest at History[0]

// Device Globals
fdserial *gyroSerial;

// Overall Settings
const float tiltThreshold = 15;

int main()
{
  print("Main Started.\n");
  
  // Open Gyro Serial Connection
  gyroSerial = fdserial_open(PIN_GYRO_RX, PIN_GYRO_TX, 0, 115200);
  
  zeroOutputs();  
  
  // Start Cogs
  int* IRCogInfo = cog_run(&IRSensorCog, 128);
  int* EyeCogInfo = cog_run(&pwmEyeCog, 128);
  int* GyroCogInfo = cog_run(&gyroLoggingCog, 128);
   
  // Trigger Emotional FSM's
  while(1)
  {
    switch(emotionalState)
    {    
     case DEFAULT_EMOTION:
      DefaultFSM();
      
     break;
     
     case ANGER: //Emotion 1
      AngerFSM();
     break;
     
     case FEAR: //Emotion 2
      FearFSM();
     break;
     
     case SADNESS: //Emotion 3
      SadnessFSM();
     break;
     
     case LOVE: //Emotion 4
      LoveFSM();
     break;
     
     default:

     break;
    }     
    
    pause(10);
    
  }  
}

void DefaultFSM() {
  print("Default State FSM Started.\n");
  
  setEyeColors(20, 20, 20);
  pause(100);
  setEyeColors(0, 0, 0);
  pause(500);
  
  // Print out Gyro History (debugging)
  /*
  for (int i = 0; i < GYRO_HISTORY_COUNT; i++)
    printf("%f,", gyroXHistory[i]);
  printf("\n");
  */
  
}  

void AngerFSM()
{
  print("Anger Emotion Started (State=%d)\n", currentState);
  
  switch(currentState) {
    case 0:
      // Default State within Anger
      
      // State Actions
      setServo(10,-10);
      setEyebrowAngle(450, 450);
      setEyeColors(250, 0, 0);
      
      // Next State Logic
      if(getProxDistance() <= 30 && getProxDistance() != -1)
      {
       currentState = 1; 
       printf("\tProximity Triggered (in Anger).\n");
      }
      if(getTiltStatus())
      {
       currentState = 2; 
       printf("\tTilt Triggered (in Anger).\n");

      }
              
    break;
    
    case 1:
      // First Elevated State within Anger
      // Triggered by Proximity Sensor
      
      // State Actions
      setEyeColors(250, 0 ,0);
      setServo(100, 100);
      
      freqout(PIN_BUZZER, 300, 300);
       
      setEyeColors(0, 0, 0);
      pause(250);
      
      // Next State Logic
      if(getProxDistance() > 30 || getProxDistance() == -1)
      {
       currentState = 0;
      }
      
      if(getTiltStatus())
      {
       currentState = 2; 
       printf("\tTilt Triggered (in Anger).\n");

      }
      
    break;
    
    case 2:
      // Second Elevated State within Anger
      // Triggered by Tilt Sensor
      
      // State Actions
      setServo(0, 0);
      for(int buzzerFreq = 300; buzzerFreq <= 600; buzzerFreq += 20)
      {
        if (emotionalState != ANGER) return;
        setEyeColors(250, 0, 0);
        freqout(PIN_BUZZER, 250, buzzerFreq);
        setEyeColors(0, 0, 0);
        pause(250);
        
        // Next State (Break Out) Logic
        if(!getTiltStatus())
        {
          currentState = 0;
          break; // break out of for loop
        }          
         
       
      }
    
    break;
  }    

  


}  

void FearFSM() {
  print("Fear Emotion Started.\n");
  setEyeColors(0, 100, 0);
  
  pause(500);
}  


void SadnessFSM() {
  print("Sadness Emotion Started.\n");
  setEyeColors(0, 0, 100);
  
  pause(500);
}  


void LoveFSM() {
  print("Love/Affection Emotion Started.\n");
  
  setEyeColors(190, 51, 255);
  pause(1000);
  setEyeColors(255, 51, 212);
  pause(1000);
}  

void IRSensorCog()
{
  int remoteCode;
  while(1)
  {
    sirc_setTimeout(100); // -1 if no remote code in timeout period (arg)
    remoteCode = -1;
    remoteCode = sirc_button(7);
    /* 21 - power button
     * 16 - up arrow
     * 17 - down arrow
     * 19 - left arrow
     * 18 - right arrow
     * 20 - mute
     * 37 - AV/TV
    */
    if(remoteCode != -1)
    {
      lastIRCode = remoteCode;
      //print("New IR Code sensed: %d.\n", lastIRCode);
      zeroOutputs();
      switch(lastIRCode)
      {    
       case 16: //Emotion 1
        //AngerDefault();
        emotionalState = ANGER;
        currentState = 0;
       break;
       
       case 17: //Emotion 2
        emotionalState = SADNESS;
        currentState = 0;
       
       break;
       
       case 19: //Emotion 3
        emotionalState = FEAR;
        currentState = 0;
       
       break;
       
       case 18: //Emotion 4
        emotionalState = LOVE;
        currentState = 0;
       
       break;
       
       default:
        emotionalState = 0;
       break;
      }     
    
    }
  }    
}  

void zeroOutputs()
{
  setServo(0,0);
  setEyebrowAngle(0,0);
  setEyeColors(0,0,0);
}  

// Input Functions
float getProxDistance() {
  // Returns the distance in cm to the nearest object (max 38ms), or -1 for no object
  pulse_out(PIN_PROX_TRIG, 20); // 20us pulse to trigger sensor (min 10us)
  float flightTime = pulse_in(PIN_PROX_ECHO, 1); // get length of pulse (HIGH) returned from sensor (in us)
  if (flightTime > 37000 || flightTime <= 0) return -1;
  return flightTime / 58.0;
  
}  

int getTiltStatus()
{
  // 1 if tilted, 0 if not
  return gyroT > tiltThreshold; 
}  

// Output Functions
void setServo(int leftSpeed, int rightSpeed) 
{
  servo_speed(SERVO_DRIVE_L, -leftSpeed);
  servo_speed(SERVO_DRIVE_R, rightSpeed);
}

void setEyebrowAngle(int leftEye, int rightEye)
{ 
  // Arguments take angle from -900 to 900
  servo_angle(SERVO_EYEBROW_L,  leftEye + 900);
  servo_angle(SERVO_EYEBROW_R, 900 - rightEye);
}

void setEyeColors(int r, int g, int b)
{
  eyeR = r;
  eyeG = g;
  eyeB = b;
}

void pwmEyeCog() {  
  while (1) {
    for (int i = 0; i < 255; i++) {
      if (i >= eyeR) 
        high(PIN_EYE_R);
      else 
        low(PIN_EYE_R);
        
      if (i >= eyeG) 
        high(PIN_EYE_G);
      else 
        low(PIN_EYE_G);
        
      if (i >= eyeB) 
        high(PIN_EYE_B);
      else 
        low(PIN_EYE_B);
        
      //pause(0);
      
    } // end for
  } // end while
}

void gyroLoggingCog() {
   while (1) {
     // Read the Values
     dscan(gyroSerial, "%f,%f,%f,%f", &gyroX, &gyroY, &gyroZ, &gyroT);
     

     // Shift Values in Array Right
     for (int i = GYRO_HISTORY_COUNT-1; i > 0; i--) {
       gyroXHistory[i] = gyroXHistory[i-1];
       gyroYHistory[i] = gyroYHistory[i-1];
       gyroTHistory[i] = gyroTHistory[i-1];
     }
     
     // Add current value at [0]
     gyroXHistory[0] = gyroX;
     gyroYHistory[0] = gyroY;
     gyroTHistory[0] = gyroT;       
        
   
   }     
   
}  