  //Setup pinos Locomocão Ponte H (TB6612fng)
  #define PWMA 3
  #define A1 5
  #define A2 4

int rec = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //ledcSetup(1, 8000, 8);
 // ledcAttachPin(PWMA,1);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  digitalWrite(A1, 0);
  digitalWrite(A2, 0);
  Serial.println("Inicio");
}

void loop() {
  rec = Serial.read();
  if (rec == 1){
    Serial.println("Girando");
   // ledcWrite(5,abs(100));
  }
  else if (rec == 2){
  //  ledcWrite(5, abs(0));
    Serial.println("Parado");
  }
}

