#include <Servo.h>
#include <AFMotor.h>

#define VELOCIDADE_MAXIMA 180
#define MARCHALENTA 30
#define PARADO 0
#define SERVO 10
#define DIRECAO_RETA 90
#define CURVA_ESQUERDA 45
#define CURVA_DIREITA 135
#define TRIGGER A1
#define ECHO A0

AF_DCMotor motor1(1, MOTOR12_64KHZ);
AF_DCMotor motor2(2, MOTOR12_64KHZ);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);
Servo sm; //OBJETO DO TIPO SERVO

volatile bool carro_parado = false;
bool em_curva;
uint8_t posicao;
uint8_t velocidade = 10;
bool tentou_direita = false;
bool tentou_esquerda = false;
char comando;

void definir_velocidade(uint8_t valor);
void seguir_em_frente();
void mover_velocidade_reduzida();
void corrigir_curso();
void marcha_re();
bool virar_direita();
bool virar_esquerda();
void parada_total();
void decidir_movimento();
bool carro_pode_seguir();
byte escolher_direcao();


void setup()
{
  Serial.begin(9600);
  // Serial.println("Iniciando robô");

  motor1.setSpeed(MARCHALENTA);
  motor1.run(RELEASE);

  motor2.setSpeed(MARCHALENTA);
  motor2.run(RELEASE);

  motor3.setSpeed(MARCHALENTA);
  motor3.run(RELEASE);

  motor4.setSpeed(MARCHALENTA);
  motor4.run(RELEASE);

  posicao = DIRECAO_RETA;
  sm.attach(SERVO);
  sm.write(posicao); //INICIA O MOTOR NA POSIÇÃO CORRIGIDA PARA 90º

  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
}

void loop() 
{
  if (Serial.available() > 0) {
    comando = Serial.read();
  }
  switch (comando) {
    case 'F':
      definir_velocidade(velocidade+10);
      break;
    case 'B':
      definir_velocidade(velocidade-10);
      break;
    defaul:
      break;
  }

  corrigir_curso();
  decidir_movimento();
}

void decidir_movimento() {
  byte direcao = 0;
  bool pode_seguir = carro_pode_seguir();
  // Serial.print("Carro pode seguir em frente? ")/
  // Serial.println(pode_seguir);
  // delay(200);

  // Serial.print("(1) O carro está parado? ");
  // Serial.println(carro_parado);
  // delay(200);

  if (carro_parado || !pode_seguir) {
    direcao = escolher_direcao();
    // Serial.print("$$$$$ Direcao escolhida: ");
    // Serial.println(direcao);
    // delay(300);
    switch (direcao) {
      case 1:
        if (!virar_esquerda()) {
          tentou_esquerda = true;
          break;
        }
        // Serial.println("Fazendo a curva para a esquerda");
        // Serial.println("%%%% Movendo o carro em baixa velocidade...");
        do {
          mover_velocidade_reduzida();
          posicao+=5;
          sm.write(posicao);
          // delay(200);
        } while (posicao < DIRECAO_RETA);
        break;
      case 2:
        if (!virar_direita()) {
          tentou_direita = true;
          break;
        }
        // Serial.println("Fazendo a curva para a direita");
        // Serial.println("%%%% Movendo o carro em baixa velocidade...");
        do {
          mover_velocidade_reduzida();
          posicao-=5;
          sm.write(posicao);
          // delay(100);
        } while (posicao > DIRECAO_RETA);
        break;
      default:
        // Serial.println("     MARCHA RÉ    ");
        // Serial.println("%%%% Movendo o carro em baixa velocidade...");
        marcha_re();
        tentou_direita = false;
        tentou_esquerda = false;
        break;
    }
    
    // Serial.print("(2) O carro está parado? ");
    // Serial.println(carro_parado);
    // delay(200);
  }
  
  if (!carro_parado) {
    tentou_direita = false;
    tentou_esquerda = false;
    seguir_em_frente();
  }
}

