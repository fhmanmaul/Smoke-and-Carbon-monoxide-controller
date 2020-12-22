#define enA 6
#define in1 5
#define in2 3
#include <MQ2.h>
#include <Wire.h>
float outfuz, utotal;
float B, P , S , C; 
float keluaran;
float uRdKarbon,uSdKarbon,uTgKarbon,uSTgKarbon;
float uRdsmoke,uSdsmoke,uTgsmoke;
float Karbon , PWM ;
bool RdKarbon, SdKarbon,TgKarbon ,STgKarbon,Rdsmoke,Sdsmoke,Tgsmoke ;
//2.7343635285;
long RL = 10000; //ohm
long Ro1 = 6120.6199634987561237872;
/************************Hardware Related Macros************************************/
const int MQ_PIN=A1;                                //define which analog input channel you are going to use
int RL_VALUE=5;                                     //define the load resistance on the board, in kilo ohms
float RO_CLEAN_AIR_FACTOR=9.83;                     //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
/************************Hardware Related Macros************************************/
const int MQ_PIN1=A4;                                //define which analog input channel you are going to use
int RL_VALUE1=5;                                     //define the load resistance on the board, in kilo ohms
float RO_CLEAN_AIR_FACTOR1=9.83;                     //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
/***********************Software Related Macros************************************/
int CALIBARAION_SAMPLE_TIMES=50;                    //define how many samples you are going to take in the calibration phase
int CALIBRATION_SAMPLE_INTERVAL=500;                //define the time interal(in milisecond) between each samples in the
                                                    //cablibration phase
int READ_SAMPLE_INTERVAL=50;                        //define how many samples you are going to take in normal operation
int READ_SAMPLE_TIMES=5;                            //define the time interal(in milisecond) between each samples in 
                                                    //normal operation                                                   
                                                    //which is derived from the chart in datasheet
/***********************Software Related Macros************************************/
int CALIBARAION_SAMPLE_TIMES1=50;                    //define how many samples you are going to take in the calibration phase
int CALIBRATION_SAMPLE_INTERVAL1=500;                //define the time interal(in milisecond) between each samples in the
                                                    //cablibration phase
int READ_SAMPLE_INTERVAL1=50;                        //define how many samples you are going to take in normal operation
int READ_SAMPLE_TIMES1=5;                            //define the time interal(in milisecond) between each samples in 
                                                    //normal operation                                                   
                                                    //which is derived from the chart in datasheet                                                    
/**********************Application Related Macros**********************************/
#define         GAS_LPG             0   
#define         GAS_CO              1   
#define         GAS_SMOKE           2                        
/**********************Application Related Macros**********************************/
#define         GAS_LPG1             (0)   
#define         GAS_CO1              (1)   
#define         GAS_SMOKE1           (2)                                   
/*****************************Globals***********************************************/
float           LPGCurve[3]  =  {2.3,0.21,-0.47};   //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent"
                                                    //to the original curve. 
                                                    //data format:{ x, y, slope}; point1: (lg200, 0.21), point2: (lg10000, -0.59) 
float           COCurve[3]  =  {2.3,0.72,-0.34};    //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent" 
                                                    //to the original curve.
                                                    //data format:{ x, y, slope}; point1: (lg200, 0.72), point2: (lg10000,  0.15) 
float           SmokeCurve[3] ={2.3,0.53,-0.44};    //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent" 
                                                    //to the original curve.
                                                    //data format:{ x, y, slope}; point1: (lg200, 0.53), point2: (lg10000,  -0.22)                                                     
/*****************************Globals***********************************************/
float           LPGCurve1[3]  =  {2.3,0.21,-0.47};   //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent"
                                                    //to the original curve. 
                                                    //data format:{ x, y, slope}; point1: (lg200, 0.21), point2: (lg10000, -0.59) 
