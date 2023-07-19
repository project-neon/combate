#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h> 

//Locomocão 
#define PWMA 27
#define PWMB 26
#define A1  13
#define A2  12
#define B2  25
#define B1  33

#define LED 2

/*
PINOS UTILIZADOS
SAÍDAS
PINO 16 - RODA ESQUERDA
PINO 17 - RODA DIREITA
*/

int SpdRight = 0;
int SpdLeft = 0;

//Brushless da arma
static const int brushPin = 4; 
Servo_ESP32 servo;


int lastVal = 0;

//Estrutura damensagem que será enviada
//DEVE SER A MESMA ESTRUTURA NO EMISSOR
typedef struct struct_message {
  int rightSpd;   //recebe o valor da velocidade da direita
  int leftSpd;  //recebe o valor da velocidade da esquerda
  String Dir; //recebe o valor da direção
  int val;
  int weapon;
} struct_message;


struct_message myData;    //Cria um objeto chamado myData

//Função de callback chamada ao receber algum dado
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.print(len);
  Serial.print("\t");
  Serial.print("VD: ");
  Serial.print(myData.rightSpd);
  Serial.print("\t");
  Serial.print("VE: ");
  Serial.print(myData.leftSpd);
  Serial.print("\t");
  Serial.print("DIR: ");
  Serial.print(myData.Dir);
  Serial.println();
  
}

void setup() {
  // Inicia o monitor Serial
  Serial.begin(115200);
  
  pinMode(LED, OUTPUT);

  ledcAttachPin(PWMA,5);
  ledcAttachPin(PWMB,6);


  ledcSetup(5, 80000, 8);
  ledcSetup(6, 80000, 8);
  
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  digitalWrite(A1, 0);
  digitalWrite(A2, 0);
  digitalWrite(B1, 0);
  digitalWrite(B2, 0);


  servo.attach(brushPin);
  int angle = 0;
  servo.write(angle);
  // Configura o ESP32 como um Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Inicia o ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  //Configura a função de callback que será chamada ao enviar algum dado
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  
  if (myData.val == lastVal){
    SpdRight = map(0, -100, 100, -180, 180);
    SpdLeft = map(0, -100, 100, -180, 180);
    digitalWrite(LED, LOW);
    servo.write(0);
    digitalWrite(A1,0);
    digitalWrite(A2,0);
    digitalWrite(B1,0);
    digitalWrite(B2,0);
  }else{
    SpdRight = map(inv*myData.rightSpd, -100, 100, -180, 180);   // Realiza a conversão para valores entre 0 e 180 para o motor da direita
    SpdLeft = map(inv*myData.leftSpd, -100, 100, -180, 180); // Realiza a conversão para valores entre 0 e 180 para o motor da esquerda
    digitalWrite(LED, HIGH);
  }

  lastVal = myData.val;

  if(myData.rightSpd > 0){
    digitalWrite(A1,1);
    digitalWrite(A2,0);
  }
  else{
    digitalWrite(A1,0);
    digitalWrite(A2,1);
  }
  ledcWrite(5, abs(myData.leftSpd));

  if(myData.rightSpd > 0){
    digitalWrite(B1,1);
    digitalWrite(B2,0);
  }
  else{
    digitalWrite(B1,0);
    digitalWrite(B2,1);
  }
  ledcWrite(6, abs(myData.rightSpd));
  servo.write(myData.weapon); //COMENTAR ESTE COMANDO EM CASO DE TESTE!!!!! (ATIVA A ARMA)

  Serial.print("Angle: ");
  Serial.println(myData.angle);
  Serial.print("SpdRight: ");
  Serial.print(SpdRight);
  Serial.print("\t");
  Serial.print("SpdLeft: ");
  Serial.println(SpdLeft);
}
