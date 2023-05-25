 //User parameters

 //Curvas e direção motores
 float multiplicadorFrente = 1.8;
 float multiplicadorCurva = 0.8;
 int sentidoMotor = -1;
 int ESCRPinL = 32;   
 int ESCRPinR = 33; 

 //PWM
 int frequenciaESC = 48;
 int Ppm_Min_Throttle = 1040;
 int Ppm_Max_Throttle = 1960;
 unsigned long tempoMilisMax = 6;

 
 //Controle
 int analogicoMargemDeErro = 30;
 char ipControle[18] = "9F:56:45:F8:96:18";
 
 
 //Codigo

  //Biblioteca do controle de PS4
  #include <PS4Controller.h>
  //Biblioteca para controle de servo p/ brushless
  #include <ESP32Servo.h>

//Variaveis auxiliares
  Servo ESCL;
  Servo ESCR;
  int angleAUXR = 90;
  int angleAUXL = 90;
  int angleR = 90;
  int angleL = 90;
  unsigned long milisR1=0;
  unsigned long milisR2=0;
  unsigned long milisL1=0;
  unsigned long milisL2=0;  

//Nomeando tarefas

TaskHandle_t Task1;

//Controle Esquerda
  void motors_controlL(int linear, int angular) {
    int result_L = linear - angular; //ao somar o angular com linear em cada motor conseguimos a ideia de direcao do robo
    //não envia valores de potencia abaixo de 15, que não são fortes o suficiente para mover o robo
    if(result_L<(analogicoMargemDeErro) && result_L >-(analogicoMargemDeErro)) result_L=0;
    //Não permite valores superiores a 255 ou inferiores a -255
    if(result_L >255 ) result_L= 255;
    if(result_L<-255 ) result_L=-255;  
    
    //manda para a funcao motor um valor de -255 a 255, o sinal indica o sentido de rotacao 
    motor_L(result_L);
  }
  void motor_L(int speedL){  // se o valor for positivo gira para um lado e se for negativo troca o sentido
    speedL = map(speedL, -255, 255, 0, 180);
 unsigned long  tempo = map(abs(speedL-90),0,90,tempoMilisMax,0);
    if(speedL > angleAUXL){
      while (speedL > angleAUXL){
       tempo = map(abs(speedL-90),0,90,tempoMilisMax,0); 
        if((millis() - milisL1) >= tempo){
          milisL1 = millis(); 
          ESCL.write(angleAUXL);         
          Serial.println(angleAUXL);
          Serial.print("angleAUXL: " );
          Serial.println(angleAUXL);
          angleAUXL++;
        }
      } 
    }else{
      while (speedL <= angleAUXL){
        tempo = map(abs(speedL-90),0,90,tempoMilisMax,0);
          if((millis() - milisL2 )>= tempo){
            milisL2 = millis();          
            ESCL.write(angleAUXL);
            Serial.print("angleAUXL: " );
            Serial.println(angleAUXL);  
            angleAUXL--;  
            
          }  
        }
     }
  }

//Controle Direita

 void motors_controlR(int linear, int angular) {
    int result_R = linear + angular; //ao somar o angular com linear em cada motor conseguimos a ideia de direcao do robo
    //não envia valores de potencia abaixo de 15, que não são fortes o suficiente para mover o robo
    if(result_R<(analogicoMargemDeErro) && result_R >-(analogicoMargemDeErro)) result_R=0; 
    //Não permite valores superiores a 255 ou inferiores a -255
    if(result_R >255 ) result_R= 255; 
    if(result_R<-255 ) result_R=-255;
    //manda para a funcao motor um valor de -255 a 255, o sinal indica o sentido de rotacao 
    motor_R(result_R); 
  }
  void motor_R(int speedR){  // se o valor for positivo gira para um lado e se for negativo troca o sentido
   speedR = map(speedR, -255, 255, 180, 0);
   unsigned long  tempo = map(abs(speedR-90),0,90,tempoMilisMax,0);    
   if(speedR > angleAUXR){
     while (speedR > angleAUXR){
      tempo = map(abs(speedR-90),0,90,tempoMilisMax,0);         
       if((millis() - milisR1) >=tempo){
         milisR1 = millis();          
         ESCR.write(angleAUXR);
         Serial.print("angleAUXR: " );
         Serial.println(angleAUXR);
         angleAUXR++;
                 
       }
     } 
   }else{
     while (speedR <= angleAUXR){
      tempo = map(abs(speedR-90),0,90,tempoMilisMax,0);        
       if((millis() - milisR2 )>=tempo){
        milisR2 = millis();
        ESCR.write(angleAUXR);
        Serial.print("angleAUXR: " );
        Serial.println(angleAUXR);
        angleAUXR--;
                   
       }  
     }
   }  
 }

