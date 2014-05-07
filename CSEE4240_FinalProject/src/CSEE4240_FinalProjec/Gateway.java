package CSEE4240_FinalProjec;

//package ....git file location
//http://www.bar54.de/2011/03/eclipse-log4j-properties-for-the-whole-workspace/ for log4j ERROR

import java.io.IOException;
import java.net.URLEncoder;
import java.util.StringTokenizer;

import org.apache.http.*;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.fluent.Form;
import org.apache.http.client.fluent.Request;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;

import com.google.gson.*;
import com.rapplogic.xbee.api.ApiId;
import com.rapplogic.xbee.api.AtCommand;
import com.rapplogic.xbee.api.XBee;
import com.rapplogic.xbee.api.XBeeAddress16;
import com.rapplogic.xbee.api.XBeeAddress64;
import com.rapplogic.xbee.api.XBeeResponse;
import com.rapplogic.xbee.api.XBeeTimeoutException;
import com.rapplogic.xbee.api.zigbee.ZNetRxResponse;
import com.rapplogic.xbee.api.zigbee.ZNetTxRequest;
import com.rapplogic.xbee.api.zigbee.ZNetTxStatusResponse;
import com.rapplogic.xbee.util.ByteUtils;


public class Gateway{

	XBee xbee = new XBee();
	
	/**String Tokenizer to replace certain words/characters in a string with another desired word/character
	 * Source: http://www.leepoint.net/notes-java/data/strings/96string_examples/example_replaceWord.html
	 * @param originalStr
	 * @param wordToReplace
	 * @param newReplacementWord
	 * @return
	 */
	static String replaceWords(String originalStr, String wordToReplace, String newReplacementWord) {
	    String resultStr = "";
	    String delimiters = ",";
	    StringTokenizer strTokenizer = new StringTokenizer(originalStr, delimiters, true);
	    while (strTokenizer.hasMoreTokens()) {
	        String currentWord = strTokenizer.nextToken();
	        if (currentWord.equals(wordToReplace)) {
	            resultStr += newReplacementWord;
	        } else {
	            resultStr += currentWord;
	        }
	    }
	    return resultStr;
	}
	
	/*
	 * The 'xbeeAddrToStringPacketFormat' method takes in the string value of the xbee 64-address in the format 
	 * of '0x12,0x34,0x56,0x78,0x99,0xab,0xcd,0xef' and removes the prefix'0x' and the commas in the string to return
	 * a string representation of the xbee address in the format of 'XX XX XX XX XX XX XX XX' to use for the XBeeAddress64
	 * for the ZNetTxRequest Packet!!
	 * 
	 */
	public String xbeeAddrToStringPacketFormat(String rxAddress64STR){
		String addrStrPacketFormat="";
		char charTemp[] = {'0','0'};
		char rxAddrCharArray[]=rxAddress64STR.toCharArray();
		String intStr;
		int count=0;
		String rxAddrStr_Array[] ={"0","0","0","0","0","0","0","0"};
		for(int i=0; i<rxAddrCharArray.length; i++){
			if(rxAddrCharArray[i]=='x'){ //0x00,0x12,0x34 ...etc The'x' indicates that the next to characters are the hex code for the address
				charTemp[0] =rxAddrCharArray[i+1];
				charTemp[1] =rxAddrCharArray[i+2];
				intStr = new String(charTemp);
				
				if(count <8){
					rxAddrStr_Array[count] =intStr;
				//	System.out.println("intStr = " + intStr);
				}
				count++;
			}
		}
		for(int i=0; i<rxAddrStr_Array.length;i++){
			if(i<7){
				addrStrPacketFormat+= rxAddrStr_Array[i] +" ";
			}
			if(i==7){
				addrStrPacketFormat+= rxAddrStr_Array[i];
			}
		}
		//System.out.println("New formated string = " + addrStrPacketFormat);
		return addrStrPacketFormat;
	}
	
	
	
