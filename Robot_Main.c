#include "simpletools.h"
#include "servo.h"
#include "stdio.h"
#include "sirc.h"

#include <Robot_Main.h>

/* -- Global Variables -- */



int main()
{
  int state = 0;
  int remoteCode;
  sirc_setTimeout(1000); // -1 if no remote code in 1s
 
  while(1)
  {
    
    remoteCode = sirc_button(7);
    /* 21 - power button
     * 16 - up arrow
     * 17 - down arrow
     * 19 - left arrow
     * 18 - right arrow
     * 20 - mute
     * 37 - AV/TV
    
    if(remoteCode != -1)
    {
      state = remoteCode;
    }
    
    /*
    switch(state)
    {    
     case : //Emotion 1
     
     break;
     
     case ; //Emotion 2
     
     break;
     
     case : //Emotion 3
     
     break;
     
     case : //Emotion 4
     
     break;
     
     default:
     
    }     */ 
    
  }  
}


void setServo(int leftSpeed, int rightSpeed) 
{
  servo_speed(12, -leftSpeed);
  servo_speed(14, rightSpeed);
}

/* Arguments take angle from -900 to 900 */
void setEyebrowAngle(int leftEye, int rightEye)
{ 
  servo_angle(17,  leftEye + 900);
  servo_angle(16, 900 - rightEye);
}