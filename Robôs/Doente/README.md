# **Doente**

Doente é um dos robôs de combate da categoria *Antweight* (454) da Project Neon. Esse robô é caracterizado pela sua alta velocidade e sua resistência, ganhando suas lutas devido à sua pilotagem e controle de luta. O Doente é composto por dois motores *brushless* que funcionam por meio de locomoção tangenciada dupla, de modo que cada um dos motores seja responsável por duas rodas.


<img src= "https://live.staticflickr.com/65535/52686287157_f60de71f6d_n.jpg" height = "320" width = "520" />

Atualmente, o Doente é controlado por meio de um controle de PS4, por meio de conexão *Bluetooth*. O robô pode ser controlado facilmente por meio dos seguintes comandos:

* **Analógico esquerdo**: controla a locomoção linear do robô, definindo a velocidade do robô e o sentido de locomoção;

* **Analógico direito**: controla a locomoção angular do robô, definindo a direção do robô e o ângulo de controle;

* **Seta para cima**: define o sentido de locomoção comum do robô, quando ele está virado para cima;

* **Seta para baixo**: define o sentido de locomoção invertido do robô, quando ele está virado para baixo.

Com base nesses comandos, é ~~im~~possível controlar o robô através do controle de PS4. Para realizar a configuração do controle de PS4, basta seguir as instruções apresentadas no `combate/README.md` do dirétorio raíz da equipe de combate, e utilizar o código apresentado no diretório `./Software/`.

# **Eletrônica**

O Doente é composto pelos seguintes componentes eletrônicos:

* ESP-32 WROOM32;

* Ponte H - TB6612FNG;

* 2 Motores Brushless;

* 2 Baterias LiPo 2S (Motores);

* 1  Bateria LiPo 1S (ESP32).
