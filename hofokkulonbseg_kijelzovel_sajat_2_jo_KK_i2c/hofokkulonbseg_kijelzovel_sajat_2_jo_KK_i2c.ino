//reuk.co.uk November 2013

//for information and the schematic circuit diagram for this
//http://www.reuk.co.uk/wordpress/arduino-solar-water-heating-pump-controller-design-and-code/

// SZUKSEGES KONYVTARAK:
// LiquidCrystal.h -> LiquidCrystal add-in by Arduino

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define NUMBER_OF_PROPERTIES 24

const int PANELSENSOR = A1;
const int TANKSENSOR = A0;
const int RELAY = 13;

const int diffON = 3;
const int diffOFF = 2;
int x = 0;
String textOFF = "KI";
String textON = "BE";
float solarPanelTemperature = 0.0;
float hotWaterTankTemperature = 0.0;
LiquidCrystal_I2C lcd(0x27,20,4);

int R1= 995; // voltage devider resistor
int Vin= 5; // operating voltage of arduino

struct SensorProperty {
  int temperature;
  int resistance;
};

// table of characteristics of the sensor KTY81-110
SensorProperty sensorProperties[NUMBER_OF_PROPERTIES] = {
  {-55, 490}, {-50, 515}, {-40, 567}, {-30, 624}, {-20, 684}, {-10, 747},
  {0, 815}, {10, 886}, {20, 961}, {25, 1000}, {30,1040}, {40, 1122},
  {50, 1209}, {60, 1299}, {70, 1392}, {80, 1490}, {90, 1591}, {100, 1696},
  {110, 1805}, {120, 1915}, {125, 1970}, {130, 2023}, {140, 2124}, {150, 2211}
};

void setup(){
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
 
  lcd.home();

  Serial.begin(9600); 
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD
  lcd.backlight();
  lcd.print("HOMERES INDITASA...");
  pinMode(PANELSENSOR, INPUT);
  pinMode(TANKSENSOR, INPUT);
  
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);
  delay(1000);
  lcd.clear();
   
}

void loop(){
  // homeresek és skálázások
  solarPanelTemperature = getTemperature(PANELSENSOR) ;   
  hotWaterTankTemperature = getTemperature(TANKSENSOR); 
  //solarPanelTemperature = analogRead(PANELSENSOR) * (92.0 / 1023.0);
  //hotWaterTankTemperature = analogRead(TANKSENSOR) * (92.0 / 1023.0);
  Serial.print("solarPanelTemperature: ");
  Serial.print(solarPanelTemperature);
  Serial.print("\t hotWaterTankTemperature: ");
  Serial.println(hotWaterTankTemperature);
  
  // ha feltétel teljesül, bekapcsolás        
  if(solarPanelTemperature > (hotWaterTankTemperature + diffON)){
    Serial.println("relay bekapcsolva");
    digitalWrite(RELAY, HIGH);
    x = 1;
  }
  
  // ha a feltétel teljesül, akkor kikapcsolás  
  if(x == 1 && solarPanelTemperature <= hotWaterTankTemperature + diffOFF){
    digitalWrite(RELAY, LOW);
    x = 0;
    Serial.println(" ki");
  }

  delay(250);
  
  // Hőmérséklet a kijelzőn
  // ----------------
  // KIJELZO ELSO SORA
  lcd.setCursor(0, 0);
  lcd.print("kazan  hofok: ");// a Temp sablon kiiratás
  // lcd.print esetén az első adat az oszlop (0-val kezd), a második adat a sor (0-val kezd ez is)
  lcd.setCursor(14, 0);// a kurzor pozicionálása. 7 = 8. oszlop. 0 = első sor
  lcd.print(solarPanelTemperature);
  
  // KIJELZO MASODIK SORA
  lcd.setCursor(0, 1);// a kurzor pozicionálása. 0 = első oszlop. 1 = második sor
  lcd.print("tarolo hofok: ");// a Temp sablon kiiratás
  lcd.setCursor(14, 1);
  lcd.print(hotWaterTankTemperature);
  
  // KIJELZO HARMADIK SORA
  lcd.setCursor(0, 2);// a kurzor pozicionálása. 0 = első oszlop. 1 = második sor
  lcd.print("szivattyu: ");// a Temp sablon kiiratás
  lcd.setCursor(14, 2);
  if(x == 0){
    lcd.print(textOFF);
  }else{
    lcd.print(textON);
  }

  delay(500);// fél másodperc várakozás
}

float getTemperature(int sensorPin) {
  
  // get currentResistance from sensor
  int raw = analogRead(sensorPin);
  float buffer = raw * Vin;
  float Vout= (buffer)/1023.0;
  buffer = (Vin/Vout) -1;
  float currentResistance = R1 / buffer;

  // calculate temperature based on measured resistance
  SensorProperty propertyBefore = sensorProperties[0];
  SensorProperty propertyAfter = sensorProperties[NUMBER_OF_PROPERTIES-1];
  if(sensorProperties[0].resistance < currentResistance &&  currentResistance < sensorProperties[NUMBER_OF_PROPERTIES-1].resistance) {
    for ( int i = 0; i < NUMBER_OF_PROPERTIES; i++ ) {
      if(currentResistance < sensorProperties[i].resistance) {
        if(i > 1) {
          propertyBefore = sensorProperties[i-1];
        }
        propertyAfter = sensorProperties[i];
        break;
      }
    }
  }
  float temperature = mapfloat(currentResistance, propertyBefore.resistance, propertyAfter.resistance, propertyBefore.temperature, propertyAfter.temperature);
  return temperature;
}

float mapfloat(long x, long in_min, long in_max, long out_min, long out_max) {
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}
