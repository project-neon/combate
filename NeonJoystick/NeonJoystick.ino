#include <esp_now.h>
#include <WiFi.h>


/*PADRÃO UTILIZADO NO DIP SWITCH
1. DOENTE
2. CIRROSE
3. SOLUÇO
4. ESGUICHO


 - Falta separar os códigos para diferentes robos

- falta programar o oled

- falta arrumar o debouncing
 * */

 
// --------------------- PINOS DOS ANALÓGICOS --------------//
#define potPinD 33  
#define potPinV 32      

//---------------- PINOS DO DIP SWITCH ---------------//
#define switch1 33 
#define switch2 32
#define switch3 35
#define switch4 34

//---------------- PINOS DOS BOTÕES ---------------//
#define B1 12
#define B2 18
/*#define B3 18
#define B4 5*/

//----------------VARIAVIES DE DETECÇAO DE ROBO (DIPSWITCH) ---------------//
#define TEMPO_DETECCAO 10000
#define TENTATIVAS_ACHA_ROBO 30

#define LED 2   //LED para verificação de status da comunicação
//#define CAL 15  //Pino usado para calibrar os joysticks

#define DT_DEBOUNCING 200

int valorDir = 0; //valor do analógico direito
int valorSpd = 0; //valor do analógico esquerdo


int broadcastIndex = 1; //índice para o Mac Address no array de Mac Addresses

int statusCom = 0;
int lastValidacao = 0;
int atualValidacao = 0;

int inv = -1; //indica se o sentido de locomoção está invertido


int button1 = 0;
int button2 = 0;


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


uint8_t addressArrays[4][6] = {{0x08, 0x3A, 0xF2, 0x50, 0xE0, 0x30}, {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF}} ;// INSERIR 4 MAC ADDRESSES RELATIVOS A ESPS RECEPTORAS DIFERENTES 

//Estrutura da mensagem que será enviada
//DEVE SER A MESMA ESTRUTURA NO RECEPTOR
typedef struct struct_message {
  int spdRight;
  int spdLeft;
  String dir;
  int cont;
  int inv;
  int arma
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
  int maior_robo = 0; //qual robo foi detectado mais vezes pelo dipswitch
  int maior_ocorr = 0; //numero de vezes que o robo mais frequente ocorreu
  // Inicia o monitor Serial
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(potPinD, INPUT);
  pinMode(potPinV, INPUT);
  pinMode(B1, INPUT_PULLDOWN); //Botões
  pinMode(B2, INPUT_PULLDOWN);
  pinMode(B3, INPUT);
  pinMode(B4, INPUT);
  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(switch3, INPUT);
  pinMode(switch4, INPUT); //VERIFICAR SE OS PINOS SÃO DE FATO PULLUP 
  // Configura o ESP32 como um Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Inicia o ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    ESP.restart();
    return;
  }

  //Configura a função de callback que será chamada ao enviar algum dado
  esp_now_register_send_cb(OnDataSent);


  //TRECHO DO CÓDIGO DE IDENTIFICAÇÃO DE ROBO
  for(int n_tentativas = 0; n_tentativas < TENTATIVAS_ACHA_ROBO; n_tentativas++){
    tempo_inicio = millis();
    while(millis()-tempo_inicio < TEMPO_DETECCAO){
      n = (1*(!digitalRead(switch1))
      + 2*(!digitalRead(switch2))
      + 3*(!digitalRead(switch3))
      + 4*(!digitalRead(switch4)));
      if (n == 1 || n == 2 || n== 3 || n==4) break;
    }
    if(n==1) n1 += 1;
    if(n==2) n2 += 1;
    if(n==3) n3 += 1;
    if(n==4) n4 += 1;
  }
  int numeros[4] = {n1, n2, n3, n4};
  for(int i = 1; i <= 4; i++){
    if(numeros[i-1] > maior){
       maior_robo = numeros[i-1];
       maior_ocorr = i;
    }
  }
  broadcastIndex = maior - 1; //de 0 a 3


  // Registra o dispositivo que receberá os dados (peer)
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Adiciona o dispositivo que receberá os dados (peer)  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  //registerPeer(lastBroadcastIndex);
}

void loop() {
  if (digitalRead(B1) == 1 && digitalRead(B2) == 1 && temp == 0 && cal == 0){ //inicializa a contagem de tempo para começar a calbração
      temp = millis(); 
  }else if (digitalRead(B1) == 1 && digitalRead(B2) == 1 && (millis() - temp) > 5000 && cal == 0){ 
      //Após 3 segundos com o botão pressionado, é dado inicio na calibração
      cal = 1;
      temp = 0;
  }else if (digitalRead(B1) == 1 && digitalRead(B2) == 1 && cal == 0){
      //Mostra na tela quanto tempo o botão está sendo pressionado, antes de entrar na calibração
      Serial.print("iniciando calibracao...");
      Serial.println((millis()-temp)/1000);  
  }else if ((digitalRead(B1) == 0 || digitalRead(B2) == 0)  && cal == 0){
      //Se soltar o botão antes dos 3s, o tempo é zerado
      temp = 0;
  }
  

  if(cal == 1){
        calibracao();
  }else{
        if(digitalRead(B1) == 1){ //Sentido de rotação normal quando B1 é acionado
          t_b1 = millis();
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

        Serial.print("VD: ");
        Serial.print(mySpd.spdRight);
        Serial.print("\t");
        Serial.print("VE: ");
        Serial.println(mySpd.spdLeft);/*
          Serial.print("B1: ");
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
        mySpd.cont = contValidacao; 


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
  delay(20);
}
