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