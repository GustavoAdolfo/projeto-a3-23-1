#include <Servo.h>
#include <AFMotor.h>
#include <SoftwareSerial.h>

#define SERVO 10
#define TRIGGER A0
#define ECHO A1
#define LED_VERMELHO A4
#define LED_VERDE A5
// #define LED_AMARELO 2 

// MOTOR12_64KHZ, MOTOR12_8KHZ, MOTOR12_2KHZ, MOTOR12_1KHZ
AF_DCMotor m1(1);
AF_DCMotor m2(2);
AF_DCMotor m3(3);
AF_DCMotor m4(4);

Servo sm; //OBJETO DO TIPO SERVO

SoftwareSerial hc06(A2,A3); // BLUETOOTH

void DefinirVelocidade(uint8_t valor);
void DecidirMovimento();
bool CarroPodeSeguir();
byte EscolherDirecao();

void SeguirEmFrente();
void MarchaRe();
void CorrigirCurso();
void VirarEsquerda();
void VirarDireita();
void Parar();

const int VELOCIDADE_MAXIMA = 255;
const int VELOCIDADE_CURVA = 120;
const int VELOCIDADE_INICIAL = 100;
const int VELOCIDADE_MINIMA = 60;
const int PARADO = 0;
const uint8_t DIRECAO_RETA = 90;
const uint8_t CURVA_ESQUERDA = 180;
const uint8_t CURVA_DIREITA = 0;

uint8_t velocidade = 80;
bool tentou_direita = false;
bool tentou_esquerda = false;
char comando;
bool carro_parado;
uint8_t posicao;


void setup()
{
  Serial.begin(9600);
  // Serial.println("Iniciando robô");

  hc06.begin(9600);

  m1.setSpeed(VELOCIDADE_INICIAL);
  m1.run(RELEASE);

  m2.setSpeed(VELOCIDADE_INICIAL);
  m2.run(RELEASE);

  m3.setSpeed(VELOCIDADE_INICIAL);
  m3.run(RELEASE);

  m4.setSpeed(VELOCIDADE_INICIAL);
  m4.run(RELEASE);

  sm.attach(SERVO);
  sm.write(DIRECAO_RETA); //INICIA O MOTOR NA POSIÇÃO CORRIGIDA PARA 90º

  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  velocidade = VELOCIDADE_INICIAL;
  carro_parado = true;
  posicao = DIRECAO_RETA;

  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  // pinMode(LED_AMARELO, OUTPUT);
  
  digitalWrite(LED_VERMELHO, HIGH);  
}

void loop() 
{
  if (hc06.available()){
    comando = hc06.read();
  }

  if (!comando) {
    if (Serial.available() > 0) {
      comando = Serial.read();
    }
  }

  if (comando) {
    Serial.print("----------------------->>>>>> COMANDO RECEBIDO:");
    Serial.println(comando);

    switch (comando) {
      case 'W':
        Parar();
        delay(200);
        SeguirEmFrente();
      case 'F':
        DefinirVelocidade(velocidade+5);
        SeguirEmFrente();
        break;
      case 'B':
        DefinirVelocidade(velocidade-5);
        SeguirEmFrente();
        break;
      case 'X':
      case 'x':
        Parar();
        delay(200);
        MarchaRe();
        delay(200);
        Parar();
        break;
      case 'L':
        Parar();
        delay(200);
        VirarEsquerda();
        delay(250);
        Parar();
        delay(200);
        DefinirVelocidade(VELOCIDADE_INICIAL);
        SeguirEmFrente();
        break;
      case 'R':
        Parar();
        delay(200);
        VirarDireita();
        delay(250);
        Parar();
        delay(200);
        DefinirVelocidade(VELOCIDADE_INICIAL);
        SeguirEmFrente();
        break;
      case 'u':
      case 'U':
      case 'w':
      defaul:
        Parar();
        break;
    }
    comando = NULL;
  }
  
  if (!carro_parado) {
    bool pode_seguir = CarroPodeSeguir();    
    if (!pode_seguir) {
      Parar();
      delay(200);
      DecidirMovimento();
    }
  }

  // Serial.print("[004] -> POSICAO: ");
  // Serial.println(posicao);
  // Serial.println("[999] *** FIM DO LOOP ***\n");
}

