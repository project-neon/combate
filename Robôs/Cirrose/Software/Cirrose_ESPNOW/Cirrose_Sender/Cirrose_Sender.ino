#include <esp_now.h>
#include <WiFi.h>

#define potPinD 35  // está marcado como D35 no DevKit
#define potPinV 34      // está marcado como D34 no DevKit

//---------------- PINOS DOS GATILHOS ---------------//
#define potTriggerR 32     // está marcado como D32 no DevKit
#define potTriggerL 39     //está marcado como D39 no DevKit


#define LED 2   //LED para verificação de status da comunicação
#define CAL 27  //Pino usado para calibrar os joysticks

//---------------- PINOS DOS BOTÕES---------------//
#define B1 14
#define B2 15
#define B3 24
#define B4 25

int valorDir = 0;
int valorSpd = 0;

int valorTriggerL = 0;
int valorTriggerR = 0;

int statusCom = 0; //status da comunicação
int contValidacao = 0; //contador para validar a comunicação
int lastValidacao = 0; //estado da ultima validação
int atualValidacao = 0; //estado atual da validação

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


/*
PINOS UTILIZADOS
ENTRADAS
PINO 32 -- DIREITA OU ESQUERDA (potenciômetro)
PINO 39 -- FRENTE OU TRÁS (potenciômetro)
*/

uint8_t broadcastAddress[] = {0x08, 0x3A, 0xF2, 0x50, 0xE0, 0x30}; //COLOQUE os valores do endereço MAC do receptor

