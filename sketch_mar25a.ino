// Nome do aluno: Bruno Soares Machado
// Prontuário: BA3139786

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <time.h>

const char* ssid = "S21+ de Bruno";
const char* password = "ecyf4792";

// COLOQUE SEU ENDPOINT DA AWS AQUI:
const char* mqtt_server = "a8yslzblurxct-ats.iot.us-east-1.amazonaws.com";

WiFiClientSecure espClient;
PubSubClient client(espClient);

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// === SUBSTITUA PELOS CERTIFICADOS GERADOS NA AWS ===
const char* root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char* device_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUXMV15LRm/cL3oUcS5Vb7cMeVBB4wDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI2MDQyMjIzMDcw
NFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAOlg0yL8sqD67kudPW13
LTgKDIZP3kY0ZnLocZGU1Amv2nY/MhSnUj6JugErpz1RfbDJ31m3CiMaRmzVvmYy
+Oe4w3jNfVDJwOgQ/FnzSqG/sYaYc8cQVZM2y3AQaBRCKwKIpmUGf3RJS9UWTg/m
dtCMWdmqI7Qn7YUJKlsuSEtf+g4+Suc04Qo89aEeVwmhwrRLLcIpN6OpHJMxHUR5
l/TCD9BgprcSKMLoIi9hL0eKgsbCYkO5IqX1qxgiS+Zcum10LkpcYkgZGU8naqcg
E0w+Eo0emKdzm6Z+VJUCSY0ZzPVus4oXjTiXHoA1BmvzMyT+5U5lR0afdG87lNDL
3g8CAwEAAaNgMF4wHwYDVR0jBBgwFoAUls/mINJikt6PmMlEIoIUr0tCS5swHQYD
VR0OBBYEFAx+NgZDSShvRiOxOF4uPywUKyqUMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCsGiVWap+/dMOZrIhO31bsPhNM
WLWY8dkKqflaHmW7dkdk2sQi/5h34DjReUVF2GJJJhW0XjlmH3oHa48Rp9xGyueF
9jUglUAHo/22SAuSJymiCgdTGWeGpt1bIwT25Nf9m6VON3x4ifPyt5SBZSncyLhJ
rAWh2RSKvGAV2vgspSFm5lSQeVvFMGW0TYIKX5J1eBVvIPKqzpqZZKFcjtm9jjL6
RdUg4sYqprTdjZb/r1TYaNisF+embfMYg6FtNlPsx7PxWT9tdOL+RkdGiMPPAuwk
ci/kWBWRuD/HQgshZh7KRU81HFI52NIRz4IfFvEJ2jP7WluPxWkx6bfpPHI5
-----END CERTIFICATE-----
)EOF";

