//Bibliotecas auxiliares
#include <Wire.h>            // Para comunicação com o arduino, ESP12E e o RTC via i2c
#include <Adafruit_BMP085.h> // Para uso das funções do sensor de pressão BMP180
#include <DHT.h>             // Para uso das funções do sensor DHT22
#include <SPI.h>             // Para comunicação do ESP12E com o modulo microsd
#include <SD.h>              // Para uso das funções do datalloger

//Definição de objetos
Adafruit_BMP085 bmp180;      // Relativo ao sensor de pressão

//Mapeamento de harware
#define DHTPIN 0             // Pino 0 do esp
#define DHTTYPE DHT22        // Sensor DHT 22  (AM2302)
#define DS1307_ADDRESS 0x68  // Definição de endereço do RTC
byte zero = 0x00;
int pino_sensor = A0;        // Pino analogico do esp12e
int valor_sensor = 0;
byte UV_index = 0;
const int chipSelect = 15 ;  // Pino 15 do esp cartaosd
DHT dht(DHTPIN, DHTTYPE);    // Definicoes do sensor : pino, tipo

//Ajuste da variavel float para envio via I2c para o arduino
//Temperatura
byte Tbyte1, Tbyte2, Tbyte3, Tbyte4;
unsigned int aux;

//Umidade
byte Ubyte1, Ubyte2;
unsigned int aux2;

//Pressão
byte Pbyte1, Pbyte2;
unsigned int aux3;

//relogio
byte Hbyte1, Hbyte2, Hbyte3, Hbyte4, Hbyte5, Hbyte6;

//Variavel auxliar para recebimento das funções
float temperatura;
int indiceUv;
float umidade;
float patm;

//Variaveis para os dados do RTC

int se;                                    // Segundos
int mi;                                    // Minutos
int hs;                                    // Horas
int ds;                                    // Dias da semana
int dm;                                    // Dias do mes
int ms;                                    // Mes
int an;                                    // Ano

//Variaveis auxilio na temporização da gravação do datalloger
unsigned long tmpAnterior = 0;             // Variavel de controle do tempo
unsigned long interval_datalloger = 1000; // Tempo em ms do intervalo a ser executado

void setup()
{         
  pinMode(pino_sensor, INPUT);             // Configura o pino analogico do esp como entrada - Sensor UV
  Wire.begin(); //Configura a transmissão I2C
  //Serial.begin(115200);
  //SelecionaDataeHora();                 // Usado para setar data e hora do RTC DS1307  
  
  if (!bmp180.begin()) 
  {
    Serial.println("Sensor nao encontrado !!");
  }

  dht.begin();                             // Iniclaiza o sensor DHT 

  Serial.print("Inicializando cartão SD...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Cartão danificado ou não esta inserido");
    return;
  }
  Serial.println("Cartão inicializado");

  delay(2000); // Usado para dar tempo suficiente para a alimentação de todo o circuito 
}


