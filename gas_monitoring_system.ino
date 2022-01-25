// Gas Leakage Monitoring System
#include <SoftwareSerial.h>
SoftwareSerial espSerial =  SoftwareSerial(2,3);    
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

float sensor=A0;
float gas_value;
String apiKey = "CT38PJ46CKXDNVV6";     
String ssid="butter";    
String password ="abc12345";  
boolean DEBUG=true;
int powerPin=12;
#define VOLTAGE_MAX 5.0
#define VOLTAGE_MAXCOUNTS 1023.0
//======== showResponse ============//

void showResponse(int waitTime){
    long t=millis();
    char c;
    while (t+waitTime>millis()){
      if (espSerial.available()){
        c=espSerial.read();
        if (DEBUG) Serial.print(c);
      }
    }
                   
}

//========= connecting to thinkspeak.com==========//
boolean thingSpeakWrite(float value1){
  String cmd = "AT+CIPSTART=\"TCP\",\"";                  
  cmd += "184.106.153.149";                               
  cmd += "\",80";
  espSerial.println(cmd);
//  Serial.println("****");
//  Serial.println(cmd);
  if (DEBUG) Serial.println(cmd);
  if(espSerial.find("Error")){
    if (DEBUG) Serial.println("AT+CIPSTART error");
    return false;
  }
  String getStr = "GET /update?api_key=";   // prepare GET string
//  Serial.println("**");
//  Serial.println(apiKey);
  getStr += apiKey;
  getStr +="&field1=";
  getStr += String(value1);
  //getStr +="&field2=";
  //getStr += String(value2);
  // ...
  getStr += "\r\n";
//  Serial.println(getStr);

  // send data length
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  espSerial.println(cmd);
  if (DEBUG)  Serial.println(cmd);
  delay(100);
  if(espSerial.find(">")){
//    Serial.println(getStr);
    espSerial.print(getStr);
    if (DEBUG)  Serial.print(getStr);
  }
  else{
    espSerial.println("AT+CIPCLOSE");
    // alert user
    if (DEBUG)   Serial.println("AT+CIPCLOSE");
    return false;
  }
  return true;
}
//=================== setup ==========//
void setup() {                
  DEBUG=true;          
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(powerPin,OUTPUT);
  pinMode(sensor,INPUT);
  Serial.begin(9600); 
  espSerial.begin(115200);  
  espSerial.println("AT+CWMODE=1");  
  showResponse(1000);

  espSerial.println("AT+CWJAP=\""+ssid+"\",\""+password+"\"");  
  showResponse(5000);

   if (DEBUG)  Serial.println("Setup completed");
}

// ================== loop ============//
void loop() {
  gas_value=analogRead(sensor);
  Serial.println(gas_value);
  delay(1);  
  if(gas_value>250){
  digitalWrite(powerPin,LOW);
  digitalWrite(LED_BUILTIN, HIGH);
//convert gas value to voltages
  float t = gas_value*(VOLTAGE_MAX / VOLTAGE_MAXCOUNTS); 
  if (isnan(t)) {
        if (DEBUG) Serial.println("Failed to read from MQ5");
      }
      
 else{
  if (DEBUG)  Serial.println("Voltage="+String(t));
//Serial.println("Connecting to thingspeak");
  thingSpeakWrite(t);        
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.print("Gas leakage");
     }
  delay(20000);  
}
else{
  lcd.begin();
  lcd.clear();
digitalWrite(LED_BUILTIN, LOW);
digitalWrite(powerPin,HIGH);
}
}