void setup() {
  Serial.begin(115200);
    while (!Serial)
      delay(10); // will pause Zero, Leonardo, etc until serial console opens
  

    PS4.begin(ipControle); //Faz o pareamento do controle de PS4

    // Definindo valores para ESC do motor esquerd

    ESCL.setPeriodHertz(frequenciaESC);
    ESCL.attach(ESCRPinL, Ppm_Min_Throttle, Ppm_Max_Throttle);
    ESCR.setPeriodHertz(frequenciaESC);
    ESCR.attach(ESCRPinR, Ppm_Min_Throttle, Ppm_Max_Throttle); 

    while(PS4.isConnected()!= true){
    delay(20);
    } 


//Multe core inicialização

  xTaskCreatePinnedToCore(
    codeForTask1,            /* Task function. */
    "Task_1",                 /* name of task. */
    1000,                    /* Stack size of task */
    NULL,                     /* parameter of the task */
    1,                        /* priority of the task */
    &Task1,                   /* Task handle to keep track of created task */
    0);                       /* Core */
}

//Controle motor esquerda core 0
void loop() {

while(PS4.isConnected()) {

    //motors_control(linear_speed*multiplicador, angular_speed* multiplicador2);
    // Multiplicadcor = 1.8 para aumentar a velocidade linear, o quao rapido o robo vai ser
    // Multiplicadcor2 = multiplic_curva, parametro que varia de 1 ate a 2.3 para suavisar as curvas em alta velocidade
      if(PS4.LStickY()<-(analogicoMargemDeErro) || PS4.LStickY()>(analogicoMargemDeErro)){
        motors_controlL((multiplicadorFrente)*PS4.LStickY(), (sentidoMotor)*PS4.RStickX());
        delay(20);
      }else { // Controle sobre valores pequenos devido a problemas na funcao map
        motors_controlL((multiplicadorFrente)*PS4.LStickY(), (sentidoMotor)*PS4.RStickX()/(multiplicadorCurva));
        delay(20);
      }
      //inicio dos Brushless
      

    }
    //Failsafe
    if(PS4.isConnected()!= true){
      Serial.println("Failsafe");
      motors_controlL(90,90); // Envia velocidade nula no failsafe
      // Os pinos dos motores de flip são setados como LOW
      Serial.println("Restart");
      setup();
      delay(20);
    }
}

//Controle motor direito core 1

void codeForTask1( void * parameter )
{
while(PS4.isConnected()) {

    //motors_control(linear_speed*multiplicador, angular_speed* multiplicador2);
    // Multiplicadcor = 1.8 para aumentar a velocidade linear, o quao rapido o robo vai ser
    // Multiplicadcor2 = multiplic_curva, parametro que varia de 1 ate a 2.3 para suavisar as curvas em alta velocidade
      if(PS4.LStickY()<-(analogicoMargemDeErro) || PS4.LStickY()>(analogicoMargemDeErro)){
        motors_controlR((sentidoMotor)*PS4.RStickX(),(multiplicadorFrente)*PS4.LStickY());
        delay(20);
      }else { // Controle sobre valores pequenos devido a problemas na funcao map
        motors_controlR((sentidoMotor)*PS4.RStickX()/(multiplicadorCurva),(multiplicadorFrente)*PS4.LStickY());
        delay(20);
      }
    }
    //Failsafe
    if(PS4.isConnected()!= true){
      Serial.println("Failsafe");
      motors_controlR(90,90); // Envia velocidade nula no failsafe
      // Os pinos dos motores de flip são setados como LOW
      Serial.println("Restart");
      ESP.restart();
      delay(20);
    }
}
 
