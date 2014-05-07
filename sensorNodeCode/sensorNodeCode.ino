#include <stdlib.h>
#include <SoftwareSerial.h>

SoftwareSerial xbee(2,3); //Software serial ports for the XBee's Rx & Tx

int tempSensor =0;
unsigned long sensorTime =10000;
unsigned long sensorLast =0;
unsigned char receiveBuffer[100];
unsigned char frameDataMSB[4];
unsigned char frameDataLSB[4];
unsigned char frameDataAddr[2];
int currentReceiveIndex = 0; //where to write the next character


const int tempPIN = A0;             // Pin A0 is for the temperature sensor
const int humidPIN = A1;            // Pin A1 is for the humdity sensor
//const int chipSelect = 4;

 void setup(){
     Serial.begin(9600);
     xbee.begin(9600);
     
   
 }
 
 void sendTempHumdSensor(){
   
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
  float degreesCelsius = tempSensorVoltage *100; //10 mv/1 degrees celsius so 1 v/100 degrees cel
  //float 
  int temperature = degreesCelsius - offset;
   //int temperature = map(tempSensor, 0, 1023, -50, 450); Since its LINEAR
   
 
  /** HUMIDITY DATA **/ 
  int humidityValue = ((0.0004*tempPIN + 0.149)*(analogRead(humidPIN)) - (0.0617*tempPIN + 24.436));
  delay(200);
 
 
  /** SENDING TEMP DATA PACKET **/ 
  unsigned char packet[24] ={0}; //actuall size will change (initialize to all zeros)
  unsigned int packetLength = 20;  //actual size will change
  packet[0] = 0x7E;
  packet[1] = (packetLength >> 8) & 0xFF; //get MSB
  packet[2] = (packetLength >> 0) & 0xFF; //get LSB
  //frame specific data
  packet[3] = 0x10; //Frame Type, send data identifier (tx request)
  packet[4] = 0x00; //Frame ID; packet id
  
  //64-bit address of destination
  /*
  packet[5] = 0x00;
  packet[6] = 0x13;
  packet[7] = 0xA2;
  packet[8] = 0x00;
  packet[9] = 0x40;
  packet[10] = 0x90;
  packet[11] = 0x2D;
  packet[12] = 0xE8;
  */
  
  //leave alone for destination coordinator
  
  //broadcast
  packet[5] = 0x00;
  packet[6] = 0x00;
  packet[7] = 0x00;
  packet[8] = 0x00;
  packet[9] = 0x00;
  packet[10] = 0x00;
  packet[11] = 0xFF;
  packet[12] = 0xFF;
  
  //16 bit address, set to 0xFFFE if not known
  packet[13] = 0xFF;
  packet[14] = 0xFE;
  
  packet[15] = 0x00; //Broadcast Radius 10 hops (maximum)
  packet[16] = 0x01; //Options -Disable ack-0x01
  
  //now comes the actual data
  packet[17] = (temperature >> 8) & 0xFF; //MSB
  packet[18] = (temperature >> 0) & 0xFF; //LSB
  
  packet[19] = 0x00;  //Delimiter character sequence
  packet[20] = 0x00;  //Delimiter character sequence
  
  packet[21] = (humidityValue >> 8) & 0xFF; //MSB
  packet[22] = (humidityValue >> 0) & 0xFF; //LSB
  
  
  //checksum
  unsigned int byteSum = 0;
  for(int i=3;i < 23;i++){
    byteSum += packet[i];
  }
  packet[23] = 0xFF - (byteSum & 0xFF);
   
  for(int i=0;i<24;i++){
    Serial.print(packet[i], HEX);
    Serial.print(" ");
    
  }
  Serial.println();
  
  //Writing the byte data to software serial
  for(int i=0;i<20;i++){
    xbee.write(packet[i]);
  }
 }
 
 void readSerial(){
    if(xbee.available()){
      int b =  xbee.read();
      if(b >= 0){
        receiveBuffer[currentReceiveIndex++] = (unsigned char) b;
        //parsePacket();
      }
      if(currentReceiveIndex >= 100){
        Serial.println("Error, receive buffer overflowed");
        currentReceiveIndex = 0; //reset our buffer
      }
      //Serial.print(b,HEX);
     // Serial.print(" ");
    }
}
 
 
 void loop(){
  
  if(millis()-sensorLast > sensorTime){
    sendTempHumdSensor();
    sensorLast = millis(); //wait until next time period passes
  }
  readSerial();
  
  
}
 

