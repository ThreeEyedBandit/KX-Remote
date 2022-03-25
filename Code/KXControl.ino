#include <Encoder.h>
#include <AnalogMultiButton.h>

Encoder AGEnc(12, 13);
Encoder RVFOEnc(A0, A1);
Encoder FVFOEnc(A2, A3);
Encoder RFGainEnc(11, 10);
Encoder ISEnc(8, 9);
Encoder BWEnc(6, 7);

const int BUTTONS_VALUES[8] = {0,91,169,235,291,339,382,420};

AnalogMultiButton buttons(A5, 8, BUTTONS_VALUES);

String str_TempRcvd = "";

const int int_AGMin = 0; //Variable Min
const int int_AGMax = 100; //Variable Max
const int int_AGLen = 3; //Power of 10 length

int int_AGCurrent = -1; //Current AG, -1 if no data has been received

const int int_RGMin = 190;
const int int_RGMax = 250;
const int int_RGLen = 3;

int int_RFGainCurrent = -1;

const int int_BWDelta = 5; 
const int int_BWMin = 5;
const int int_BWMax = 400;
const int int_BWLen = 4;

int int_BWCurrent = -1;

const int int_ISDelta = 50; //Filter Change Delta
const int int_ISMin = 400;
const int int_ISMax = 2600;
const int int_ISLen = 4;

int int_ISCurrent = -1;

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{ 
  int AGTemp = AGEnc.read();
  int RVFOTemp = RVFOEnc.read();
  int FVFOTemp = FVFOEnc.read();
  int RFGainTemp = RFGainEnc.read();
  int ISTemp = ISEnc.read();
  int BWTemp = BWEnc.read();

  buttons.update();

  if(buttons.onPress(0))
  {
    Serial.print("SWT11;");
    Serial.print("SWT27;");
  }
  else if(buttons.onPress(1))
  {
    Serial.print("SWT11;");
    Serial.print("SWT19;");
  }
  else if(buttons.onPress(2))
  {
    Serial.print("SWT14;");
  }
  else if(buttons.onPress(3))
  {
    Serial.print("SWT08;");
  }
  else if(buttons.onPress(4))
  {
    Serial.print("SWT20;");
  }
  else if(buttons.onPress(5))
  {
    Serial.print("SWT19;");
  }
  else if(buttons.onPress(6))
  {
    Serial.print("SWH19;");
  }
  else if(buttons.onPress(7))
  {
    Serial.print("SWH32;");
  }
     
  
  if(BWTemp !=0)
  {
    if(int_BWCurrent == -1)
    {
      Serial.print("BW;");
      delay(50);
    }
    else if(BWTemp >= 4 || BWTemp <= -4)
    {
       int_BWCurrent += (BWTemp/4 * int_BWDelta);
       if(int_BWCurrent < int_BWMin){int_BWCurrent = int_BWMin;}
       else if(int_BWCurrent > int_BWMax){int_BWCurrent = int_BWMax;}
       sendData("BW", int_BWCurrent, int_BWLen);
       BWEnc.write(0);
    } 
  }

  
  if(RVFOTemp >= 4)
  {
    Serial.print("UP4;");
    RVFOEnc.write(0);
  }
  else if(RVFOTemp <= -4)
  {
    Serial.print("DN4;");
    RVFOEnc.write(0);
  }

  if(FVFOTemp >=1)
  {
    Serial.print("UP;");
    FVFOEnc.write(0);
  }
  else if(FVFOTemp < -1)
  {
    Serial.print("DN;");
    FVFOEnc.write(0);
  }
  
  if(ISTemp != 0)
  {
    if(int_ISCurrent == -1)
    {
      Serial.print("IS;");
      delay(50);
    }
    else if(ISTemp >= 4 || ISTemp <= -4)
    {
       int_ISCurrent += (ISTemp/4 * int_ISDelta);
       if(int_ISCurrent < int_ISMin){int_ISCurrent = int_ISMin;}
       else if(int_ISCurrent > int_ISMax){int_ISCurrent = int_ISMax;}
       sendData("IS ", int_ISCurrent, int_ISLen);
       ISEnc.write(0);
    }
  }
  if(AGTemp != 0)
  {
    if(int_AGCurrent == -1)
    {
      Serial.print("AG;");
      delay(50);
    }
    else
    {
       int_AGCurrent += AGTemp;
       if(int_AGCurrent < int_AGMin){int_AGCurrent = int_AGMin;}
       else if(int_AGCurrent > int_AGMax){int_AGCurrent = int_AGMax;}
       sendData("AG", int_AGCurrent, int_AGLen);
       AGEnc.write(0);
    }
  }

  if(RFGainTemp != 0)
  {
    if(int_RFGainCurrent == -1)
    {
      Serial.print("RG;");
      delay(50);
    }
    else
    {
       int_RFGainCurrent += RFGainTemp;
       if(int_RFGainCurrent < int_RGMin){int_RFGainCurrent = int_RGMin;}
       else if(int_RFGainCurrent > int_RGMax){int_RFGainCurrent = int_RGMax;}
       sendData("RG", int_RFGainCurrent, int_RGLen);
       RFGainEnc.write(0);  
    }
  }
}

void sendData(String str_CMDSend, int int_Value, int int_Len)
{
   Serial.print(str_CMDSend);
   switch(int_Len)
   {
      case 4:{if(int_Value < 1000 && int_Value >= 100){Serial.print("0");}else if(int_Value < 100 && int_Value >= 10){Serial.print("00");}else if(int_Value < 10){Serial.print("000");}break;}
      case 3:{if(int_Value < 100 && int_Value >= 10){Serial.print("0");}else if(int_Value < 10){Serial.print("00");}break;}
      case 2:{if(int_Value < 10){Serial.print("0");}break;}
   }
   
   Serial.print(String(int_Value));
   Serial.print(";");  
}

void serialEvent()
{   
  while(Serial.available()) {str_TempRcvd += (char)Serial.read();}

  parseRcvd();
}

void parseRcvd()
{
  if(str_TempRcvd.substring(0, 2).equals("AG"))
  {
    int_AGCurrent = (str_TempRcvd.substring(2,str_TempRcvd.length() - 1)).toInt();
  }
  else if(str_TempRcvd.substring(0, 2).equals("RG"))
  {
    int_RFGainCurrent = (str_TempRcvd.substring(2,str_TempRcvd.length() - 1)).toInt();
  }
  else if(str_TempRcvd.substring(0, 2).equals("BW"))
  {
    int_BWCurrent = (str_TempRcvd.substring(2,str_TempRcvd.length() - 1)).toInt();
  }
  else if(str_TempRcvd.substring(0, 2).equals("IS"));
  {
    int_ISCurrent = (str_TempRcvd.substring(3,str_TempRcvd.length() -1)).toInt();
  }
  str_TempRcvd = "";
}


