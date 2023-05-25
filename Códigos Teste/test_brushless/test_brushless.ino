 //User paramiters

 //Curvas e direção motores
 int ESCRPinL = 13;   


 //PWM
 int frequenciaESC = 50;
 int Ppm_Min_Throttle = 1050;
 int Ppm_Max_Throttle = 1950;


 
 //Controle
 char ipControle[18] = "01:01:01:01:01:01";
 
 
 //Codigo

  //Biblioteca do controle de PS4
  #include <PS4Controller.h>
  //Biblioteca para controle de servo p/ brushless
  #include <ESP32Servo.h>

//Variaveis auxiliares
  Servo ESCL;

void setup() {
  Serial.begin(115200);
    while (!Serial)
      delay(10); // will pause Zero, Leonardo, etc until serial console opens
    Serial.println("Inicio");

    PS4.begin(ipControle); //Faz o pareamento do controle de PS4

    // Definindo valores para ESC do motor esquerd

    ESCL.setPeriodHertz(frequenciaESC);
    ESCL.attach(ESCRPinL, Ppm_Min_Throttle, Ppm_Max_Throttle);

    while(PS4.isConnected()!= true){
    delay(20);
    
    } 
}
//Controle motor esquerda core 0
void loop() {

while(PS4.isConnected()) {
  Serial.println("Angulo");
  ESCL.write(map(PS4.R2Value(), 0, 255, 0, 180));
  Serial.println(map(PS4.R2Value(), 0, 255, 0, 180)); 
  delay(10);   
  
    }
    //Failsafe
    if(PS4.isConnected()!= true){
      Serial.println("Failsafe");
      ESCL.write(0); // Envia velocidade nula no failsafe
      // Os pinos dos motores de flip são setados como LOW
      Serial.println("Restart");
      ESP.restart();
      delay(20);
    }
}


 
