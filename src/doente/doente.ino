  //Biblioteca do controle de PS4
  #include <PS4Controller.h>
 
  //Locomocão 
  #define PWMB 27
  #define PWMA 26
  #define B1  13
  #define B2  12
  #define A2  25
  #define A1  33

  
  int inv = -1; //Permite inverter a pilotagem conforme o lado do robo que esta para cima
  
  void motors_control(int linear, int angular) {
    int result_R = linear - angular; //ao somar o angular com linear em cada motor conseguimos a ideia de direcao do robo
    int result_L = linear + angular;
    //não envia valores de potencia abaixo de 15, que não são fortes o suficiente para mover o robo
    if(result_R<15 && result_R >-15) result_R=0; 
    if(result_L<15 && result_L >-15 ) result_L=0;
    //Não permite valores superiores a 255 ou inferiores a -255
    if(result_R >255 ) result_R= 254; 
    if(result_R<-255 ) result_R=-254;
    if(result_L>255 ) result_L= 254;
    if(result_L<-255 ) result_L=-254;  
    
    //manda para a funcao motor um valor de -255 a 255, o sinal signifca a direcao  
    motor_A(result_R); 
    motor_B(result_L);
  }
  
  void motor_A(int speedA){  // se o valor for positivo gira para um lado e se for negativo troca o sentido
    if(speedA>0){  
      digitalWrite(A1, 1);
      digitalWrite(A2, 0);
    }else{
      digitalWrite(A1, 0);
      digitalWrite(A2, 1);
    }
    ledcWrite(5,abs(speedA));


  }
  
  void motor_B(int speedB){
    if(speedB>0){
      digitalWrite(B1, 1);
      digitalWrite(B2, 0);
    }else{
      digitalWrite(B1, 0);
      digitalWrite(B2, 1);
    }
    ledcWrite(6,abs(speedB));

  }
  
  
  void setup(void) {
    Serial.begin(115200);
    while (!Serial)
      delay(10); // will pause Zero, Leonardo, etc until serial console opens
  
  
    PS4.begin("00:e0:4c:65:4d:0f");
    Serial.println("Ready.");
    
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
  
   while(PS4.isConnected()!= true){
    delay(20);}
  }
  
  void loop() {
  
    while(PS4.isConnected()) {
    //motors_control(linear_speed*multiplicador, angular_speed* multiplicador2);
    // Multiplicadcor = 1.8 para aumentar a velocidade linear, o quao rapido o robo vai ser
    // Multiplicadcor2 = multiplic_curva, parametro que varia de 1 ate a 2.3 para suavisar as curvas em alta velocidade
      if(PS4.LStickY()<-15 || PS4.LStickY()>15){
        int curva = map(abs(PS4.LStickY()), 0, 127, 80, 190);
        float multiplic_curva = (float) curva/100;
        motors_control(1.8*inv*PS4.LStickY(), (-1)*PS4.RStickX()/multiplic_curva);

      }else { // Controle sobre valores pequenos devido a problemas na funcao map
        motors_control(1.8*inv*PS4.LStickY(), (-1)*PS4.RStickX()/0.8);

      }
        //Sentido de locomocao invertido
       if(PS4.R3()){
          inv = 1;
          delay(100);
       }
       //Sentido de locomocao principal
        if(PS4.L3()){
          inv = -1;
          delay(100);
       }
       
    }
     
    //Failsafe
    if(PS4.isConnected()!= true){
    motors_control(0,0);
    Serial.println("Restart");
    delay(20);
    }
    
  }
