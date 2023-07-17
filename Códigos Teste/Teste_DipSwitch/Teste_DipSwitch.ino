//---------------- PINOS DO DIP SWITCH ---------------//
#define switch1 3 
#define switch2 4
#define switch3 5
#define switch4 6

#define TEMPO_DETECCAO 10000
#define TENTATIVAS_ACHA_ROBO 30

int broadcastIndex = 0; //índice para o Mac Address no array de Mac Addresses
int lastBroadcastIndex = 0;
int detect = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(switch3, INPUT);
  pinMode(switch4, INPUT); //VERIFICAR SE OS PINOS SÃO DE FATO PULLUP 
  //Se não forem PULLUP, a função getIndex deverá ser alterada retirando o
}

int detectaRobo(){
  int n = 0;
  int tempo_inicio = 0;
  int n1, n2, n3, n4 = 0;
  int maior = 0;
  int maior_n = 0;
  //Serial.println("Iniciando detecção de robô.");
  for(int n_tentativas = 0; n_tentativas < TENTATIVAS_ACHA_ROBO; n_tentativas++){
    tempo_inicio = millis();
    while(millis()-tempo_inicio < TEMPO_DETECCAO){
      n = (1*(!digitalRead(switch1))
      + 2*(!digitalRead(switch2))
      + 3*(!digitalRead(switch3))
      + 4*(!digitalRead(switch4)));
      if (n == 1 || n == 2 || n== 3 || n==4) break;
    }
    if(n==1) n1 += 1;
    if(n==2) n2 += 1;
    if(n==3) n3 += 1;
    if(n==4) n4 += 1;
  }
  int numeros[4] = {n1, n2, n3, n4};
  for(int i = 1; i <= 4; i++){
   // Serial.println(numeros[i-1]);
    if(numeros[i-1] > maior){
       maior = numeros[i-1];
       maior_n = i;
    }
  }
  Serial.print("Robô detectado: ");
  Serial.println(maior_n);
  return maior;
}

void loop() {
  //delay(5000);
  if (detect == 0) {
    broadcastIndex = detectaRobo();
    detect = 1;
  }
  lastBroadcastIndex = broadcastIndex;
  /*if (digitalRead(switch1) == 1 && digitalRead(switch2) == 1 && digitalRead(switch3) == 1 && digitalRead(switch4) == 1) broadcastIndex = 0;
  else if (digitalRead(switch1) == 1 && digitalRead(switch2) == 0 && digitalRead(switch3) == 1 && digitalRead(switch4) == 1) broadcastIndex = 1;
  else if (digitalRead(switch1) == 1 && digitalRead(switch2) == 1 && digitalRead(switch3) == 0 && digitalRead(switch4) == 1) broadcastIndex = 2;
  else if (digitalRead(switch1) == 1 && digitalRead(switch2) == 1 && digitalRead(switch3) == 1 && digitalRead(switch4) == 0) broadcastIndex = 3; */
 /* if (digitalRead(switch1) == 1 && digitalRead(switch2) == 0 && digitalRead(switch3) == 0 && digitalRead(switch4) == 0) broadcastIndex = 0;
  else if (digitalRead(switch1) == 0 && digitalRead(switch2) == 1 && digitalRead(switch3) == 0 && digitalRead(switch4) == 0) broadcastIndex = 1;
  else if (digitalRead(switch1) == 0 && digitalRead(switch2) == 0 && digitalRead(switch3) == 1 && digitalRead(switch4) == 0) broadcastIndex = 2;
  else if (digitalRead(switch1) == 0 && digitalRead(switch2) == 0 && digitalRead(switch3) == 0 && digitalRead(switch4) == 1) broadcastIndex = 3;
  else broadcastIndex = 0;*/
  broadcastIndex = (1*(!digitalRead(switch1))
  + 2*(!digitalRead(switch2))
  + 3*(!digitalRead(switch3))
  + 4*(!digitalRead(switch4)));
 // if (broadcastIndex != lastBroadcastIndex){
  //Serial.print("Dip Switch: ");
 // Serial.println(broadcastIndex);
  //}
}
