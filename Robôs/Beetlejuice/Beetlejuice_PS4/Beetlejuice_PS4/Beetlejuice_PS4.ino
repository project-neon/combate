//Biblioteca do controle de PS4
#include <PS4Controller.h>
#include <ESP32Servo.h>


#define L_center 17
#define R_center 17

//Setup pino ESC locomoção
#define ESCPinL 15
#define ESCPinR 4

//Setup pino da ESC da arma
#define ESCPinArma 2
//Setup frequência e largura min/max de pulso do Brushless da arma
//ARRUMAR PARAMETROS DE PULSO NA ESC
#define frequenciaESC 48
#define Ppm_Min_Throttle  1040
#define Ppm_Max_Throttle  1960
//Setup MAC address do controle
#define MAC_ADDRESS "64:64:64:64:64:64"

//VARIÁVEL DE INVERSÃO DE MOVIMENTO
int inv = 1; //se o robô estiver começando invertindo, alternar para -1 ou 1

int PS4_L = 0;
int PS4_R = 0;

Servo ESCL;
Servo ESCR;
Servo ESCArma;
int angle = 0;

void motors_control(int linear, int angular) {
  int result_R = 0;
  int result_L = 0;
  //max 128 min -127
 /* if (angular < 0 ) angular = map(angular, -127,0, -100, 0);
  else if (angular > 0) angular = map(angular, 0, 128, 0, 100);
  if (linear < 0) linear = map(linear, -127, 0, -100, 0);
  else if (linear > 0) linear = map(linear, 0, 128, 0, 100);
  if(angular>0){
      result_R = linear - linear*(angular/100);
      result_L = linear;
  }
  else if (angular<0){
    result_R = linear;
    result_L = linear + linear*(angular/100);
  }
  result_R = map(result_R, -100, 100, 0, 180);
  result_L = map(result_L, -100, 100, 0, 180);*/
  //angular = map(angular, -127,128,0, 100);
  if (angular != 0){
    result_R = linear - angular; //ao somar o angular com linear em cada motor conseguimos a ideia de direcao do robo
    result_L = linear + angular;
  }
  else{
    result_R = map(linear, -128, 127, -256, 254);
    result_L = map(linear, -128, 127, -256, 254);
  }
//não envia valores de potencia abaixo de 15, que não são fortes o suficiente para mover o robo
  //if(result_R<15 && result_R >-15) result_R=0; 
 // if(result_L<15 && result_L >-15 ) result_L=0;
  //manda para a funcao motor um valor de -255 a 255, o sinal signifca a direcao  
  motor_A(result_L);
  motor_B(result_R);
}

void motor_A(int speedA){  // se o valor for positivo gira para um lado e se for negativo troca o sentido
  speedA =  map(speedA, -256, 254, 0, 180);
  ESCR.write(speedA);
  Serial.print("L: ");
  Serial.print(speedA);
  Serial.print("\t");

}

void motor_B(int speedB){
  speedB = map(speedB, -256, 254, 0 ,180);
  ESCL.write(speedB);
  Serial.print("R: ");
  Serial.print(speedB);
  Serial.print("\t");
}


void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens


  PS4.begin(MAC_ADDRESS);
  Serial.println("Ready.");
  

  ESCL.setPeriodHertz(frequenciaESC);
  ESCL.attach(ESCPinL, Ppm_Min_Throttle, Ppm_Max_Throttle);
  ESCR.setPeriodHertz(frequenciaESC);
  ESCR.attach(ESCPinR, Ppm_Min_Throttle, Ppm_Max_Throttle);


//Pino brushless, iniciado como parado
  ESCArma.setPeriodHertz(frequenciaESC);
  ESCArma.attach(ESCPinArma, Ppm_Min_Throttle, Ppm_Max_Throttle);

  while(PS4.isConnected()!= true){
  delay(20);}
}

void loop() {

  while(PS4.isConnected()) {
    PS4_L = PS4.LStickY();
    PS4_R = PS4.RStickX();
    if (((L_center * -1) < PS4_L) && (PS4_L < L_center)) PS4_L = 0;
    if (((R_center * -1) < PS4_R) && (PS4_R < R_center)) PS4_R = 0;
  //motors_control(linear_speed*multiplicador, angular_speed* multiplicador2);
  // Multiplicadcor = 1.8 para aumentar a velocidade linear, o quao rapido o robo vai ser
  // Multiplicadcor2 = multiplic_curva, parametro que varia de 1 ate a 2.3 para suavisar as curvas em alta velocidade
    Serial.print("AnalogL: ");
    Serial.print(PS4_L);
    Serial.print("\t");
    Serial.print("AnalogR: ");
    Serial.println(PS4_R);
    motors_control((inv)*PS4_L, PS4_R);     
      //inicio do Brushless - seta o piso do valor (primeiro beep)- Botão quadrado
      if (PS4.Cross()) { 
          angle=0;
          ESCArma.write(angle);
          Serial.println(angle);
          delay(100);
      }
      if (PS4.Square()) { 
          angle=0;
          ESCArma.write(angle);
          Serial.println(angle);
          delay(500);
          angle=180;
          ESCArma.write(angle);
          Serial.println(angle);
          delay(500);
          angle=0;
          ESCArma.write(angle);
          Serial.println(angle);
          delay(500);
      }
      //Função para arma ativa, aceleração discreta com o gatilho - Botão R1
      if (PS4.R1()){
        angle=angle+2;
        ESCArma.write(angle);
        Serial.println(angle);
        delay(200);
    }
        //Sentido de locomocao invertido - Botão Seta para baixo
       if(PS4.Down()){
          inv = -1;
          delay(100);
       }
       //Sentido de locomocao principal
        if(PS4.Up()){
          inv = 1;
          delay(100);
       }
        
       //Função para arma ativa, aceleração gradativa/dinamica com o gatilho - Botão R2
        if(PS4.R2()){ 
        angle=map(PS4.R2Value(),0,255,0,90);
        ESCArma.write(angle);
        Serial.println(angle); 
        } 
      delay(400);

  }
    
  //Failsafe
  if(PS4.isConnected()!= true){
    angle = 0;
    ESCArma.write(angle);
    motors_control(0,0);
    Serial.println("Restart");
    PS4.end();
    ESP.restart();
    delay(20);
  }
  
}

