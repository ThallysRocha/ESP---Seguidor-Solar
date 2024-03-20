#include <Deneyap_Servo.h> //Deneyap_Servo - https://github.com/deneyapkart/deneyap-servo-arduino-library
#include "DHT.h" //DHT kxn - https://github.com/adafruit/DHT-sensor-library
//ldr
#define LDR_TL 15
#define LDR_TR 36
#define LDR_BL 39
#define LDR_BR 4

#define DEFAULT_VALUE_MIN 5000
#define DEFAULT_VALUE_MAX 0

#define QTD_LDRS 4

//servo
#define SERVO_TOP 19
#define SERVO_BOT 23

#define SERVO_INITIAL_POS 120

//dht
#define DHTPIN 0
#define DHTTYPE DHT11

typedef struct {
  int port;
  int value_min;
  int value_max;
  int value;
  int limiar;

} LDR;
void readLDR(LDR* ldr){
  int new_value = analogRead(ldr->port);
  if(ldr->value_max < new_value) ldr->value_max = new_value;
  if(ldr->value_min > new_value) ldr->value_min = new_value;
  ldr->limiar = (ldr->value_max + ldr->value_min)/2;
  ldr->value = new_value;

}
void readLDRs(LDR* ldrs){
  for(int i=0;i<QTD_LDRS;i++){
    readLDR(&(ldrs[i]));
  }
}

DHT dht(DHTPIN, DHTTYPE);
Servo servo_top,servo_bot;
int servoAngleTop = 90;
int servoAngleBot = 90;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
  servo_top.attach(SERVO_TOP);
  servo_bot.attach(SERVO_BOT);
  
}
LDR ldrs [QTD_LDRS] = {
  {LDR_TL,DEFAULT_VALUE_MIN,DEFAULT_VALUE_MAX}
  ,{LDR_TR,DEFAULT_VALUE_MIN,DEFAULT_VALUE_MAX}
  ,{LDR_BL,DEFAULT_VALUE_MIN,DEFAULT_VALUE_MAX}
  ,{LDR_BR,DEFAULT_VALUE_MIN,DEFAULT_VALUE_MAX}
};
int deg=0;
void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();  
  if (isnan(t) || isnan(h)) 
  {
    Serial.println("Failed to read from DHT");
  } 
  else
  {
    Serial.print("Umidade: ");
    Serial.print(h);
    Serial.print(" %t");
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.println(" *C");
  }
  readLDRs(ldrs);
  Serial.println(ldrs[0].value_max);

  bool es = ldrs[0].value > ldrs[0].limiar;
  bool ds = ldrs[1].value > ldrs[1].limiar;
  bool ei = ldrs[2].value > ldrs[2].limiar;
  bool di = ldrs[3].value > ldrs[3].limiar;

  // Serial.printf("Es: %d, Ds: %d, Ei: %d, Di: %d\n",ldrs[0].value,ldrs[1].value,ldrs[2].value,ldrs[3].value);
  Serial.printf("Es: %d, Ds: %d, Ei: %d, Di: %d\n",es,ds,ei,di);

  int x = 0, y = 0;
  
  x = ds + di - es - ei;
  y = es + ds - ei - di;

  if(y){
    x = 0;
  }

  delay(250);

  servoAngleTop -= 2*((y > 0) - (y < 0));
  servoAngleBot -= 2*((x > 0) - (x < 0));
  servoAngleTop = min(max(0, servoAngleTop), 180);
  servoAngleBot = min(max(0, servoAngleBot), 360);
  Serial.printf("x: %d, y: %d\n", x, y);
  Serial.printf("servoAngleTop: %d\n", servoAngleTop);
  Serial.printf("servoAngleBot: %d\n", servoAngleBot);
  servo_bot.write(servoAngleBot);
  servo_top.write(servoAngleTop);
  
}
