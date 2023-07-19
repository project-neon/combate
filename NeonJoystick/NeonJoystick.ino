#include <esp_now.h>
#include <WiFi.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*PADRÃO UTILIZADO NO DIP SWITCH
1. CIRROSE
2. DOENTE
3. SOLUÇO
4. BEETLEJUICE


 - Falta separar os códigos para diferentes robos


- falta arrumar o debouncing
 * */

 
// --------------------- PINOS DOS ANALÓGICOS --------------//
#define potPinD 33  
#define potPinV 32      

//---------------- PINOS DO DIP SWITCH ---------------//
#define switch1 34 
#define switch2 35
#define switch3 25
#define switch4 26

//---------------- PINOS DOS BOTÕES ---------------//
#define B1 12
#define B2 18
/*#define B3 18
#define B4 5*/

//----------------VARIAVIES DE DETECÇAO DE ROBO (DIPSWITCH) ---------------//
#define TEMPO_DETECCAO 100
#define TENTATIVAS_ACHA_ROBO 40
#define DETECT_AGAIN 2000

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define LED 2   //LED para verificação de status da comunicação
//#define CAL 15  //Pino usado para calibrar os joysticks

#define DT_DEBOUNCING 200

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int valorDir = 0; //valor do analógico direito
int valorSpd = 0; //valor do analógico esquerdo

int mac_index = 0;

int status_connect = 0;

int contValidacao = 0; //contador para validar a comunicação
int statusCom = 0;
int lastValidacao = 0;
int atualValidacao = 0;

int inv = -1; //indica se o sentido de locomoção está invertido


int button1 = 0;
int button2 = 0;


int detect = 0; //verifica se a detecção ja foi executada

//---------- VARIÁVEIS DE CALIBRAÇÃO ---------------- //
int cal = 0;
int temp = 0;
int etapa = 0;

int menor = -1;
int var1 = 0;
int maior = 0;

int menorLX = 0;
int menorMidLX = 1450;
int maiorMidLX = 1900;
int maiorLX = 4095;

int menorRY = 0;
int menorMidRY = 1450;
int maiorMidRY = 1900;
int maiorRY = 4095;
//---------- ----------------------- ---------------- //

uint8_t mac_address[6] = {};
uint8_t addressArrays[4][6] = {{0x08, 0x3A, 0xF2, 0x50, 0xE0, 0x30}, {0x0C, 0xDC, 0x7E, 0x5E, 0x97, 0x0C},
                              {0x11, 0x22, 0x33, 0x44, 0x55, 0x66}, {0xFF, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE}} ;// INSERIR 4 MAC ADDRESSES RELATIVOS A ESPS RECEPTORAS DIFERENTES 

static const uint8_t neon_logo[1024] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x03, 0xff, 0xf0, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x03, 0xff, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x3f, 0xff, 0xff, 0x00, 0x0f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 
    0x00, 0x03, 0xff, 0xcf, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 
    0x00, 0x3f, 0xfc, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xf0, 0x00, 
    0x00, 0x3f, 0xc0, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x0f, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xfc, 0x3c, 0x00, 0x00, 0xff, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xfc, 0x0f, 0xf0, 0x0f, 0xc3, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0xfc, 0xfc, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x0f, 0xf0, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x03, 0x00, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x03, 0x00, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x03, 0x00, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xfc, 0x00, 0x03, 0x00, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0x00, 0x03, 0x00, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0x03, 0x00, 0x3f, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0xfc, 0x03, 0x00, 0xfc, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0x0f, 0xc3, 0x0f, 0xc0, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0x00, 0xf3, 0xfc, 0x00, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0x00, 0x3f, 0xf0, 0x00, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0x00, 0x0f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x03, 0xf0, 0x00, 
    0x00, 0x3f, 0xf0, 0x0f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0x3f, 0xf0, 0x00, 
    0x00, 0x0f, 0xfc, 0x0f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xc0, 0xff, 0xc0, 0x00, 
    0x00, 0x03, 0xff, 0xc3, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xcf, 0xff, 0x00, 0x00, 
    0x00, 0x00, 0x3f, 0xfc, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfc, 0xff, 0xf0, 0x00, 0x00, 
    0x00, 0x00, 0x03, 0xff, 0xcf, 0xfc, 0x00, 0x00, 0x00, 0x00, 0xff, 0xcf, 0xff, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x3f, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xf0, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xc0, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xfc, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//Estrutura da mensagem que será enviada
//DEVE SER A MESMA ESTRUTURA NO RECEPTOR
typedef struct struct_message {
  int spdRight;
  int spdLeft;
  String dir;
  int cont;
  int arma;
} struct_message;


struct_message mySpd;     //Cria um objeto chamado mySpd

esp_now_peer_info_t peerInfo; //Cria um objeto chamado peerInfo

