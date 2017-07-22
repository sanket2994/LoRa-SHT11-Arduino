//structure in which temperature and humidity is received
struct data{
  float tempRecv;
  float humRecv;
}*recvData;


#include<SPI.h>			                            //spi hardware library
#include<RH_RF95.h>		                          //radiohead library for rfm95

#define SS_RF95 10	                            //CS pin as pin 10
#define RST_RF95 9                              //RESET pin as pin 9
#define INT_RF95 2                              //interrupt pin as pin 2
#define FREQ_RF95 915.0                         //frequency as 915MHz

//create object of RH_RF95 class
RH_RF95 rf95(SS_RF95,INT_RF95);
//declare receiver buffer
uint8_t recvBuff[RH_RF95_MAX_MESSAGE_LEN];
uint8_t len;
char ack[]="received data bro !!!";
void setup()
{

	recvData=(struct data*)malloc(sizeof(struct data));

  //RESETlogic
	pinMode(RST_RF95,OUTPUT);
	digitalWrite(RST_RF95,HIGH);

	Serial.begin(38400);

	digitalWrite(RST_RF95,LOW);
	digitalWrite(RST_RF95,HIGH);

  //Initialize LoRa module
	if(!rf95.init())
		Serial.println("Initialization not successfull");
	else
		Serial.println("Initializing Successfull");

  //Set frequency to 915MHz
	if(!rf95.setFrequency(FREQ_RF95))
		Serial.println("Frequency not assgned");
	else
		Serial.println("Frequency assigend to 915 MHz");

  //set transmission power
	rf95.setTxPower(5,false);
}

void loop()
{
  //wait untill there is a msg to receive
	if(!rf95.available());

  len = sizeof(recvBuff);
  //receive a msg
	if(rf95.recv(recvBuff, &len))
	{
    recvData=(struct data *)recvBuff;
		Serial.println("The data received is: ");
		Serial.print("Temperature: ");
    Serial.println(recvData->tempRecv,DEC);
		Serial.print("Humidity:" );
	  Serial.print(recvData->humRecv,DEC);
    Serial.println();

    //sending Acknowledgementthat the data was received successfully.
    rf95.send((uint8_t*)ack, sizeof(ack));
    rf95.waitPacketSent();
	}
}