void loop()
{
  int i;
  int nmedidas = 10;
  float mTmp = 0;
  int mUv = 0;
  float mPatm = 0;
  float mUmi = 0;

  unsigned long tmpAtual = millis(); // Tempo atual em ms

  for(i = 0 ; i < nmedidas ; i++)
  {
    mTmp += sensorTemperatura();
    mUv += sensorUv();    
    mPatm += sensorPressaoAtm();
    mUmi += sensorUmidade();

    Serial.println("Temperatura: " + (String)mTmp + " Umidade: " + (String)mUmi + " Pressao: " + (String)mPatm + " IndiceUv: " + (String)mUv);
  }

int teste;
  //Calculando a media
  temperatura = mTmp / nmedidas;
  teste = mUv / nmedidas;    
  patm = mPatm / nmedidas;
  umidade = mUmi / nmedidas;
  
  
  if(tmpAtual - tmpAnterior >= interval_datalloger)
  {
   tmpAnterior = tmpAtual;          // Salva o tempo atual
      
   dataLogger();
  }
/*
  Serial.println("DS1307");
  Serial.println(relogio());
  Serial.println("Temperatura");
  Serial.println(temperatura);
  Serial.println("Indice UV");
  Serial.println(indiceUv);
  Serial.println("Umidade");
  Serial.println(umidade);
  Serial.println("Pressão ATM");
  Serial.println(patm);
*/
  
  //Ajustando o número antes da vírgula para variáveis float positivas 
  
  //Temperatura
  //Ajustando o número antes da vírgula
  aux = (unsigned int) temperatura; // aux = 46689, Pega somente a parte inteira da variável float (0 - 65536)
  Tbyte1 = aux;                     // byte2 = 0B01100001, pega apenas os primeros 8 bits
    
  // Ajustando o número depois da vírgula
  temperatura -= aux;               // Deixa apenas o número depois da vírgula
  temperatura *= 100;               // Multiplica por 100 para pegar 2 dígitos após a vírgula
  aux = (unsigned int) temperatura; // Pega somente o valor antes da vírgula
  Tbyte2 = aux;                     // byte2 = 0B00101110, pega apenas os primeros 8 bits

//===========================================================================================================

  // Ajustando o número antes da vírgula para variáveis float positivas
  //Umidade
  aux2 = (unsigned int) umidade;    // Pega somente a parte inteira da variável float (0 - 65536)
  Ubyte1 = aux2;                    // byte2 = 0B01100001, pega apenas os primeros 8 bits
  
  // Ajustando o número depois da vírgula
  umidade -= aux2;                  // Deixa apenas o número depois da vírgula
  umidade *= 100;                   // Multiplica por 100 para pegar 2 dígitos após a vírgula
  aux2 = (unsigned int) umidade;    // Pega somente o valor antes da vírgula
  Ubyte2 = aux2;                    // byte2 = 0B00101110, pega apenas os primeros 8 bits

//===========================================================================================================

  // Ajustando o número antes da vírgula para variáveis float positivas
  //Pressão
  aux3 = (unsigned int) patm;       // aux = 46689, Pega somente a parte inteira da variável float (0 - 65536)
  Pbyte1 = aux3;                    // byte2 = 0B01100001, pega apenas os primeros 8 bits
  
  // Ajustando o número depois da vírgula
  patm -= aux3;                     // Deixa apenas o número depois da vírgula
  patm *= 100;                      // Multiplica por 100 para pegar 2 dígitos após a vírgula
  aux3 = (unsigned int) patm;       // Pega somente o valor antes da vírgula
  Pbyte2 = aux3;                    // byte2 = 0B00101110, pega apenas os primeros 8 bits

//===========================================================================================================

//Atribuindo o valor das horas, minutos e segundos a cada byte

  Hbyte1 = hs;                      //Horas
  Hbyte2 = mi;                      //Minutos
  Hbyte3 = se;                      //Segundos
  Hbyte4 = dm;                      //Dia
  Hbyte5 = ms;                      //Mês
  Hbyte6 = an;                      //Ano

//Envio dos dados gerados pelos sensores para o Arduino

  //temperatura
  Wire.beginTransmission(5);        // Começa transmissão para o escravo no endereço 5
  Wire.write(Tbyte1);               // Envia os bytes do número antes da vírgua
  Wire.write(Tbyte2);               // Envia os bytes do número depois da vírgua

  //IndiceUV
  Wire.write(teste);             // Envia os bytes do número

  //Umidade
  Wire.write(Ubyte1);               // Envia os bytes do número antes da vírgua    
  Wire.write(Ubyte2);               // Envia os bytes do número depois da vírgua

  //Pressão
  Wire.write(Pbyte1);               // Envia os bytes do número antes da vírgua     
  Wire.write(Pbyte2);               // Envia os bytes do número depois da vírgua

  //Data e hora
  Wire.write(Hbyte1);               // Envia os bytes do número
  Wire.write(Hbyte2);               // Envia os bytes do número
  Wire.write(Hbyte3);               // Envia os bytes do número
  Wire.write(Hbyte4);               // Envia os bytes do número
  Wire.write(Hbyte5);               // Envia os bytes do número
  Wire.write(Hbyte6);               // Envia os bytes do número
    
  Wire.endTransmission();           // Termina a transmissão para o Arduino
  delay(1000);                      // Tempo de atualização das informações enviadas para o Arduino
}

/*
float sensorPressao() 
{   
  return(bmp180.readPressure());
}
*/

float sensorPressaoAtm() 
{
  float p;
  
  p = bmp180.readPressure() / 101325.0; 
  //1atm == 101325
  // x   == bmp180.readPressure()

  return(p);
}
 
int sensorUv()
{
    
  valor_sensor = analogRead(pino_sensor);
  //Calcula tensao em milivolts
  int tensao = (valor_sensor * (5.0 / 1023.0)) * 1000;
  
  //Compara com valores tabela UV_Index
  if (tensao > 0 && tensao < 50)
  {
    UV_index = 0;
  }
  else if (tensao > 50 && tensao <= 227)
  {
    UV_index = 0;
  }
  else if (tensao > 227 && tensao <= 318)
  {
    UV_index = 1;
  }
  else if (tensao > 318 && tensao <= 408)
  {
    UV_index = 2;
  }
  else if (tensao > 408 && tensao <= 503)
  {
    UV_index = 3;
  }
  else if (tensao > 503 && tensao <= 606)
  {
    UV_index = 4;
  }
  else if (tensao > 606 && tensao <= 696)
  {
    UV_index = 5;
  }
  else if (tensao > 696 && tensao <= 795)
  {
    UV_index = 6;
  }
  else if (tensao > 795 && tensao <= 881)
  {
    UV_index = 7;
  }
  else if (tensao > 881 && tensao <= 976)
  {
    UV_index = 8;
  }
  else if (tensao > 976 && tensao <= 1079)
  {
    UV_index = 9;
  }
  else if (tensao > 1079 && tensao <= 1170)
  {
    UV_index = 10;
  }
  else if (tensao > 1170)
  {
    UV_index = 11;
  }

  return(UV_index);
}


