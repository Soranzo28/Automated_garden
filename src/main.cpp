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
void atualiza_lcd();
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
int umidade_bruta, umidade_porcentagem, display = 0;
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

  if (pausado && !pausadoAnt){
    pausadoAnt = pausado;
    if (manual){
      manual = 0;
      manualAnt = manual; //Atualiza a variavel
      digitalWrite(pin_led_manual, LOW); //Desliga o LED do modo manual
      intervalo_leitura = 3600000; //Quando desliga o manual, volta pra uma hora de intervalo
    }
    if (irrigando){
      irrigando = false;
    }
    digitalWrite(pin_rele, LOW); //Desliga o rele
    digitalWrite(pin_led_irrigar, LOW); //Desliga o LED da irrigação
    digitalWrite(pin_led_pausa, HIGH);
    display = 3;
    atualiza_lcd();
  }

  if (!pausado && pausadoAnt){
    pausadoAnt = pausado;
    digitalWrite(pin_led_pausa, LOW);
    display = 0;
    rega_auto(umidade_porcentagem);
  }

  if (manual && !manualAnt){
    manualAnt = manual; //Atualiza a variavel
    display = 2; //Troca o display do LCD pra mostrar que está no modo manual
    if (!digitalRead(pin_rele)){ //Se o rele não estava ativo, ativa
      digitalWrite(pin_rele, HIGH);
    }
    if (!digitalRead(pin_led_irrigar)){ //Se o led não estava ligado, liga
      digitalWrite(pin_led_irrigar, HIGH);
    }
    irrigando = 1; //Troca a variavel de controle
    digitalWrite(pin_led_manual, HIGH); //Liga o led manual
    intervalo_leitura = 60000; //Troca o intervalo para 1m
    atualiza_lcd();
  }

  if (!manual && manualAnt){
    manualAnt = manual; //Atualiza a variavel
    digitalWrite(pin_rele, LOW); //Desliga o rele
    digitalWrite(pin_led_irrigar, LOW); //Desliga o LED da irrigação
    irrigando = 0; //Troca a variavel de controle
    display = 0; //Volta o display pro modo padrão
    digitalWrite(pin_led_manual, LOW); //Desliga o LED do modo manual
    intervalo_leitura = 3600000; //Quando desliga o manual, volta pra uma hora de intervalo
    atualiza_lcd();
  }

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
    atualiza_lcd();
  }
}

void control_pause(){
  pausado = !pausado;
}

void control_manual(){
  if (!pausado){ //Apenas troca o modo caso a pausa esteja desativada
    /*
    O SWITCH está aqui pois eu troco a variavel MANUAL via software, o que faz ela deixar de ser exclusiva do interruptor físico
    a função PAUSE desativa a manual, porém fisicamente o interruptor pode continuar na posição ativada, e sem esse tratamento,
    ao desliga-lo, ele ativaria o modo manual, invertendo suas posições de ligado e desligado. Deste modo, mesmo que ele fique na posição
    ligada fiscamente, ao desliga-lo, nada muda, o modo manual continuará desativado.
    */
    switch(digitalRead(pin_manual)){
      case LOW:
        manual = 1;
        break;
      case HIGH:
        manual = 0;
        break;
    }
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
    intervalo_leitura = 60000; //Se não tava irrigando e começou a irrigar, ele troca a leitura de umidade para cada minuto
    display = 1;
    digitalWrite(pin_led_irrigar, HIGH);
  }

  else if (irrigando && umidade >= max_umidade){
    digitalWrite(pin_rele, LOW);
    irrigando = false;
    intervalo_leitura = 3600000; //Quando desliga a irrigação, volta pra uma hora de intervalo
    display = 0;
    digitalWrite(pin_led_irrigar, LOW);
  }
  atualiza_lcd();
}

void atualiza_lcd(){
  lcd.setCursor(2, 0);
  lcd.print("Smart Garden");
  lcd.setCursor(0,1);
  lcd.print("                "); //Limpa a segunda linha

  switch(display){
    case 0: //Caso base
      lcd.setCursor(2, 1);
      lcd.print("Umidade: ");
      lcd.print(umidade_porcentagem);
      lcd.print("%");
      break;

    case 1: //Irrigando
      lcd.setCursor(0, 1);
      lcd.print("Umidade: ");
      lcd.print(umidade_porcentagem);
      lcd.print("% | I");
      break;

    case 2: //Modo manual
      lcd.setCursor(0, 1);
      lcd.print("Umidade: ");
      lcd.print(umidade_porcentagem);
      lcd.print("% | M");
      break;

    case 3: //Pausado
      lcd.setCursor(0, 1);
      lcd.print("Umidade: ");
      lcd.print(umidade_porcentagem);
      lcd.print("% | P");
      break;
  }
}