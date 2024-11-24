#include <WiFi.h>
#include <HTTPClient.h>
#include "DHTesp.h"

#define DHTPIN 14 // Define the GPIO pin connected to the DHT sensor
DHTesp dht;

const char* ssid = "Koldzo Wi-Fi";       // Enter your WiFi network name
const char* password = "koldzokoldzo"; // Enter your WiFi network password

const char* serverEndpoint = "http://your-server.com/endpoint"; // Replace with your endpoint URL

void sendToServer(float temperature, float humidity);

void setup() {
  Serial.begin(115200);

  // Initialize the DHT sensor
  dht.setup(DHTPIN, DHTesp::DHT11);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  TempAndHumidity data = dht.getTempAndHumidity();

  // Check if the readings are valid
  if (isnan(data.temperature) || isnan(data.humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print the temperature and humidity readings to the Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(data.temperature);
  Serial.println(" °C");
  
  Serial.print("Humidity: ");
  Serial.print(data.humidity);
  Serial.println(" %");

  // Send temperature and humidity to the server
  sendToServer(data.temperature, data.humidity);

  delay(10000); // Wait 10 seconds before sending the next request
}

// Function to send data to the server
void sendToServer(float temperature, float humidity) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Create a JSON string with the data
    String jsonData = "{";
    jsonData += "\"temperature\": " + String(temperature) + ",";
    jsonData += "\"humidity\": " + String(humidity);
    jsonData += "}";

    // Start the connection to the server
    http.begin(serverEndpoint);
    http.addHeader("Content-Type", "application/json"); // Specify JSON content type

    int httpResponseCode = http.POST(jsonData); // Send POST request

    if (httpResponseCode > 0) {
      // Print the server response
      String response = http.getString();
      Serial.println("Server Response:");
      Serial.println(response);
    } else {
      // Print the error
      Serial.print("Error sending data: ");
      Serial.println(httpResponseCode);
    }

    http.end(); // Close the connection
  } else {
    Serial.println("WiFi not connected");
  }
}
