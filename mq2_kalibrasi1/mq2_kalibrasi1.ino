
/************************Hardware Related Macros************************************/
const int MQ_PIN1=A2;                                //define which analog input channel you are going to use
int RL_VALUE1=5;                                     //define the load resistance on the board, in kilo ohms
float RO_CLEAN_AIR_FACTOR1=9.83;                     //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
/***********************Software Related Macros************************************/
int CALIBARAION_SAMPLE_TIMES1=50;                    //define how many samples you are going to take in the calibration phase
int CALIBRATION_SAMPLE_INTERVAL1=500;                //define the time interal(in milisecond) between each samples in the
                                                    //cablibration phase
int READ_SAMPLE_INTERVAL1=50;                        //define how many samples you are going to take in normal operation
int READ_SAMPLE_TIMES1=5;                            //define the time interal(in milisecond) between each samples in 
                                                    //normal operation                                                   
                                                    //which is derived from the chart in datasheet
/**********************Application Related Macros**********************************/
#define         GAS_LPG1             (0)   
#define         GAS_CO1              (1)   
#define         GAS_SMOKE1           (2)                                                        
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
                                                    //to the original curve.
                                                    //data format:{ x, y, slope}; point1: (lg200, 0.53), point2: (lg10000,  -0.22)                                                     
float Ro1           =  10;                
float LPG1;
float CO1;
float smoke1;
const int RELAY=8;
bool on = LOW;
bool off = HIGH;
void setup()
{
 Serial.begin(9600);
 Serial.print("Calibrating...");                        
 Ro1 = MQCalibration1(MQ_PIN1);                           
 Serial.println("done!");                                
 Serial.print("RoMq2BoxIonisator= ");
 Serial.print(Ro1);
 Serial.println("kohm");
 Serial.print("\n");
 pinMode(RELAY,OUTPUT);
 delay(3000);
}
float MQResistanceCalculation1(int raw_adc1)
{
  return ( ((float)RL_VALUE1*(1023-raw_adc1)/raw_adc1));
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
long  MQGetPercentage1(float rs_ro_ratio1, float *pcurve1)
{
  return (pow(10,( ((log(rs_ro_ratio1)-pcurve1[1])/pcurve1[2]) + pcurve1[0])));
}
void Mq21()
{
 smoke1 = MQGetGasPercentage1(MQRead1(MQ_PIN1)/Ro1,GAS_SMOKE1);
 
Serial.print("Concentration of smoke in box ionisator ");
Serial.print("Smoke: ");
Serial.print(smoke1);
Serial.println(" ppm");  

delay(2000);
}
void loop()
{
  Mq21();
}
