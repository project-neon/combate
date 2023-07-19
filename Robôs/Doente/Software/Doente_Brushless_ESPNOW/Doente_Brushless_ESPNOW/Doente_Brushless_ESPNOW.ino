#include <esp_now.h>
#include <WiFi.h>
//Biblioteca para controle de servo p/ brushless
#include <ESP32Servo.h>


//Pinos das ESCs
int ESCRPinL = 32;   
int ESCRPinR = 33; 

//PWM
int frequenciaESC = 48;
int Ppm_Min_Throttle = 1040;
int Ppm_Max_Throttle = 1960;

Servo ESCL;
Servo ESCR;

#define LED 2

/*
 *Pode ser que entre a linha 105 e 123 haja alguma confusão nas portas
 *do PWM. Caso o sentido de rotação esteja diferindo nos testes, essa
 *é uma possível causa, mas basta consertar no código.
 */


int SpdRight = 0;
int SpdLeft = 0;

int lastVal = 0;

//Estrutura damensagem que será enviada
//DEVE SER A MESMA ESTRUTURA NO EMISSOR
typedef struct struct_message {
  int rightSpd;   //recebe o valor da velocidade da direita
  int leftSpd;  //recebe o valor da velocidade da esquerda
  String Dir; //recebe o valor da direção
  int val;
  int weapon
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
}

void setup() {
  // Inicia o monitor Serial
  Serial.begin(115200);
  
  pinMode(LED, OUTPUT);

  ESCL.setPeriodHertz(frequenciaESC);
  ESCL.attach(ESCRPinL, Ppm_Min_Throttle, Ppm_Max_Throttle);
  ESCR.setPeriodHertz(frequenciaESC);
  ESCR.attach(ESCRPinR, Ppm_Min_Throttle, Ppm_Max_Throttle);
 
  // Configura o ESP32 como um Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Inicia o ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    ESP.restart();
    return;
  }
  
  //Configura a função de callback que será chamada ao enviar algum dado
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  
  if (myData.val == lastVal){
    SpdRight = map(0, -100, 100, 0, 180);
    SpdLeft = map(0, -100, 100, 0, 180);
    digitalWrite(LED, LOW);
  }else{
    SpdRight = map(myData.rightSpd, -100, 100, 0, 180);   // Realiza a conversão para valores entre 0 e 180 para o motor da direita
    SpdLeft = map(myData.leftSpd, -100, 100, 0, 180); // Realiza a conversão para valores entre 0 e 180 para o motor da esquerda
    digitalWrite(LED, HIGH);
  }

  lastVal = myData.val;

  ESCR.write(SpdRight);
  ESCL.write(SpdLeft);

 /* Serial.print("SpdRight: ");
  Serial.print(SpdRight);
  Serial.print("\t");
  Serial.print("SpdLeft: ");
  Serial.println(SpdLeft); */
  delay(20);
}


 