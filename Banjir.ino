#include <ESP8266WiFi.h>
#define THINGSBOARD_ENABLE_PROGMEM 0
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ThingsBoard.h>

#define THINGSBOARD_ENABLE_PSRAM 0
#define THINGSBOARD_ENABLE_DYNAMIC 1

// WiFi credentials
constexpr char WIFI_SSID[] = "RUMAH";
constexpr char WIFI_PASSWORD[] = "HomeC7A4";

// ThingsBoard server details
constexpr char TOKEN[] = "TubesMikom";
constexpr char THINGSBOARD_SERVER[] = "192.168.100.211";
constexpr uint16_t THINGSBOARD_PORT = 1883U;
constexpr uint32_t MAX_MESSAGE_SIZE = 256U;
constexpr uint32_t SERIAL_DEBUG_BAUD = 115200U;
const char* serverURL = "http://192.168.100.11/subscribe/NOTIF_API.php";

// Initialize underlying client and ThingsBoard instance
WiFiClient tbClient;
WiFiClient apiClient;
ThingsBoard tb(tbClient, MAX_MESSAGE_SIZE);

// Buffer size for JSON payload
const size_t bufferSize = JSON_OBJECT_SIZE(2);


// Initialize ultrasonic sensor pins
const int trigPin = D1;
const int echoPin = D2;

// Initialize variables
long duration;
int distance;
int ketinggian;
int tinggiair;
String kondisi="Normal";
String status="Normal";
String longitude = "-6.988111518049175";
String latitude = "107.61800630698525";


void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Connect to WiFi
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

  // Connect to ThingsBoard
  Serial.print("Connecting to ThingsBoard...");
  if (tb.connect(THINGSBOARD_SERVER, TOKEN, THINGSBOARD_PORT)) {
    Serial.println("Connected!");
  } else {
    Serial.println("Connection failed!");
  }

  // Set ultrasonic sensor pin modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
    //Inisialisasi Ultrasonik
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    ketinggian = 400;
    duration = pulseIn(echoPin, HIGH);
    distance = duration * 0.034 / 2;
    tinggiair = ketinggian - distance;

    if(tinggiair >=250){
        status = String("Banjir");
    }
    else if(tinggiair > 180 && tinggiair < 220){
        status = String("Siaga");
    }
    else if(tinggiair < 180){
        status = String("Aman");  
    }

    Serial.print("Ketinggian: ");
    Serial.println(tinggiair);

    // Send ultrasonic distance data to ThingsBoard as telemetry
    tb.sendTelemetryInt("ultrasonicDistance", tinggiair);
    tb.sendTelemetryString("status", status.c_str());
    tb.sendTelemetryString("longitude", longitude.c_str());
    tb.sendTelemetryString("latitude", latitude.c_str());

    if(status != kondisi){
      if((status ="Banjir") && (tinggiair>=250)){
        mail(ketinggian, status);
      }        
    }
    
    if(tinggiair >=250){
        kondisi = String("Banjir");
    }
    else if(tinggiair > 180 && tinggiair < 220){
        kondisi = String("Siaga");
    }
    else if(tinggiair < 180){
        kondisi = String("Aman");  
    }

    delay(1500);
}

void mail(int ketinggian, String status){
  // Create a JSON document
    StaticJsonDocument<bufferSize> jsonDoc;

    // Populate the JSON document
    jsonDoc["ketinggian"] = ketinggian;
    jsonDoc["status"] = status;

    // Serialize the JSON document to a string
    String payload;
    serializeJson(jsonDoc, payload);
    String url = String(serverURL) + "?ketinggian=" + String(tinggiair) + "&status=" + String(status) + "&lokasi=Gerbang%20Citarum";
    Serial.print("Status berubah menjadi: ");
        Serial.println(status);
        // Send HTTP GET request
        HTTPClient api;
        api.begin(apiClient, url);
        int httpResponseCode = api.GET();

        // Mengakhiri http
        api.end();
        Serial.println("E-mail Terkirim");
}