byte escolher_direcao() {
  if (tentou_direita && tentou_esquerda) {
    // Serial.println("**** **** **** JÁ TENTOU DIREITA E ESQUERDA ");
    return 0;
  }
  long aleatorio = random(0,2);
  // Serial.print("===>Movimento aleatorio ");
  // Serial.println(aleatorio);
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

bool carro_pode_seguir() {
  // Verificar se o carro poder seguir na direção
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
  // delay(200);

  if (distanciaCm <= 30)          // SE FOR MENOR OU IGUAL A 30 CM
  {
    parada_total();
    return false;
  }

  return true;
}

void marcha_re() {
  // Serial.println(" @ @ @ @ @ @ Em marcha ré...");
  // delay(300);

  if (em_curva) {
    parada_total();
    corrigir_curso();
  }

  carro_parado = false;
  
  motor1.setSpeed(MARCHALENTA);
  motor1.run(BACKWARD);

  motor2.setSpeed(MARCHALENTA);
  motor2.run(BACKWARD);

  motor3.setSpeed(MARCHALENTA);
  motor3.run(FORWARD);

  motor4.setSpeed(MARCHALENTA);
  motor4.run(FORWARD);
  
  unsigned long tempo1 = millis();
  while(millis() - tempo1 < 5000) {
    // marcha ré
  }
  parada_total();
}

void parada_total() {    
  motor1.setSpeed(PARADO);
  motor1.run(RELEASE);

  motor2.setSpeed(PARADO);
  motor2.run(RELEASE);

  motor3.setSpeed(PARADO);
  motor3.run(RELEASE);

  motor4.setSpeed(PARADO);
  motor4.run(RELEASE);

  carro_parado = true; 
}

void mover_velocidade_reduzida() {  
  motor1.setSpeed(MARCHALENTA);
  motor1.run(FORWARD);

  motor2.setSpeed(MARCHALENTA);
  motor2.run(FORWARD);

  motor3.setSpeed(MARCHALENTA);
  motor3.run(BACKWARD);

  motor4.setSpeed(MARCHALENTA);
  motor4.run(BACKWARD);

  carro_parado = false;
}

void seguir_em_frente() {
  // Serial.println("**** Movendo o carro em velocidade normal...");
  
  if (em_curva) {
    parada_total();
    corrigir_curso();
  }
  
  motor1.setSpeed(velocidade);
  motor1.run(FORWARD);

  motor2.setSpeed(velocidade);
  motor2.run(FORWARD);

  motor3.setSpeed(velocidade);
  motor3.run(FORWARD);

  motor4.setSpeed(velocidade);
  motor4.run(FORWARD);

  carro_parado = false;
}

void corrigir_curso() {
  // Corrigindo curso para linha reta
  // virando aos poucos para não dar uma "pancada" no motor
  if (posicao > DIRECAO_RETA) {
    while(posicao > DIRECAO_RETA) {
      posicao--;
      // Serial.print("### POSICAO: ");
      // Serial.println(posicao);
      sm.write(posicao);
      delay(10);
    }
  } 
  if (posicao < DIRECAO_RETA) {
    while(posicao < DIRECAO_RETA) {
      posicao++;
      // Serial.print("### POSICAO: ");
      // Serial.println(posicao);
      sm.write(posicao);
      delay(10);
    }
  }
  
  em_curva = false;
}

bool virar_esquerda() {
  // Serial.println("===>Virando o servo motor para a ESQUERDA...");
  
  em_curva = true;
  // virando aos poucos para não dar uma "pancada" no motor
  if (posicao > CURVA_ESQUERDA) {
    while(posicao > CURVA_ESQUERDA) {
      posicao-=5;
      sm.write(posicao);
      delay(10);
    }
  }

  return carro_pode_seguir();
}

bool virar_direita() {
  // Serial.println("===>Virando o servo motor para a DIREITA...");

  em_curva = true;
  // virando aos poucos para não dar uma "pancada" no motor
  if (posicao < CURVA_DIREITA) {
    while(posicao < CURVA_DIREITA) {
      posicao+=5;
      sm.write(posicao);
      delay(10);
    }
  }

  return carro_pode_seguir();
}

void definir_velocidade(uint8_t valor) {
  if (valor > VELOCIDADE_MAXIMA) {
    velocidade = VELOCIDADE_MAXIMA;
    return;
  }
  velocidade = valor;
}