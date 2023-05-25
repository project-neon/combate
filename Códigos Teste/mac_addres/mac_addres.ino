// Código para a obtenção do MAC ADDRESS da ESP32 do robô
// que receberá o sinal do controle. 
#include <WiFi.h>

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}
 
void loop(){

}
