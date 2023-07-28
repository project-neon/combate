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
  if (((L_center * -1) < linear) && (linear < L_center)) linear = 0;
  if (((R_center * -1) < angular) && (angular < R_center)) angular = 0;
/*  Serial.print("Linear: ");
  Serial.print(linear);
  Serial.print("\t");
  Serial.print("Angular: ");
  Serial.print(angular);
  Serial.print("\t");*/
  if ((linear == 0) && (angular != 0)){
    result_L = angular*(0.8);
    result_R = angular*(-1)*0.8;
  }
  else if (angular == 0){
    result_L = linear;
    result_R = linear;
  }
  else if (angular > 0){
    result_L = linear; //ao somar o angular com linear em cada motor conseguimos a ideia de direcao do robo
    result_R = linear - angular;
  }
  else if (angular < 0){
    result_L = linear + angular;
    result_R = linear;
  }
  //manda para a funcao motor um valor de -255 a 255, o sinal signifca a direcao  
  Serial.print("resultL: ");
  Serial.print(result_L);
  Serial.print("\t");
  Serial.print("resultR: ");
  Serial.print(result_R);
  Serial.print("\t");
  motor_B(result_L);
  motor_A(result_R); 
}

void motor_A(int speedA){  // se o valor for positivo gira para um lado e se for negativo troca o sentido
  speedA =  map(speedA, -128, 127, 40, 140);
  ESCR.write(speedA);
 /* Serial.print("r: ");
  Serial.print(speedA);
  Serial.print("\t");*/

}

void motor_B(int speedB){
  speedB = map(speedB, -128, 127, 140 ,40);
  ESCL.write(speedB);
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
  //motors_control(linear_speed*multiplicador, angular_speed* multiplicador2);
  // Multiplicadcor = 1.8 para aumentar a velocidade linear, o quao rapido o robo vai ser
  // Multiplicadcor2 = multiplic_curva, parametro que varia de 1 ate a 2.3 para suavisar as curvas em alta velocidade
    /*Serial.print("AnalogL: ");
    Serial.print(PS4_L);
    Serial.print("\t");
    Serial.print("AnalogR: ");
    Serial.println(PS4_R);*/
    if ((PS4_L > -40) && (PS4_L< 40))  motors_control(inv*PS4_L, PS4_R*0.5);
    else motors_control(inv*PS4_L*0.8, PS4_R * 0.5);     
      //inicio do Brushless - seta o piso do valor (primeiro beep)- Botão quadrado
      if (PS4.Cross()) { 
          angle=0;
          ESCArma.write(angle);
        //  Serial.println(angle);
          delay(100);
      }
      if (PS4.Square()) { 
          angle=0;
          ESCArma.write(angle);
         // Serial.println(angle);
          delay(500);
          angle=180;
          ESCArma.write(angle);
       //   Serial.println(angle);
          delay(500);
          angle=0;
          ESCArma.write(angle);
         // Serial.println(angle);
          delay(500);
      }
      //Função para arma ativa, aceleração discreta com o gatilho - Botão R1
      if (PS4.R1()){
        angle=angle+2;
        ESCArma.write(angle);
     //   Serial.println(angle);
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
        //Serial.println(angle); 
        } 
      delay(20);

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

