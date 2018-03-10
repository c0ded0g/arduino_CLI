char dataString[50] = {0};
int a=0;
int led=13;

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
}

void loop() {
  a = a+1;
  sprintf(dataString,"%02X",a);
  Serial.println(dataString);
  digitalWrite(led,HIGH);
  delay(50);
  digitalWrite(led,LOW);
  delay(950);
}
