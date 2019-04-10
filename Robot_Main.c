#include "simpletools.h"
#include "servo.h"
#include "stdio.h"
#include "sirc.h"
#include "fdserial.h" // for serial communication with Arduino/gyro
#include "adcDCpropab.h" // for AD analog/digital reading
#include "mstimer.h" // for timing, mostly for mic


#include <Robot_Main.h>

/* -- Global Variables -- */
// State Codes
volatile int lastIRCode = 0;
volatile int emotionalState = 0;
volatile int currentState = 0; // current state within each emotional state machine
volatile int timesad;

// Output Globals
volatile int eyeR = 0, eyeG = 0, eyeB = 0;

// Input Globals
volatile float gyroX, gyroY, gyroZ, gyroT; // x, y, y, and total tilt as sum (updated 4 times per second), all in signed degrees
volatile float gyroXHistory[GYRO_HISTORY_COUNT], gyroYHistory[GYRO_HISTORY_COUNT], gyroTHistory[GYRO_HISTORY_COUNT]; // each stores 4 val/sec, newest at History[0]
volatile float micLastTrig = 0; // time in seconds microphone was last triggered
volatile int gyroRockCount = 0; // number of alternating rockings in the last GYRO_HISTORY_COUNT period (at 4 values/sec)

// Device Globals
fdserial *gyroSerial;

// Overall Settings
const float tiltThreshold = 15;
const float micPeriod = 0.33; // period in s; 3Hz sampling (measure pk-pk this often)
const int micThresholdPk = 3; // pk-pk threshold (volts)
const int gyroRockThreshold = 2; // number of rocks required to trigger

int main()
{
  print("Main Started.\n");
  
  // Start timer
  mstime_start();
  
  // Open Gyro Serial Connection
  gyroSerial = fdserial_open(PIN_GYRO_RX, PIN_GYRO_TX, 0, 115200);
  
  // Open A/D Connection
  adc_init(21, 20, 19, 18);
  
  // Reset all Outputs to Default Setting
  resetOutputs();
  
  // Start Cogs
  int* IRCogInfo = cog_run(&IRSensorCog, 128);
  int* EyeCogInfo = cog_run(&pwmEyeCog, 128);
  int* GyroCogInfo = cog_run(&gyroLoggingCog, 128);
  int* MicCogInfo = cog_run(&micCog, 128);
   
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
     
     case TEST_MODE: //Test Mode
      TestFSM();
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
  
}  

