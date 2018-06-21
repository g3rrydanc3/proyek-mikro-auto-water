#include <OneWire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

const int trigUltra = 38;
const int echoUltra = 39;
const int pompa = 37;

const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns

char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = { 36, 34, 32, 30 };
byte colPins[COLS] = { 28, 26, 24 }; 

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 

Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

double persen;
long duration, distance, jarak;
long batasnol = -1, bataspenuh = -1, batasnolLama, bataspenuhLama;
bool lcdlampu = true;
String inputString;
char junk, inChar;
int statusPompa = 0;

void printJudul(){
  lcd.setCursor(0,0);
  lcd.print("   Auto Water   ");
  lcd.setCursor(0,1);
  lcd.print("     Filler     ");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(trigUltra, OUTPUT);
  pinMode(echoUltra, INPUT);
  pinMode(pompa, OUTPUT);

  digitalWrite(pompa,LOW);

  lcd.begin(16,2);
  lcd.backlight();
  delay(250);
  lcd.noBacklight();
  delay(250);
  lcd.backlight();

  printJudul();
  delay(5000);

  
  Serial.println("setup finished");
}

void SonarSensor(int trigPin, int echoPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;
}

void nyalakanPompa(){
  statusPompa = 1;
  digitalWrite(pompa, HIGH);
}

void matikanPompa(){
  statusPompa = 0;
  digitalWrite(pompa, LOW);
}

void jalan(){
  SonarSensor(trigUltra, echoUltra);
  jarak = distance;

  if(batasnol==-1){
    lcd.setCursor(0,0);
    lcd.print("Kosong : null");
  } else {
    if(batasnol!=batasnolLama){
      lcd.clear();
      batasnolLama = batasnol;
    }
    lcd.setCursor(0,0);
    lcd.print("Kosong : ");
    lcd.print(batasnol);
  }

  if(bataspenuh==-1){
    lcd.setCursor(0,1);
    lcd.print("Penuh  : null");
  } else {
    if(bataspenuh!=bataspenuhLama){
      lcd.clear();
      bataspenuhLama = bataspenuh;
    }
    lcd.setCursor(0,1);
    lcd.print("Penuh  : ");
    lcd.print(bataspenuh);
  }

  char key = kpd.getKey();
  if(key)  // Check for a valid key.
  {
    switch (key)
    {
      case '*':
        batasnol = jarak;
        Serial.print("Batas Kosong : ");
        Serial.println(batasnol);
        break;
      case '#':
        bataspenuh = jarak;
        Serial.print("Batas Penuh : ");
        Serial.println(bataspenuh);
        break;
      case '0':
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Kondisi Air :");
        lcd.setCursor(0,1);
        lcd.print((int)persen);
        lcd.print("%");
        delay(2000);
        lcd.clear();
        break;
      case '1':
        if (lcdlampu==true){
          lcd.noBacklight();
          lcdlampu=false;
        } else {
          lcd.backlight();
          lcdlampu=true;
        }
        break;
      case '2':
        nyalakanPompa();
        break;
      case '3':
        matikanPompa();
        break;
      default:
        break;
    }
  }

  persen = (double)(jarak-batasnol)/(bataspenuh-batasnol);
  persen *= 100;

  if((int)persen>=100) {
    matikanPompa();
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()) {
    while (Serial.available())
    {
      char inChar = (char)Serial.read();
      inputString += inChar;
    }

    while (Serial.available() > 0)
    {
      junk = Serial.read() ;
    }
    
    inputString.trim();
    
    if (inputString == "A") {
      jalan();
      Serial.print((int)persen);
      Serial.print("%");
      Serial.println(statusPompa);
    } else if (inputString == "N") {
      jalan();
      nyalakanPompa();
    } else if (inputString == "M") {
      jalan();
      matikanPompa();
    } 
    
    inputString = "";
    
  } else {
    jalan();
  }
  
}
