
//Read \/
/* HOW TO ADD SISTER BOARD TO MAIN BOARD! 
 * Pretty simple! 
 * Simply have the main board send out a signal on any pin, and have that pin input on the M5 on G36. 
 * Put the code to send out the pulse in an "if" statement for the input pin that the M5 communicates to the main board through.
 * MAKE SURE THE INPUT VOLTAGE IS 3.3V!
 * 
 * !!! Make sure that the time on until the time off is exactly 3ms. !!!
 * 
 * Sample code:
 * 
 * void setup(){
 * pinMode(D12, INPUT);
 * pinMode(D13, OUTPUT);
 * }
 * 
 * void loop(){
 * if (digitalRead(D12) == HIGH){
 *  digitalWrite(D13, HIGH);
 *  delay(3);
 *  digitalWrite(D13, LOW);
 * }
 *}
 * 
 * 
*/
//Read /\


#include "M5StickC.h"
int displayOff = 0;
int checkingPoll = 0;
int powerErrors = 0;
int verboseOutput = 0;
String readString;

const unsigned long poll = 0.01; //default code checking speed
unsigned long sendpoll = 1000; //default polling speed is 3 seconds.
unsigned long previousTime_poll = 0;
unsigned long previousTime_sendpoll = 0;

void setup() {
  Serial.begin(115200); //begin serial
  pinMode(G10, OUTPUT); //open G10 (LED) as an output
  pinMode(G0, OUTPUT);  //open G0 (SB -> Arduino) as an output
  pinMode(G26, OUTPUT); //open G26 (Buzzer) as an output
  pinMode(G36, INPUT);  //open G36 (Arduino -> SB) as an input
  pinMode(G37, INPUT);  //open G37 (m5 button) as an input
  pinMode(G39, INPUT);  //open G39 (small button) as an input
  M5.begin(); //start m5 stick
  M5.Lcd.fillScreen(YELLOW); //fill screen with yellow
  M5.Lcd.setRotation(3);  //rotate screen
  M5.Lcd.setCursor(4, 30);  //set cursor position
  M5.Lcd.setTextSize(1);  //set text size
  M5.Lcd.setTextColor(TFT_BLACK); //set text color
  Serial.println("Board started..."); //send new line to serial connection
  Serial.println("Loading settings...");
  M5.Lcd.printf("Serial 115200 - Type help"); //display waiting text
  M5.Lcd.setTextSize(2);
  Serial.println("");
  Serial.println("----Settings----");
  Serial.println("");
  Serial.print("Arduino -> Sister Board Polling Speed (MS) (CURRENTLY UNUSED): ");
  Serial.println(poll);
  Serial.print("Sister Board -> Arduino Response Check (MS): ");
  Serial.println(sendpoll);
  Serial.println("");
  Serial.println("----------------");
  Serial.println("");
  Serial.println("© 2020, Zach Bowden."); //DO NOT REMOVE
  Serial.println("-=-=- TYPE HELP FOR COMMANDS -=-=-");
  Serial.println("");
  delay(2000); //wait for 2000 seconds
}



