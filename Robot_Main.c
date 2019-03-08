#include "simpletools.h"
#include "servo.h"
#include "stdio.h"
#include "sirc.h"

#include <Robot_Main.h>

/* -- Global Variables -- */
volatile int lastIRCode = 0;
volatile int emotionalState = 0;
volatile int currentState = 0; // current state within each emotional state machine
 
volatile int eyeR = 0, eyeG = 0, eyeB = 0;



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
  
  switch(currentState) {
    case 0:
      // Default State within Anger
      setServo(50,50);
      setEyebrowAngle(45, 45);
      setEyeColors(255, 0, 0);
      freqout(PIN_BUZZER, 500, 300);
      if(getProxDistance() <= 30)
      {
       currentState = 1; 
      }
      else if(isTilted())
      {
       currentState = 2; 
      }        
            
              
    break;
    
    case 1:
      // First Elevated State within Anger
      // Triggered by Proximitty Sensor
      
      setServo(100, 100);
      
      freqout(PIN_BUZZER, 300, 300);
       
      setEyeColors(0, 0, 0);
      pause(250);
      
      if(getProxDistance() > 30)
      {
       currentState = 0; 
      }            
    break;
    
    case 2:
      setServo(0, 0);
      for(int i = 300; i <= 600; i += 20)
      {
        if (emotionalState != ANGER) return;
        setEyeColors(255, 0, 0);
        freqout(PIN_BUZZER, 250, i);
        setEyeColors(0, 0, 0);
        pause(250);
        if(!isTilted())
        {
          currentState = 0;
          break;
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

// Input Functions
float getProxDistance() {
  // Returns the distance in cm to the nearest object (max 38ms), or -1 for no object
  pulse_out(PIN_PROX_TRIG, 20); // 20us pulse to trigger sensor (min 10us)
  float flightTime = pulse_in(PIN_PROX_ECHO, 1); // get length of pulse (HIGH) returned from sensor (in us)
  if (flightTime > 37000) return -1;
  return flightTime / 58.0;
  
}  

int isTilted()
{
 return 0; 
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