/***************************************/

void SeguirEmFrente() {
  // Serial.println("**** Movendo o carro em velocidade normal...");
  digitalWrite(LED_VERDE, HIGH);
  digitalWrite(LED_VERMELHO, LOW);
  // digitalWrite(LED_AMARELO, LOW);

  m1.setSpeed(velocidade);
  m1.run(FORWARD);

  m2.setSpeed(velocidade);
  m2.run(FORWARD);

  m3.setSpeed(velocidade);
  m3.run(FORWARD);

  m4.setSpeed(velocidade);
  m4.run(FORWARD);

  carro_parado = false;    
}

void MarchaRe() {
  // Serial.println(" @ @ @ @ @ @ Em marcha ré...");
  carro_parado = false;
  digitalWrite(LED_VERMELHO, HIGH);
  // digitalWrite(LED_AMARELO, HIGH);
  // digitalWrite(LED_VERDE, LOW);

  DefinirVelocidade(VELOCIDADE_MINIMA);
  
  m1.setSpeed(velocidade);
  m1.run(BACKWARD);

  m2.setSpeed(velocidade);
  m2.run(BACKWARD);

  m3.setSpeed(velocidade);
  m3.run(BACKWARD);

  m4.setSpeed(velocidade);
  m4.run(BACKWARD);
  
  unsigned long tempo1 = millis();
  while(millis() - tempo1 < 1000) {}
}

void CorrigirCurso() {
  Serial.println("++++++++++++ Corrigindo curso para linha reta ++++++++");
  //virando aos poucos para não dar uma "pancada" no motor
  if (posicao > DIRECAO_RETA) {
    while(posicao > DIRECAO_RETA) {
      posicao--;
      sm.write(posicao);
      delay(20);
    }
    return;
  }

  if (posicao < DIRECAO_RETA) {
    while(posicao < DIRECAO_RETA) {
      posicao++;
      sm.write(posicao);
      delay(20);
    }
  }
}

void VirarEsquerda() {
  // Serial.println("===>Virando o servo motor para a ESQUERDA...");
  digitalWrite(LED_VERMELHO, HIGH);
  digitalWrite(LED_VERMELHO, LOW);
  // digitalWrite(LED_AMARELO, HIGH);

  DefinirVelocidade(VELOCIDADE_CURVA);
  
  m1.setSpeed(velocidade);
  m1.run(FORWARD);

  m2.setSpeed(velocidade);
  m2.run(BACKWARD);

  m3.setSpeed(velocidade);
  m3.run(BACKWARD);

  m4.setSpeed(velocidade);
  m4.run(FORWARD);
}

void VirarDireita() {
  // Serial.println("===>Virando o servo motor para a DIREITA...");
  digitalWrite(LED_VERMELHO, HIGH);
  digitalWrite(LED_VERMELHO, LOW);
  // digitalWrite(LED_AMARELO, HIGH);

  DefinirVelocidade(VELOCIDADE_CURVA);

  m1.setSpeed(velocidade);
  m1.run(BACKWARD);

  m2.setSpeed(velocidade);
  m2.run(FORWARD);

  m3.setSpeed(velocidade);
  m3.run(FORWARD);

  m4.setSpeed(velocidade);
  m4.run(BACKWARD);
}

void Parar() {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, HIGH);
  // digitalWrite(LED_AMARELO, LOW);

  m1.setSpeed(PARADO);
  m1.run(RELEASE);

  m2.setSpeed(PARADO);
  m2.run(RELEASE);

  m3.setSpeed(PARADO);
  m3.run(RELEASE);

  m4.setSpeed(PARADO);
  m4.run(RELEASE);

  carro_parado = true;

  Serial.print("[003] -> Carro Parado: ");
  Serial.println(carro_parado);
}