// Função callback chamada ao enviar algum dado
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  //Last Packet Send Status
  if (status == ESP_NOW_SEND_SUCCESS){
      statusCom = 1; //Delivery Success
  }else{
      statusCom = 0; //Delivery Fail
  }
}

void setup() {
  int maior_robo = 0; //qual robo foi detectado mais vezes pelo dipswitch
  int maior_ocorr = 0; //numero de vezes que o robo mais frequente ocorreu
  // Inicia o monitor Serial
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(potPinD, INPUT);
  pinMode(potPinV, INPUT);
 // pinMode(B1, INPUT_PULLDOWN); //Botões
 // pinMode(B2, INPUT_PULLDOWN);
  //pinMode(B3, INPUT);
 // pinMode(B4, INPUT);
  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(switch3, INPUT);
  pinMode(switch4, INPUT); //VERIFICAR SE OS PINOS SÃO DE FATO PULLUP 
  // Configura o ESP32 como um Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Inicia o ESP-NOW
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    ESP.restart();
    return;
  }

  //Configura a função de callback que será chamada ao enviar algum dado
  esp_now_register_send_cb(OnDataSent);
  
  // Adiciona o dispositivo que receberá os dados (peer)  
  display.clearDisplay();
  display.drawBitmap(0, 0, neon_logo, 128, 64, 1); //(x, y, image array, image width, image height, rotation)
  display.display();
  delay(3000); // Pause for 2 seconds
  display.setTextSize(1);         // set text size
  display.setTextColor(WHITE);    // set text color
  display.setCursor(0, 10);       // set position to display
}

void loop() {
    if (detect == 0){ //conecta o robô assim que iniciar
      while(true){
        mac_index = detectaRobo();
        delay(1500);
        Serial.print("Index: ");
        Serial.println(mac_index);
        if (mac_index != 0){
          mac_index = mac_index - 1;
          memcpy(mac_address, addressArrays[mac_index], 6);
          status_connect = connectRobot(mac_address);
          delay(1000);
          if(status_connect == 1){
            detect = 1;
            break;
          }
          if(detect == 0){
            display.setCursor(0, 10);
            display.clearDisplay();
            display.println("Tentando novamente...");
            display.display();
            Serial.println("Tentando novamente");
            delay(1000);            
          }
        }
        else if (mac_index == 0){
           display.setCursor(0, 10);
           display.clearDisplay();
           display.println("Tentando novamente...");
           display.display();
           Serial.println("Tentando novamente");
           delay(1000);
        }
      }
    }



   /* if(digitalRead(B1) == 1){ //Sentido de rotação normal quando B1 é acionado
      button1 = 1;
      inv = 1;
    }
    if (button1 = 1 && (millis() - t_b1) > DT_DEBOUNCING){
      button1 = 0;
    }

    if(digitalRead(B2) == 1){ //Inverte o sentido de rotação quando B2 é acionado
      inv = -1;
      button2 = 1;
      Serial.print("B2: ");
      Serial.print(button2);
    }
    else{
      button2 = 0;
    }

*/
    valorDir = analogRead(potPinD);
    valorSpd = analogRead(potPinV);

    if (valorDir >= menorMidLX && valorDir <= maiorMidLX){
        valorDir = 0; 
    }else if (valorDir < menorMidLX){
        valorDir = map(valorDir, menorLX, menorMidLX, -100, 0);  
    }else {
        valorDir = map(valorDir, maiorMidLX, maiorLX, 0, 100); 
    }

    if (valorSpd >= menorMidRY && valorSpd <= maiorMidRY){
        valorSpd = 0; 
    }else if (valorSpd < menorMidRY){
        valorSpd = map(valorSpd, menorRY, menorMidRY, -100, 0); 
    }else {
        valorSpd = map(valorSpd, maiorMidRY, maiorRY, 0, 100); 
    }
    valorSpd = inv*valorSpd;
    if (valorDir > 0){
        mySpd.spdLeft = valorSpd;
        mySpd.spdRight = valorSpd - valorSpd*(valorDir/100);
        mySpd.dir = "RIGHT";
    }else if(valorDir < 0){
        mySpd.spdLeft = valorSpd + valorSpd*(valorDir/100);
        mySpd.spdRight = valorSpd;
        mySpd.dir = "LEFT";
    }else{
        mySpd.spdLeft = valorSpd;
        mySpd.spdRight = valorSpd;
        mySpd.dir = "CENTRO";
    }

   // Serial.print("VD: ");
   // Serial.print(mySpd.spdRight);
   // Serial.print("\t");
   // Serial.print("VE: ");
   // Serial.println(mySpd.spdLeft);/*
  /*    Serial.print("B1: ");
      Serial.print(button1);
    Serial.print("\t");
    Serial.print("|INT: ");
    Serial.print(valorSpd);
    Serial.print("\t");
    Serial.print("DIR: ");
    Serial.print(mySpd.dir);
    Serial.println("\t");
    //Serial.print("DipSwitch: ");
    //Serial.print(mySpd.dip_switch);
    Serial.print("\t");
    Serial.print("B1: ");
    Serial.print(mySpd.b1);
    Serial.print("B2: ");
    Serial.println(mySpd.b2);
    Serial.print("B3: ");
    Serial.print(mySpd.b3);
    Serial.print("\t");
    Serial.print("B4: ");
    Serial.print(mySpd.b4);  
    Serial.println();
    */

    // Envia os dados via ESP-NOW
    
    esp_err_t result = esp_now_send(mac_address, (uint8_t *) &mySpd, sizeof(mySpd));

    if (result == ESP_OK && statusCom == 1) {
        //Success
        atualValidacao = 1;
    }else{
        //Error
        statusCom = 0;
        atualValidacao = 0;
    }

    if (atualValidacao == lastValidacao) {
        contValidacao += 1;
    }else{
        contValidacao = 1;
        lastValidacao = atualValidacao;
    }

    if (contValidacao == 3 && lastValidacao == 1){
        digitalWrite(LED, HIGH);
        contValidacao = 0;
    }

    if (contValidacao == 3 && lastValidacao == 0){
        digitalWrite(LED, LOW);
        contValidacao = 0;
    } 
   mySpd.cont = contValidacao; 
  delay(20);
}

