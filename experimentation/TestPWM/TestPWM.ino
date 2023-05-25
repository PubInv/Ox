/*
 * A simple sketch to explore PWM
 */
 #define PWM_PIN 3
void setup() {
    // initialize
    Serial.begin(9600);
    pinMode(PWM_PIN, OUTPUT);

    pinMode(PWM_PIN,155);
//    TCCR2B = TCCR2B & B11111000 | B00000001; // for PWM frequency of 31372.55 Hz

 //   TCCR0B = TCCR0B & B11111000 | B00000001; // PWM 31372.55 Hz pins 5 and 6 
//    TCCR1B = TCCR1B & B11111000 | B00000001; // PWM 62745.10 Hz pins 9 and 10 
    TCCR2B = TCCR2B & B11111000 | B00000001; // PWM 31372.55 Hz pins 3 and 11 
    delay(1000);
}
const int d = 2000;
const int D_MS = 5000; // for 30 kHz?
void loop() {
  Serial.println("Begin!");
 Serial.println(255);
  analogWrite(PWM_PIN,255);
  delay(D_MS); 
   Serial.println(220);
  analogWrite(PWM_PIN,220);
  delay(D_MS);
    Serial.println(200);
  analogWrite(PWM_PIN,200);
  delay(D_MS);
  Serial.println(190);
    analogWrite(PWM_PIN,190);
  delay(D_MS);
    Serial.println(80);
    analogWrite(PWM_PIN,80);
  delay(D_MS);
      Serial.println(40);
    analogWrite(PWM_PIN,40);
  delay(D_MS);
        Serial.println(12);
    analogWrite(PWM_PIN,12);
  delay(D_MS);
}
