// Please Note! There are an About "tab" (About.ino) with more information about the project!
// Current version: 0.50

// ************[ Deinition of Inputs and Outputs ]************

// Tranmission successful and loop control 
boolean Sent          = 0;                   // Leave at 0, since we have not transmitted anything, will be used with !Sent, i.e. NOT Sent, will run if Sent is 0. 
boolean doneClosed    = 0;                   // To control loop's: See above
boolean doneOpen      = 0;                   // To control loop's: See above
boolean loopControl   = 0;                   // Need to copy above loop controller to this generic

// DTR and DSR is used for Flow control, or hardware handshake. 
const int DSR         = 2;                   // use const instead of #define
const int DTR         = 3;                   // Define as a constant (read-only), since it is a I/O that does not change

const int LED         = 13;                  // Built-in LED of Arduino Uno's main board
const int OK          = 6;                   // Added Green LED for OK messages
const int ERROR       = 7;                   // Added Red LED for Error messages

// INPUT for Hardware
const int switchPin   = A0;                  // Define I/O for user file switch

// Define Commands for camera
const byte FileA[]    = {0x11,0x06,0x01};    // Array for FileA
const byte FileB[]    = {0x11,0x06,0x00};    // Array for FileB
const byte ACK[]      = {0x71,0x01};         // Array for ACK (71 01) + dummy data
const byte NAK[]      = {0x91,0x01};         // Array for NAK (91 01) + err data

// Define buffer for serial data
byte incomingByte     =  0x00;               // for incoming serial data (single byte)
byte incomingBytes[3] = {0x00,0x00,0x00};    // for incoming serial data (array)

// Define Time 
unsigned long Time;                          // No value, will collect from millis()
unsigned long Counter;                       // No value, will collect from millis()

// Define Variable
// int Command;                              // Variable to pass to function to tell which command should be sent

// ************[ SETUP - Run once ]************************************************************************************************************************************************************************************************************************
void setup() {
  pinMode(LED, OUTPUT);                      // D13, built-in LED of board set as output
  pinMode(OK, OUTPUT);                       // D6, Green LED for OK messages set as output
  pinMode(ERROR, OUTPUT);                    // D7, Red LED for Error messages set as output
  pinMode(DTR, OUTPUT);                      // Set DTR (Data Terminal Ready) to Output
  pinMode(DSR, INPUT);                       // Set DST (Data Set Ready) to Input
  pinMode(switchPin, INPUT);                 // Set switchPin to Input to be able to read
  digitalWrite(LED, HIGH);                   // Turn on the LED
  digitalWrite(DSR, HIGH);                   // Set DSR HIGH (inverted) since we DO NOT want to send anything
  Serial.begin(9600, SERIAL_8O1);            // Opens serial port, sets data rate to 9600 bps, and 8 databits, ODD parity, 1 stop bit for Toshiba IK-CU51 Camera Controller Unit
} 
// ************[ EXIT SETUP ]******************************************************************************************************************************************************************************************************************************



// ************[ Main LOOP - Run repeatedly ]**************************************************************************************************************************************************************************************************************
void loop() {

    // ******[ Read input and debounce ]********
      int bounce1 = digitalRead(switchPin);           // Read switchPin once
      delay(25);                                      // Delay 25ms for debounce
      int bounce2 = digitalRead(switchPin);           // Read switchPin again
    // *****************************************

    // -----[ Check if switch is active ]------------------------------------------------------------------------------------------
    while ((bounce1 == bounce2) && (bounce1 == LOW))  // Compare AND if LOW (inverted) then do:
        {
        // *****[ Send File B ]*******************  
            sendCommand(2);                           // This should call the new Function sendCommand and give FileB as data
            doneOpen = 0;                             // Reset the action required when the switch is open, since we are inside the "switch closed" option; i.e. OPEN is NOT done
        // *************************************** 
        
        // *****[ Read Input and debounce ]*******
            bounce1 = digitalRead(switchPin);         // Read switchPin once
            delay(25);                                // Delay 25ms for debounce
            bounce2 = digitalRead(switchPin);         // Read switchPin again
        // ***************************************      
        } // END of WHILE loop  
    // ----------------------------------------------------------------------------------------------------------------------------

    // *****[ Continue Main Loop ]***************    

    // *****[ Send File A ]*********************      // This since the switch is now open 
      sendCommand(1);                                 // This should call the new Function sendCommand and give FileA as data
      doneClosed = 0;                                 // Reset the action required when the switch is closed, since we are inside the "switch open" option; i.e. CLOSED is NOT done      
    // *****************************************  
} 
// ************[ END of MAIN LOOP ]************************************************************************************************************************************************************************************************************************


