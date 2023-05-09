#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>

#include<Wire.h>

SoftwareSerial energyMeter(4, 3);
PZEM004Tv30 pzem;

String data;

#define tempEngine A0
#define tempAlternator A1

int ETArray[10], a;
int ATArray[10], b;
int i, j;

float avgET = 0;
float avgAT = 0;

int sumET = 0;
int sumAT = 0;

int tempET;
int tempAT;

int smoke=A2;
int S[10],T[10],te;
float avg;

char c;
float rev=0;
int rpm;

void isr()
{
  rev++;
}


void setup()
{
  Wire.begin(8);
  Wire.onRequest(SendEvent);
  
  pinMode(smoke,INPUT);

  Serial.begin(9600);
  
  attachInterrupt(0,isr,RISING);
  
  pzem = PZEM004Tv30(energyMeter);
}
void loop()
{
  ////////////////////Enginetemp/////////////////////////////////

  for (int i = 0; i < 10; i++)
  {
    ETArray[i] = analogRead(tempEngine);
    // Serial.println("ET="+ETArray[i]);
    delay(50);
  }
  for (i = 0; i < 10; ++i)
  {
    for (j = i + 1; j < 10; ++j)
    {
      if (ETArray[i] > ETArray[j])
      {
        a = ETArray[i];
        ETArray[i] = ETArray[j];
        ETArray[j] = a;
      }
    }
  }
  sumET = 0;

  for (i = 2; i < 8; i++)
  {
    sumET += ETArray[i];
  }
  avgET = sumET / 6;
  float millivoltET = avgET * 4.8875855327468;
  float tempET = millivoltET / 100;
  Serial.println("Engine temperature =" + String(tempET));
  delay(1000);




  ////////////*****ALTERNATORTEMP*****///////////////////////

  for (i = 0; i < 10; i++)
  {
    ATArray[i] = analogRead(tempAlternator);
    //Serial.println("ET="+ETArray[i]);
    delay(50);
  }
  for (i = 0; i < 10; ++i)
  {
    for (j = i + 1; j < 10; ++j)
    {
      if (ATArray[i] > ATArray[j])
      {
        b = ATArray[i];
        ATArray[i] = ATArray[j];
        ATArray[j] = b;
      }
    }
  }
  sumAT=0;
  for (i = 2; i < 8; i++)
  {
    sumAT += ATArray[i];
  }
  avgAT = sumAT / 6;
  float millivoltAT = avgAT * 4.8875855327468;
  float tempAT = millivoltAT / 10;
  Serial.println("alternator temperature =" + String(tempAT));
  delay(1000);
  //////////////////smog//////////////////////
  for(i=0;i<10;i++)
  {
   S[i]= analogRead(smoke);
   
  }
  for(i=0;i<9;i++)
  {
    for(j=i+1;j<10;j++)
    {
      if(S[i]>S[j])
      {
        te=S[i];
        S[i]=S[j];
        S[j]=te;
      }
    }
  }
 int  s=0;
  for(i=2;i<8;i++)
  {
   s=s+S[i]; 
  }
  avg=s/6.0;
  Serial.println("the smog level average is" +String(avg));
 
  delay(1000);


//////////////////proximity///////////////////////
  attachInterrupt(0,isr,RISING);
  delay(1000);
  detachInterrupt(0);           
  rpm = rev * 60;    
  rev=0;
  Serial.print("RPM =");
  Serial.println(rpm);

                  ///////////////energy meter/////////////////////////



//////////////////////!!!!!!!!!!!!!!!!!!!voltage
float voltage = pzem.voltage();
if(isnan(voltage))
{
  voltage = 0.0;
//Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
} 


////////////////////////!!!!!!!!!!!!!!!!!!!current
float current = pzem.current();
if(isnan(current))
{
  current=0.0;
//Serial.print("Current: "); Serial.print(current); Serial.println("A");
} 
//else 
//{
//  current=0;
//Serial.println("Error reading current");
//}

///////////////////////!!!!!!!!!!!!!!!!!!!power
float power = pzem.power();
if(isnan(power))
{
  power=0.0;
//Serial.print("Power: "); Serial.print(power); Serial.println("W");
} 
//else 
//{
//  power=0;
//Serial.println("Error reading power");
//}

///////////////////!!!!!!!!!!!!!!!!!!energy
float energy = pzem.energy();
if(isnan(energy))
{
  energy=0.0;
//Serial.print("Energy: "); Serial.print(energy,3); Serial.println("kWh");
} 
//else 
//{
//  energy=0;
//Serial.println("Error reading energy");
//}

////////////////////!!!!!!!!!!!!!!!!!!!!frequency
float frequency = pzem.frequency();
if(isnan(frequency))
{
  frequency=0.0;
//Serial.print("Frequency: "); Serial.print(frequency, 1); Serial.println("Hz");
}
//else 
//{
//  frequency=0;
//Serial.println("Error reading frequency");
//}

/////////////!!!!!!!!!!!!!!!!!!!!!!!!!powerfactor
float pf = pzem.pf();
if(isnan(pf))
{
  pf=0.0;
//Serial.print("PF: "); Serial.println(pf);
} else 
//{
//  pf=0;
//Serial.println("Error reading power factor");
//}
Serial.println();
delay(1000);



/////////////////////////fuel level///////////////////////////////
  int tankvalue = 0;
  int value=analogRead(A3);
  //Serial.println(value);
  tankvalue=map(value,0,167,0,100);//min value=0;max value=167

  Serial.print("Fuel level =");
  Serial.println(tankvalue);
  delay(1000);
  
//////////////////////////////////finalstring/////////////////////////////////

  data = String(tempET) + "," + String(tempAT) + "," +String(avg) + "," + String(rpm) + "," + String(voltage) + "," + String(current) + "," + String(power) + "," + String(energy,3) + "," + String(frequency) + "," + String(pf) + "," + String(tankvalue) +",";
  Serial.println(data);
}

void SendEvent(int howmany)
{
  for (int i = 0; i < data.length(); i++)
  {
    Wire.write(data[i]);
  }
}