void AngerFSM()
{
  //print("Anger Emotion Started (State=%d)\n", currentState);
  
  switch(currentState) {
    case 0:
      // Default State within Anger
      
      // State Actions
      setServo(30,-30);
      setEyebrowAngle(450, 450);
      setEyeColors(250, 0, 0);
      
      // Next State Logic
      if(getProxDistance() <= 30 && getProxDistance() != -1)
      {
       currentState = 1; 
       //print("\tProximity Triggered (in Anger).\n");
      }
      if(getTiltStatus())
      {
       currentState = 2; 
       //print("\tTilt Triggered (in Anger).\n");

      }
              
    break;
    
    case 1:
      // First Elevated State within Anger
      // Triggered by Proximity Sensor
      
      // State Actions

      if(getProxDistance() < 5 || getProxDistance() > 330)
      {
        for(int i = 0; i < 10; i++)
        {
          setEyeColors(250, 0, 0);
          if(i < 5)
          {
            setServo(-100, -100);
          }
          else
          {
            setServo(100, 100);
          }
          freqout(PIN_BUZZER, 100, 300);
          setEyeColors(0, 0, 0);
          pause(100);
          if(getProxDistance() >= 5)
          {
            break;
          }
          
        }
      }
      else
      {
        setEyeColors(250, 0 ,0);
        setServo(100, 100);
      
        freqout(PIN_BUZZER, 300, 300);
       
        setEyeColors(0, 0, 0);
        pause(250);
      
      }        
      
      // Next State Logic
      if(getProxDistance() > 30 || getProxDistance() == -1)
      {
       currentState = 0;
      }
      
      if(getTiltStatus())
      {
       currentState = 2; 
       print("\tTilt Triggered (in Anger).\n");

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


void FearFSM()
{
 
	switch(currentState)
	{
		case 0:
	
			setEyebrowAngle(-450, -450);
			setEyeColors(30, 30, 30);
			
			for (int i=0; i<=360; i++)
			{
				
				if(i%2 == 1)
				{
					setServo(15,-15);
				}
				else
				{
					setServo(-15,15);
				}                    
				pause(120);
				
				
				if(i>30)
				{
					if(getProxDistance() <= 20 && getProxDistance() != -1)
					{
						currentState = 2; 
						print("\tProximity Triggered (in Fear 0).\n");
						break;
					} 
					else if(getTimeSinceMic() <= 2 ) 
					{
						currentState = 1; 
						print("\tMic Triggered (in Fear 0).\n");
						break;
					}

				} 
      	if (emotionalState != FEAR) return;

			}
			
		
		break;
		
		case 1:
		
			print("\tState1 Triggered (in Fear).\n");
			
			for(int j = 1; j <= 3; j++)
			{			
				for(int i = 0; i < 40 ; i++)
				{					
					if((i >= 10 && i < 20) || (i >= 30 && i < 40))
					{					
						if(i%2 == 1)
						{
							setServo(15,-15);
						}
						else
						{
							setServo(-15,15);
						}                    
						pause(120);
					}
					else if ( i >= 0 && i < 10)
					{
						setServo(90,-90);
						pause(20);
					}	
					else if ( i >= 20 && i < 30)
					{
						setServo(-90,90);
						pause(20);
					}
				
					if (emotionalState != FEAR) return;
				
					if(getProxDistance() <= 30 && getProxDistance() != -1 && ((i >= 10 && i < 20) || (i >= 30 && i < 40)))
					{
						currentState = 2; 
						print("\tProximity Triggered (in Fear 1).\n");
						return;
					}
     

      
				}
			}
			currentState = 0;
		
		break;
		
		
		case 2:
		  
      //setEyeColors(200, 200, 200);
      setServo(100, -100);
      freqout(PIN_BUZZER, 800, 800);
      pause(800);
      for(int i = 0; i < 10; i++)
      {
         setServo(100, 100);
         pause(300);
         freqout(PIN_BUZZER, 200, 800);
         
         if (emotionalState != FEAR) return;
         
         if(getProxDistance() <= 30 && getProxDistance() != -1)
         {
            currentState = 2;
            return; 
         }           
      }        
      currentState = 0;
		
		break;

	}
  
}  


void SadnessFSM() {
  print("Sadness Emotion Started (State=%d)\n", currentState);
  int dspeed;
  int count;
  int lspeed;
  int rspeed;
  int x;
  //int timesad;
  
  switch(currentState) {
    case 0:
      // Default State within Sadness
      
      // State Actions
      
      dspeed = 20;
      count = 0;
      
      while(1)
      {
        if (emotionalState != SADNESS) return;
        
        if (count % 15 == 0) 
          dspeed = -dspeed;
          
        setServo(dspeed, dspeed);
        pause(100);
        setEyebrowAngle(-450, -450);
        setEyeColors(0, 0, 100);
      
        // Next State Logic
        if(getTimeSinceMic() <= 2)
        {
          currentState = 1; 
          print("\tMicrophone Triggered (in Sadness).\n");
          break;
        }
        count++;
      }      
              
    break;
    
    case 1:
      // First Elevated State within Sadness
      // Triggered by Microphone Sensor
      
      // State Actions
      
      lspeed = 20;
      rspeed = 30;
      x = 10;
      timesad = 0;
      
      while (1)
      {
        if (emotionalState != SADNESS) return;
        
        setServo(lspeed, rspeed);
        setEyeColors(0, 0 ,100);
        pause(500);
        setEyeColors(0, 30, 100);
        pause(500);
        
        lspeed += x;
        x = -x;
        rspeed +=x;
        timesad += 1000;
        
        // Next State Logic
        if(getProxDistance() <= 20 && getProxDistance() != -1)
        {
          currentState = 2;
          print("\tUltrasonic Triggered (in Sadness).\n");
          break;
        }
      }        
      
    break;
    
    case 2:
      // Second Elevated State within Sadness
      // Triggered by Ultrasonic Sensor
      
      // State Actions
      
      setServo(0, 0);
      
      for(int i = 0; i <= 5; i++)
      {
        if (emotionalState != SADNESS) return;
        
        setEyebrowAngle(-400, -400);
        setEyeColors(0, 0, 100);
        pause(200);
        setEyeColors(0, 15, 100);
        freqout(PIN_BUZZER, 250, 2000);
        setEyebrowAngle(-500, -500);
        setEyeColors(0, 30, 100);
        pause(200);
        setEyeColors(0, 15, 100);
        freqout(PIN_BUZZER, 250, 2000);
        setEyebrowAngle(-400, -400);
        setEyeColors(0, 0, 100);
        pause(100);
        setEyeColors(0, 15, 100);
        pause(100);
        setEyeColors(0, 30, 100);
        pause(100);
        setEyeColors(0, 15, 100);
        freqout(PIN_BUZZER, 400, 1000);             
      }
      
      
      setServo(30, -30);
      pause(4500);
      setServo(25, 25);
      pause(timesad);
      setServo(-30, 30);
      pause(4500);
      currentState = 0;
      break;
  }  
}  


void LoveFSM() {
  print("Love Emotion Started (State=%d)\n", currentState);
  
  // Set the pulse eye colors
  int r1 = 80, g1 = 0, b1 = 80;
  int r2 = 255, g2 = 160, b2 = 202;
  float colorChangePeriod; // ms (transition time, full period)
  int defaultEyebrowAngle = -200;
  
  switch (currentState) {
    case 0:
      // Default State
      setEyebrowAngle(defaultEyebrowAngle, defaultEyebrowAngle);
      
      // Fade Between Colors
      colorChangePeriod = 5000;
      for (float i = 0; i < 1.0; i += 0.01) {
        setEyeColors(r1 + (r2-r1)*i, g1 + (g2-g1)*i, b1 + (b2-b1)*i);
        
        //pause(colorChangePeriod / 200);
        if ((int)(i*300) % 2 == 0)
          freqout(PIN_BUZZER, colorChangePeriod/200, 300);
        else
          pause(colorChangePeriod/200);
        
        // Check Next State
        if (getTiltStatus()) {
          print("\tTilt Sensor Triggered (Into State 2)\n");
          currentState = 2;
          break;
        }   
        else if (getProxDistance() < 30) {
          print("\tProx Sensor Triggered (Into State 1)\n");
          currentState = 1;
          break;
        }
        if (emotionalState != LOVE) return;
        
      }        
      for (float i = 1.0; i > 0; i -= 0.01) {
        setEyeColors(r1 + (r2-r1)*i, g1 + (g2-g1)*i, b1 + (b2-b1)*i);
        pause(colorChangePeriod / 200);
        
        
        // Check Next State
        if (getTiltStatus()) {
          print("\tTilt Sensor Triggered (Into State 2)\n");
          currentState = 2;
          break;
        }   
        else if (getProxDistance() < 30) {
          print("\tProx Sensor Triggered (Into State 1)\n");
          currentState = 1;
          break;
        }
        if (emotionalState != LOVE) return;
        
      }    
      
      // Make Robot Wink
      for (int i = 0; i < 3; i++) {
        // Wink
        setEyebrowAngle(-600, -600);
        pause(200);
        setEyebrowAngle(0, 0);
        pause(600);
        
        
        // Check Next State
        if (getTiltStatus()) {
          print("\tTilt Sensor Triggered (Into State 2)\n");
          currentState = 2;
          break;
        }  
        else if (getProxDistance() < 30) {
          print("\tProx Sensor Triggered (Into State 1)\n");
          currentState = 1;
          break;
        } 
        if (emotionalState != LOVE) return;
      
      }        
      setEyebrowAngle(defaultEyebrowAngle, defaultEyebrowAngle);
           
      
    break;
    
    case 1:
      // Run toward object and nudge it
      
      // Run forward up to 4s, or until it hits hand
      print("\tForward up to 4s or until prox<5. Prox: ");
      for (int i = 0; i < 10 && getProxDistance() > 10 && getProxDistance() < 30; i++) {
        print("%f,", getProxDistance());
        setServo(60, 60);
        pause(400);
        
        if (getTiltStatus()) {
          currentState = 2;
          return;
        }   
      }
      print("\n");
      
      // Make it go forward just a little more to nudge
      setServo(30, 30);
      pause(800);
      
      // Make it go side to side (cuddle nudge)
      setEyeColors(128, 0, 128);
      for (int i = 0; i < 4; i++) {
        setServo(40, -40);
        //pause(200);
        freqout(PIN_BUZZER, 200, 10000);
        setServo(-40, 40);
        pause(200);
        if (getTiltStatus()) {
          currentState = 2;
          return;
        }          
      }        
      
      // Back up for 1s
      if (getProxDistance() < 30) {
        print("\tBackwards 1s.\n");
        setServo(-40, -40);
        pause(1000);
      }        
      setServo(0, 0);
      
    
      // Prox Sensor Triggered, Run at Object and Nudge
      if (getProxDistance() > 30 && getProxDistance() < 338) {
        // No object, go back to default state
        if (getTiltStatus()) {
          currentState = 2;
        }
        else {         
          currentState = 0;
        }          
      }
      
      
    break;
    
    case 2:
      // Robot is picked up/tilted
      
      setServo(0, 0);
      
      // Fade Between Colors
      colorChangePeriod = 1000;
      for (float i = 0; i < 1.0; i += 0.01) {
        setEyeColors(r1 + (r2-r1)*i, g1 + (g2-g1)*i, b1 + (b2-b1)*i);
        
        
        //pause(colorChangePeriod / 200);
        
        if ((int)(i*1000) % 2 == 0)
          freqout(PIN_BUZZER, colorChangePeriod/200, 1000);
        else
          pause(colorChangePeriod/200);
        
        // Next State Logic      
        if (!getTiltStatus()) {
          // Return to default state if set down
          currentState = 0;
          break;
        }
        if (emotionalState != LOVE) return;
      }        
      for (float i = 1.0; i > 0; i -= 0.01) {
        setEyeColors(r1 + (r2-r1)*i, g1 + (g2-g1)*i, b1 + (b2-b1)*i);
        pause(colorChangePeriod / 200);
        
        // Next State Logic      
        if (!getTiltStatus()) {
          // Return to default state if set down
          currentState = 0;
          break;
        }
        if (emotionalState != LOVE) return;
      }
      
      // Next State Logic      
      if (!getTiltStatus()) {
        // Return to default state if set down
        currentState = 0;
      }
      
      // Check if the history contains some negative and positive tilt values (indicating rocking)
      if (gyroRockCount > gyroRockThreshold) {
        print("\tGyro rocked greater than %d times.\n", gyroRockThreshold);
        currentState = 3;
      }
      
                      
    break;
    
    case 3:
      // Robot being rocked
      setServo(0, 0);
      setEyebrowAngle(-200, -200);
      
      // Fade Between Colors
      colorChangePeriod = 500;
      for (float i = 0; i < 1.0; i += 0.01) {
        setEyeColors(r1 + (r2-r1)*i, g1 + (g2-g1)*i, b1 + (b2-b1)*i);
        
        //pause(colorChangePeriod / 200);
        freqout(PIN_BUZZER, colorChangePeriod/200, i*1000 + 200);
        
        if (i > 0.2 && i < 0.4) setEyebrowAngle(0, 0);
        else setEyebrowAngle(-200, -200);
        
        if (emotionalState != LOVE) return;
      }        
      for (float i = 1.0; i > 0; i -= 0.01) {
        setEyeColors(r1 + (r2-r1)*i, g1 + (g2-g1)*i, b1 + (b2-b1)*i);
        
        //pause(colorChangePeriod / 200);
        freqout(PIN_BUZZER, colorChangePeriod/200, i*1000 + 200);
        
        
        if (i > 0.2 && i < 0.3) setEyebrowAngle(-600, -600);
        else setEyebrowAngle(-200, -200);
        
        if (emotionalState != LOVE) return;
      }
      
      // Next State
      if (gyroRockCount == 0 && !getTiltStatus()) {
        print("\tRobot set down.\n");
        currentState = 0;
      }       
      
      pause(400); 
      
    break;
        
  }
  
  
}  

void TestFSM() {
  // Test Mode for Testing IO Devices
  
  print("Test Mode Started.\n");
  
  switch (currentState) {
  
    // Gyro Testing
    case 0:
    print("Gyroscope Test:\n");
      for (int i = 0; i < 5; i++) {
        print("\tTry tilting robot. Current Gyroscope Result: ");
        if (getTiltStatus())
          print("Tilted.\n");
        else
          print("Not Tilted.\n");
          
        pause(1000);
      }
    break;
    
    case 1:  
      // Microphone Testing
      print("Microphone Test:\n");
      for (int i = 0; i < 5; i++) {
        print("\tMake loud noise. Time since last microphone trigger: %fs.\n", getTimeSinceMic());
        pause(2000);
      }
    break;
    
    case 2:
      // Prox Sensor Testing
      print("Proximity Sensor Test:\n");
      for (int i = 0; i < 5; i++) {
        print("\tGet near proximity sensor.  Distance to object: %fcm.\n", getProxDistance());
        pause(2000);
      }        
    break;
    
    case 3:
      // RGB LED Output Testing
      print("RGB LED Test:\n");
      
      print("\tStarting RED Test from 0 to Full to 0.\n");
      for (int i = 0; i <= 255; i++) {
        setEyeColors(i, 0, 0);
        pause(10);
      }
      for (int i = 255; i >= 0; i--) {
        setEyeColors(i, 0, 0);
        pause(10);
      }
              
      print("\tStarting GREEN Test from 0 to Full to 0.\n");
      for (int i = 0; i <= 255; i++) {
        setEyeColors(0, i, 0);
        pause(10);
      }
      for (int i = 255; i >= 0; i--) {
        setEyeColors(0, i, 0);
        pause(10);
      }
              
      print("\tStarting BLUE Test from 0 to Full to 0.\n");
      for (int i = 0; i <= 255; i++) {
        setEyeColors(0, 0, i);
        pause(10);
      }
      for (int i = 255; i >= 0; i--) {
        setEyeColors(0, 0, i);
        pause(10);
      }
              
      setEyeColors(0,0,0);
      
    break;
    
    case 4:
      // Servo Testing
      print("Servo Test:\n");
      
      print("\tTesting Eyebrows: -90 Degress, wait 1 second.\n");
      setEyebrowAngle(-900, -900);
      pause(1000);
      print("\tTesting Eyebrows: +90 Degress, wait 1 second.\n");
      setEyebrowAngle(900, 900);
      pause(1000);
      
      print("\tTesting Drive Servos: Full Speed Forwards for 1 second.");
      setServo(100, 100);
      pause(1000);
      print("\tTesting Drive Servos: Full Speed Backwards for 1 second.");
      setServo(-100, -100);
      pause(1000);
      
      setServo(0, 0);
      
    break;
    
    case 5:
      // Buzzer Testing
      print("\tBuzzer Testing with Increasing Frequencies (100 to 4000) for 300ms.\n");
      for (int freq = 100; freq <= 4000; freq += 100)
        freqout(PIN_BUZZER, 300, freq);
        
     break;
        
  }    
    
  // Increase currentState
  currentState = (currentState+1) % 6; // currentState+1, mod (max state number + 1)
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
      resetOutputs();
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
       
       case 20: // Test Mode (Mute)
        emotionalState = TEST_MODE;
        currentState = 0;
        
       break;
       
       default:
        emotionalState = 0;
       break;
      }     
    
    }
    
    // Check Button State
    if (input(PIN_BUTTON)) {
      emotionalState = (emotionalState + 1) % 5;
      
      resetOutputs();
      
      pause(500); // stupid person debouncing
    }            
    
  }   // end while 
}  

