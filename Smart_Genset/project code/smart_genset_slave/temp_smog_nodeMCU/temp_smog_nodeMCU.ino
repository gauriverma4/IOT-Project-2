#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <String.h>
#define ACCEPT_TYPE "text/csv" 
WiFiClient client;

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include<Wire.h>
Adafruit_MPU6050 mpu;

const char ssid[] = "GLA-CSED";
const char password[] = "GLACAMPUS";
const int httpsPort = 7080;
const char host[] = "dcs.glaitm.org"; 
const char appKey[]= "8045dcca-ce0c-4a50-a8fb-47b9785604cf";


const char Thing[] = "LM35_thing_1";
const char Property[] = "current_t";

const char Thing1[] = "LM35_thing_1";
const char Property1[] = "Energy_t";

const char Thing2[] = "LM35_thing_1";
const char Property2[] = "Frequency_t";

const char Thing3[] ="LM35_thing_1";
const char Property3[]="level";

const char Thing4[] = "LM35_thing_1";
const char Property4[] = "pollution_level";

const char Thing5[] = "LM35_thing_1";
const char Property5[] = "Power_factor_t";

const char Thing6[] = "LM35_thing_1";
const char Property6[] = "power_t";

const char Thing7[] = "LM35_thing_1";
const char Property7[] = "RPM";

const char Thing8[] = "LM35_thing_1";
const char Property8[] = "Temperature1";

const char Thing9[] = "LM35_thing_1";
const char Property9[] = "Temperature2";

const char Thing10[] = "LM35_thing_1";
const char Property10[] = "Voltage_t";

const char Thing11[] = "LM35_thing_1";
const char Property11[] = "X-axis";

const char Thing12[] = "LM35_thing_1";
const char Property12[] = "Y-axis";


const char Thing13[] = "LM35_thing_1";
const char Property13[] = "Z-axis";

void setup() 
{
  Wire.begin(4,5);  
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
{
    delay(500);
    Serial.print(".");
 }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP()); 


/////////////////gyro/////////////////////
Wire.begin(4,5);
  if (!mpu.begin()) 
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1) 
    {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);  
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  delay(100);

}


void loop() 
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  Wire.requestFrom(8, 80);
  char array[80];
  int i=0;
  while (0 < Wire.available())
  {
    char c = Wire.read();
    array[i] = c;
    Serial.print(array[i]);
    i++;
    
  }
  Serial.println("");
 
  char *strings[11]; 
  char *ptr = NULL;   
  byte index = 0;
  ptr = strtok(array, ",");
  while (ptr != NULL)
  {
    strings[index] = ptr;
    index++;
    ptr = strtok(NULL, ",");
  }
  Serial.println(strings[0]);   //temperature1
  Serial.println(strings[1]);   //temperature2
  Serial.println(strings[2]);   //smog
  Serial.println(strings[3]);   //proximity
  Serial.println(strings[4]);   //voltage
  Serial.println(strings[5]);   //current
  Serial.println(strings[6]);   //power
  Serial.println(strings[7]);   //energy
  Serial.println(strings[8]);   //frequency
  Serial.println(strings[9]);   //pf
  Serial.println(strings[10]);  //fuel level
  
  Put(Thing,Property,strings[5]);
  Put(Thing1,Property1,strings[7]);
  Put(Thing2,Property2,strings[8]);
  Put(Thing2,Property3,strings[10]);
  Put(Thing3,Property4,strings[2]);
  Put(Thing4,Property5,strings[9]);
  Put(Thing5,Property6,strings[6]);
  Put(Thing6,Property7,strings[3]);
  Put(Thing7,Property8,strings[0]);
  Put(Thing8,Property9,strings[1]);
  Put(Thing9,Property10,strings[4]);
  Put(Thing10,Property11,String(g.gyro.x));
  Put(Thing11,Property12,String(g.gyro.y));
  Put(Thing12,Property13,String(g.gyro.z));

  delay(1000);
}

void Put(String ThingName, String ThingProperty, String Value)
  {

    Serial.println(host);
  if (!client.connect(host, httpsPort))
 {
    Serial.println("connection failed");
    return;
  } else

 {
    Serial.println("Connected to ThingWorx.");
  }
  String url = "/Thingworx/Things/" + ThingName + "/Properties/" + ThingProperty; 
  Serial.print("requesting URL: ");
  Serial.println(url);

  String strPUTReqVal = "{\"" + ThingProperty + "\":\"" + Value + "\"}";  
  Serial.print("PUT Value: ");
  Serial.println(strPUTReqVal);

  client.print(String("PUT ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "appKey:"+ appKey + "\r\n" +
               "x-thingworx-session: false" + "\r\n" +
               "Accept: application/json" + "\r\n" +
               "Connection: close" + "\r\n" +
               "Content-Type: application/json" + "\r\n" +
               "Content-Length: " + String(strPUTReqVal.length()) + "\r\n\r\n" +
               strPUTReqVal + "\r\n\r\n");   

  while (client.connected()) 
{
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  client.stop();
}

String Get(String get_Thing, String get_Property)
{              
        HTTPClient http;
        int httpCode = -1;
        String fullRequestURL = "http://" + String(host) +":"+ String(httpsPort)+ "/Thingworx/Things/" 
                                + get_Thing + "/Properties/" + get_Property + "?appKey=" + appKey;
                                
        Serial.println(fullRequestURL);
        http.begin(client, fullRequestURL);
        http.addHeader("Accept",ACCEPT_TYPE,false,false);        
        httpCode = http.GET();
        Serial.println(httpCode);
        
        String responses;
        if(httpCode > 0)        
        {             
           responses= http.getString(); 
             //Serial.println(responses);      
             //Serial.print(responses[9]);        
        }
        else         
        {
            Serial.printf("[httpGetPropertry] failed, error: %s\n\n", http.errorToString(httpCode).c_str());
        }
        http.end();
        return responses;        
}
