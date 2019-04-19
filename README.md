# ENEL300_Robot
ENEL 300 - Emotional Robot Project

This codebase, created for the Second Year Electrical Engineering Professionalism course at the University of Calgary, is to be run on a Parallax Propeller Activity Board attached to a 2-servo robot with expansibility capabilities.

Our finished robot (including hardware and software) was able to demonstrate **four** distinct emotions by responding to human stimulus, detected by several sensors and output devices.

## Emotions
1. Anger
2. Sadness
3. Fear
4. Love/Affection

## Input Devices
1. IR Remote Control Receiver: switch between emotions via Television IR Remote Control
2. Ultrasonic Proximity Sensor: detect distances
3. Microphone: op-amp amplified electret microphone decoded via ADC
4. Gyroscope: MPU-9150 Accelerometer/Gyroscope/Magnetometer via I2C protocol, co-processed by Arduino Pro Micro interfacing via Serial UART
5. Arcade Button: redundancy device for switching robot emotions

## Output Devices
1. RGB LED Eyes: full color spectrum eyeballs (with googly eyes)
2. Eyebrow Servos: two independent servos capable of communicating emotions
3. Driving Servos: two independent driving servos attached to wheels used to move the robot across the testing area
4. Buzzer: auditory communication
