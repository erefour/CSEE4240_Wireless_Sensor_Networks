#include <stdlib.h>
#include <SoftwareSerial.h>
#include <XBee.h>


SoftwareSerial xbeeSS(2,3); //Software serial ports for the XBee's Rx & Tx
XBee xbee = XBee();
uint8_t payload[] = {'H', 'e', 'l', 'l', 'o'};
XBeeAddress64 addr64 = XBeeAddress64(0, 0);
ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

void setup(){
  Serial.begin(9600);
  xbeeSS.begin(9600);
  xbee.setSerial(xbeeSS);
  
}

void loop(){
 // Serial.println(sizeof(payload));
  xbee.send(zbTx);
  Serial.println("Sent packet");
  delay(500);
  
}
  
  