float           COCurve1[3]  =  {2.3,0.72,-0.34};    //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent" 
                                                    //to the original curve.
                                                    //data format:{ x, y, slope}; point1: (lg200, 0.72), point2: (lg10000,  0.15) 
float           SmokeCurve1[3] ={2.3,0.53,-0.44};    //two points are taken from the curve. 
                                                    //with these two points, a line is formed which is "approximately equivalent" 
float Ro           =  10;                 
float LPG;
float CO;
float smoke; 
float ro           =10;              
float LPG1;
float CO1;
float smoke1;
const int RELAY=8;
bool on = LOW;
bool off = HIGH;
                                                    
void setup() {
 Serial.begin(9600);
 pinMode(A0, INPUT);
 pinMode(5,OUTPUT);
 pinMode(3,OUTPUT);
 RdKarbon = 0; 
 SdKarbon = 0;
 TgKarbon = 0;
 STgKarbon = 0; 
 Rdsmoke = 0; 
 Sdsmoke = 0; 
 Tgsmoke =0;
 B = 0;
 P = 150;
 S = 200;
 C = 255;  
 Serial.print("Calibrating...");                        //LCD display
 Ro = MQCalibration(MQ_PIN);                           //Calibrating the sensor. Please make sure the sensor is in clean air  
 Serial.println("done!");                                //LCD display
 Serial.print("Ro= ");
 Serial.print(Ro);
 Serial.println("kohm");
 delay(1000);
 //============================================================================================================//
 Serial.print("Calibrating...");                        
 ro = MQCalibration1(MQ_PIN1);                           
 Serial.println("done!");                                
 Serial.print("RoMq2BoxIonisator= ");
 Serial.print(ro);
 Serial.println("kohm");
 Serial.print("\n");
 pinMode(RELAY,OUTPUT);
 delay(1000);
}
/****************** MQResistanceCalculation ****************************************
Input:   raw_adc - raw value read from adc, which represents the voltage
Output:  the calculated sensor resistance
Remarks: The sensor and the load resistor forms a voltage divider. Given the voltage
         across the load resistor and its resistance, the resistance of the sensor
         could be derived.
************************************************************************************/ 
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}
float MQResistanceCalculation1(int raw_adc1)
{
  return ( ((float)RL_VALUE1*(1023-raw_adc1)/raw_adc1));
}
 
