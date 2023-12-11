void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <HardwareSerial.h>

#include <FirebaseESP32.h>
#include <HardwareSerial.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

#define WIFI_SSID "tansutan"
#define WIFI_PASSWORD "duatigabudi"
#define API_KEY "AIzaSyAPtS5WFD6AftqVHj0uyibOpHJLIuilwgA"
#define DATABASE_URL "https://belajaresp32-f869f-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
HardwareSerial SerialBLE(2);

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
int count = 0;
// bool dataSudahDicetak = false; // Variabel untuk melacak apakah data sudah dicetak

void setup() {
  SerialBLE.begin(9600); // Inisialisasi komunikasi dengan modul HM-11
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Sign up
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  String name = getFirebaseData("test/int");
      if (name != "") {
        setBluetoothName(name);
        Serial.println("Nama Bluetooth telah diubah menjadi " + name);
      } else {
        Serial.println("Gagal mengambil data dari Firebase");
      }
}

void loop() {
  if (SerialBLE.available()) {
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
      sendDataPrevMillis = millis();

      // if (!dataSudahDicetak) { // Cek apakah data sudah dicetak
      if (Firebase.getFloat(firebaseData, "test/float")) {
        Serial.print("Float data dari RTDB: ");
        Serial.println(firebaseData.floatData(), 2); // Menampilkan float dengan 2 digit desimal
        SerialBLE.print("Float data dari RTDB: ");
        SerialBLE.println(firebaseData.floatData(), 2); // Menampilkan float dengan 2 digit desimal ke perangkat Bluetooth
      }

      // Retrieve and Display Data from RTDB
      if (Firebase.getString(firebaseData, "test/input")) {
        Serial.print("Data dari RTDB: ");
        Serial.println(firebaseData.stringData());
        SerialBLE.print("Data dari RTDB: ");
        SerialBLE.println(firebaseData.stringData());
      }

      if (Firebase.getInt(firebaseData, "test/int")) {
        Serial.print("Int data dari RTDB: ");
        Serial.println(firebaseData.intData());
        SerialBLE.print("Int data dari RTDB: ");
        SerialBLE.println(firebaseData.intData());
      }

      if (Firebase.getString(firebaseData, "test/string")) {
        Serial.print("String data dari RTDB: ");
        Serial.println(firebaseData.stringData());
        SerialBLE.print("String data dari RTDB: ");
        SerialBLE.println(firebaseData.stringData());
      }
    }
  }
}

String getFirebaseData(String path) {
  if (Firebase.getString(firebaseData, path)) {
    return firebaseData.stringData();
  } else {
    Serial.println(firebaseData.errorReason());
    return "";
  }
}

void setBluetoothName(String name) {
  String command = "AT+NAME" + name;
  SerialBLE.println(command);
  delay(500);
  while (SerialBLE.available()) {
    char c = SerialBLE.read();
    Serial.print(c);
  }
}