// ************[ Function to read all remaining byte i serial buffer ]************
void flushBytes(){
  while (Serial.available() > 0) {
      incomingByte = Serial.read();                   // read the incoming byte; do nothing, just to clear the buffer
      }
  }
// *******************************************************************************

  
// ************[ Function to clear all remaining data i array ]*******************
void clearArray(){
  for(int c=0; c<3; c++) incomingBytes[c] = 0x00;     // clear the incoming bytes: 
  }
// *******************************************************************************


// ************[ Function to send the command to camera ]*************************************************************************************************************************************************************************************************
void sendCommand(int Command) 
  {
          // ******[ Loop Control ]**************************************************************************       
           switch (Command){                          // This is only to select the right option for loop control
           case 1: 
               loopControl = doneOpen;                // We completed the action required when the switch is pressed
               break;
           case 2: 
               loopControl = doneClosed;              // We completed the action required when the switch is pressed
               break;
           default: 
               break;
           } // END of switch / case
          // ************************************************************************************************
 
          // ******[ Repeat check ]**************************************************************************  
          if (!loopControl){
          // Once we are inside the WHILE loop, we need to set "Sent" to control the transmission.
          // We only want to send once, so it must be early in the loop, and not in later IF statments
          Sent = 0;                                   // Time to send to serial port; reset Sent to 0; we have not sent anything yet 
          digitalWrite(DTR, LOW);                     // Set DTR LOW (inverted) since we want to send
                                                      // This might actually be the wrong order of thing: 
                                                      // "The CONTROLLER turns on the DTR within 2 seconds after sending out the command, 
                                                      // and receives the response from the CAMERA. If the DTR is not turned on within 
                                                      // 2 seconds, the CAMERA terminates the communication without sending out the response."  
          delay(20);                                  // Wait 20ms to make sure that the camera has time to respond
          if (digitalRead(DSR) == HIGH){              // Check DSR, if DSR is still OFF (HIGH) then display error messages
              digitalWrite(OK, LOW);                  // Set OK LED to OFF, 
              digitalWrite(ERROR, HIGH);              // Set ERROR LED to ON
              }                                       // and continue 
          } // But all this is only if we have not done it already, for this command.
          // ************************************************************************************************     


          
          // ======[ We are clear to send ]======================================================================================================================================================================================
          if (digitalRead(DSR) == LOW && !loopControl)// Hold and wait for DSR to go ON (LOW) AND will only run if doneClosed is false = 0 (not done yet)
              {
                // -----[ Send COMMAND, if not sent ]-------------------------------------------------------------------------------------------------------------------------------------------------          
                if (!Sent) {                          // Will run if Sent is false = 0
                  flushBytes();                       // Flush the serial buffer, to make sure nothing is in it before sending a new command; i.e. be ready for the answer to our new command
                  switch (Command){
                     case 1: 
                         Serial.write(FileA,sizeof(FileA));  // Send the byte array
                         break;
                     case 2: 
                         Serial.write(FileB,sizeof(FileB));  // Send the byte array
                         break;
                     default: 
                         break;
                     } // END of switch / case
                  } // END of !Sent IF statment
              // -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------   
 
                // ******[ Wait for answer or time-out ]***********************************************************                 
                // Here we will wait and poll for 2000ms, before time-out. Moving on without the correct answer will result in a resend. 
                if (!loopControl){
                  Counter = millis() + 2000;          // Set Counter to current time + 2sek                 
                  while (Counter > millis() && Serial.available() < 3){        // Compare set Counter against Current time  // Function: do something during 2000ms and then stop // While current time is lower that set time do this
                      delay(2);                          // NOOP
                      } // END of counter test                               
                      if (Serial.available() >= 3) {
                        for(int b=0; b<3; b++)
                        incomingBytes[b] = Serial.read(); // Read the incoming byte
                        } // END of Serial available and Serial read function
                } // END of !loopControl IF
                // ************************************************************************************************

                // ******[ Error messages / Debug messages ]*******************************************************                  
                if (incomingBytes[0] == 0x00 && incomingBytes[1] == 0x00){  // Check if the answer is 00; i.e. we did not receive any answer. We cleared the Array! 
                  digitalWrite(OK, LOW);            // Set OK LED to OFF, 
                  digitalWrite(ERROR, HIGH);        // Set ERROR LED to ON
                }
                // ************************************************************************************************  

                // ******[ Error messages / Debug messages ]*******************************************************                  
                if (incomingBytes[0] == NAK[0] && incomingBytes[1] == NAK[1] ){  // Check if the answer is NAK
                  digitalWrite(OK, LOW);            // Set OK LED to OFF, 
                  digitalWrite(ERROR, HIGH);        // Set ERROR LED to ON
                   //Serial.print("NAK : ");          // Error message; Find out what went wrong
                   byte e = incomingBytes[2]; 
                   switch (e){
                     case 0x01: 
                         //Serial.print("The FUNC.DATA NUMB and CMD are undue");  // Do not know what this means! 
                         break;
                     case 0x02: 
                         //Serial.print("Time-Over Two Seconds Error");
                         break;
                     case 0x03: 
                         //Serial.print("Parity Error");
                         break;
                     case 0x04: 
                         //Serial.print("Over-run Error");
                         break;
                     case 0x05: 
                         //Serial.print("Framing Error");
                         break;
                     default: 
                         //Serial.print("Unexpected Error");                    // Should never happen! 
                         break;
                     } // End of switch / case
                } // End of NAK handeling
                // ************************************************************************************************  

                // ******[ Acknowledge messages / Finished ]*******************************************************       
                if (incomingBytes[0] == ACK[0] && incomingBytes[1] == ACK[1] ){  // Check if the answer is ACK
                  Sent = 1;                          // Transmission was successful, set Sent to true = 1
                  digitalWrite(OK, HIGH);            // Set OK LED to ON, 
                  digitalWrite(ERROR, LOW);          // Set ERROR LED to OFF
                  delay(40);                         // The CONTROLLER must wait 40ms after receiveing the response from the CAMERA before sending the next command. 
                  digitalWrite(DTR, HIGH);           // Set DTR HIGH(inverted) since we DO NOT want to send anything. We have successfully sent and received the command.
                  switch (Command){                  // These statments are to control the loops, and only send once. Check Command and set controller
                     case 1: 
                         doneOpen = 1;               // We completed the action required when the switch is pressed
                         break;
                     case 2: 
                         doneClosed = 1;             // We completed the action required when the switch is pressed
                         break;
                     default: 
                         break;
                     } // END of switch / case
                  } // END of IF statment
                // ************************************************************************************************
                
                // If we end up here something went wrong! Either we did not get the answer we expected, or the NAK answer. Either way, we need to clear the incomming serial buffer, and resend the command until we do get ACK answer.
                clearArray(); // Clear arrays of data
                
           } // END of DSR check and send loop
          // =======================================================================================================================================================================================================================

} // END of Function
// *******************************************************************************************************************************************************************************************************************************************************