// the loop function runs over and over again forever
void loop() {
  unsigned long currentTime = millis(); //establish a current time varaible and update it all the time
  
  if(currentTime - previousTime_poll >= poll){
    if(Serial.available()){
    String ch;
    ch = Serial.readString();
    ch.trim();
    if (ch.length() >0) {
    if (ch=="help"){  //What is displayed when user types "help" into serial.
    Serial.println("");
    Serial.println("-=-=-=-=- HELP -=-=-=-=-");
    Serial.println("");
    Serial.println("toggledisplay - toggle SB display");
    Serial.println("purge - erase all logs");
    Serial.println("pollspeed - prompted to change SB -> Arduino polling check speed (MS).");
    Serial.println("logs - display logs.");
    Serial.println("verbose - enable/disable verbose output");
    Serial.println("printvars - print the values of all variables. (Used for debug.)");
    Serial.println("trigger - manually trigger response error. (Used for debug.)");
    Serial.println("");
    Serial.println("========================");
    Serial.println("");
    }
    
    if (ch=="purge"){ //Erases all variables when user types "purge" into serial.
    powerErrors = 0;  //The only variable that gets erased. It is the only log storing variable right now.
    Serial.println("Logs erased!");
    }
    
    if (ch=="toggledisplay"){ //Toggles display when user types "toggledisplay". 
    if (displayOff == 0){
      displayOff = 1;
      Serial.println("Display turned off.");
    } else {
      displayOff = 0;
      Serial.println("Display turned on.");
      }
    }
    
    if (ch=="logs"){
    displayLog(); //Refers to the void at the very bottom of this code, it displays all log variables
    }
    
    if (ch=="verbose"){ //Toggles verbose output (displays all actions between the Sister Board and the main board.)
    if (verboseOutput == 0){
      verboseOutput = 1;
      Serial.println("Verbose toggle on.");
    } else {
      verboseOutput = 0;
      Serial.println("Verbose toggle off.");
      }
    }

    if (ch=="printvars"){ //Displays all variables.
    Serial.print("");
    Serial.print("displayOff: ");
    Serial.println(displayOff);
    Serial.print("checkingPoll: ");
    Serial.println(checkingPoll);
    Serial.print("powerErrors: ");
    Serial.println(powerErrors);
    Serial.print("verboseOutput: ");
    Serial.println(verboseOutput);
    Serial.print("poll: ");
    Serial.println(poll);
    Serial.print("sendpoll: ");
    Serial.println(sendpoll);
    Serial.print("previousTime_poll: ");
    Serial.println(previousTime_poll);
    Serial.print("previousTime_sendpoll: ");
    Serial.println(previousTime_sendpoll);
    }

    if (ch=="pollspeed"){ //Changes the variable "sendpoll" to change how quickly the Sister Board polls the main board without requiring reflash of the Sister Board.
      ch="";
      Serial.println("-=-=- Please enter new polling speed (MS) -=-=-");
      while (ch==""){
      ch = Serial.readString();
      ch.trim();
      }
      previousTime_sendpoll = currentTime;
      sendpoll = ch.toInt();  //Sets "sendpoll" to serial response.
      Serial.print("Polling speed has been set to: ");
      Serial.print(sendpoll);
      Serial.print(" ");
      Serial.println("MS");
    }
    
    if (ch=="trigger"){ //Manually triggers response error event.
   M5.Lcd.setCursor(4, 30);
   M5.Lcd.fillScreen(RED);
   M5.Axp.ScreenBreath(100);
   displayOff = 0;
   powerErrors++;
   Serial.println("");
   Serial.println("-=-=-=-=-=-=-=-=-=-=- BEGIN ALERT -=-=-=-=-=-=-=-=-=-=-");
   Serial.println("");
   Serial.println("Sister Board is not receiving responses from the main board.");
   Serial.println("Setting system status to: ERROR: RESPONSE");
   Serial.println("Sister Board is now sending out fast pulses to attempt reconnect!");
   Serial.println("");
   Serial.println("-=-=-=-=-=-=-=-=-=-=- END ALERT -=-=-=-=-=-=-=-=-=-=-");
   displayLog();
   M5.Lcd.printf("ERR:RESPONSE");
   checkingPoll = 1;
   while (digitalRead(G36) == LOW){
   digitalWrite(G10, HIGH);
   digitalWrite(G26, HIGH);
   digitalWrite(G0, LOW);
   delay(35);
   digitalWrite(G10, LOW);
   digitalWrite(G26, LOW);
   delay(10);
   digitalWrite(G0, HIGH);
   delay(25);
      }
   checkingPoll = 0;
   previousTime_sendpoll = currentTime;
      
    }
    ch="";  //Resets serial.
    } 
  }
  
    previousTime_poll = currentTime;
  }
  
  if(currentTime - previousTime_sendpoll >= sendpoll){
    checkingPoll = 1;
    if (verboseOutput == 1){
    Serial.println("[SB -> Arduino] Sending response polling check...");
    }
    digitalWrite(G0, HIGH); //Starts sending poll request to main board.
    if (displayOff == 0){
    digitalWrite(G10, LOW);
    }
    delay(24);
    digitalWrite(G0, LOW);  //Stops sending poll request to main board.
    digitalWrite(G10, HIGH);
    checkingPoll = 0;
    if (digitalRead(G36) == HIGH){  //Starts listening on G36 for Sister Board response.
    if (verboseOutput == 1){
    Serial.println("[Arduino -> SB] Received response to polling check from arduino.");
    }
    
  if (digitalRead(G37) == LOW){ //Unrelated to polling, toggles display.
    if (displayOff == 0){
      displayOff = 1;
      Serial.println("User toggled display off.");
    } else {
      displayOff = 0;
      Serial.println("User toggled display on.");
    }
  }
  if (displayOff == 1){ //M5 stick-c uses ScreenBreath to dictate how bright the display is. In this case, it sets the brightness to 0 if displayOff variable is toggled on.
    M5.Axp.ScreenBreath(0);
  } else {
    M5.Axp.ScreenBreath(100);
  }
  
    if(powerErrors == 0){          //set display color depending on if there is a recorded response error.
    M5.Lcd.fillScreen(GREEN);
   }else{
    M5.Lcd.fillScreen(YELLOW);
   }
   M5.Lcd.setCursor(4, 30);
   if (powerErrors == 0){     //sets display text depending on if there is a recorded response error.
   M5.Lcd.printf("System Normal");
   } else{
   M5.Lcd.printf("System Normal Response Err");
   }
   
  if (digitalRead(G39) == LOW){ //Purges the logs if the G39 button on the M5 stick is pressed while polling is in progress.
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(4, 30);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.printf("Log Purged!");
    M5.Lcd.setTextColor(TFT_BLACK);
    powerErrors = 0; 
    Serial.println("User has erased all log data.");
  }
   } else {   //This is all of the response error code
   M5.Lcd.setCursor(4, 30);
   M5.Lcd.fillScreen(RED);
   M5.Axp.ScreenBreath(100);
   displayOff = 0;
   powerErrors++;
   Serial.println("");
   Serial.println("-=-=-=-=-=-=-=-=-=-=- BEGIN ALERT -=-=-=-=-=-=-=-=-=-=-");
   Serial.println("");
   Serial.println("Sister Board is not receiving responses from the main board.");
   Serial.println("Setting system status to: ERROR: RESPONSE");
   Serial.println("Sister Board is now sending out fast pulses to attempt reconnect!");
   Serial.println("");
   Serial.println("-=-=-=-=-=-=-=-=-=-=- END ALERT -=-=-=-=-=-=-=-=-=-=-");
   displayLog();
   M5.Lcd.printf("ERR:RESPONSE");
   checkingPoll = 1;
   while (digitalRead(G36) == LOW){
   digitalWrite(G10, HIGH);
   digitalWrite(G26, HIGH);
   digitalWrite(G0, LOW);
   delay(35);
   digitalWrite(G10, LOW);
   digitalWrite(G26, LOW);
   delay(10);
   digitalWrite(G0, HIGH);
   delay(25);
    }
   digitalWrite(G0, LOW);
   delay(100);
   if (verboseOutput == 1){
   Serial.println("[Arduino -> SB] Received response to emergency polling requests. Response error has been logged. Resuming system as normal.");
   }
   checkingPoll = 0;
  }
  
  
  previousTime_sendpoll = currentTime;
}
}

void displayLog(){    //Displays if user uses "logs" in serial monitor, or when response error is detected.
   Serial.println("");
   Serial.println("---Logged Events---");
   Serial.println("");
   Serial.print("Poll Response Errors: ");
   Serial.println(powerErrors);
   Serial.println("");
   Serial.println("-------------------");
   Serial.println("");
}
