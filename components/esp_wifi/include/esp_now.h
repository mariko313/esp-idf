 #include <esp_now.h>
#include <WiFi.h>

const char nom[]="Responent1"; 
uint8_t broadcastAddress[] = {0x94,0xB5,0x55,0x24,0xBB,0x54};
// SUBSTITUEIX PER LA MAC DE LA PLACA INICIADORA

// exemple d'estructura per a rebre dades
// ha de coincidir amb l'estructura de la iniciadora
typedef struct estructura_missatge {
  char a[32];
  int b;
  float c;
  char d[16];
  bool e;
} estructura_missatge;

estructura_missatge dadesEnviades;
estructura_missatge dadesRebudes;

unsigned long tempsPrevi=0;

// callbacks per a enviar i rebre dades
void EnEnviarDades(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nPaquet" + String(nom)+" enviat:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Entrega correcta" : "Entrega errònia!");
}

void EnRebreDades(const uint8_t * mac, const uint8_t *dadesEntrants, int longitud)
{
  memcpy(&dadesRebudes, dadesEntrants, sizeof(dadesRebudes));
  Serial.print("\r\nBytes rebuts: ");
  Serial.println(longitud);
  Serial.print("De: ");
  Serial.println(dadesRebudes.a);
  Serial.print("A: ");
  Serial.println(dadesRebudes.d);
  Serial.print("Sensor: ");
  Serial.println(dadesRebudes.b);
  Serial.print("Estat: ");
  Serial.println(dadesRebudes.c);
  Serial.println();
}
void setup()
{
  Serial.begin(115200);

  // estableix el dispositiu com a WiFi Station
  WiFi.mode(WIFI_STA);

  // inicia ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println(F("Error iniciant ESP-NOW"));
    return;
  }
  Serial.print(F("Responent inicialitzada: "));
  Serial.println(WiFi.macAddress());
  
  // defineix les funcions de callback
  esp_now_register_send_cb(EnEnviarDades);
  esp_now_register_recv_cb(EnRebreDades);

  // registra company (peer)
  esp_now_peer_info_t infoCompany;
  memcpy(infoCompany.peer_addr, broadcastAddress, 6);
  infoCompany.channel = 0;
  infoCompany.encrypt = false;

  // afegeix company (peer)
  if (esp_now_add_peer(&infoCompany) != ESP_OK)
  {
    Serial.println(F("No he pogut afegir company!"));
    return;
  }
}

void loop()
{
  if((millis() - tempsPrevi) > 1500)
  {
    // estableix les dades a enviar
    strcpy(dadesEnviades.a, nom);
    dadesEnviades.b = random(100, 200);
    dadesEnviades.c = false;
  
    // envia missatge via ESP-NOW
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &dadesEnviades, sizeof(dadesEnviades));
  
    tempsPrevi=millis();
  }
