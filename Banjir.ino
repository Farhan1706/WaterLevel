#include <ESP8266WiFi.h>
#define THINGSBOARD_ENABLE_PROGMEM 0
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <Arduino_MQTT_Client.h>
#include <ThingsBoard.h>
#include <AccelStepper.h>
#define HALFSTEP 8

constexpr char WIFI_SSID[] = "RUMAH";
constexpr char WIFI_PASSWORD[] = "HomeC7A4";
constexpr char TOKEN[] = "fksJTMBTpif5sY44BJwu";
constexpr char THINGSBOARD_SERVER[] = "thingsboard.cloud";
constexpr uint16_t THINGSBOARD_PORT = 1883U;
constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;
const char* serverURL = "http://192.168.100.214/subscribe/NOTIF_API.php";

// Initialize underlying client, used to establish a connection
WiFiClient wifiClient;
WiFiClient apiClient;
HTTPClient http;
// Initalize the Mqtt client instance
Arduino_MQTT_Client mqttClient(wifiClient);
// Initialize ThingsBoard instance with the maximum needed buffer size
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

// Attribute names for attribute request and attribute updates functionality

constexpr int16_t telemetrySendInterval = 2000U;
uint32_t previousDataSend;

/// @brief Initalizes WiFi connection,
// will endlessly delay until a connection has been successfully established
void InitWiFi() {
  Serial.println("Connecting to AP ...");
  // Attempting to establish a connection to the given WiFi network
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    // Delay 500ms until a connection has been succesfully established
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}

/// @brief Reconnects the WiFi uses InitWiFi if the connection has been removed
/// @return Returns true as soon as a connection has been established again
const bool reconnect() {
  // Check to ensure we aren't connected yet
  const wl_status_t status = WiFi.status();
  if (status == WL_CONNECTED) {
    return true;
  }

  // If we aren't establish a new connection to the given WiFi network
  InitWiFi();
  return true;
}

const size_t bufferSize = JSON_OBJECT_SIZE(2);

// Initialize ultrasonic sensor pins
const int trigPin = D1;
const int echoPin = D2;
// Initialize variables
long duration;
int distance;
int ketinggian;
int tinggiair;
float persentase;
String kondisi="Normal";
String setatus="Normal";
#define motorPin1 0 // IN1 pada D3
#define motorPin2 2 // IN2 pada D4
#define motorPin3 14 // IN3 pada D5
#define motorPin4 12 // IN4 pada D6

AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

unsigned long previousMillis = 0;
const long interval = 5000; // interval untuk kembali ke posisi awal (10 detik)

int motorMovementFlag = 0;

void setup() {
  // Initalize serial connection for debugging
  Serial.begin(SERIAL_DEBUG_BAUD);
  if (LED_BUILTIN != 99) {
    pinMode(LED_BUILTIN, OUTPUT);
  }
  delay(1000);
  InitWiFi();
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  stepper1.setMaxSpeed(1000.0);    // setting kecepatan maksimal motor (ditingkatkan dari 2000.0)
  stepper1.setAcceleration(1000.0); // setting akselerasi (ditingkatkan dari 1500.0)
  stepper1.setSpeed(1000);            // setting kecepatan (ditingkatkan dari 2000)
  stepper1.moveTo(0);
}

void loop() {
  delay(10);
  unsigned long currentMillis = millis();
  if (!reconnect()) {
    return;
  }

  if (!tb.connected()) {
    // Connect to the ThingsBoard
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
    if (!tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
      Serial.println("Failed to connect");
      return;
    }
    tb.sendAttributeData("macAddress", WiFi.macAddress().c_str());
    Serial.println("Subscribing for RPC...");
  }
    //Inisialisasi Ultrasonik
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    ketinggian = 18;
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
    tinggiair = ketinggian - distance;

    persentase = ((static_cast<float>(tinggiair) / ketinggian) * 100);

    tb.sendTelemetryData("ketinggian", tinggiair);
    tb.sendTelemetryData("persentase", persentase);

    
    if (tinggiair >= 12) {
      setatus = String("Waspada Banjir");
      motorMovementFlag = 1;
      if (tinggiair >= 12 && motorMovementFlag == 1) {
        Serial.println("Motor bergerak 3 putaran");
        stepper1.moveTo(0);
      }
    }else if (tinggiair > 7 && tinggiair < 12) {
      setatus = String("Siaga Banjir");
    }else if (tinggiair <= 7) {
      setatus = String("Aman");
      motorMovementFlag = 0;
      if (tinggiair <= 7 && motorMovementFlag == 0) {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
          previousMillis = currentMillis;
          stepper1.moveTo(3 * 4096); // 2 putaran (2 x 4096 step)
          Serial.println("Motor kembali ke posisi awal");
        }
      }
    }

    tb.sendTelemetryData("status", setatus.c_str());
    
    if(setatus != kondisi){
      if((setatus ="Waspada Banjir") && (tinggiair>=12)){
        mail(tinggiair, setatus);
      }        
    }
    
    if(tinggiair >=12){
        kondisi = String("Waspada Banjir");
    }
    else if(tinggiair > 7 && tinggiair < 12){
        kondisi = String("Siaga Banjir");
    }
    else if(tinggiair <= 7){
        kondisi = String("Aman");  
    }
  stepper1.runToPosition();
  tb.loop();
  delay(5000);
}

void mail(int tinggiair, String setatus) {
  // Buat URL dengan memasukkan nilai variabel
  String emailStatus = setatus;
  emailStatus.replace(" ", "%20");
  String url = String(serverURL) + "?ketinggian=" + String(tinggiair) + "&status=" + String(emailStatus) + "&lokasi=Gerbang%20Citarum";

  Serial.print("Connecting to ");
  Serial.println(url);

  if (!wifiClient.connect(serverURL, 80)) {
    Serial.println("Connection failed.");
    return;
  }

  // Kirim permintaan HTTP GET
  if (http.begin(wifiClient, url)) {
    int httpCode = http.GET();

    // Cek status koneksi dan tampilkan hasil
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    // Selesai dengan koneksi
    http.end();
    delay(1000);
  } else {
    Serial.printf("[HTTP] Unable to connect\n");
  }

  wifiClient.stop();  // Hentikan koneksi setelah penggunaan
}