void resetOutputs()
{
  pause(100);
  setServo(0,0);
  setEyebrowAngle(0,0);
  setEyeColors(0,0,0);
  
  pause(100);
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

float getTimeSinceMic() {
  // Returns the number of seconds since the microphone was last triggered
  return mstime_get()/1000.0 - micLastTrig; // current time - mic last trig time
}

// Output Functions
void setServo(int leftSpeed, int rightSpeed) 
{
  servo_speed(SERVO_DRIVE_L, leftSpeed);
  servo_speed(SERVO_DRIVE_R, -rightSpeed);
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
     //print("\tRead Gyro: %f,%f,%f,%f", gyroX, gyroY, gyroZ, gyroT); // Run from cog, will fail

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
     
     // Determine if it's being rocked
     gyroRockCount = 0;
      for (int i = 0; i < GYRO_HISTORY_COUNT-1; i++) {
        // Test for X-Axis
        if (gyroXHistory[i] > tiltThreshold && gyroXHistory[i+1] < -tiltThreshold)
          // current is +tilt and last was -tilt
          gyroRockCount ++;
        if (gyroXHistory[i] < -tiltThreshold && gyroXHistory[i+1] > tiltThreshold)
          // current is -tilt and last was +tilt
          gyroRockCount ++;
          
        // Test for Y-Axis
        if (gyroYHistory[i] > tiltThreshold && gyroYHistory[i+1] < -tiltThreshold)
          // current is +tilt and last was -tilt
          gyroRockCount ++;
        if (gyroYHistory[i] < -tiltThreshold && gyroYHistory[i+1] > tiltThreshold)
          // current is -tilt and last was +tilt
          gyroRockCount ++;
      }
      
      // Debug Printing for Gyro Rocking Sensing
      /*
      print("\tGyroXHistory: ");
      for (int i = 0; i < GYRO_HISTORY_COUNT; i++) print("%f,", gyroXHistory[i]);
      print("\n\tGyro Count: %d\n", gyroRockCount);     
      */
        
   
   }     
   
}  

void micCog() {
  float max, min, maxPkPk, read;
  float startTime;
  
  while (1) {
    
    // Set Extreme Initial Values
    max = -1000;
    min = 1000;
    maxPkPk = 0;
    
    read = adc_volts(PIN_MIC_AD);
   
    startTime = mstime_get() / 1000.0;
    
    // Record the highest and lowest mic values for a while (micPeriod)
    while (mstime_get()/1000.0 - startTime < micPeriod)
    {
      read = adc_volts(PIN_MIC_AD);
      
      if (read > max)
        max = read;
      else if (read < min)
        min = read;
        
    }
    
    // After reading a full period of values, determine pk-pk value
    maxPkPk = max - min;
    
    if (maxPkPk > micThresholdPk) {
      //print("NEW MICROPHONE TRIGGER, pk-pk=%f\n", maxPkPk); // should fail because inside cog
      micLastTrig = mstime_get()/1000.0; // set time to the current trigger time
      
    }    
  }
}