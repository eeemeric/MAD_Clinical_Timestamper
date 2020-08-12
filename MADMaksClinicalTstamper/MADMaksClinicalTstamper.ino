const byte numChars = 32;
char receivedChars[numChars];   // an array to store the received data

boolean newData = false;
boolean bytesFlag = false;
boolean readFlg = false;
String inString = "";    // string to hold input

boolean pdFlg = false;
int oldPDstate;
int newPDstate;
int idx = 0;

const int A0_0  = 2;
const int A0_1  = 3;
const int A0_2  = 8;

const int pwdth = 400; // pulse width in microseconds
const int ipi   = 400; // inter pulse interval in microseconds
const int BR    = 19200;
unsigned long strt_b;
unsigned long strt_r;
unsigned long strt_p;
String serialOutString;

String bin      = "";
String pBin     = "";

const int blueLEDpin    = 7; // blue tooth connection intdicator
const int greenLEDpin   = 5; // transmit pulses 
const int yellowLEDpin  = 6; // serial receive
const int redLEDpin     = 4; // serial transmit

void setup() {
  // used to for output to physiol system
  pinMode(A0_0, OUTPUT); // sets the digital pin 0 as output
  pinMode(A0_1, OUTPUT); // sets the digital pin 1 as output
  // used to read the state of photodiode
  pinMode(A0_2, INPUT); // sets the digital pin 1 as input

  pinMode(redLEDpin, OUTPUT);
  pinMode(yellowLEDpin, OUTPUT);
  pinMode(greenLEDpin, OUTPUT);
  pinMode(blueLEDpin, OUTPUT);
  
  // set LEDs to off
  digitalWrite(blueLEDpin, LOW);
  digitalWrite(greenLEDpin, LOW);
  digitalWrite(yellowLEDpin, LOW);
  digitalWrite(redLEDpin, LOW);
  
  // set physiol channels to low
  digitalWrite(A0_0, LOW);
  digitalWrite(A0_1, LOW);
  // put your setup code here, to run once:
  // Serial.begin(BR); // hard line
  Serial.begin(BR);//bluetooth
  //Serial2.begin(BR);//bluetooth
  // wait for bluetooth serial port to connect. Needed for native USB
  //while (!Serial & !Serial2) {    
  while (!Serial) {
  }
  // bluetooth connected indicator
  //digitalWrite(blueLEDpin, HIGH);
  // max baud rate of the hc-06 bluetooth module is 1382400
  // has to be set manually
  //   http://www.ablab.in/how-to-know-the-change-the-baud-rate-of-a-hc-06-bluetooth-module-with-avr-atmega32-microcontroller/
  Serial.flush();
  // get initial state of pd input   
  oldPDstate = digitalRead(A0_2);
  newPDstate = oldPDstate;
}

void loop() {
  // check if bt still connected
  if (!Serial){
    digitalWrite(blueLEDpin, LOW); 
  }
  else{
    digitalWrite(blueLEDpin, HIGH);  
  }
  checkPDstate();
  recvDateWithEndMarker();
  checkPDstate();
}
 
void recvDateWithEndMarker() {
  // Read serial input:
  checkPDstate();
  while (Serial.available() > 0) {
    int inChar = Serial.read();
    String pStr = "";
    //send single pulse when the first byte arrives
    if (idx==0){
      strt_b = millis();
      digitalWrite(yellowLEDpin, HIGH);
      sendSinglePulse();
      // Serial.println(strt_b);
      idx++;
    }
    if (isDigit(inChar)) {
      // convert the incoming byte to a char and add it to the string:
      inString += (char)inChar;
      idx++;
      checkPDstate();
    }
    // if you get a newline, 
    if (inChar == '\n') {
      digitalWrite(yellowLEDpin,LOW);
      checkPDstate();
      if (inString.length()==11){        
        parseDateStr();
      }
      else{
        divide(inString.toInt());
        bin = pBin;
      }

      if (inString.length()<3){
        switch (inString.toInt()) {
          case 0:    // 0, trial parameter stream done
            serialOutString = "Trial Params, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 1:    // 1, trial start
            serialOutString = "Trialstart, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 2:    // 2, Clicked O1A1/A+1
            serialOutString = "A+1 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 3:    // 3, Clicked O1P1/P+1
            serialOutString = "P+1 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 4:    // 4, Clicked O1A2/A+2
            serialOutString = "A+2 Clicked , " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 5:    // 5, Clicked O1P2/P+2
            serialOutString = "P+2 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 6:    // 6, Clicked O2A1/A-1
            serialOutString = "A-1 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 7:    // 7, Clicked O2P1/P-1
            serialOutString = "P-1 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 8:    // 8, Clicked O2A2/A-2
            serialOutString = "A-2 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 9:    // 9, Clicked O2P2/P-2
            serialOutString = "P-2 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 10:    // 10, Clicked O3A1/A+3
            serialOutString = "A+3 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 11:    // 11, Clicked O3P1/P+3
            serialOutString = "P+3 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break; 
          case 12:    // 12, Clicked O3A2/A-3
            serialOutString = "A-3 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 13:    // 13, Clicked O3P2/P-3
            serialOutString = "P-3 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 14:    // 14, Clicked O4A1/A+4
            serialOutString = "A+4 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 15:    // 15, Clicked O4P1/P+4
            serialOutString = "P+4 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 16:    // 16, Clicked O4A2/A-4
            serialOutString = "A-4 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 17:    // 17, Clicked O4P2/P-4
            serialOutString = "P-4 Clicked, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 18:    // 18, Displayed O1A1/A+1
            serialOutString = "A+1 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 19:    // 19, Displayed O1P1/P+1
            serialOutString = "P+1 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 20:    // 20, Displayed O1A2/A+2
            serialOutString = "A+2 , " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 21:    // 21, Displayed O1P2/P+2
            serialOutString = "P+2 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 22:    // 22, Displayed O2A1/A-1
            serialOutString = "A-1 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 23:    // 23, Displayed O2P1/P-1
            serialOutString = "P-1 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;            
          case 24:    // 24, Displayed O2A2/A-2
            serialOutString = "A-2 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 25:    // 25, Displayed O2P2/P-2
            serialOutString = "P-2 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 26:    // 26, Displayed O3A1/A+3
            serialOutString = "A+3 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 27:    // 27, Displayed O3P1/P+3
            serialOutString = "P+3 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 28:    // 28, Displayed O3A2/A-3
            serialOutString = "A-3 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 29:    // 29, Displayed O3P2/P-3
            serialOutString = "P-3 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 30:    // 30, Displayed O4A1/A+4
            serialOutString = "A+4 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 31:    // 31, Displayed O4P1/P+4
            serialOutString = "P+4 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 32:    // 32, Displayed O4A2/A-4
            serialOutString = "A-4 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 33:    // 33, Displayed O4P2/P-4
            serialOutString = "P-4 Displayed, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 34:    // 34, Mask On
            serialOutString = "Covered, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 35:    // 35, EarlyMaskOn
            serialOutString = "Early Mask On, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 36:    // 36, Choice
            serialOutString = "Choice, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 37:    // 37, Result
            serialOutString = "Result, " + String(strt_b);
            Serial.println(serialOutString);
            break;
        }
      }
      sendPulses();
      sendSinglePulse();
      
      //Serial.println(inString);
      //send arduino timestamp back to tablet
      digitalWrite(redLEDpin, HIGH);      
      //Serial.println(strt_b);
      digitalWrite(redLEDpin, LOW);
      //Serial.println(inString);
      // clear the strings for new input:
      inString  = "";
      bin       = "";
      pBin      = "";
      newData   = true;
      idx = 0; 
    }
  }
}

