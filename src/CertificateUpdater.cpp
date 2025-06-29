#include "CertificateUpdater.h"

#include <HTTPClient.h>
#include <WiFiClientSecure.h>

String insecureFetchAndPrint(const String& url) {
    // We need WiFiClientSecure for an "https://" URL, even an insecure one.
    WiFiClientSecure client;

    // THIS IS THE KEY: It tells the client not to validate the server's certificate.
    client.setInsecure();

    HTTPClient http;
    String cert;

    Serial.println("Attempting INSECURE fetch from: " + url);

    // Start the connection
    if (http.begin(client, url)) {
        // It's good practice to set a User-Agent, some servers require it.
        http.addHeader("User-Agent", "ESP32-Test-Fetcher");

        // Make the GET request
        int httpCode = http.GET();

        // Check the result
        if (httpCode > 0) { // Check for a valid HTTP response
            if (httpCode == HTTP_CODE_OK) {
                Serial.println("\n--- Successfully Fetched Content ---");
                // Print the response payload
                cert = http.getString();
                Serial.println(cert);
                Serial.println("------------------------------------\n");
            } else {
                // This will show errors like 404 Not Found, 403 Forbidden, etc.
                Serial.printf("[HTTP] Request failed, HTTP Code: %d\n", httpCode);
            }
        } else {
            // This will show connection-level errors
            Serial.printf("[HTTP] Request failed, error: %s\n", HTTPClient::errorToString(httpCode).c_str());
        }

        // Free resources
        http.end();
    } else {
        Serial.printf("[HTTP] Unable to connect to %s\n", url.c_str());
    }
    return cert;
}
