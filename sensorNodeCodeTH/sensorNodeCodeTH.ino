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

static int commandType=0;
const int redLEDPin_Temp = 13;
const int yellowLEDPin_Humid = 12;
const int greenLEDPin_Gps = 11;

const int tempPIN = A0;             // Pin A0 is for the temperature sensor
const int humidPIN = A1;            // Pin A1 is for the humdity sensor
const int vSupply_humid = 5;       //Supplying 5 volts to the humidity sensor
//const int chipSelect = 4;

 void setup(){
     Serial.begin(9600);
     xbee.begin(9600);
     pinMode(redLEDPin_Temp, OUTPUT);
     pinMode(yellowLEDPin_Humid, OUTPUT);
     pinMode(greenLEDPin_Gps, OUTPUT);
   
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
 /* int humidityValue = ((0.0004*tempPIN + 0.149)*(analogRead(humidPIN)) - (0.0617*tempPIN + 24.436));
  delay(200);
 */
  float vout_humid = analogRead(humidPIN) *5.0f/1023.f;    //Analog to Digital Converter = Takes a voltage and output a corresponding bit values: 0-5V == (0-1023 bit value)
  float sensorRH = (vout_humid -0.958)/0.0307;
  float trueRH = (sensorRH)/(1.0546 - (0.00216*temperature));
   int humidityValue = (int)trueRH;
    
    /*When the Humidity Sensor is not plugged in correctly/the connection wire for the Vout output of the sensor is not connected properly, 
      the humidity sensor (float trueRH) will generally (majority of the time) have a negative value ( less than < 0).
      Since a negative float value can not be represented as a negative value for an integer (and therefore as a hexidemical for the xbee packet (at least to my knowledge)), I will use an
      special value of int humidityValue ='111' which is 6F in Hexidecimal (0x6F)
      
      NOTE: Have to use a digit value that is no more than 127 (7F in hexi) since that's the highest digit value for the ASCII code which has a corresponding HEXI value that can
             be intrepreted correctly when another program (Gateway Code in java) is trying to convert the hexi value being sent through the xbee packet back to its digit value!!!
    */
    if(trueRH <0 ){
      humidityValue = 111; //Since a negative value cannot be represented as an int in Arduino and therefore hexidemical for the xbee packet (at least to my knowledge), 
                              // I am using the value of '111' as a special indicator of a negative sensor value.
                              //Plus the humidity sensor should never have a reading of over 100% rh (relative humidity) since the max percent reading is 100% according to the DATASHEET
    }
   

   Serial.println("Temperature = " + temperature);
   Serial.println("Humidity = " + humidityValue);
 
 
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
    
    packet[19] = 0x01;  //Delimiter character 0x01 is Decimal Value "1"
    packet[20] = 0x01;  //Delimiter character sequence ('1','1')
    
    packet[21] = (humidityValue >> 8) & 0xFF; //MSB
    packet[22] = (humidityValue >> 0) & 0xFF; //LSB
    
    
    //checksum
    unsigned int byteSum = 0;
    for(int i=3;i < 23;i++){
      byteSum += packet[i];
    }
    packet[23] = 0xFF - (byteSum & 0xFF);
     
    for(int i=0;i<24;i++){
    //  Serial.print(packet[i], HEX);
   //   Serial.print(" ");
      
    }
   // Serial.println();
    
    //Writing the byte data to software serial
    for(int i=0;i<24;i++){
      xbee.write(packet[i]);
    }
   }
   
   /*************************************8
     Determine Command Method
   **************************************/
   int determineCommand(unsigned char arrayData[], int arraySize){
     
     for(int j =0; j <arraySize; j++){
       Serial.print((char)arrayData[j]);
     }
     
     unsigned char temperatureSensor[] = {'t','e','m','p','e','r','a','t','u','r','e'};
     unsigned char humiditySensor[] ={'h','u','m','i','d','i','t','y'};
     unsigned char gpsSensor[] ={'g','p','s'};
     
     int flagSensor =0;
     if(arraySize ==11){
         int eleven =11;
         for(int i=0;i<11;i++){
           if(arrayData[i]!=temperatureSensor[i]){
             eleven= eleven-1;
           }
         }
         if(eleven ==11){
          // Serial.print("t");
           flagSensor =1;   //1 = Temperature
         }
     }
     else if(arraySize ==8){
         int eight =8;
         for(int i=0;i<8;i++){
           if(arrayData[i]!=humiditySensor[i]){
             eight= eight-1;
           }
         }
         if(eight ==8){
           //Serial.print("h");
           flagSensor =2;   //1 = Temperature
         }
     }
      else if(arraySize ==3){
         int three =3;
         for(int i=0;i<3;i++){
           if(arrayData[i]!=gpsSensor[i]){
             three= three-1;
           }
         }
         if(three ==3){
           //Serial.print("g");
           flagSensor =3;   //1 = Temperature
         }
     }
     else{
       //Serial.print("-1");
        flagSensor =-1; 
     }
     
     return flagSensor;
   }
   
   
   
   
   /****************************8
   PARSE PACKET METHOD
   ****************************/
   
   void parsePacket(){
        if(receiveBuffer[0] != 0x7E){
          currentReceiveIndex = 0;
          return;
        }
        if(currentReceiveIndex < 3){
          return;
        }
        unsigned int length = ((receiveBuffer[1] << 8) & 0xFF00) | receiveBuffer[2]; //[1] &[2] is the length MSB-LSB
        
        if(currentReceiveIndex < 4 + length){
          return;
        }
        unsigned char * frameData = receiveBuffer + 3;  //Starting the data at [3] which is the start of frama data API Package
        int checksum = frameData[length+1];
        
        switch(frameData[0]){
            case 0x90: //ZigBee receive packet
              Serial.println("received an rx packet");
              currentReceiveIndex = 0;
              break;
       
            default:
              Serial.println("Unknown packet received");
              currentReceiveIndex = 0;
              break;
        }
        //Getting the MSB & LSB of the frame data (framedata(1-8))
        frameDataMSB[0] =frameData[1];
        frameDataMSB[1] =frameData[2];
        frameDataMSB[2] =frameData[3];
        frameDataMSB[3] =frameData[4];
        
        frameDataLSB[0] =frameData[5];
        frameDataLSB[1] =frameData[6];
        frameDataLSB[2] =frameData[7];
        frameDataLSB[3] =frameData[8];
        
        Serial.println("FrameData MSB: ");
        for(int i=0; i<4;i++){
         // Serial.println(frameDataMSB[i]);
        }
        Serial.println("FrameData LSB: ");
        //16-bit address of sender
        frameDataAddr[0] = frameData[9];
        frameDataAddr[1] = frameData[10];
        
        //Options =frameData[11]
        
        //RF Data, the actual DATA THAT WAS SENT -frameData[12 --n] where n=byte length of entire RF packet frame
        unsigned char frameRFData[length -12]; //the RF data should be size 12th byte -nth byte
        //Serial.println("The actual data (RF Data) of the packet was: ");
    
         for(int i =0; i<length -12; i++){
           frameRFData[i] = frameData[i+12]; 
           Serial.println((char)frameRFData[i]);//, HEX);
        }
         commandType =determineCommand(frameRFData, length-12);
  }
  
   
   void excuteVerifyCommand(int sensorCommand){
   
     switch (sensorCommand){
       case 1:  //Temperature
         // Serial.println("100");
         digitalWrite(redLEDPin_Temp, HIGH);
         delay(5000);
         digitalWrite(redLEDPin_Temp, LOW);
         delay(1000);
         digitalWrite(redLEDPin_Temp, HIGH);
         delay(5000);
         digitalWrite(redLEDPin_Temp, LOW);
         break;
        
      case 2:  //Humidiity
          digitalWrite(yellowLEDPin_Humid, HIGH);
          delay(5000);
          digitalWrite(yellowLEDPin_Humid, LOW);
          delay(1000);
          digitalWrite(yellowLEDPin_Humid, HIGH);
          delay(5000);
          digitalWrite(yellowLEDPin_Humid, LOW);
          break;
          
      case 3:  //GPS
          digitalWrite(greenLEDPin_Gps, HIGH);
          delay(5000);
          digitalWrite(greenLEDPin_Gps, LOW);
          delay(1000);
          digitalWrite(greenLEDPin_Gps, HIGH);
          delay(5000);
          digitalWrite(greenLEDPin_Gps, LOW);
          break;
      default:
          //Do nothing
          break;
     }
  }
   
   
   
   
 void readSerial(){
    if(xbee.available()){
      int b =  xbee.read();
      if(b >= 0){
        receiveBuffer[currentReceiveIndex++] = (unsigned char) b;
        parsePacket();
        //Serial.println(commandType);
        excuteVerifyCommand(commandType);
        commandType =0;
      }
      if(currentReceiveIndex >= 100){
        Serial.println("Error, receive buffer overflowed");
        currentReceiveIndex = 0; //reset our buffer
      }
     // Serial.print(b,HEX);
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
 

