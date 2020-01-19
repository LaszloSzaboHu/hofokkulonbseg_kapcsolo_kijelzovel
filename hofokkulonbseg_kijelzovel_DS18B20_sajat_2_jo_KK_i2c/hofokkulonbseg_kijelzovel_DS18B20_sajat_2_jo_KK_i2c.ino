//reuk.co.uk November 2013

//for information and the schematic circuit diagram for this
//http://www.reuk.co.uk/wordpress/arduino-solar-water-heating-pump-controller-design-and-code/

// SZUKSEGES KONYVTARAK:
// LiquidCrystal.h -> LiquidCrystal add-in by Arduino

#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>

const int PANELSENSOR = 6;
const int TANKSENSOR = 7;
const int RELAY = 13;

const int diffON = 3;
const int diffOFF = 2;
int x = 0;
String textOFF = "KI";
String textON = "BE";
float solarPanelTemperature = 0.0;
float hotWaterTankTemperature = 0.0;
LiquidCrystal_I2C lcd(0x27,20,4);

OneWire ds18x20[] = { PANELSENSOR, TANKSENSOR };
const int oneWireCount = sizeof(ds18x20)/sizeof(OneWire);
DallasTemperature sensor[oneWireCount];

void setup(){
  lcd.init();                      // initialize the lcd 
  lcd.backlight(); 
  lcd.home();

  Serial.begin(9600); 

  // Start up the library on all defined bus-wires
  DeviceAddress deviceAddress;
  for (int i = 0; i < oneWireCount; i++) {;
    sensor[i].setOneWire(&ds18x20[i]);
    sensor[i].begin();
    if (sensor[i].getAddress(deviceAddress, 0)) sensor[i].setResolution(deviceAddress, 12);
  }
  
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
  solarPanelTemperature = getTemperature(0) ;   
  hotWaterTankTemperature = getTemperature(1); 
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

float getTemperature(int i) {
  sensor[i].requestTemperatures();
  delay(100);
  return sensor[i].getTempCByIndex(0);
}

