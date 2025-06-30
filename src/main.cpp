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
bool pausado = 0, pausadoAnt;
bool manual = 0, manualAnt;
bool irrigando;
//===== Fim Declarações de variáveis de controle =====

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

}

void loop() {

}
