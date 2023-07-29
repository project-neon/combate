//Biblioteca do controle de PS4
#include <PS4Controller.h>
//Biblioteca para controle de servo p/ brushless
#include <ESP32Servo.h>

#define MAC_ADDRESS "64:64:64:64:64:64" //MAC address atrubuído ao controle

//PWM
#define frequenciaESC 48 //Frequencia do brushless de locomoçao
#define Ppm_Min_Throttle 1040 //Largura de pulso min/max do brushless de locomoçao
#define Ppm_Max_Throttle 1960


int analogicoMargemDeErro = 30; //definiçao do ponto morto

#define L_center 10
#define R_center 10

//Pinos das ESCs
int ESCRPinL = 26;   
int ESCRPinR = 27; 

int PS4_L = 0;
int PS4_R = 0;

//PWM
int frequenciaESC = 48; //Frequencia do brushless de locomoçao
int Ppm_Min_Throttle = 1040; //Largura de pulso min/max do brushless de locomoçao
int Ppm_Max_Throttle = 1960;

//Definiçao das variáveis de Servo (brushless)
Servo ESCL;
Servo ESCR;

int inv = -1; //Permite inverter a pilotagem conforme o lado do robo que esta para cima
void motors_control(int linear, int angular) {
  int result_R = 0;
  int result_L = 0;
  if (((L_center * -1) < linear) && (linear < L_center)) linear = 0;
  if (((R_center * -1) < angular) && (angular < R_center)) angular = 0;
  if ((linear == 0) && (angular != 0)){
    result_L = angular*(0.4);
    result_R = angular*(-1)*0.4;
  }
  else if (angular == 0){
    result_L = linear;
    result_R = linear;
  }
  else if (angular > 0){
    result_L = linear; //ao somar o angular com linear em cada motor conseguimos a ideia de direcao do robo
    result_R = linear - (angular*0.4);
  }
  else if (angular<0){
    result_L = linear + (angular*0.4);
    result_R = linear;
  }
  //manda para a funcao motor um valor de -255 a 255, o sinal signifca a direcao  

  motor_B(result_R);
  motor_A(result_L); 

}

void motor_A(int speedA){  // se o valor for positivo gira para um lado e se for negativo troca o sentido
  if((PS4.R2()) && (abs(speedA) > 110 )) speedA = map(inv*PS4.R2Value(), -255, 255, 20, 160);
  else speedA = map(speedA, -128, 127, 50, 130);  
  ESCR.write(speedA);
  Serial.print("R: ");
  Serial.println(speedA);
}

void motor_B(int speedB){
  if((PS4.R2()) && (abs(speedB) > 110 )) speedB = map(inv*PS4.R2Value(), -255, 255, 20, 160);
  else speedB = map(speedB, -128, 127, 50, 130);  
  ESCL.write(speedB);
  Serial.print("L: ");
  Serial.print(speedB);
  Serial.print("\t");
}


void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens


  PS4.begin(MAC_ADDRESS);
  Serial.println("Ready.");
  
  ESCL.setPeriodHertz(frequenciaESC); //define a frequencia de operaçao do ESC do motor esquerdo
  ESCL.attach(ESCRPinL, Ppm_Min_Throttle, Ppm_Max_Throttle); //atribui o pino às larguras de puslo
  ESCR.setPeriodHertz(frequenciaESC);
  ESCR.attach(ESCRPinR, Ppm_Min_Throttle, Ppm_Max_Throttle); 

  while(PS4.isConnected()!= true){
  delay(20);}
}

void loop() {

  while(PS4.isConnected()) {
  //motors_control(linear_speed*multiplicador, angular_speed* multiplicador2);
  // Multiplicadcor = 1.8 para aumentar a velocidade linear, o quao rapido o robo vai ser
  // Multiplicadcor2 = multiplic_curva, parametro que varia de 1 ate a 2.3 para suavisar as curvas em alta velocidade
    PS4_L = PS4.LStickY();
    PS4_R = PS4.RStickX();
    motors_control(inv*PS4_L, PS4_R); //em baixa velocidade
  //Sentido de locomocao invertido - Seta para baixo
    if(PS4.Down()){
      inv = -1;
      delay(100);
    } 
  //Sentido de locomocao normal - Seta para cima
    if(PS4.Up()){
      inv = 1;
      delay(100);
    }
  }
    delay(10);
  //Failsafe
  if(PS4.isConnected()!= true){
    motors_control(0,0);
    Serial.println("Restart");
    PS4.end();
    ESP.restart();
    delay(20);
  }
}
 