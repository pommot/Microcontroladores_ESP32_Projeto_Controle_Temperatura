#include <dummy.h>
#include <LiquidCrystal.h> //Carrega a biblioteca LCD
#define BLYNK_PRINT Serial // Mensagens Blynk serão printadas na porta monitor serial
#include <OneWire.h> //controle de dados do sensor
#include <DallasTemperature.h> //biblioteca do sensor
#include <WiFi.h> //biblioteca wifi esp32
#include <BlynkSimpleEsp32.h> //Blynk App esp32

//Define os pinos de controle do LCD 
LiquidCrystal lcd(19, 18, 05, 17, 16, 04);

//cria vetor caracter grau º exibido no display LCD
byte grau[8] = { B00001100,
                 B00010010,
                 B00010010,
                 B00001100,
                 B00000000,
                 B00000000,
                 B00000000,
                 B00000000,
               };

// define pino GPIO23 que será conectado ao DS18B20
const int oneWireBus = 23;

// Objeto que tratará da troca de dados com o sensor DS18B20
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);
//Timer Blynk para temporizar as requisições de temperatura
BlynkTimer timer;

//variavel de autenticação com Blynk
char auth[] = "Ns3_Tph-ZRVIzNwz8dSZYcoqwc3h5im9";
//variavel de autenticação wi-fi
char ssid[] = "Andrenet2G";
char pass[] = "maia0481";

//led interno servira como alarme de temperatura
const int LED_BUILTIN = 2;
// define pino 22 para  0 acionamento do RELE 0
const int RELE = 22;
// define pino 21 para  0 acionamento do RELE 1
const int RELE1 = 21;

//Rotina de leitura do sensor de temperatura para o Blynk
void getTemperature()
{
  //Colete informações de temperatura
  sensors.requestTemperatures();

  //Obtenha a temperatura em graus Celsius
  float temperatureC = sensors.getTempCByIndex(0);

  //Atualize variaveis Blynk com os valores de temperatura
  Blynk.virtualWrite(V1, temperatureC);

  //Criar um Widget LED para alarme de temperatura no Blynk na porta vitual V2
  WidgetLED led1(V2);
  //Criar um Widget LED para temperatura normal no Blynk na porta vitual V3
  WidgetLED led2(V3);
  //condicional de temperatura alta
  if (temperatureC >= 32)
  {
    //liga led alarme Blynk
    led1.on();
    //desliga  led normal Blynk
    led2.off();
    //Envia email de alarme de temperatura
    Blynk.email("Alarme de temperatura alta", temperatureC);
  }
  else
  {
    //desliga led alarme
    led1.off();
    //liga led normal
    led2.on();
  }
}

void setup() {
  //Inicia o LED interno
  pinMode(RELE, OUTPUT);
  //Define pino RELE 0 como controle de saida
  pinMode(RELE1, OUTPUT);
  //Define pino RELE 1 como controle de saida
  pinMode(LED_BUILTIN, OUTPUT);
  //inicia a comunicação serial 
  Serial.begin(115200);
  //Inicie o serviço de autenticação com ervidor Blynk
  Blynk.begin(auth, ssid, pass);
  // inicia o sensor DS18B20
  sensors.begin();
  //Configure o timer para fazer a leitura de temperatura a cada 2 segundos
  timer.setInterval(2000L, getTemperature);
  lcd.begin(16, 2); //Inicializa o Display LCD.
  lcd.clear(); //Limpa o Display LCD.
  //Cria o Caractere Customizado com o Símbolo do °.
  lcd.createChar(0, grau);

}

void loop()
{
  //Inicia as rotinas do Blynk IOT
  Blynk.run();
  //Atualiza as rotinas do Timer
  timer.run();
  //Coleae informações de temperatura
  sensors.requestTemperatures();
  //Obtem a temperatura em graus Celsius
  float temperatureC = sensors.getTempCByIndex(0);
  if (temperatureC >= 32)
  {
    lcd.setCursor(0, 0);
    lcd.print("Temp : ");
    lcd.print(" ");
    lcd.setCursor(7, 0);
    lcd.print(temperatureC, 1);
    lcd.setCursor(12, 0);
    //Exibe o Símbolo do ° no Display LCD.
    lcd.write((byte)0);

    lcd.setCursor(0, 1);
    lcd.print("Temperatura Alta");

    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(RELE, LOW);
    digitalWrite(RELE1, LOW);

  }
  else
  {

    lcd.setCursor(0, 0);
    lcd.print("Temp : ");
    lcd.print(" ");
    lcd.setCursor(7, 0);
    lcd.print(temperatureC, 1);
    lcd.setCursor(12, 0);
    //Exibe o Símbolo do ° no Display LCD.
    lcd.write((byte)0);
    lcd.setCursor(0, 1);
    lcd.print("Temperatura Normal");

    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(RELE, HIGH);
    digitalWrite(RELE1, HIGH);

  }
  if (temperatureC < 32)
  { 
    digitalWrite(RELE, LOW);
    digitalWrite(RELE1, HIGH);
    delay(15000);

    digitalWrite(RELE, HIGH);
    digitalWrite(RELE1, LOW);
    delay(15000);
  }
}