float sensorTemperatura() 
{
  float sTemperatura = 0;
  
    // Leitura da temperatura (Celsius)
    sTemperatura = dht.readTemperature();
  
    // Verifica se o sensor esta respondendo
    /*while(isnan(h) || isnan(t))
    {
    Serial.println("Falha ao ler dados do sensor DHT !!!");
    }*/

  return(sTemperatura);
}

float sensorUmidade()
{
  float sUmidade = 0;

  //Leitura da umidade
  sUmidade = dht.readHumidity();

  return(sUmidade);
}

void SelecionaDataeHora()                 // Seta a data e a hora do DS1307
{
  byte segundos = 00;                     // Valores de 0 a 59
  byte minutos = 7;                      // Valores de 0 a 59
  byte horas = 21;                        // Valores de 0 a 23
  byte diadasemana = 0;                   // Valores de 0 a 6 - 0=Domingo, 1 = Segunda, etc.
  byte diadomes = 12;                      // Valores de 1 a 31
  byte mes = 8;                           // Valores de 1 a 12
  byte ano = 18;                          // Valores de 0 a 99
  Wire.beginTransmission(DS1307_ADDRESS); // Inicia a transmissção I2C para o endereço especificado
  Wire.write(zero);                       // Stop no CI para que o mesmo possa receber os dados

  //As linhas abaixo escrevem no CI os valores de 
  //data e hora que foram colocados nas variaveis acima
  Wire.write(ConverteParaBCD(segundos));
  Wire.write(ConverteParaBCD(minutos));
  Wire.write(ConverteParaBCD(horas));
  Wire.write(ConverteParaBCD(diadasemana));
  Wire.write(ConverteParaBCD(diadomes));
  Wire.write(ConverteParaBCD(mes));
  Wire.write(ConverteParaBCD(ano));
  Wire.write(zero);                       // Start no CI
  Wire.endTransmission();                 // Termina a transmissão para o DS1307
}

byte ConverteParaBCD(byte val){           //Converte o número de decimal para BCD
  return ( (val/10*16) + (val%10) );
}

byte ConverteparaDecimal(byte val)  {     // Converte de BCD para decimal
  return ( (val/16*10) + (val%16) );
}


String relogio()
{
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
  int segundos = ConverteparaDecimal(Wire.read());
  int minutos = ConverteparaDecimal(Wire.read());
  int horas = ConverteparaDecimal(Wire.read() & 0b111111); 
  int diadasemana = ConverteparaDecimal(Wire.read()); 
  int diadomes = ConverteparaDecimal(Wire.read());
  int mes = ConverteparaDecimal(Wire.read());
  int ano = ConverteparaDecimal(Wire.read());

  se = segundos;     
  mi = minutos;      
  hs = horas;        
  ds = diadasemana;  
  dm = diadomes;     
  ms = mes;          
  an = ano;          

  return((String)diadomes + "/" + (String)mes + "/" + (String)ano + ";" + (String)horas + ":" + (String)minutos + ":" + (String)segundos);
}

void dataLogger() {
    
  String linhaDeSaida = "";
  int cont = 0;

  if(cont == 0)
  {
    String cabecalho = "DMA;HMS;UMIDADE;TEMPERATURA;PRESSAO;INDICE UV";
    linhaDeSaida = cabecalho; //Cabeçalho
    cont++;
  }

  linhaDeSaida =  relogio() + ";" +(String)sensorUmidade() + ";" + (String)sensorTemperatura() + ";" + (String)sensorPressaoAtm() + ";" + (String)sensorUv();

  // Abre o arquivo Log.txt no cartão microsd
  File dataFile = SD.open("Log.csv", FILE_WRITE);

  // Verifica se o arquivo esta disponivel
  if (dataFile) {
    dataFile.println(linhaDeSaida); // Escreve no cartao microsd
    dataFile.close();               // Fecha o arquivo no cartão microsd
  }
  // Caso o arquivo Log.txt não esteja disponivel no cartão exibe a seguinte frase
  else {
    Serial.println("Erro ao abrir o arquivo Log.txt");
  }
}