/***************************** MQCalibration ****************************************
Input:   mq_pin - analog channel
Output:  Ro of the sensor
Remarks: This function assumes that the sensor is in clean air. It use  
         MQResistanceCalculation to calculates the sensor resistance in clean air 
         and then divides it with RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR is about 
         10, which differs slightly between different sensors.
************************************************************************************/ 
float MQCalibration(int mq_pin)
{
  int i;
  float val=0;

  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {            //take multiple samples
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;                   //calculate the average value
  val = val/RO_CLEAN_AIR_FACTOR;                        //divided by RO_CLEAN_AIR_FACTOR yields the Ro                                        
  return val;                                           //according to the chart in the datasheet 

}
float MQCalibration1(int mq_pin1)
{
  int i1;
  float val1=0;

  for (i1=0;i1<CALIBARAION_SAMPLE_TIMES1;i1++) {            //take multiple samples
    val1 += MQResistanceCalculation1(analogRead(mq_pin1));
    delay(CALIBRATION_SAMPLE_INTERVAL1);
  }
  val1 = val1/CALIBARAION_SAMPLE_TIMES1;                   //calculate the average value
  val1 = val1/RO_CLEAN_AIR_FACTOR1;                        //divided by RO_CLEAN_AIR_FACTOR yields the Ro                                        
  return val1;                                           //according to the chart in the datasheet 

}
 
/*****************************  MQRead *********************************************
Input:   mq_pin - analog channel
Output:  Rs of the sensor
Remarks: This function use MQResistanceCalculation to caculate the sensor resistenc (Rs).
         The Rs changes as the sensor is in the different consentration of the target
         gas. The sample times and the time interval between samples could be configured
         by changing the definition of the macros.
************************************************************************************/ 
float MQRead(int mq_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
 
  rs = rs/READ_SAMPLE_TIMES;

  return rs;  
}
float MQRead1(int mq_pin1)
{
  int i1;
  float rs1=0;
 
  for (i1=0;i1<READ_SAMPLE_TIMES1;i1++) {
    rs1 += MQResistanceCalculation1(analogRead(mq_pin1));
    delay(READ_SAMPLE_INTERVAL1);
  }
 
  rs1 = rs1/READ_SAMPLE_TIMES1;

  return rs1;  
}
 
/*****************************  MQGetGasPercentage **********************************
Input:   rs_ro_ratio - Rs divided by Ro
         gas_id      - target gas type
Output:  ppm of the target gas
Remarks: This function passes different curves to the MQGetPercentage function which 
         calculates the ppm (parts per million) of the target gas.
************************************************************************************/ 
long MQGetGasPercentage(float rs_ro_ratio, int gas_id)

{
  if ( gas_id == GAS_LPG ) {
     return MQGetPercentage(rs_ro_ratio,LPGCurve);
  } else if ( gas_id == GAS_CO ) {
     return MQGetPercentage(rs_ro_ratio,COCurve);
  } else if ( gas_id == GAS_SMOKE ) {
     return MQGetPercentage(rs_ro_ratio,SmokeCurve);
  }    
 
  return 0;
}
long MQGetGasPercentage1(float rs_ro_ratio1, int gas_id1)

{
  if ( gas_id1 == GAS_LPG1 ) {
     return MQGetPercentage1(rs_ro_ratio1,LPGCurve1);
  } else if ( gas_id1 == GAS_CO1 ) {
     return MQGetPercentage1(rs_ro_ratio1,COCurve1);
  } else if ( gas_id1 == GAS_SMOKE1 ) {
     return MQGetPercentage1(rs_ro_ratio1,SmokeCurve1);
  }    
 
  return 0;
}
 
/*****************************  MQGetPercentage **********************************
Input:   rs_ro_ratio - Rs divided by Ro
         pcurve      - pointer to the curve of the target gas
Output:  ppm of the target gas
Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm) 
         of the line could be derived if y(rs_ro_ratio) is provided. As it is a 
         logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic 
         value.
************************************************************************************/ 
long  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}
long  MQGetPercentage1(float rs_ro_ratio1, float *pcurve1)
{
  return (pow(10,( ((log(rs_ro_ratio1)-pcurve1[1])/pcurve1[2]) + pcurve1[0])));
}
//==================================================================================/
void Mq2()
{
smoke = MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_SMOKE);
Serial.print("Smoke: ");
Serial.print(smoke);
Serial.println(" ppm");  

   delay(1000);
}
//===============================================================================//
void sensorgasMQ7()
{
int sens = analogRead(A0);
float VRL = sens*5.00/1024;
float Rs= ( 5.00* RL/VRL)-RL;
Serial.println(Rs);
float a = Rs/Ro1;
Karbon = 105.1*pow(a,-1.60);
Serial.print("Karbon monoksida: ");
Serial.print(Karbon);
Serial.println("  ppm");
delay(1000);
}
//===============================================================================//
void Mq21()
{
 smoke1 = MQGetGasPercentage1(MQRead1(MQ_PIN1)/ro,GAS_SMOKE1);
 
Serial.print("Concentration of smoke in box ionisator ");
Serial.print("Smoke: ");
Serial.print(smoke1);
Serial.println(" ppm");
Serial.println();

delay(1000);
}
//===============================================================================//
void relay() {
  if(smoke1 > 25)
  {
    digitalWrite(RELAY,on);
  }
  else
  {
   digitalWrite(RELAY,off);
  }
}
void clr()
{
 Karbon = 0;
 RdKarbon = 0;
 uRdKarbon = 0;
 SdKarbon = 0;
 uSdKarbon = 0;
 TgKarbon = 0;
 uTgKarbon = 0;
 STgKarbon = 0;
 uSTgKarbon = 0;
 smoke =0;
 Rdsmoke = 0;
 uRdsmoke = 0;
 Sdsmoke = 0;
 uSdsmoke = 0;
 Tgsmoke =0;
 uTgsmoke =0;
 outfuz = 0;
 utotal = 0; 
 keluaran = 0;
 smoke1 = 0;
}
void fuzzyfikasi()
{
  //====================cek bagian CO========================
  // cek bagian rendah
  if (Karbon <= 40)
  {
    RdKarbon =1;
    if(Karbon <= 20 )
    {
      uRdKarbon = 1;
     }
     else 
     {
      uRdKarbon = -(Karbon - 40)/20;
     }
   }
   // cek bagian sedang
  if(Karbon <= 60 && Karbon > 20 )
  {
    SdKarbon =1 ;
    if (Karbon <= 40)
    {
      uSdKarbon = (Karbon - 20)/20;
    }
    else
    {
      uSdKarbon = -(Karbon - 60)/20;
    }
  }
  //cek bagian tinggi
  if (Karbon <= 80 && Karbon > 40)
  {
    TgKarbon = 1;
    if (Karbon <= 60)
    {
      uTgKarbon = (Karbon - 40)/ 20;
    }
    else
    {
      uTgKarbon = -(Karbon - 80)/20;
    }
  }
  // cek bagian S.tinggi
  if(Karbon > 60 )
  {
    STgKarbon = 1;
    if (Karbon <=  80 )
    {
      uSTgKarbon = (Karbon - 60)/20;
    }
    else 
    {
      uSTgKarbon = 1;
    }
  }
  //============== cek bagian ASAP==============================
  //cek bagian rendah
  if(smoke <= 25 )
  {
   Rdsmoke =1;
   if (smoke <= 10)
  {
    uRdsmoke = 1;
  }
   else 
     {
      uRdsmoke = -(smoke - 25)/15;
     }
  }
  // cek bagian sedang
  if(smoke <= 40 && smoke > 10 )
  {
    Sdsmoke =1 ;
    if (smoke <= 25)
    {
      uSdsmoke = (smoke - 10)/15;
    }
    else
    {
      uSdsmoke = -(smoke - 40)/15;
    }
  }
  // cek bagian tinggi
  if(smoke > 25)
  {
    Tgsmoke = 1;
    if (smoke <=  40 )
    {
      uTgsmoke = (smoke - 25)/15;
    }
    else 
    {
      uTgsmoke = 1;
    }
  }
}
void inference()
{
  //=-----------rendah CO bertemu Asap--------------------//
  if (RdKarbon ==1 && Rdsmoke ==1)
  {
    outfuz = outfuz + (min(uRdKarbon, uRdsmoke)*B);
    utotal = utotal + min(uRdKarbon,uRdsmoke);
    //Serial.print("outfuz :");
    //Serial.println(outfuz);
    //Serial.print("utotal:");
    //Serial.println(utotal);
    
  }
  if (RdKarbon==1 && Sdsmoke ==1)
  {
    outfuz = outfuz + (min(uRdKarbon, uSdsmoke)*B);
    utotal = utotal + min(uRdKarbon,uSdsmoke);
    //Serial.print("outfuz :");
    //Serial.println(outfuz);
    //Serial.print("utotal:");
    //Serial.println(utotal);

  }
  if (RdKarbon==1 && Tgsmoke ==1)
  {
    outfuz = outfuz + (min(uRdKarbon, uTgsmoke)*P);
    utotal = utotal + min(uRdKarbon,uTgsmoke);
    //Serial.print("outfuz :");
    //Serial.println(outfuz);
    //Serial.print("utotal:");
    //Serial.println(utotal);
  }
  //=-----------sedang CO bertemu Asap--------------------//
   if (SdKarbon==1 && Rdsmoke ==1)
  {
    outfuz = outfuz + (min(uSdKarbon, uRdsmoke)*B);
    utotal = utotal + min(uSdKarbon,uRdsmoke);
    //Serial.print("outfuz :");
    //Serial.println(outfuz);
    //Serial.print("utotal:");
    //Serial.println(utotal);
  }
  if (SdKarbon==1 && Sdsmoke ==1)
  {
    outfuz = outfuz + (min(uSdKarbon, uSdsmoke)*P);
    utotal = utotal + min(uSdKarbon,uSdsmoke);
    //Serial.print("outfuz :");
    //Serial.println(outfuz);
    //Serial.print("utotal:");
    //Serial.println(utotal);
  }
   if (SdKarbon==1 && Tgsmoke ==1)
  {
    outfuz = outfuz + (min(uSdKarbon, uTgsmoke)*S);
    utotal = utotal + min(uSdKarbon,uTgsmoke);
    //Serial.print("outfuz :");
    //Serial.println(outfuz);
    //Serial.print("utotal:");
    //Serial.println(utotal);
  }
  //=-----------tinggi CO bertemu Asap--------------------//
  if (TgKarbon==1 && Rdsmoke ==1)
  {
    outfuz = outfuz + (min(uTgKarbon, uRdsmoke)*P);
    utotal = utotal + min(uTgKarbon,uRdsmoke);
    //Serial.print("outfuz :");
    //Serial.println(outfuz);
    //Serial.print("utotal:");
    //Serial.println(utotal);
  }
  if (TgKarbon==1 && Sdsmoke ==1)
  {
    outfuz = outfuz + (min(uTgKarbon, uSdsmoke)*S);
    utotal = utotal + min(uTgKarbon,uSdsmoke);
   // Serial.print("outfuz :");
   // Serial.println(outfuz);
    //Serial.print("utotal:");
    //Serial.println(utotal);
  }
   if (TgKarbon==1 && Tgsmoke ==1)
  {
    outfuz = outfuz + (min(uTgKarbon, uTgsmoke)*C);
    utotal = utotal + min(uTgKarbon,uTgsmoke);
    //Serial.print("outfuz :");
    //Serial.println(outfuz);
    //Serial.print("utotal:");
    //Serial.println(utotal);
  }
  //=-----------Stinggi CO bertemu Asap--------------------//
   if (STgKarbon==1 && Rdsmoke ==1)
  {
    outfuz = outfuz + (min(uSTgKarbon, uRdsmoke)*S);
    utotal = utotal + min(uSTgKarbon,uRdsmoke);
    //Serial.print("outfuz :");
    //Serial.println(outfuz);
    //Serial.print("utotal:");
    //Serial.println(utotal);
  }
  if (STgKarbon==1 && Sdsmoke ==1)
  {
    outfuz = outfuz + (min(uSTgKarbon, uSdsmoke)*C);
    utotal = utotal + min(uSTgKarbon,uSdsmoke);
    //Serial.print("outfuz :");
    //Serial.println(outfuz);
    //Serial.print("utotal:");
    //Serial.println(utotal);
  }
   if (STgKarbon==1 && Tgsmoke ==1)
  {
    outfuz = outfuz + (min(uSTgKarbon, uTgsmoke)*C);
    utotal = utotal + min(uSTgKarbon,uTgsmoke);
    //Serial.print("outfuz :");
    //Serial.println(outfuz);
    //Serial.print("utotal:");
    //Serial.println(utotal);
  }}
  void defuzifikasi()
{
  keluaran = outfuz/utotal;
  analogWrite(enA, keluaran);
  digitalWrite(in1,LOW);
  digitalWrite(in2, HIGH);
  Serial.print("PWM : ");
  Serial.println(keluaran);
  delay(1000);
}
void loop() {
 Mq2();
 sensorgasMQ7();
 fuzzyfikasi();
 inference();
 defuzifikasi();
 Mq21();
 relay();
 clr();
}
