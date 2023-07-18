//---------------- PINOS DO DIP SWITCH ---------------//
#define switch1 34 
#define switch2 35
#define switch3 32
#define switch4 33
#define connect_button 12

#define TEMPO_DETECCAO 100
#define TENTATIVAS_ACHA_ROBO 20
#define DETECT_AGAIN 2000

int broadcastIndex = 0; //índice para o Mac Address no array de Mac Addresses
int lastBroadcastIndex = 0;
int detect = 0;

unsigned long detect_time = 0;
unsigned long start_time = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(connect_button, INPUT);
  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(switch3, INPUT);
  pinMode(switch4, INPUT); //VERIFICAR SE OS PINOS SÃO DE FATO PULLUP 
  //Se não forem PULLUP, a função getIndex deverá ser alterada retirando o
}

int detectaRobo(){
  int n = 0;
  int tempo_inicio = 0;
  uint8_t n0 = 0;
  uint8_t n1 = 0;
  uint8_t n2 = 0;
  uint8_t n3 = 0;
  uint8_t n4 = 0;
  uint8_t n_error = 0;
  int maior = 0;
  int maior_n = 0;
  int n_def = 0;
  //Serial.println("Iniciando detecção de robô.");
  for(int n_tentativas = 0; n_tentativas < TENTATIVAS_ACHA_ROBO; n_tentativas++){
    tempo_inicio = millis();
    while(millis()-tempo_inicio < TEMPO_DETECCAO){
      n = 0;
      n = (5*(!digitalRead(switch1))
      + 6*(!digitalRead(switch2))
      + 7*(!digitalRead(switch3))
      + 8*(!digitalRead(switch4)));
      if (n == 0 || n == 5 || n == 6 || n== 7 || n==8) break;
    }
    n_def = n;
    if (n_def > 8) n_error += 1;
    else{
      if(n_def==0) n0 += 1;
      if(n_def==5) n1 += 1;
      if(n_def==6) n2 += 1;
      if(n_def==7) n3 += 1;
      if(n_def==8) n4 += 1;
    }
   /* Serial.print("N: ");
    Serial.print(n);
    Serial.print("\t");
   /* Serial.print("N0:");
    Serial.print(n0);
    Serial.print("\t");
    Serial.print("N1:");
    Serial.print(n1);
    Serial.print("\t");
    Serial.print("N2:");
    Serial.print(n2);
    Serial.print("\t");
    Serial.print("N3:");
    Serial.print(n3);
    Serial.print("\t");
    Serial.print("N4:");
    Serial.println(n4);*/
  }
  Serial.print("N0:");
  Serial.print(n0);
  Serial.print("\t");
  Serial.print("N1:");
  Serial.print(n1);
  Serial.print("\t");
  Serial.print("N2:");
  Serial.print(n2);
  Serial.print("\t");
  Serial.print("N3:");
  Serial.print(n3);
  Serial.print("\t");
  Serial.print("N4:");
  Serial.print(n4);
  Serial.print("\t");
  int numeros[5] = {n0 , n1, n2, n3, n4};
  for(int i = 0; i <= 4; i++){
    if(numeros[i] > maior){
       maior = numeros[i];
       maior_n = i;
    }
  }
  if (n_error > maior){
    maior_n = 5;
    Serial.println("Seleção Inválida");
  }
  else{
    if (maior_n != 0){
      Serial.print("Robô detectado: ");
      Serial.println(maior_n);
    }
    else{
      Serial.println("Nenhum robô selecionado");
    }
  }

  return maior_n;
}



void loop() {
  //delay(5000);
  if (detect == 0) {
 // if(digitalRead(connect_button == 0)) {
    broadcastIndex = detectaRobo();
    detect = 1 ;
  }
 /* if ((digitalRead(connect_button == HIGH)) && (detect == 1) && (millis() - start_time > 2000))
  {
    broadcastIndex = detectaRobo();
    detect = 0;
  }*/
  //Serial.println(digitalRead(connect_button));

}
