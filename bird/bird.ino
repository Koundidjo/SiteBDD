#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Tomoto_HM330X.h>
#include <ESP32Servo.h>
#include <HTTPClient.h>
#include <esp_sleep.h>

const char* ssid = "KOUNDIDJO";
const char* password = "arduino785";
const char* serverAddress = "http://192.168.137.1:8080";
const String apiEndpoint = "/post_pm25";

#define LCD_ADDRESS 0x3F
#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);
Tomoto_HM330X sensor;
Servo motor;

const int GOOD_QUALITY = 1;
const int MODERATE_QUALITY = 2;
const int UNHEALTHY_QUALITY = 3;

int motorPosition = 0;
bool isFirstBoot = true;

void controlMotor(int airQuality) {
  switch (airQuality) {
    case GOOD_QUALITY:
      motorPosition = 0;
      break;
    case MODERATE_QUALITY:
      motorPosition = 45;
      break;
    case UNHEALTHY_QUALITY:
      motorPosition = 180;
      break;
    default:
      break;
  }

  motorPosition = motorPosition % 360;

  motor.write(motorPosition);
  delay(100);
}

void displayPM2_5(int pm2_5) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PM2.5: ");
  lcd.print(pm2_5);
  lcd.print(" ug/m^3");

  int airQuality;

  if (pm2_5 <= 12) {
    airQuality = GOOD_QUALITY;
  } else if (pm2_5 <= 35) {
    airQuality = MODERATE_QUALITY;
  } else if (pm2_5 <= 250) {
    airQuality = UNHEALTHY_QUALITY;
  }
  lcd.setCursor(0, 1);
  lcd.print("Air Quality: ");
  lcd.print(airQuality);

  controlMotor(airQuality);

  sendPM2_5Data(pm2_5);
}

void sendPM2_5Data(int pm2_5) {
  HTTPClient http;

  String url = serverAddress + apiEndpoint;
  String requestBody = "pm2_5=" + String(pm2_5);

  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST(requestBody);

  if (httpCode > 0) {
    String response = http.getString();
    Serial.println("Réponse du serveur : " + response);
  } else {
    Serial.println("Échec de la requête HTTP");
  }

  http.end();
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println();
  Serial.print("Connected to Wifi with IP: ");
  Serial.println(WiFi.localIP());
}

void initServo() {
  unsigned long startTime = millis();
  while (millis() - startTime < 10000) {
    for (int angle = 0; angle <= 180; angle++) {
      motor.write(angle);
      delay(20);
    }
    for (int angle = 180; angle >= 0; angle--) {
      motor.write(angle);
      delay(20);
    }
  }

  motor.write(0);
  delay(1000);
}

void enterDeepSleep() {
  // Configuration de la période de deep sleep (1 minute)
  esp_sleep_enable_timer_wakeup(60 * 1000000);  // 60 seconds in microseconds

  Serial.println("Entering deep sleep for 1 minute...");
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);
  delay(100);

  if (!sensor.begin()) {
    Serial.println("Failed to initialize HM330X");
    while (1)
      ;
  }

  lcd.begin(LCD_COLUMNS, LCD_ROWS);
  lcd.backlight();

  motor.attach(8);

  delay(2000);
  lcd.clear();
  initWiFi();

  // Exécuter initServo seulement si isFirstBoot est vrai
  if (isFirstBoot == true) {
    initServo();
    isFirstBoot = false;  // Marquer que l'initialisation a été effectuée
  }
}

void loop() {
  unsigned long loopStartTime = millis();

  while (millis() - loopStartTime < 180000) {  // Exécuter la boucle pendant 3 minutes (180000 millisecondes)
    if (!sensor.readSensor()) {
      Serial.println("Failed to read HM330X");
    } else {
      int pm2_5 = sensor.std.getPM2_5();
      displayPM2_5(pm2_5);
      delay(5000);  // 5 secondes entre chaque lecture
    }
  }

  // Entrer en deep sleep après 3 minutes
  enterDeepSleep();
}
