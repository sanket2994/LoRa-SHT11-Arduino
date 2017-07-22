//structure to strore temperature and humidity readings
struct data{
    float tempSend;
    float humSend;
};

#include<SPI.h>			                  //spi hardware library
#include<RH_RF95.h>		                //radiohead library for rfm95
extern "C"{
  #include<sht11.h>		               //sht11 library
}
#define SCK_SHT 4                   //set clk pin as pin 4
#define SDA_SHT 5                   //set clk pin as pin 5

#define SS_RF95 10	                 //set CS as pin 10
#define RST_RF95 9                   //set reset pin as pin 9
#define INT_RF95 2                   //set interrupt as pin 2
#define FREQ_RF95 915.0              //set frequency to 915MHz

RH_RF95 rf95(SS_RF95,INT_RF95);       //create object of RH_RF95
struct data *sendData;
uint8_t recvAckBuf[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len;
void setup()
{
  len = sizeof(recvAckBuf);
  sendData=(struct data*)malloc(sizeof(struct data));

  //RESET logic
	pinMode(RST_RF95,OUTPUT);
	digitalWrite(RST_RF95,HIGH);

	Serial.begin(38400);

	digitalWrite(RST_RF95,LOW);
	digitalWrite(RST_RF95,HIGH);

  //initialize the rf95
	if(!rf95.init())
		Serial.println("Initialization not successfull");
	else
		Serial.println("Initializing Successfull");

  //change the frequency to 915MHz
	if(!rf95.setFrequency(FREQ_RF95))
		Serial.println("Frequency not assgned");
	else
		Serial.println("Frequency assigend to 915 MHz");

  //set transmission power
	rf95.setTxPower(5,false);
}


void loop()
{
	//calculate the temperature and humidity from sensor
  sendData->tempSend = readTempC(SDA_SHT, SCK_SHT);
	sendData->humSend = readHumidity(SDA_SHT, SCK_SHT);

	Serial.print("The temp is: ");
	Serial.println(sendData->tempSend,DEC);
	Serial.print("The Humidity val is: ");
	Serial.println(sendData->humSend,DEC);

  //Send the data using LoRa
	rf95.send((uint8_t*)sendData, sizeof(struct data));
	rf95.waitPacketSent();

  //wait for specific time for message to receive msg
	bool recAck =  rf95.waitAvailableTimeout(1000);
	if(recAck)
	{
		//receive the ack Buffer
		if(rf95.recv(recvAckBuf,&len))
		{
			Serial.println("The Received Buffer is:  ");
			Serial.println((char*)recvAckBuf);
		}
		else
			Serial.println("Acknowlwdgement not received ");
	}
	else
		Serial.println("Acknowledgement not received");

	delay(5000);
}