int detectaRobo(){
  display.clearDisplay();
  display.setCursor(0, 10);
  int n = 0;
  int tempo_inicio = 0;
  uint8_t n0 = 0;
  uint8_t n1 = 0;
  uint8_t n2 = 0;
  uint8_t n3 = 0;
  uint8_t n4 = 0;
  uint8_t n_error = 0;
  int maior = 0;
  int maior_n = 0;
  int n_def = 0;
  //Serial.println("Iniciando detecção de robô.");
  for(int n_tentativas = 0; n_tentativas < TENTATIVAS_ACHA_ROBO; n_tentativas++){
    tempo_inicio = millis();
    while(millis()-tempo_inicio < TEMPO_DETECCAO){
      n = 0;
      n = (5*(!digitalRead(switch1))
      + 6*(!digitalRead(switch2))
      + 7*(!digitalRead(switch3))
      + 8*(!digitalRead(switch4)));
      if (n == 0 || n == 5 || n == 6 || n== 7 || n==8) break;
    }
    n_def = n;
    if (n_def > 8) n_error += 1;
    else{
      if(n_def==0) n0 += 1;
      if(n_def==5) n1 += 1;
      if(n_def==6) n2 += 1;
      if(n_def==7) n3 += 1;
      if(n_def==8) n4 += 1;
    }
  }
  Serial.print("N0:");
  Serial.print(n0);
  Serial.print("\t");
  Serial.print("N1:");
  Serial.print(n1);
  Serial.print("\t");
  Serial.print("N2:");
  Serial.print(n2);
  Serial.print("\t");
  Serial.print("N3:");
  Serial.print(n3);
  Serial.print("\t");
  Serial.print("N4:");
  Serial.print("\t");
  int numeros[5] = {n0 , n1, n2, n3, n4};
  for(int i = 0; i <= 4; i++){
    if(numeros[i] > maior){
       maior = numeros[i];
       maior_n = i;
    }
  }
  if (n_error > maior){
    maior_n = 5;
    Serial.println("Selecao Invalida");
    display.println("Selecao Invalida");
    maior_n = 0;
  }
  else{
    if(maior_n == 1){
    Serial.println("Robo detectado: 1");
    display.print("Robo detectado:       1. Cirrose ");
    }
  else if(maior_n == 2){
    Serial.println("Robo detectado: 2");
    display.print("Robo detectado:       2. Doente  ");
    }
  else if(maior_n == 3){
    Serial.println("Robo detectado: 3");
    display.print("Robo detectado:       3. Soluco ");
    }
  else if(maior_n == 4){
    Serial.println("Robo detectado: 4");
    display.print("Robo detectado:       4. Beetlejuice  ");
    }
  else{
    Serial.println("Nenhum robo");
    display.println("Nenhum robo selecionado");
    }
  }
  display.display();
  return maior_n;
}


int connectRobot(uint8_t* MacAddress){
  int status = 0;
  memcpy(peerInfo.peer_addr, MacAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  display.clearDisplay();
  display.setCursor(0, 10);
  // Adiciona o dispositivo que receberá os dados (peer)  
  if (esp_now_add_peer(&peerInfo) == ESP_OK){
      display.println("CONECTADO");
      status = 1;
  }
  else{
      display.println("FALHA NA CONEXAO");
      Serial.println("Falha conexao");
  }
  display.display();
  return status;
}


