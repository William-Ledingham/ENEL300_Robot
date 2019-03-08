#include "simpletools.h"
#include "servo.h"
#include "stdio.h"
#include "sirc.h"

#include <Robot_Main.h>

/* -- Global Variables -- */
 int lastIRCode = 0;
 int emotionalState = 0;



int main()
{
  
  //int state = 0;
  //int remoteCode;
  int* coginfo = cog_run(&IRSensorCog, 2000);
 
  while(1)
  {
    switch(emotionalState)
    {    
     case ANGER: //Emotion 1
      AngerFSM();

     break;
     
     case FEAR: //Emotion 2
     
     break;
     
     case SADNESS: //Emotion 3
      SadnessFSM();
     
     break;
     
     case LOVE: //Emotion 4
     
     break;
     
     default:

     break;
    }     
    
    pause(10);
    
  }  
}

void AngerFSM()
{
      printf("Anger Emotion Started.\n");

      freqout(14, 3000, 500);
      
      if (emotionalState != ANGER) return;
      
      freqout(14, 20000, 500);


}  

void SadnessFSM() {
  printf("Sadness Emotion Started.\n");
}  

void IRSensorCog()
{
  int remoteCode;
  while(1)
  {
    sirc_setTimeout(100); // -1 if no remote code in 1s
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
      printf("New IR Code sensed: %d.\n", lastIRCode);
      
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

void setLEDColors(int r, int g, int b)
{
  
}  