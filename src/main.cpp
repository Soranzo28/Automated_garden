//===== Inclusão das bibliotecas ===== 
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
//===== Fim Inclusão das bibliotecas ===== 

//===== Criação do objeto LCD ===== 
LiquidCrystal_I2C lcd(0x27, 16 ,2);
//===== Fim Criação do objeto LCD ===== 

//===== Declaração pinos =====
#define pin_sensor A1
#define pin_manual 2
#define pin_pausa 3
#define pin_led_irrigar 4
#define pin_led_manual 5
#define pin_led_pausa 6
#define pin_rele 7
#define pin_transistor 8
//===== Fim Declaração pinos =====

//=====  Declarações de funções  =====
void atualiza_lcd(int padrao);
int leitura_umidade();
void rega_manual();
void rega_auto(int umidade);
void control_pause();
void control_manual();
//===== Fim Declarações de funções =====

//===== Declarações de variáveis de controle =====
volatile bool pausado = 0, pausadoAnt = 0;
volatile bool manual = 0, manualAnt = 0;
bool irrigando = 0;
int umidade_bruta, umidade_porcentagem;
//===== Fim Declarações de variáveis de controle =====

//===== Declaração variáveis de calibração =====
const int quantia_leituras = 15;
const int delay_transistor = 500;
unsigned long tempo_ultima_leitura = 0;
unsigned long intervalo_leitura = 3600000; // 1 hora em ms
const int min_umidade = 40;
const int max_umidade = 80;
//===== Fim Declaração variáveis de calibração =====

void setup() {
  //===== Declaração das interrupções =====
  attachInterrupt(digitalPinToInterrupt(2), control_pause, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), control_manual, CHANGE);
  //===== Fim Declaração das interrupções =====

  //===== PinMode =====
  pinMode(pin_sensor, INPUT);
  pinMode(pin_manual, INPUT_PULLUP);
  pinMode(pin_pausa, INPUT_PULLUP);
  pinMode(pin_led_irrigar, OUTPUT);
  pinMode(pin_led_manual, OUTPUT);
  pinMode(pin_led_pausa, OUTPUT);
  pinMode(pin_rele, OUTPUT);
  pinMode(pin_transistor, OUTPUT); //Minimo 500ms de delay entre ativação e leitura
  //===== Fim PinMode =====

  //Iniciação serial
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Umidade bruta | Umidade porcentagem");
}

void loop() {
  unsigned long agora = millis();

  if (((long)(agora - tempo_ultima_leitura) >= intervalo_leitura) ) { //Checa o intervalo (ele muda quando está irrigando!)

    //===== Configuração leitura da umidade =====
    tempo_ultima_leitura = agora; // Atualiza o tempo da última leitura
    umidade_bruta = leitura_umidade(); // Chama a função
    umidade_porcentagem = map(umidade_bruta, 0, 1023, 100, 0); // Mapeia para porcentagem
    umidade_porcentagem = constrain(umidade_porcentagem, 0, 100); //Garante que o valor fique na faixa de 0 - 100%
    //===== Fim Configuração leitura da umidade =====

    //===== Printa no monitor serial para debug =====
    Serial.print(umidade_bruta);
    Serial.print("     |    ");
    Serial.println(umidade_porcentagem);
    //===== Fim Printa no monitor serial para debug =====

    if (!manual){rega_auto(umidade_porcentagem);} //Aciona a rega automatica apenas se a manual estiver desligada

  }
}

int leitura_umidade(){
  digitalWrite(pin_transistor, HIGH);
  delay(delay_transistor);
  unsigned long leituras = 0;
  for (int i = 0; i < quantia_leituras; i++){
    leituras += analogRead(pin_sensor);
    delay(10);
  }
  digitalWrite(pin_transistor, LOW);
  leituras /= quantia_leituras;
  return leituras;
}

void rega_auto(int umidade){

  if (!irrigando && umidade < min_umidade){
    digitalWrite(pin_rele, HIGH);
    irrigando = true;
    intervalo_leitura = 60000;
  }

  else if (irrigando && umidade >= max_umidade){
    digitalWrite(pin_rele, LOW);
    irrigando = false;
    intervalo_leitura = 3600000;
  }
}