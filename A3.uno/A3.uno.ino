#include <Servo.h>
#include <AFMotor.h>

#define SERVO 10
#define DIRECAO_RETA 90
#define CURVA_ESQUERDA 180
#define CURVA_DIREITA 0
#define TRIGGER A1
#define ECHO A0

// MOTOR12_64KHZ, MOTOR12_8KHZ, MOTOR12_2KHZ, MOTOR12_1KHZ.
AF_DCMotor m1(1);
AF_DCMotor m2(2);
AF_DCMotor m3(3);
AF_DCMotor m4(4);

Servo sm; //OBJETO DO TIPO SERVO

uint8_t velocidade = 50;
bool tentou_direita = false;
bool tentou_esquerda = false;
char comando;

void definir_velocidade(uint8_t valor);
void decidir_movimento();
bool carro_pode_seguir();
byte escolher_direcao();

void SeguirEmFrente();
void MarchaRe();
void CorrigirCurso();
void VirarEsquerda();
void VirarDireita();
void Parar();
void ReduzirVelocidade();

const int VELOCIDADE_MAXIMA = 255;
const int VELOCIDADE_CURVA = 160;
const int VELOCIDADE_INICIAL = 80;
const int VELOCIDADE_MINIMA = 50;
const int PARADO = 0;

bool carro_parado;
uint8_t posicao;


void setup()
{
  Serial.begin(9600);
  // Serial.println("Iniciando robô");

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

  pinMode(2, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A4, OUTPUT);
  
  digitalWrite(A2, HIGH);
  
}

void loop() 
{
  if (Serial.available() > 0) {
    comando = Serial.read();
  }
  Serial.print("[001] -> Velocidade mantida: ");
  Serial.println(velocidade);
  
  switch (comando) {
    case 'S':
    case 'w':
      Parar();
      definir_velocidade(VELOCIDADE_INICIAL);
      break;
    case 'F':
      definir_velocidade(velocidade+5);
      SeguirEmFrente();
      break;
    case 'B':
      definir_velocidade(velocidade-5);
      SeguirEmFrente();
      break;
    case 'W':
      definir_velocidade(VELOCIDADE_INICIAL);
      SeguirEmFrente();
    defaul:
      break;
  }

  if (!carro_parado) {
    bool pode_seguir = carro_pode_seguir();
    Serial.print("[002] -> Carro pode seguir em frente? ");
    Serial.println(pode_seguir);
    if (!pode_seguir) {
      Parar();
      Serial.print("[003] -> Carro Parado: ");
      Serial.println(carro_parado);
      decidir_movimento();
    }
  }

  Serial.print("[004] -> POSICAO: ");
  Serial.println(posicao);
  Serial.println("[999] *** FIM DO LOOP ***\n");
}

/***************************************/

void SeguirEmFrente() {
  // Serial.println("**** Movendo o carro em velocidade normal...");
  digitalWrite(2, HIGH);
  digitalWrite(A2, LOW);
  digitalWrite(A4, LOW);

  m1.setSpeed(velocidade);
  m1.run(BACKWARD);

  m2.setSpeed(velocidade);
  m2.run(BACKWARD);

  m3.setSpeed(velocidade);
  m3.run(BACKWARD);

  m4.setSpeed(velocidade);
  m4.run(BACKWARD);

  carro_parado = false;    
}

void MarchaRe() {
  // Serial.println(" @ @ @ @ @ @ Em marcha ré...");
  carro_parado = false;
  digitalWrite(2, LOW);
  
  m1.setSpeed(velocidade);
  m1.run(FORWARD);

  m2.setSpeed(velocidade);
  m2.run(FORWARD);

  m3.setSpeed(velocidade);
  m3.run(FORWARD);

  m4.setSpeed(velocidade);
  m4.run(FORWARD);
  
  unsigned long tempo1 = millis();
  while(millis() - tempo1 < 1000) {
    // apenas para andar em marcha ré
    digitalWrite(A2, HIGH);
    digitalWrite(A4, LOW);
    delay(100);
    digitalWrite(A2, LOW);
    digitalWrite(A4, HIGH);
    delay(100);
  }
}

