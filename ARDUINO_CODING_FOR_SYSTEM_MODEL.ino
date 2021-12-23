/*This is the code for the automated operation of the cutting process and hand sanitizing as sensory information is received from the sensors.
 * The code will also be able to implement auto-shutdown of cutting process after five minutes of inactivity. 
 * This is to conserve the battery power.
 * The system will have to reset or be refreshed again before it resumes normal operation after auto-shutdown.
 * The sketch was compiled after loops and several hours of brainstorming by the project team, in August, 2021.
 * It moderately achieves the aim. NOTE that in this coding, the cutting process and the sanitizing process is not allowed to operate simultaneously!
 * It is open to modification. The name of the sketch is "ARDUINO_CODING_FOR_SYSTEM_MODEL".
 * Read the comments to understand each line.
 * There are a total of 129 lines in this sketch (spaces inclusive, for clearity and easy unnderstanding of code).
*/

#include <Servo.h>                      //Import the Servo Library to enable easy use of the Servo Motor, MG996R.

Servo myServo;
int pos = 0;

const int cutSenPin = 2;                // Sensor for detecting the placement of object on the dicer is connected to D2.

const int servoSenPin = 4;              // Servomotor sensor for auto-hand sanitizing is connected to D4.

const int relayFPin1 = 7;               // Relay pin for triggering forward movement of linear actuator to D7.

const int relayBPin2 = 8;               // Relay pin for triggering backward or retract movement of linear actuator to D8.

const int relayPowerPin = 12;           // Relay pin for triggering ON and OFF the cutting process.

unsigned long processInactiveInterval = 3*60000;      /*To set the maximum interval of inactivity after which the microcontroller will trip OFF 
                                                        the cutting process to save primary power supply (12V BATTERY). 
                                                        This corresponds to 5 minutes. Every start of a new cutting phase resets this countdown time. 
                                                        Also, the end of every cutting process restarts the countdown*/

unsigned long previousNewStart = 0;               /*This variable will hold the current time a proccess starts. At the reset or switching ON of the 
                                                    microcontroller, it is set to 0. As the process goes on in time, previousNewStart is frequently updated.*/

unsigned long currentMillis = millis();           //This variable holds the instanteneous present time in milli seconds.

void setup() {

myServo.attach(9);                    // Servo motor control pin being attached to digital (PWM) pin D9.

pinMode(cutSenPin, INPUT);            // Declaring the sensor for detecting object to be cut as a source of input information.

pinMode(servoSenPin, INPUT);          // Declaring the sensor for detecting hand to be sanitized as a source of input information.

pinMode(relayFPin1, OUTPUT);          /* Declaring the relay that controls the forward movement of the Linear 
                                         actuator as an object to receive info from microcontroller.*/

pinMode(relayBPin2, OUTPUT);          /* Declaring the relay that controls the retract movement of the Linear 
                                         actuator as an object to receive infomation from microcontroller.*/

digitalWrite(relayFPin1, HIGH);       // Setting the LOW triggered relay for forward motion initially OFF.

digitalWrite(relayBPin2, HIGH);       // Setting the LOW triggered relay for retract motion initially in OFF state.


digitalWrite(relayPowerPin, LOW);     //This is to trigger ON the cutting system once the microcontroller comes ON, or on RESET.

 digitalWrite(relayBPin2, LOW);       
 delay(6000);
 digitalWrite(relayBPin2, HIGH);
 delay(500);                          
                                        /*This will ensure that the Linear Actuator's position is always initially 
                                        reset to "full retract" irrespective of the Linear Actuator's position at the instant the 
                                        microcontroller is switched ON or reset.*/                       
 
 myServo.write(0);                    //This line resets the position of the servo motor.

}

void loop() {
  
  bool processStart = false;          //To check if the cutting process has started.
  bool  systemInUse = false;          /*To check if the system is in use. This is critical in determining whether 
                                        the system stays operational within every five minutes, else it will be tripped OFF
                                        if nothing happens for an interval of five minutes. i.e., if systemInUse remains purely false for 5 minutes.*/

if(digitalRead(cutSenPin) == LOW || digitalRead(servoSenPin)== LOW && (currentMillis - previousNewStart < processInactiveInterval)){
  systemInUse = true;               //This is to flip the value of systemInUse from 'false' to 'true' every  time the sensor detects presence.
  
     while(systemInUse){                                                              //Enter the while loop of processes each time systemInUse is true. 
  
        if (digitalRead(cutSenPin) == LOW && digitalRead(servoSenPin)== HIGH ){       /*This condition ensures that cutting process and hand sanitizing process 
                                                                                      will not occur simultanously. Precedence here is given to the cutting process*/
              processStart = true;  
              while(processStart){
          
                previousNewStart = currentMillis;                                     //Update the countdowntime at the begining of the process.
               
                digitalWrite(relayFPin1, LOW);                                        //Trigger linear actuator in forward direction and wait for 3 seconds.
                delay(5100);                                                          //In 5.1 seconds, the distance covered with speed 25mm/s is roughly 130mm.
                digitalWrite(relayFPin1, HIGH);                                       //Stop the linear autuator from advancing forward than needed.
                delay(3000);
                digitalWrite(relayBPin2, LOW);                                        //Do the same thing as above, but in the opposite direction.
                delay(6100);
                digitalWrite(relayBPin2, HIGH);
                delay(3000);
                
                processStart = !processStart;                                         // Set processStart to 'false' to exit the while loop.
              }
            
         previousNewStart = currentMillis;                         // Update the previousNewStart variable to begin a new countdwn at the end of each cutting cycle.
         systemInUse=false;                                        // Set systemInUse to 'false' to  exit the  while loop and wait!
         
      }else if(digitalRead(servoSenPin) == LOW && digitalRead(cutSenPin) == HIGH){        //Like the previous condition, this one prefers hand sanitizing over cutting.
      
      systemInUse = true;
      
            while(systemInUse){
              previousNewStart = currentMillis;
              
              for (pos = 0; pos <= 90; pos++) { // goes from 0 degrees to 90 degrees in steps of 1 degree
              myServo.write(pos);              // tell servo to go to position in variable 'pos'
              delay(20);                      // waits ms for the servo to reach the position 
              }
            
               myServo.write(0);                    // Reset the Position of the servo  
               previousNewStart = currentMillis;
               systemInUse = false;                 /*At this point, the last thing that the system remembers is that System isn't in use
                                                      so it exits this while loop and the main while loop*/
            }     
           }
     
     }
 

 } else if(digitalRead(cutSenPin) == HIGH && digitalRead(servoSenPin)== HIGH){             //This will execute if the sensor states are not interrupted in 5 minutes.
  systemInUse = false;
  if(systemInUse == false && currentMillis - previousNewStart >= processInactiveInterval){
  digitalWrite(relayPowerPin, HIGH);
  previousNewStart = currentMillis;                                                        //As usual, update the countdown. (This part may not be necessary).
  }
 }
}
