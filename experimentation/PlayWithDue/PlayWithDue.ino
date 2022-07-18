#define THERMO_COUPLE_ONE_WIRE 2

// Note: In a real solution, we need to know the addresses for the temperature sensors
// so that we can be definite about which one is where. See:
// https://lastminuteengineers.com/multiple-ds18b20-arduino-tutorial/
// For now, we will rely on "indexing" and if it is wrong just switch the sensors.
// This is a quick-and-dirty approach for the "RibbonFish" POC.
#define POST_STACK_0_IDX 0
#define POST_HEATER_0_IDX 1



#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire(THERMO_COUPLE_ONE_WIRE);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

double read_temp(int idx) {
  sensors.requestTemperatures(); // Send the command to get temperatures
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
  float tempC = sensors.getTempCByIndex(idx);

  // Check if reading was successful
  if (tempC != DEVICE_DISCONNECTED_C)
  {
//    Serial.print(F("Temperature for the device 1 (index 0) is: "));
//   Serial.println(tempC);
  }
  else
  {
    Serial.print(F("Error: Could not read temperature data: "));
    Serial.println(idx);
  }
  return tempC;
}

const int TRANSISTOR_PIN = 4;
void setup() {
    // initialize
    pinMode(TRANSISTOR_PIN, OUTPUT);
    pinMode(THERMO_COUPLE_ONE_WIRE, INPUT);
    Serial.begin(9600);
    delay(1000);

    
  // Start up the library
  sensors.begin();
}
const int d = 2000;
void loop() {
  Serial.println("on!");
  digitalWrite(TRANSISTOR_PIN,HIGH);
  delay(1000);
  Serial.println("off!");
  digitalWrite(TRANSISTOR_PIN,LOW);
  delay(1000); 
  sensors.requestTemperatures();
  float postStackTempC = read_temp(POST_STACK_0_IDX);
  Serial.print("Post Stack temp:"); 
  Serial.println(postStackTempC);
  float postHeaterTempC = read_temp(POST_HEATER_0_IDX);
  Serial.print("Post Heater temp:"); 
  Serial.println(postHeaterTempC);
}
// END FILE
