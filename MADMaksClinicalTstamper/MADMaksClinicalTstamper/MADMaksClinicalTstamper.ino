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
          case 0:    // 0, reset arduino clock
            serialOutString = "Reset Arduino clock, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 1:    // 1, trial start
            serialOutString = "Trialstart, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 2:    // 2, Tap O1A1/A+1
            serialOutString = "Tap A+1, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 3:    // 3, Tap O1P1/P+1
            serialOutString = "Tap P+1, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 4:    // 4, Tap O1A2/A+2
            serialOutString = "Tap A+2, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 5:    // 5, Tap O1P2/P+2
            serialOutString = "Tap P+2, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 6:    // 6, Tap O2A1/A-1
            serialOutString = "Tap A-1, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 7:    // 7, Tap O2P1/P-1
            serialOutString = "Tap P-1, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 8:    // 8, Tap O2A2/A-2
            serialOutString = "Tap A-2, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 9:    // 9, Tap O2P2/P-2
            serialOutString = "Tap P-2, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 10:    // 10, Tap O3A1/A+3
            serialOutString = "Tap A+3, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 11:    // 11, Tap O3P1/P+3
            serialOutString = "Tap P+3, " + String(strt_b);
            Serial.println(serialOutString);
            break; 
          case 12:    // 12, Tap O3A2/A-3
            serialOutString = "Tap A-3, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 13:    // 13, Tap O3P2/P-3
            serialOutString = "Tap P-3, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 14:    // 14, Tap O4A1/A+4
            serialOutString = "Tap O4A1, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 15:    // 15, Tap O4P1/P+4
            serialOutString = "Tap P+4, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 16:    // 16, Tap O4A2/A-4
            serialOutString = "Tap A-4, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 17:    // 17, Tap O4P2/P-4
            serialOutString = "Tap P-4, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 18:    // 18, Show O1A1/A+1
            serialOutString = "Show A+1, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 19:    // 19, Show O1P1/P+1
            serialOutString = "Show P+1, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 20:    // 20, Show O1A2/A+2
            serialOutString = "Show A+2, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 21:    // 21, Show O1P2/P+2
            serialOutString = "Show P+2, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 22:    // 22, Show O2A1/A-1
            serialOutString = "Show A-1, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 23:    // 23, Show O2P1/P-1
            serialOutString = "Show P-1, " + String(strt_b);
            Serial.println(serialOutString);
            break;            
          case 24:    // 24, Show O2A2/A-2
            serialOutString = "Show A-2, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 25:    // 25, Show O2P2/P-2
            serialOutString = "Show P-2, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 26:    // 26, Show O3A1/A+3
            serialOutString = "Show A+3, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 27:    // 27, Show O3P1/P+3
            serialOutString = "Show P+3, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 28:    // 28, Show O3A2/A-3
            serialOutString = "Show A-3, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 29:    // 29, Show O3P2/P-3
            serialOutString = "Show P-3, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 30:    // 30, Show O4A1/A+4
            serialOutString = "Show A+4, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 31:    // 31, Show O4P1/P+4
            serialOutString = "Show P+4, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 32:    // 32, Show O4A2/A+4
            serialOutString = "Show A+4, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 33:    // 33, Show O4P2/P-4
            serialOutString = "Show P-4, " + String(strt_b);
            Serial.println(serialOutString);
            break;
          case 34:    // 34, Mask On
            serialOutString = "Mask On, " + String(strt_b);
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
