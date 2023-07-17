//Biblioteca do controle de PS4
#include <PS4Controller.h>
#include <ESP32Servo.h>


//Setup pino ESC locomoção
#define ESCPinL 14
#define ESCPinR 12

//Setup pino da ESC da arma
#define ESCPinArma 2
//Setup frequência e largura min/max de pulso do Brushless da arma
//ARRUMAR PARAMETROS DE PULSO NA ESC
#define frequenciaESC 50
#define Ppm_Min_Throttle  1048
#define Ppm_Max_Throttle  1952
//Setup MAC address do controle
#define MAC_ADDRESS "64:64:64:64:64:64"

//VARIÁVEL DE INVERSÃO DE MOVIMENTO
int inv = 1; //se o robô estiver começando invertindo, alternar para -1 ou 1

Servo ESCL;
Servo ESCR;
Servo ESCArma;
int angle = 0;

void motors_control(int linear, int angular) {
  int result_R = linear - angular; //ao somar o angular com linear em cada motor conseguimos a ideia de direcao do robo
  int result_L = linear + angular;
  //não envia valores de potencia abaixo de 15, que não são fortes o suficiente para mover o robo
  if(result_R<15 && result_R >-15) result_R=0; 
  if(result_L<15 && result_L >-15 ) result_L=0;
  //Não permite valores superiores a 255 ou inferiores a -255
  if(result_R >255 ) result_R= 254;  
  if(result_R<-255 ) result_R=-254;
  if(result_L >255 ) result_L= 254;
  if(result_L<-255 ) result_L=-254;  
  
  //manda para a funcao motor um valor de -255 a 255, o sinal signifca a direcao  
  motor_A(result_L); 
  motor_B(result_R);
}

void motor_A(int speedA){  // se o valor for positivo gira para um lado e se for negativo troca o sentido
  speedA =  map(speedA, -255, 255, 0, 180);
  ESCR.write(speedA);

}

void motor_B(int speedB){
  speedB = map(speedB, -255, 255, 0 ,180);
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
  //motors_control(linear_speed*multiplicador, angular_speed* multiplicador2);
  // Multiplicadcor = 1.8 para aumentar a velocidade linear, o quao rapido o robo vai ser
  // Multiplicadcor2 = multiplic_curva, parametro que varia de 1 ate a 2.3 para suavisar as curvas em alta velocidade
    if(PS4.LStickY()<-25 || PS4.LStickY()>25){
      motors_control((1.8)*inv*PS4.LStickY(),(1.3)*PS4.RStickX());

    }else { // Controle sobre valores pequenos devido a problemas na funcao map
      motors_control((inv)*PS4.LStickY(), (1.5)*PS4.RStickX());

    }       
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