bool CarroPodeSeguir() {
  Serial.println("=>=>=> Verificar se o carro poder seguir na direção");
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER, HIGH);    //pulso de 10 microsegundos com base no fator 340m/s
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  
  long duracao = pulseIn(ECHO, HIGH);     //checando o tempo da distância nos 10 microsegundos
  int distanciaCm = duracao * 0.034 / 2; //convertendo a distância em centímetros
  
  // Serial.print("*** Distancia de um obstaculo: ");
  // Serial.print(distanciaCm);
  // Serial.println("cm");

  Serial.print("[002] -> Carro pode seguir em frente? ");
  Serial.println(distanciaCm > 60);
  return distanciaCm > 50; // DISTÂNCIA PARA COMPENSAR O LOOP E A VELOCIDADE
}

/**************************************/

void DecidirMovimento() {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  // digitalWrite(LED_AMARELO, HIGH);

  byte direcao = 0;
  Serial.print("***  (1) O carro está parado? ");
  Serial.println(carro_parado);

  if (carro_parado) {
    direcao = EscolherDirecao();
    Serial.print("$$$$$ Direcao escolhida: ");
    Serial.println(direcao);

    switch (direcao) {
      case 1:
        Serial.println("#### Testando a curva para a ESQUERDA");
        while (posicao < CURVA_ESQUERDA) {
          posicao+=5;
          sm.write(posicao);
          delay(20);
        }
        
        if (!CarroPodeSeguir()) {
          Parar();
          delay(200);
          CorrigirCurso();
          Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>> Carro não pode seguir para ESQUERDA");
          tentou_esquerda = true;
          DecidirMovimento();
        } else {
          VirarEsquerda();
          delay(200);
          Parar();
          delay(200);
          CorrigirCurso();
          SeguirEmFrente();
          tentou_esquerda = false;
        }
        break;

      case 2:
        Serial.println("%%%% Testando a curva para a DIREITA");
        while (posicao > CURVA_DIREITA) {
          posicao-=5;
          sm.write(posicao);
          delay(20);
        }
        if (!CarroPodeSeguir()) {
          Parar();
          delay(200);
          Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>> Carro não pode seguir para DIREITA");
          CorrigirCurso();
          tentou_direita = true;
          DecidirMovimento();
        } else {
          VirarDireita();
          delay(200);
          Parar();
          delay(200);
          CorrigirCurso();
          SeguirEmFrente();
          tentou_direita = false;
        }
        break;

      default:
        Serial.println("$$$$$$$ Tentando MARCHA RÉ para decidir nova direção");
        MarchaRe();
        Parar();
        delay(200);
        tentou_direita = false;
        tentou_esquerda = false;
        DecidirMovimento();
        break;
    }
  }
}

byte EscolherDirecao() {
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  // digitalWrite(LED_AMARELO, HIGH);
  
  if (tentou_direita && tentou_esquerda) {
    // Serial.println("**** **** **** JÁ TENTOU DIREITA E ESQUERDA ");
    return 0;
  }

  long aleatorio = random(0,2);

  if (aleatorio <= 1) {
    if (tentou_esquerda) {
      if (!tentou_direita) {
        return 2;
      } else {
        return 0;
      }
    } 
    return 1;
  }

  if (tentou_direita) {
    if (!tentou_esquerda) {
      return 1;
    } else {
      return 0;
    }
  }

  return 2;
}


void DefinirVelocidade(uint8_t valor) {
  if (valor > VELOCIDADE_MAXIMA) {
    velocidade = VELOCIDADE_MAXIMA;
    return;
  }

  if (valor < VELOCIDADE_MINIMA) {
    velocidade = VELOCIDADE_MINIMA;
    return;
  }

  velocidade = valor;

  Serial.print("[001] -> Velocidade mantida: ");
  Serial.println(velocidade);
}