const char* private_key = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA6WDTIvyyoPruS509bXctOAoMhk/eRjRmcuhxkZTUCa/adj8y
FKdSPom6ASunPVF9sMnfWbcKIxpGbNW+ZjL457jDeM19UMnA6BD8WfNKob+xhphz
xxBVkzbLcBBoFEIrAoimZQZ/dElL1RZOD+Z20IxZ2aojtCfthQkqWy5IS1/6Dj5K
5zThCjz1oR5XCaHCtEstwik3o6kckzEdRHmX9MIP0GCmtxIowugiL2EvR4qCxsJi
Q7kipfWrGCJL5ly6bXQuSlxiSBkZTydqpyATTD4SjR6Yp3Obpn5UlQJJjRnM9W6z
iheNOJcegDUGa/MzJP7lTmVHRp90bzuU0MveDwIDAQABAoIBAQC/sH378N+wBiwR
A5he/1KvAuICuDt0Iro7i90snLgFNOIV2haCjGSrZWjYaUi2vuakfUvtXsD+0Cgk
YPRA8YEso8A6y14xK2Y9tyXvpDwa218vlK0URsBjaNB0vaUfbrHxJNi3hjzOAR4+
alG4Ea5p4b4zYOJuqZUYfszBlCRhc2RL1cNGRb+kNq8ze/9+1G96xAPrsdCqfC4D
MUvjJUQMSjkjQ4VUSHM/7VHBydh4zQ+qwWJH+FWi7SGts/d6nmwosXaGF7ZmTG2c
6RGgo9KmZwCq09LsyLD2F6Tqs4Ca6w1PXmaLU7aNUvmT+f1jcpxig8X6x0uvMc0s
rVRnr1/BAoGBAPl9Lz5OMJ/eNS9YCewTuj/vp/DD+H/4xLVuy5Mxqn+5/mUUnbry
+EhJPISC9IGV3ViUyTMCYI1O1nkFmj8ND5xSvLiemAbOQHPoFp8eShmMJqH5D/xo
qm1/J8wKJKq1nemMIe0aDvupaig0Vxnsgp4hfrLvjLskMzshZdMfPRy/AoGBAO94
AWKx6aRbeMhW0WyXtBmfhFeFjHQT841fH5sxIqp11qeKQFX2Rl+fSQzu019Cf+XR
hcSdwZAVgiZLFBwcpIE/A5fDFBH7K09xBVTLlvJeUNYMCpaS1KSMU9PLkedADd5b
17V9VEe1KDFM+1YO62xFWP0CPwCUnB8ixj0DjoKxAoGBALo0PVtiHhZjUQyJx7YD
r0Fiy9P9X22brKzBCX1NVzRHC+BDN8m856ODzYAVAQnqYS5otZW1k3xDt/zROqMO
S3VfpuyfqVWDAEXBlYiioalZmcfPzKeVMoXQnlJBy8YBEN+rL/L3PPGFKu7gyZVW
mNIOq7I+6akOxpG+RnIA4aqfAoGAYPUe4n4UknZyzfLQ8n3t7t32cJVjC/C0EQ5t
tqFLXuc9VTKV3/hKABd3NWRRcuVy8XNHDReuso7y1lxZtRQnyyjfZIgYzTr8CztA
pFeWM2Xapv/THaovrLwgiMWIuq/lrZ1UFmi7QhloP6XisbJywbDGWhKZvbtuBvFG
gJe7ENECgYEAyqrc1hPSe26XZp+M13q4mvsVmo1qG7TruQfC81NwivJeMmMNVgin
ARziAI6Zd4X8JkXo1rJRjjwId7KtBpp3iA+yDGrEPIeWdJUla6DbZj6L6lciYdAo
oxyw5U6G1GAwB/nF90oK3lhyiE9E0fDCGm2psJfsScnpFUggsqM4Fps=
-----END RSA PRIVATE KEY-----
)EOF";
// ===================================================

unsigned long lastMsg = 0;

void setup_wifi() {
  delay(10);
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void sincronizarTempo() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  Serial.print("Aguardando sincronização de horário (necessário para AWS)...");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("\nHorário sincronizado!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao AWS IoT Core...");
    // A AWS exige que o ClientID seja único. Usar o MAC address é uma boa prática.
    String clientId = "ESP32_Bruno_";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT conectado!");
      // Subscrevendo nos tópicos para garantir
      client.subscribe("brunoEmiguel/umidade");
      client.subscribe("brunoEmiguel/temperatura");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  setup_wifi();
  sincronizarTempo(); // Tem que ser rodado uma vez no setup!
  
  espClient.setCACert(root_ca);
  espClient.setCertificate(device_cert);
  espClient.setPrivateKey(private_key);
  
  client.setServer(mqtt_server, 8883);
  client.setBufferSize(512);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Mantém a conexão MQTT viva
  
  // Lógica para enviar dados a cada 2 segundos sem usar delay()
  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    
    float umidade = dht.readHumidity();
    float temperatura = dht.readTemperature();
    
    if (isnan(umidade) || isnan(temperatura)) {
      Serial.println("Erro ao ler o DHT22!");
      return;
    }

    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" °C");

    Serial.print("Umidade: ");
    Serial.print(umidade);
    Serial.println(" %");

    char tempString[8];
    char humString[8];
    dtostrf(temperatura, 1, 2, tempString);
    dtostrf(umidade, 1, 2, humString);

    // Publica nos tópicos definidos no enunciado
    client.publish("brunoEmiguel/temperatura", tempString);
    client.publish("brunoEmiguel/umidade", humString);

    Serial.println("Dados enviados ao broker MQTT (AWS).");
    Serial.println("-----------------------------");
  }
}