void CorrigirCurso() {
  // Serial.println("++++++++++++ Corrigindo curso para linha reta ++++++++");
  // virando aos poucos para não dar uma "pancada" no motor
  if (posicao > DIRECAO_RETA) {
    while(posicao > DIRECAO_RETA) {
      posicao--;
      sm.write(posicao);
      delay(20);
    }
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
  Serial.println("===>Virando o servo motor para a ESQUERDA...");
  digitalWrite(2, LOW);
  digitalWrite(A2, HIGH);
  digitalWrite(A4, HIGH);
  
  m1.setSpeed(VELOCIDADE_CURVA);
  m1.run(FORWARD);

  m2.setSpeed(VELOCIDADE_CURVA);
  m2.run(FORWARD);

  m3.setSpeed(VELOCIDADE_CURVA);
  m3.run(BACKWARD);

  m4.setSpeed(VELOCIDADE_CURVA);
  m4.run(BACKWARD);
  
  delay(100);
}

void VirarDireita() {
  Serial.println("===>Virando o servo motor para a DIREITA...");
  digitalWrite(2, LOW);
  digitalWrite(A2, HIGH);
  digitalWrite(A4, HIGH);

  m1.setSpeed(VELOCIDADE_CURVA);
  m1.run(BACKWARD);

  m2.setSpeed(VELOCIDADE_CURVA);
  m2.run(BACKWARD);

  m3.setSpeed(VELOCIDADE_CURVA);
  m3.run(FORWARD);

  m4.setSpeed(VELOCIDADE_CURVA);
  m4.run(FORWARD);
  
  delay(100);
}

void Parar() {
  digitalWrite(2, LOW);
  digitalWrite(A2, HIGH);
  digitalWrite(A4, LOW);

  m1.setSpeed(PARADO);
  m1.run(RELEASE);

  m2.setSpeed(PARADO);
  m2.run(RELEASE);

  m3.setSpeed(PARADO);
  m3.run(RELEASE);

  m4.setSpeed(PARADO);
  m4.run(RELEASE);

  carro_parado = true;
}

void ReduzirVelocidade() {
  m1.setSpeed(VELOCIDADE_MINIMA);
  m1.run(FORWARD);

  m2.setSpeed(VELOCIDADE_MINIMA);
  m2.run(FORWARD);

  m3.setSpeed(VELOCIDADE_MINIMA);
  m3.run(FORWARD);

  m4.setSpeed(VELOCIDADE_MINIMA);
  m4.run(BACKWARD);

  carro_parado = false;
}

bool carro_pode_seguir() {
  // Serial.println("=>=>=> Verificar se o carro poder seguir na direção");
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

  if (distanciaCm <= 40) // SE FOR MENOR OU IGUAL A 40 CM (tamanho aproximado do carro atualmente)
  {
    Parar();
    return false;
  }

  return true;
}

/**************************************/

void decidir_movimento() {
  digitalWrite(2, HIGH);
  digitalWrite(A2, LOW);
  digitalWrite(A4, HIGH);

  byte direcao = 0;
  // Serial.print("***  (1) O carro está parado? ");
  // Serial.println(carro_parado);

  if (carro_parado) {
    direcao = escolher_direcao();
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
        
        if (!carro_pode_seguir()) {
          Parar();
          CorrigirCurso();
          Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>> Carro não pode seguir para ESQUERDA");
          tentou_esquerda = true;
          decidir_movimento();
          break;
        } else {
          VirarEsquerda();
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
        if (!carro_pode_seguir()) {
          Parar();
          Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>> Carro não pode seguir para DIREITA");
          CorrigirCurso();
          tentou_direita = true;
          decidir_movimento();
          break;
        } else {
          VirarDireita();
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
        decidir_movimento();
        break;
    }
  }
  
  Serial.print("*** (2) O carro está parado? ");
  Serial.println(carro_parado);
  delay(500);
}

byte escolher_direcao() {
  digitalWrite(2, HIGH);
  digitalWrite(A2, LOW);
  digitalWrite(A4, HIGH);
  
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


void definir_velocidade(uint8_t valor) {
  if (valor > VELOCIDADE_MAXIMA) {
    velocidade = VELOCIDADE_MAXIMA;
    return;
  }

  if (valor < VELOCIDADE_MINIMA) {
    velocidade = VELOCIDADE_MINIMA;
    return;
  }
  velocidade = valor;
}