	public Gateway() throws Exception{
	try{
		xbee.open("COM4", 9600);
	
		while (true){
			Thread.sleep(100);	//only execute one time a second (Better sugg
			try{
				System.out.println("Beginning ...");
				XBeeResponse response = xbee.getResponse(10000); //Wait for 10 seconds
				//System.out.println("Beginning 2 ...");
			/****************************************************************
			 * Checking to see if the XBee Series 2 Radio has received any ZNet RX Response packets.
			 * The XBee Radio that is connected to this Gateway Code is acting as the main Coordinator for the Sensor Network!
			 * The Sensor_Motes should be sending packets to this XBee containing any sensor reading and data
			 ****************************************************************/
				if(response.getApiId() == ApiId.ZNET_RX_RESPONSE){
					//Successfully received a ZNET RX packet
					ZNetRxResponse rxPacket =(ZNetRxResponse) response;
					System.out.println("Received an RX packet");
					System.out.println("Address of RX Sender: " + rxPacket.getRemoteAddress64());
					//System.out.println("Address of RX Sender as a string: " + URLEncoder.encode(rxPacket.getRemoteAddress64().toString(), "UTF-8"));				//TODO: make a variable to store the 64bit Address of the xbee radio in the format needed to store in the database 
					String rxAddress64 = URLEncoder.encode(rxPacket.getRemoteAddress64().toString(), "UTF-8");				
					
					int[] dataRX =rxPacket.getData();
				/*	System.out.println(dataRX.length);
					System.out.println("Data (temp): " + dataRX[0] + ", " + dataRX[1]);
					System.out.println("Data (delimiter'11'): " + dataRX[2] + ", " + dataRX[3]);
					System.out.println("Data (humidity): " + dataRX[4] + ", " + dataRX[5]);
				*/
					String tempStr ="", humidityStr ="", pressureStr ="", gps=""; 
					
					//Temperature value is dataRX[0,1] (MSB, LSB)
					if(dataRX[0] !=0){
						tempStr += dataRX[0] +"";
					}
					tempStr += dataRX[1] + "";
		//			System.out.println("tempSTR = " + tempStr);
					
					//Humidity value is dataRX[2,3] (MSB, LSB)
					if(dataRX[2] !=0){
						humidityStr += dataRX[4] +"";
					}
					humidityStr += dataRX[5] + "";
		//			System.out.println("humidityStr = " + humidityStr);
					
					
					/*	
						int readingRXData = dataRX[0] | dataRX[1] <<8;
						System.out.println("Reading of RX Data[]: " + readingRXData);
					 */	
					/**
					 * Connecting to the Database PHP File to access the database in order to store data sent from the radio into the appropriate tables 
					 */
					System.out.println("before php ...");
					
					/***NOTE!!!!!!!
					 * 	When using Localhost Database, use this link for the php file on your local computer and not the SensorNetwork ENGR.UGA.EDU server
						String httpResponse = Request.Get("http://localhost/logSensorData.php?"
					 */
					String httpResponseLogData = Request.Get("http://sensornetworks.engr.uga.edu/sp14/erefour/logSensorData.php?"
						/**NOTE: Write this Request.Get string the way you would manually enter parameter
						 * 		values when typing in the PHP file in the website bar of your internet browser NO spaces!! */
								//+ "password=" 
								//+"mote_id=" + 1 +"&"
								+ "radio_address=" + rxAddress64 +"&" //rxPacket.getRemoteAddress64()+"&" //TODO: Set equal to the variable that stored the 64 bit addr.
								+ "temperature=" + Integer.parseInt(tempStr) +"&"	//TODO: Set equal to the data for temperature
								+ "humidity=" + Integer.parseInt(humidityStr) +"&"
								+ "pressure="+96 +"&"
								+ "gps=" + '7'
							).execute().returnContent().asString();
					
		//			System.out.println("after php ...");
						//NOTE: mote_id is determined on the PHP File using the xbee_address
					//"password=penigun&").execute().returnContent().asString();
				System.out.println("SQL Query-INSERT into LogSensorData Table of the Database = " +httpResponseLogData);
				
				}else{
					System.out.println("Did not receive a packet");
				}
				
			} //Inner try
			catch(Exception e){
			}//catch
			
			
		/*************************************************************************************************************
		* Executing the 'Verify Sensor Motes Data PHP File' so that the Verify_Sensor_Mote table in the Database is updated to keep records
		* of any Sensor Motes that has been reporting/sending sensor readings/data to the XBee Coordinator (to store in the database) that seems to be 
		* inaccurate data, which may indicate that the Sensor Mote is producing errors and faulty data.
		* In this case, the "Verify Sensor Motes Data PHP File" will store this Sensor Mote in the table for the Gateway Code to Poll to send a "verify warning" to
		* the users and Sensor Mote itself (the Sensor Mote will either make a buzzing sound or flash a red/yellow LED). The Verify Sensor Mote table will also
		* be shown on the Website to notify Users to Check the Sensor Mote
		*************************************************************************************************************/
			String httpResponseVerifyLog = Request.Get("http://sensornetworks.engr.uga.edu/sp14/erefour/verifySensorMotesData.php?").execute().returnContent().asString();
			System.out.println("SQL Query-INSERT into Verify Sensor Mote Table of the Database = " +httpResponseVerifyLog);
			
			
			
			
		/**************************************************************************
		 * Polling the Verify Sensor Mote Table of the Database to see if there are any Sensor Motes that need to signaled for verification.
		 * If there is a sensor mote in the table that needs to signaled for verification, the Gateway Code will take the information from that Sensor Mote's
		 * entry in the Verify Sensor Mote table (mote_id, sensor name, and radio_address), and send a "Verify Packet" to that particular Sensor Mote.
		 * 		The Verify Packet will tell the Sensor Mote to either:
		 * 			a) Temperature Sensor Error - Flash Red LED
		 * 			b) Humidity Sensor Error - Flash Yellow LED
		 * 			c) GPS Sensor - Flash Green LED
		 */
			String httpResponseGetCommand = Request.Get("http://sensornetworks.engr.uga.edu/sp14/erefour/getCheckMoteCommand.php?").execute().returnContent().asString();
						//"password=penigun&").execute().returnContent().asString();
			System.out.println("httpRespComm = " + httpResponseGetCommand);
			JsonParser parser = new JsonParser();
			JsonElement jParser = parser.parse(httpResponseGetCommand);
			JsonObject jObjectArray = jParser.getAsJsonObject();
			
			boolean returned_successflag = jObjectArray.get("success_flag").getAsBoolean();
		//	System.out.println("Success Flag= " + returned_successflag);
			JsonArray returned_entryArray = jObjectArray.get("data_field").getAsJsonArray();
			if(returned_successflag = true){
				String moteToCheck_radioAddr = returned_entryArray.get(0).getAsString(); //Getting the 0 element of the JsonArray (jsonArray.get('ith element')) as a string
				String moteToCheck_sensor = returned_entryArray.get(1).getAsString(); //Getting the 1 element of the JsonArray (jsonArray.get('ith element')) as a string
			
		//		System.out.println("moteToCheck_radioAddr ====  " + moteToCheck_radioAddr);
		//		System.out.println("moteToCheck_sensor = " +moteToCheck_sensor);
				
				if(returned_successflag !=false){
					if((moteToCheck_radioAddr!="null") && (moteToCheck_sensor !="null") &&
							(moteToCheck_radioAddr!=null) && (moteToCheck_sensor !=null)){
						
						String rxAddress64CorrFormat_string = xbeeAddrToStringPacketFormat(moteToCheck_radioAddr);
						XBeeAddress64 xbeeAddr64Dest = new XBeeAddress64(rxAddress64CorrFormat_string); //Enter the radio address in db as "00 13 A2 00 40 86 83 9D" so it'll be easily to use as a paramater in the XBeeAdd64 constructuer (int[])
						
						char[] payloadChar = moteToCheck_sensor.toCharArray();
						int[] payloadASCII = new int[payloadChar.length];
						for(int i =0; i<payloadASCII.length; i++){
							payloadASCII[i] = (int)payloadChar[i];
						}
						ZNetTxRequest txPacket = new ZNetTxRequest(xbeeAddr64Dest, payloadASCII);
						
						
						try {
							ZNetTxStatusResponse response = (ZNetTxStatusResponse) xbee.sendSynchronous(txPacket, 10000);
							
							if (response.getDeliveryStatus() == ZNetTxStatusResponse.DeliveryStatus.SUCCESS) {
								System.out.println(" the packet was successfully delivered");
															
							} else {
								// packet failed.  log error
								// it's easy to create this error by unplugging/powering off your remote xbee.  when doing so I get: packet failed due to error: ADDRESS_NOT_FOUND  
								System.out.println("packet failed due to error: " + response.getDeliveryStatus());
							}
							
						} catch (XBeeTimeoutException e) {
				
						}	
					}//if inside of the if(==null)		
				}//if(returned_successflag !=false){
			}//if(returned_successflag =true){
			//System.out.println("Printing json object" + j.get("radio_address").getAsString()); //asking json object to get the field "succes" from database
			/*	JsonArray motes = j.get("data").getAsJsonArray(); //Thats the field name being returned from db query (have to write that in php file)
				for(int i -0li<motes.size();i++){
					//JsonObject obj = motes.get(i).getAsJsonOnject; ==replace motes.get... with just obj
					String radio_address =motes.get(i).getAsJsonObject().get("radio_address").getAsString(); //System.out.println(motes...
					JsonElement mailbox = obj.get("mailbox");
						
					if(!mailbox.isJsonNull()){
							XBeeAddress64 xbeeAddr64 = new XBeeAddress64 //Enter the radio address in db as "00 13 A2 00 40 86 83 9D" so it'll be easily to use as a paramater in the XBeeAdd64 constructuer (int[])
							ZNetTxRequest tx = new ZNetTxRequest(dest. pa_)
							String message = mailbox.getAsString(); 
							System.out.println(message);
					}
					int light_status = motes.get(i).getAsJSonObject().get("light").getAsInt();
					System.out.println(light_status);
					//NOTE that these mailbox, light, radio_addr, etc are all columns inside Dr. Johnsen's db--in the mote table
				}
			*/
			//After polling, we need to create an XBee Transmit package to send to the mote
			
		} //While
	}//First Try
  	finally{
		if(xbee.isConnected()){
			xbee.close();
		}
	}//finally
	}  //(Public Gateway throws)
	
	public static void main(String[] args) throws Exception{
		//PropertyConfigurator.configure(args[0]);
		new Gateway();
	}
	
}