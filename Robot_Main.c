#include "simpletools.h"
#include "servo.h"
#include "stdio.h"
#include "sirc.h"

#include <Robot_Main.h>

/* -- Global Variables -- */
 int lastIRCode = 0;
 int emotionalState = 0;
 
 int eyeR = 0, eyeG = 0, eyeB = 0;



int main()
{
  
  //int state = 0;
  //int remoteCode;
  int* IRCogInfo = cog_run(&IRSensorCog, 128);
  int* EyeCogInfo = cog_run(&pwmEyeCog, 128);
  
  print("Main Started.\n");
 
  while(1)
  {
    switch(emotionalState)
    {    
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

void AngerFSM()
{
  print("Anger Emotion Started.\n");

  setEyeColors(230, 126, 34);
  freqout(14, 500, 800);
  
  if (emotionalState != ANGER) return;
  
  setEyeColors(128, 128, 0);
  freqout(14, 500, 500);


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
      
      switch(lastIRCode)
      {    
       case 16: //Emotion 1
        //AngerDefault();
        emotionalState = ANGER;
       break;
       
       case 17: //Emotion 2
        emotionalState = SADNESS;
       
       break;
       
       case 19: //Emotion 3
        emotionalState = FEAR;
       
       break;
       
       case 18: //Emotion 4
        emotionalState = LOVE;
       
       break;
       
       default:
        emotionalState = 0;
       break;
      }     
    
    }
  }    
}  

// Input Functions
float getProxDistance() {
  // Returns the distance in cm to the nearest object, or -1 for no object
  pulse_out(PIN_PROX_TRIG, 20); // 20us pulse to trigger sensor (min 10us)
  float distance = pulse_in(PIN_PROX_ECHO, 1); // get length of pulse (HIGH) returned from sensor (in us)
  if (distance > 37000) return -1;
  return distance / 58.0;
  
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
  int dutyResolutionTime = 1; // in ms
  
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