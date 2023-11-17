#include <DHT.h>
#include <DHT_U.h>

#include <LiquidCrystal.h>

int lcd_rs = 12, lcd_en = 11, lcd_d4 = 4, lcd_d5 = 5, lcd_d6 = 6, lcd_d7 = 7;

LiquidCrystal lcd(lcd_rs, lcd_en, lcd_d4, lcd_d5, lcd_d6, lcd_d7);

const int pinoSensorSolo = A0;
const int pinoLed = 8;
const int porcentagemSeco = 60;
int umidadeSolo = 0;
float umidadeAr = 0;
float temperatura = 0;

int PINO_DHT = A5;

#define DHTTYPE DHT11;

DHT dht(PINO_DHT, DHT11);

uint8_t icone[] = { 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f };

void setup() {

  lcd.createChar(0, icone);

  lcd.begin(16, 2);

  Serial.begin(9600);

  dht.begin();

  pinMode(pinoLed, OUTPUT);

  apresentacao();
}

void apresentacao() {
  lcd.setCursor(3, 0);

  String mensagem = "Bem vindo!";
  for (byte i = 0; i < mensagem.length(); i++) {
    lcd.print(mensagem[i]);
    delay(100);
  }

  for (byte i = 0; i < 16; i++) {
    lcd.setCursor(i, 1);
    lcd.write(byte(0));
    delay(100);
  }

  for (byte i = 0; i < 16; i++) {
    lcd.scrollDisplayLeft();
    delay(50);
  }

  delay(100);
}

void avisarUsuario() {
  Serial.print("{\"umidade_solo\":");
  Serial.print(umidadeSolo);
  Serial.print(",");

  Serial.print("\"umidade_ar\":");
  Serial.print(umidadeAr);
  Serial.print(",");

  Serial.print("\"temperatura\":");
  Serial.print(temperatura);
  Serial.println("}");
}

int contador = 0;

void loop() {

  contador = 0;

  while (true) {
    lerSensores();

    if (contador >= 4) {
      if (isnan(umidadeAr) || isnan(temperatura)) {
        return;
      }

      avisarUsuario();

      if (umidadeSolo < porcentagemSeco)
        avisarNoDisplay();

      break;
    };

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Umidade do Solo");

    lcd.setCursor(0, 1);

    lcd.print("Umidade: ");

    lcd.print(umidadeSolo);
    lcd.print(" %   ");

    contador++;

    delay(1000);
  }

  contador = 0;

  while (true) {
    lerSensores();

    if (isnan(umidadeAr) || isnan(temperatura)) {
      return;
    }

    if (contador == 4) {
      avisarUsuario();

      if (umidadeSolo < porcentagemSeco)
        avisarNoDisplay();
        
      break;
    };

    lcd.clear();
    lcd.setCursor(0, 0);

    lcd.print("Clima: ");
    lcd.print(temperatura, 0);
    lcd.print((char)223);
    lcd.print("C");

    lcd.setCursor(0, 1);

    lcd.print("Umidade: ");
    lcd.print(umidadeAr, 0);
    lcd.print("%  ");

    contador++;

    delay(1000);
  }
}

void lerSensores() {
  temperatura = dht.readTemperature();
  umidadeAr = dht.readHumidity();

  umidadeSolo = analogRead(pinoSensorSolo);
  umidadeSolo = map(umidadeSolo, 1005, 362, 0, 100);

  if (umidadeSolo > 100)
    umidadeSolo = 100;

  if (umidadeSolo < 0)
    umidadeSolo = 0;

  if (umidadeSolo < porcentagemSeco) {
    digitalWrite(pinoLed, HIGH);
  } else {
    digitalWrite(pinoLed, LOW);
  }
}

void avisarNoDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);

  lcd.print("Planta precisa");
  lcd.setCursor(0, 1);
  lcd.print("de agua!");

  avisarUsuario();

  delay(3000);
}
