float sensorValue;
float sensorValuenew;
float Karbon;
//2.7343635285;
long RL = 10000; //ohm
long Ro1 = 9942.86594179169;
void setup() {
   pinMode(A0, INPUT);
Serial.begin(9600);
}

void loop() {
int sens = analogRead(A0);
float VRL = sens*5.00/1024;
 Serial.print("VRL: ");
 Serial.println(VRL);
float Rs= ( 5.00* RL/VRL)-RL;
Serial.print(Rs);
Serial.println();
float a = Rs/Ro1;
Karbon = 105.1*pow(a,-1.60);
Serial.print("Karbon monoksida: ");
Serial.print(Karbon);
Serial.println("  ppm");
delay(1000);
}
