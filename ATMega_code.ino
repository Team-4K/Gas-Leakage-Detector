#include <SoftwareSerial.h>

SoftwareSerial mySerial(7, 8);
int ledPin = 11;
int buzzer = 10;
int smokeA0 = A5;
int MessageSend =0;

bool numberAdded = true;
char msg[200];
int i = 0;
bool store = false;
String fullNumber;

// Your threshold value
int sensorThres = 100;
void setup()

{

  mySerial.begin(9600);   // Setting the baud rate of GSM Module 
  pinMode(buzzer, OUTPUT);
  pinMode(smokeA0, INPUT);
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  Serial.println("GSM SIM 800A BEGIN ");
  delay(100);

  Serial.println("Initializing..."); 
  delay(1000);

  mySerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  
  mySerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();

}

void loop()

{

  if(numberAdded){
        updateSerial();
    
        if(store){
          String number = String(msg);
          Serial.print("Num: ");
          Serial.print(number.substring(2));

          fullNumber = "+94" + number.substring(2);
          Serial.print("Number Success: " + fullNumber);

          numberAdded = false;
        }

        
  }else{
        Serial.print("AT+CMGS=\""+fullNumber+"\"");
        int analogSensor = analogRead(smokeA0);
        Serial.print("Pin A0: ");
        Serial.println(analogSensor);
        Serial.println(MessageSend);
        // Checks if it has reached the threshold value
        if (analogSensor > sensorThres)
        {
          tone(buzzer, 1000, 200);
          
          digitalWrite(ledPin, HIGH);
      
          if (MessageSend ==0){
            SendMessage();
            MessageSend=1;
          }
              
        }
        else
        {
          noTone(buzzer);
          digitalWrite(ledPin, LOW);
          MessageSend =0;
        }
        delay(100);
        
      
       if (mySerial.available()>0)
      
         Serial.write(mySerial.read());
    }



  
  

}

 void SendMessage()
{

  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode

  delay(100);  // Delay of 1000 milli seconds or 1 second

  //mySerial.println("AT+CMGS=\""+fullNumber+"\"\r"); // Replace x with mobile number
  mySerial.println("AT+CMGS=\""+fullNumber+"\""); // Replace x with mobile number

  delay(100);

  mySerial.println("Warning!!!  Gas leakage has been detected");// The SMS text you want to send

  delay(100);

  mySerial.println((char)26);// ASCII code of CTRL+Z

  delay(100);

}

void updateSerial()
{
  delay(500);
  while (Serial.available()) 
  {
    mySerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  
  while(mySerial.available()) 
  {
    char msgChar = mySerial.read();
    if(msgChar=='*') store = true;
    Serial.write(msgChar);//Forward what Software Serial received to Serial Port

    if(store){
      msg[i] = msgChar;
      i++;
    }
  }
}
