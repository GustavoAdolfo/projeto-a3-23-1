#include <Servo.h>
#include <AFMotor.h>

#define VELOCIDADE_MAXIMA 255
#define MARCHALENTA 60
#define PARADO 0
#define SERVO 10
#define DIRECAO_RETA 90
#define CURVA_ESQUERDA 45
#define CURVA_DIREITA 135
#define TRIGGER A1
#define ECHO A0

AF_DCMotor motor1(1);//, MOTOR12_64KHZ);
AF_DCMotor motor2(2);//, MOTOR12_64KHZ);
AF_DCMotor motor3(3);
AF_DCMotor motor4(4);
Servo sm; //OBJETO DO TIPO SERVO

volatile bool carro_parado = false;
bool em_curva;
uint8_t posicao;
uint8_t velocidade = 180;
bool tentou_direita = false;
bool tentou_esquerda = false;

// int pos; //POSIÇÃO DO SERVO

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
  Serial.println("Iniciando robô");

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
  corrigir_curso();
  decidir_movimento();

  delay(2000);

  // parada_total();

  // delay(2000);

  // seguir_em_frente();

  // delay(5000);
  
  // digitalWrite(TRIGGER, HIGH);
  // delay(1);
  // digitalWrite(TRIGGER, LOW);

  // int duration, distance;
  // duration = pulseIn(ECHO, HIGH);
  // distance = duration * 0.034 / 2;

  // if (distance <= 30) {
  //   Serial.print("DISTANCIA ALCANÇADA NO LOOP: ");
  //   Serial.println(distance);
  //   PORTD = B00001100;      
    
  //   // parar o carro

  //   // decidir para onde virar
  //   
  //   switch (aleatorio) {
  //     case -1: // virar à esquerda
  //       // virando aos poucos para não dar uma "pancada" no motor
  //       for(pos = 0; pos > -180; pos = pos - 20) {
  //         
  //         delay(15);
  //       }
  //       delay(1000);
  //       // voltando pra frente
  //       for(pos = -180; pos < 0; pos = pos + 20) {
  //         s.write(pos);
  //         delay(15);
  //       }
  //       break;
  //     default: // virar à direita
  //       // virando aos poucos para não dar uma "pancada" no motor
  //       for(pos = 0; pos < 180; pos = pos + 20) {
  //         s.write(pos);
  //         delay(15);
  //       }
  //       delay(1000); //INTERVALO DE 1 SEGUNDO
  //       for(pos = 180; pos >= 0; pos = pos - 10){ //PARA "pos" IGUAL A 180, ENQUANTO "pos" MAIOR OU IGUAL QUE 0, DECREMENTA "pos"
  //         s.write(pos); //ESCREVE O VALOR DA POSIÇÃO QUE O SERVO DEVE GIRAR
  //         delay(15); //INTERVALO DE 15 MILISSEGUNDOS
  //       }
  //       break;
  // }
    
  // } else {
  //   PORTD = B00010000;
  // }

  // delay(50);
}

void decidir_movimento() {
  Serial.println("===>Decidir movimento");

  byte direcao = 0;
  bool pode_seguir = carro_pode_seguir();
  Serial.print("Carro pode seguir em frente? ")/
  Serial.println(pode_seguir);
  delay(200);

  Serial.print("(1) O carro está parado? ");
  Serial.println(carro_parado);
  delay(200);

  if (carro_parado || !pode_seguir) {
    direcao = escolher_direcao();
    Serial.print("$$$$$ Direcao escolhida: ");
    Serial.println(direcao);
    delay(300);
    switch (direcao) {
      case 1:
        if (!virar_esquerda()) {
          tentou_esquerda = true;
          break;
        }
        Serial.println("Fazendo a curva para a esquerda");
        Serial.println("%%%% Movendo o carro em baixa velocidade...");
        do {
          mover_velocidade_reduzida();
          posicao+=5;
          sm.write(posicao);
          delay(200);
        } while (posicao < DIRECAO_RETA);
        break;
      case 2:
        if (!virar_direita()) {
          tentou_direita = true;
          break;
        }
        Serial.println("Fazendo a curva para a direita");
        Serial.println("%%%% Movendo o carro em baixa velocidade...");
        do {
          mover_velocidade_reduzida();
          posicao-=5;
          sm.write(posicao);
          delay(100);
        } while (posicao > DIRECAO_RETA);
        break;
      default:
        Serial.println("     MARCHA RÉ    ");
        Serial.println("%%%% Movendo o carro em baixa velocidade...");
        marcha_re();
        tentou_direita = false;
        tentou_esquerda = false;
        break;
    }
    
    Serial.print("(2) O carro está parado? ");
    Serial.println(carro_parado);
    delay(200);
  }
  
  if (!carro_parado) {
    tentou_direita = false;
    tentou_esquerda = false;
    seguir_em_frente();
  }
}

