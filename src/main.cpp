#include <Arduino.h>
#include <BluetoothSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Definir pino do sensor de umidade
const int soilMoisturePin = 34;  // GPIO34, entrada analógica

// Credenciais Wi-Fi
const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA";

// Configuração da API Supabase
const String supabaseUrl = "https://sua-url.supabase.co";
const String supabaseApiKey = "sua-chave-api";
const String endpoint = "/rest/v1/soil_moisture";

BluetoothSerial SerialBT;  // Objeto Bluetooth
int soilMoistureValue = 0;

void setup() {
  Serial.begin(115200);  // Inicializa a comunicação serial
  SerialBT.begin("ESP32_Soil_Moisture");  // Inicializa o Bluetooth com nome
  pinMode(soilMoisturePin, INPUT);  // Configura o pino do sensor como entrada

  // Inicializa Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }
  Serial.println("Wi-Fi conectado!");

  Serial.println("ESP32 Inicializado. Aguardando conexões Bluetooth...");
}

void loop() {
  // Leitura do sensor de umidade (0-4095 no ADC)
  soilMoistureValue = analogRead(soilMoisturePin);

  // Enviar valor via Bluetooth
  String message = "Soil Moisture: " + String(soilMoistureValue);
  SerialBT.println(message);  // Enviar para o dispositivo conectado
  Serial.println(message);  // Mostrar no monitor serial

  // Enviar dados para o Supabase
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = supabaseUrl + endpoint;
    http.begin(url.c_str());
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apikey", supabaseApiKey);

    // Criar payload JSON
    String payload = "{\"moisture_value\":" + String(soilMoistureValue) + "}";
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Dados enviados para o Supabase:");
      Serial.println(response);
    } else {
      Serial.println("Erro ao enviar dados para o Supabase.");
    }

    http.end();  // Finalizar a requisição
  }

  delay(2000);  // Enviar a cada 2 segundos
}
