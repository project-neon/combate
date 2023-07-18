#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


//---------------- PINOS DO DIP SWITCH ---------------//
#define switch1 34 
#define switch2 35
#define switch3 32
#define switch4 33
#define connect_button 12

#define TEMPO_DETECCAO 100
#define TENTATIVAS_ACHA_ROBO 20
#define DETECT_AGAIN 2000

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


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

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.clearDisplay();
  delay(2000); // Pause for 2 seconds
  display.setTextSize(1);         // set text size
  display.setTextColor(WHITE);    // set text color
  display.setCursor(0, 10);       // set position to display
  display.println("Selecao de Robos:");
  display.display();

}

int detectaRobo(){
  display.clearDisplay();
  display.setCursor(0, 10);
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
  Serial.println(n4);
  int numeros[5] = {n0 , n1, n2, n3, n4};
  for(int i = 0; i <= 4; i++){
    if(numeros[i] > maior){
       maior = numeros[i];
       maior_n = i;
    }
  }
  if (n_error > maior){
    maior_n = 5;
    display.println("Selecao Invalida");
  }
  else{
    if (maior_n != 0){
      display.print("Robo detectado: \t ");
    }
    else if(maior_n == 1){
      display.print("1. Cirrose ");
    }
    }

    else{
      display.println("Nenhum robo selecionado");
    }
  }
  display.display();

  return maior_n;
}




void loop() {
  //delay(5000);
  delay(2000);
 // if (detect == 0) {
 // if(digitalRead(connect_button == 0)) {
    broadcastIndex = detectaRobo();
    detect = 1 ;
 //}
  delay(2500);
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("----");
  display.display();
  delay(2000);
 /* if ((digitalRead(connect_button == HIGH)) && (detect == 1) && (millis() - start_time > 2000))
  {
    broadcastIndex = detectaRobo();
    detect = 0;
  }*/
  //Serial.println(digitalRead(connect_button));

}