byte escolher_direcao() {
  if (tentou_direita && tentou_esquerda) {
    Serial.println("**** **** **** JÁ TENTOU DIREITA E ESQUERDA ");
    return 0;
  }
  long aleatorio = random(0,2);
  Serial.print("===>Movimento aleatorio ");
  Serial.println(aleatorio);
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
  Serial.println("===>Verificar se o carro poder seguir na direção");
  digitalWrite(TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER, HIGH);    //pulso de 10 microsegundos com base no fator 340m/s
  delayMicroseconds(10);
  digitalWrite(TRIGGER, LOW);
  
  long duracao = pulseIn(ECHO, HIGH);     //checando o tempo da distância nos 10 microsegundos
  int distanciaCm = duracao * 0.034 / 2; //convertendo a distância em centímetros
  
  Serial.print("*** Distancia de um obstaculo: ");
  Serial.print(distanciaCm);
  Serial.println("cm");
  delay(200);

  if (distanciaCm <= 30)          // SE FOR MENOR OU IGUAL A 30 CM
  {
    parada_total();
    return false;
  }

  return true;
}

void marcha_re() {
  Serial.println(" @ @ @ @ @ @ Em marcha ré...");
  delay(300);

  if (em_curva) {
    parada_total();
    // voltando pra frente
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
  motor3.run(FORWARD);

  motor4.setSpeed(MARCHALENTA);
  motor4.run(FORWARD);

  carro_parado = false;
}

void seguir_em_frente() {
  Serial.println("**** Movendo o carro em velocidade normal...");
  
  if (em_curva) {
    parada_total();
    // voltando pra frente
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
  Serial.println("===> Corrigindo curso para linha reta");
  // virando aos poucos para não dar uma "pancada" no motor
  if (posicao > DIRECAO_RETA) {
    while(posicao > DIRECAO_RETA) {
      posicao--;
      // Serial.print("### POSICAO: ");
      // Serial.println(posicao);
      sm.write(posicao);
      delay(20);
    }
  } 
  if (posicao < DIRECAO_RETA) {
    while(posicao < DIRECAO_RETA) {
      posicao++;
      // Serial.print("### POSICAO: ");
      // Serial.println(posicao);
      sm.write(posicao);
      delay(20);
    }
  }
  
  em_curva = false;
}

bool virar_esquerda() {
  em_curva = true;

  Serial.println("===>Virando o servo motor para a ESQUERDA...");
  // virando aos poucos para não dar uma "pancada" no motor
  if (posicao > CURVA_ESQUERDA) {
    while(posicao > CURVA_ESQUERDA) {
      posicao-=5;
      // Serial.print("### POSICAO: ");
      // Serial.println(posicao);
      sm.write(posicao);
      delay(100);
    }
  }

  return carro_pode_seguir();
}

bool virar_direita() {
  em_curva = true;

  Serial.println("===>Virando o servo motor para a DIREITA...");
  // virando aos poucos para não dar uma "pancada" no motor
  if (posicao < CURVA_DIREITA) {
    while(posicao < CURVA_DIREITA) {
      posicao+=5;
      // Serial.print("### POSICAO: ");
      // Serial.println(posicao);
      sm.write(posicao);
      delay(100);
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