void sendSinglePulse(){
  // send single pulse to ephys system
  digitalWrite(A0_0, HIGH);// sets the digital pin 0 to HIGH  
  digitalWrite(A0_1, HIGH);// sets the digital pin 1 to HIGH               
  delayMicroseconds(pwdth);// waits for a pwdth ms
  digitalWrite(A0_0, LOW); // sets the digital pin 0 to LOW  
  digitalWrite(A0_1, LOW); // sets the digital pin 1 to LOW
}

void sendPulses(){  
  int  n = bin.length();  
  digitalWrite(greenLEDpin, HIGH);
  sendSinglePulse();
  delayMicroseconds(ipi);
  for (int i = 0; i < n;i++){
    if (bin[i]=='0'){
      digitalWrite(A0_0, HIGH); // sets the digital pin 0 to HIGH 
      delayMicroseconds(pwdth); 
      digitalWrite(A0_0, LOW); // sets the digital pin 0 to LOW
    }
    else{
      digitalWrite(A0_1, HIGH); // sets the digital pin 0 to HIGH
      delayMicroseconds(pwdth);
      digitalWrite(A0_1, LOW); // sets the digital pin 0 to LOW 
    }
    delayMicroseconds(ipi);      
  }
  sendSinglePulse();
  delayMicroseconds(ipi);
  digitalWrite(greenLEDpin, LOW);
}

void checkPDstate() {     
    newPDstate = digitalRead(A0_2);
    if (newPDstate != oldPDstate){
      //strt_p = millis();
      //digitalWrite(A0_0, HIGH); // sets the digital pin 0 to HIGH               
      //delayMicroseconds(pwdth);            // waits for a pwdth ms
      //digitalWrite(A0_0, LOW); // sets the digital pin 0 to LOW
      pdFlg = true; 
      oldPDstate = newPDstate;             
    }   
}

void divide(int num){
  if (num>1){
    divide(num/2);
  }
  //bin += num%2;
  pBin += num%2;
}

void parseDateStr(){
      /* 
      parse the timestamp str, receivedChars
      5 bits for day
      5 bits for hour (24 hr clock)
      6 bits for minute
      6 bits for second
      10 bits for milisecond
      */
      
      // 5 bits for day 
      String pStr = inString.substring(0,2);
      divide(pStr.toInt()); // convert to binary
      padStr(5); // pad string with zeros so the desired number of bits equals nBits 
      bin += pBin; // concatenate strings
      pBin = "";
            
      // 5 bits for hour (24 hr clock)
      pStr = inString.substring(2,4);
      divide(pStr.toInt()); // convert to binary      
      padStr(5); // pad string with zeros
      bin += pBin; // concatenate strings
      pBin = "";
      
      // 6 bits for minute
      pStr = inString.substring(4,6);      
      divide(pStr.toInt()); // convert to binary      
      padStr(6); // pad string with zeros
      bin += pBin; // concatenate strings
      pBin = "";

      // 6 bits for second
      pStr = inString.substring(6,8);
      divide(pStr.toInt()); // convert to binary      
      padStr(6); // pad string with zeros
      bin += pBin; // concatenate strings
      pBin = "";

      // 10 bits for milisecond
      pStr = inString.substring(8,11);
      divide(pStr.toInt()); // convert to binary      
      padStr(10); // pad string with zeros
      bin += pBin; // concatenate strings
      pBin = "";
}

void padStr(int nBits){ 
  int  n = pBin.length();
  if (n<nBits){
    for (int i = 0; i< nBits - n;i++){
      pBin = "0" + pBin;
    }
  }  
}
