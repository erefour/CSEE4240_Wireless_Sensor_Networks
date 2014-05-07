#include <stdlib.h>
#include <SoftwareSerial.h>

//SoftwareSerial xbee(2,3); //Software serial ports for the XBee's Rx & Tx

int tempSensor =0;
unsigned long sensorTime =10000;
unsigned long sensorLast =0;

int currentReceiveIndex = 0; //where to write the next character


const int tempPIN = A0;             // Pin A0 is for the temperature sensor
const int humidPIN = A1;            // Pin A1 is for the humdity sensor
const int vSupply_humid = 5;       //Supplying 5 volts to the humidity sensor
//const int chipSelect = 4;

 void setup(){
     Serial.begin(9600);
    // xbee.begin(9600);
     
   
 }
 
 void calTempHumdSensor(){
   
   /** TEMPERATURE DATA **/
   tempSensor = analogRead(A0); //0-1023
   //float 
   float tempSensorVoltage = tempSensor * 5.0f/1023.f;
   //Analog to Digital Converter = Takes a voltage and output a corresponding bit values
   //0-5V == (0-1023)
   
   //TMP 36gz is 10 mv/1 degree Celius
   //if we have 750mV, then temperature is 25 degrees
   //if we have 0mv, then temperature is ...-50 degrees (750/10) = 75, then 25dg-75
   //if we had 5V, then the temperature is 450 degrees
       //Range from 0v to 5v = -50 degrees and 450 degrees
 
   //Multiply by 100 to convert voltage to degrees, and then offset by what the datasheet says
   //Because if your at 0v, you should get 50 degrees
   //if at 5v,
  int offset =50;
  float degreesCelsius = tempSensorVoltage *100; //10 mv/one-degrees-celsius so 1 v/100 degrees cel 
  float temperature = degreesCelsius - offset;
   //int temperature = map(tempSensor, 0, 1023, -50, 450); Since its LINEAR
   
 
  /** HUMIDITY DATA **/ 
  //Finding SensorRH 
  //According to the Datasheet for HIH4030 Humidity Sensor.. True RH (Relative Humidity) = (Sensor RH)/(1.0546 - 0.00216*Temp), where Temp is in Celius
  //And Sensor RH = (Vout - 0.958)/0.0307), where Vout is the output value from the Humidity Sensor output pin(humidPIN) BUT HAS to be converted into digital bit respsentation
  
  float vout_humid = analogRead(humidPIN) *5.0f/1023.f;    //Analog to Digital Converter = Takes a voltage and output a corresponding bit values: 0-5V == (0-1023 bit value)
  float sensorRH = (vout_humid -0.958)/0.0307;
  float trueRH = (sensorRH)/(1.0546 - (0.00216*temperature));
  
  /*float humidityValue = ((0.0004*tempPIN + 0.149)*(analogRead(humidPIN)) - (0.0617*tempPIN + 24.436));
  delay(200);
  */
  
  Serial.print("Temperature = ");
  Serial.println((int)temperature);
  Serial.print("Humidity = ");
  Serial.println(trueRH);
  delay(500);
 //Serial.println(analogRead(humidPIN)/1023);

 }
 
 
 void loop(){
  
//  if(millis()-sensorLast > sensorTime){
    calTempHumdSensor();
   // sensorLast = millis(); //wait until next time period passes
//  }
  
  
  
}
 