//Estrutura da mensagem que será enviada
//DEVE SER A MESMA ESTRUTURA NO RECEPTOR
typedef struct struct_message {
  int spdRight;
  int spdLeft;
  String dir;
  int cont;
  int angle;
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

void calibracao(){
  //Ao iniciar, atualizamos o valor de "temp" e fazemos um "reset" na outras variáveis
  if (temp == 0){
      temp = millis();
      menor = -1;
      maior = 0;
  }else{
      //Cada etapa da calibração dura 3s, o processo todo dura 18s
      switch(etapa){
          case 0:
              //Deixe o joystick relacionado a direção solto sem mexê-lo
              //O software irá detectar a oscilação máxima e mínima do joystick
              if (millis() - temp <= 5000){
                  Serial.print("calibrando direcao centro...");
                  Serial.print("\t");
                  Serial.println((millis()-temp)/1000);

                  var1 = analogRead(potPinD);

                  if (menor == -1 || var1 < menor){
                      menor = var1;
                  }

                  if (maior == 0 || var1 > maior){
                      maior = var1;
                  }
              }else { //Grava os valores identificados e vai para a próxima etapa
                  menorMidLX = menor;
                  maiorMidLX = maior;
                  temp = 0;
                  etapa = 1;
              }
              break;
        
          case 1:
              //Deixe o joystick relacionado a direção na posição onde será enviado o maior valor para o controlador
              //O software irá detectar o valor infomado
              if (millis() - temp <= 5000){
                  Serial.print("calibrando direcao MAX...");
                  Serial.print("\t");
                  Serial.println((millis()-temp)/1000);

                  var1 = analogRead(potPinD);

                  if (maior == 0 || var1 > maior){
                      maior = var1;
                  }

              }else {//Grava o valor identificado e vai para a próxima etapa
                  maiorLX = maior;
                  temp = 0;
                  etapa = 2;
              } 
              break;
        
          case 2:
              //Deixe o joystick relacionado a direção na posição onde será enviado o menor valor para o controlador
              //O software irá detectar o valor infomado
              if (millis() - temp <= 5000){
                  Serial.print("calibrando direcao MIN...");
                  Serial.print("\t");
                  Serial.println((millis()-temp)/1000);

                  var1 = analogRead(potPinD);

                  if (menor == -1 || var1 < menor){
                      menor = var1;
                  }

              }else {//Grava o valor identificado e vai para a próxima etapa
                  menorLX = menor;
                  temp = 0;
                  etapa = 3;
              } 
              break;
        
          case 3:
              //Deixe o joystick relacionado a velocidade solto sem mexê-lo
              //O software irá detectar a oscilação máxima e mínima do joystick
              if (millis() - temp <= 5000){
                  Serial.print("calibrando velocidade centro...");
                  Serial.print("\t");
                  Serial.println((millis()-temp)/1000);

                  var1 = analogRead(potPinV);

                  if (menor == -1 || var1 < menor){
                      menor = var1;
                  }

                  if (maior == 0 || var1 > maior){
                      maior = var1;
                  }
              }else {//Grava os valores identificados e vai para a próxima etapa
                  menorMidRY = menor;
                  maiorMidRY = maior;
                  temp = 0;
                  etapa = 4;
              } 
              break;
        
          case 4:
              //Deixe o joystick relacionado a velocidade na posição onde será enviado o maior valor para o controlador
              //O software irá detectar o valor infomado
              if (millis() - temp <= 5000){
                  Serial.print("calibrando velocidade MAX...");
                  Serial.print("\t");
                  Serial.println((millis()-temp)/1000);

                  var1 = analogRead(potPinV);

                  if (maior == 0 || var1 > maior){
                      maior = var1;
                  }
              }else {//Grava o valor identificado e vai para a próxima etapa
                  maiorRY = maior;
                  temp = 0;
                  etapa = 5;
              } 
              break;
          
          case 5:
              //Deixe o joystick relacionado a velocidade na posição onde será enviado o menor valor para o controlador
              //O software irá detectar o valor infomado
              if (millis() - temp <= 5000){
                  Serial.print("calibrando velocidade MIN...");
                  Serial.print("\t");
                  Serial.println((millis()-temp)/1000);

                  var1 = analogRead(potPinV);

                  if (menor == -1 || var1 < menor){
                      menor = var1;
                  }

              }else {//Grava o valor identificado e vai para a próxima etapa
                  menorRY = menor;
                  temp = 0;
                  etapa = 6;
              }
              break;
        
          case 6:
              //Finaliza a calibração e mostra os valores coletados
              Serial.println("FINALIZADO");

              Serial.print("LX: ");
              Serial.print(menorLX);
              Serial.print("\t");
              Serial.print(menorMidLX);
              Serial.print("\t");
              Serial.print(maiorMidLX);
              Serial.print("\t");
              Serial.println(maiorLX);

              Serial.print("RY: ");
              Serial.print(menorRY);
              Serial.print("\t");
              Serial.print(menorMidRY);
              Serial.print("\t");
              Serial.print(maiorMidRY);
              Serial.print("\t");
              Serial.println(maiorRY);


              etapa = 0;
              cal = 0;
              break;
    }
  }
}
 
void setup() {
  // Inicia o monitor Serial
  Serial.begin(115200);
  
  pinMode(CAL, INPUT_PULLDOWN);
  pinMode(LED, OUTPUT);
  pinMode(B1, INPUT);
  pinMode(B2, INPUT);
  pinMode(B3, INPUT);
  pinMode(B4, INPUT);

  pinMode(potPinD, INPUT);
  pinMode(potPinV, INPUT);
  pinMode(potTriggerL, INPUT);
  pinMode(potTriggerR, INPUT);
 
  // Configura o ESP32 como um Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Inicia o ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  
  //Configura a função de callback que será chamada ao enviar algum dado
  esp_now_register_send_cb(OnDataSent);
  
  // Registra o dispositivo que receberá os dados (peer)
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Adiciona o dispositivo que receberá os dados (peer)  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

}
 
void loop() {
  
  if (digitalRead(CAL) == 1 && temp == 0 && cal == 0){ //inicializa a contagem de tempo para começar a calbração
      temp = millis(); 
  }else if (digitalRead(CAL) == 1 && (millis() - temp) > 5000 && cal == 0){ 
      //Após 3 segundos com o botão pressionado, é dado inicio na calibração
      cal = 1;
      temp = 0;
  }else if (digitalRead(CAL) == 1 && cal == 0){
      //Mostra na tela quanto tempo o botão está sendo pressionado, antes de entrar na calibração
      Serial.print("iniciando calibracao...");
      Serial.println((millis()-temp)/1000);  
  }else if (digitalRead(CAL) == 0 && cal == 0){
      //Se soltar o botão antes dos 3s, o tempo é zerado
      temp = 0;
  }
  

  if(cal == 1){
        calibracao();
  }else{
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

        Serial.print("VD: ");
        Serial.print(mySpd.spdRight);
        Serial.print("\t");
        Serial.print("VE: ");
        Serial.print(mySpd.spdLeft);
        Serial.print("\t");
        Serial.print("|INT: ");
        Serial.print(valorSpd);
        Serial.print("\t");
        Serial.print("DIR: ");
        Serial.print(mySpd.dir);
        Serial.println("\t");

        mySpd.cont = contValidacao; 

        valorTriggerR = analogRead(potTriggerR);
        valorTriggerL = analogRead(potTriggerL);

        //Função de arma - acelaração contínua (R2)
        if (potTriggerL > 10){ 
          angle = map(valorTriggerL, 0, 255, 40, 140);
        }
        //Função de arma - aceleração discreta (R1)
        if (digitalRead(B1) == 1){
          angle = angle+2;
        }

        //Quadrado
        if (digitalRead(B2) == 1){
          angle = 40;
        }
        
        mySpd.angle = angle;
        
        // Envia os dados via ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &mySpd, sizeof(mySpd));

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
  }
}
