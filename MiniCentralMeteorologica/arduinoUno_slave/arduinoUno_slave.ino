#include <Wire.h>                //Inclui a biblioteca I2C
#include <LiquidCrystal.h>

//============================================================================================================//
// --- Mapeamento de Hardware ---
#define butUp    12                                     
#define butDown  11                                     
#define butLeft  10                                     
#define butRight  9                                     
#define butEnter  8                                     
#define Lamp1    A0                                     
#define Lamp2    A1                                     

//============================================================================================================//
// --- Prototipo das funções Auxiliares ---
void changeMenu();                                      
void dispMenu();                                        
void data_hora();                                       
void painelSensores();                                     


void readEnter(char option);
void acionamentoSaidas();
void dispSubMenuAcionamentoSaidas();
void subMenuSaidas_temperatura();
void subMenuSaidas_umidade();
void subMenuSaidas_pressao();
void subMenuSaidas_indiceUv();

byte caracter = 0x00;


// --- Variaveis Globais ---
char menu = 0x01;                                                  
char subMenu3 = 0x01;
unsigned int opc = 0;                                     
boolean t_butUp, t_butDown, t_butLeft, t_butRight, t_butEnter;     

// --- Hardware do LCD ---
LiquidCrystal disp(7,  //RS no digital 7
                   6,  //EN no digital 6
                   5,  //D4 no digital 5
                   4,  //D5 no digital 4
                   3,  //D6 no digital 3
                   2); //D7 no digital 2

//Tratamento do valor da temperatura
byte Tbyte1, Tbyte2;
unsigned int aux;
int quantidade_bytes_esperados = 13;

//Tratamento do valor da umidade
byte Ubyte1, Ubyte2;
unsigned int aux2;

//Tratamento do valor da pressão
byte Pbyte1, Pbyte2;
unsigned int aux3;

//Tratamento dos valores do relogio
int horas, minutos, segundos, dia, mes, ano;

//Adicionando zeros a dia e mes
int verif_0, verif_1, verif_2, verif_3;

// Array simbolo grau
byte grau[8] ={ B00001100, 
                B00010010, 
                B00010010, 
                B00001100, 
                B00000000, 
                B00000000, 
                B00000000, 
                B00000000,};

void setup()
{
  Serial.begin(115200);
  disp.begin(20,4); //Inicializa LCD 20 x 4

  for(char i=8; i<13; i++) pinMode(i, INPUT_PULLUP);     

  t_butUp   = 0x00;                                      
  t_butDown = 0x00;                                      
  t_butLeft = 0x00;                                      
  t_butRight = 0x00;                                     
  t_butEnter = 0x00;                                     
  
  Wire.begin(5);                
  
  Wire.onReceive(bytesTemperatura);                     
  Serial.begin(115200);            
  
  // Cria o caracter customizado com o simbolo do grau
   disp.createChar(0, grau); 
}

float temperaturadht;
int indiceUV;
float umidadedht;
float pAtm;
bool condLcdsUv = false;


void loop()
{
  changeMenu();
  dispMenu();
  //delay(1000);
}
//Wire.setClock(frequencia em hertz); setar a frequencia de comunicao I2C
// --- Desenvolvimento das funções Auxiliares ---
void changeMenu()                                       //Modifica o menu atual
{
   if(!digitalRead(butUp))   t_butUp   = 0x01;          //Botao Up pressionado? Seta flag
   if(!digitalRead(butDown)) t_butDown = 0x01;          //Botao Down pressionado? Seta flag
      
   if(digitalRead(butUp) && t_butUp)                    //Botao Up solto e flag setada?
   {                                                   
      t_butUp = 0x00;                                   //Limpa flag
      
      disp.clear();                                     //Limpa display
      menu++;                                           //Incrementa menu
      
      if(menu > 0x03) menu = 0x01;                      //Se menu maior que 4, volta a ser 1
   
   } //end butUp
   
   if(digitalRead(butDown) && t_butDown)                //Botao Down solto e flag setada?
   {                                                    
      t_butDown = 0x00;                                 //Limpa flag
      
      disp.clear();                                     //Limpa display
      menu--;                                           //Decrementa menu
      
      if(menu < 0x01) menu = 0x03;                      //Se menu menor que 1, volta a ser 4
   
   } //end butDown   

} //end changeMenu

void dispMenu()                                         //Mostra o menu atual
{
    switch(menu)                                        
    {
       case 0x01:                                       
             data_hora();                               
             
             break;                                     
       case 0x02:                                       
             painelSensores();                             
             
             break;                                     
       case 0x03:                                       
             acionamentoSaidas();                             
             
             break;
    } //end switch menu

} //end dispMenu

void data_hora()                                        
{    
   disp.setCursor(0,0);                                 
   disp.print("Data");                          
   disp.setCursor(0,1);                                 
   
   verif_0 = 0;
   
   if(mes < 10)
   {
     verif_0 += 1;
   }

   if(dia < 10)
   {
    verif_0 += 2;
   }

   if(verif_0 == 1)
   {
    disp.print((String)dia +"/"+ "0" + (String)mes + "/" + "20" + (String)ano);
    
   }

   if(verif_0 == 2 || verif_0 == 3)
   {
    disp.print( "0" + (String)dia +"/"+ "0" + (String)mes + "/" + "20" + (String)ano);
    
   }

   if(verif_0 == 0)
   {
    disp.print((String)dia +"/" + (String)mes + "/" + "20" + (String)ano);
   }

   verif_1 = 0; //verificador relacionado aos segundos

   if(segundos < 10)
   {
    verif_1 += 1;
   }

   if(segundos > 9)
   {
    verif_1 += 2;
   }

   verif_2 = 0; //verificador relacionado aos minutos

   if(minutos > 9)
   {
    verif_2 += 3;
   }
   
   if(minutos < 10)
   {
    verif_2 += 4;
   }

   verif_3 = 0; //verificador relacionado as horas

   if(horas > 9)
   {
    verif_3 += 5;
   }
   
   if(horas < 10)
   {
    verif_3 += 6;
   }

   disp.setCursor(0,2);                                 
   disp.print("Hora");                           
   disp.setCursor(0,3);

   if(verif_1 == 1)
   {
    disp.print((String)horas + ":" + (String)minutos + ":" + "0" + (String)segundos);
   }

   if(verif_1 == 2)
   {
    disp.print((String)horas + ":" + (String)minutos + ":" + "" + (String)segundos);
   }
   
} //end data_hora

void painelSensores()                                      //Temperatura (menu2)
{
   disp.setCursor(0,0);                                 //Posiciona cursor na coluna 1, linha 1
   disp.print("Temperatura");                           //Imprime mensagem
   disp.setCursor(1,1);                                 //Posiciona cursor na coluna 2, linha 2
   
   disp.print(temperaturadht);                              
   // Mostra o simbolo do grau
   disp.write(byte(0));
   disp.print("C");

   disp.setCursor(0,2);                                 
   disp.print("Indice UV");                           
   disp.setCursor(1,3);
   
   if(indiceUV < 10)
   {
    disp.setCursor(1,4);
    disp.print("       ");
   } 

   disp.setCursor(1,3);
   disp.print(indiceUV);
   
   if(indiceUV > 9)
   {
    disp.setCursor(1,3);
    disp.print(indiceUV);                            
   }

   disp.setCursor(12,0);                                
   disp.print("Umidade");                           
   disp.setCursor(13,1);                                 
   disp.print(umidadedht);
   disp.print("%");
   
   disp.setCursor(12,2);                                 
   disp.print("Pressao");                           
   disp.setCursor(13,3);                                 
   disp.print(pAtm);
   disp.print("ATM");
   
} //end temperatura()

void acionamentoSaidas()                                            
{
   disp.setCursor(0,0);                                 
   disp.print("Configuracoes");                                
    
   dispSubMenu3();

} //end menu4

void dispSubMenu3()                                     
{
  
  if(!digitalRead(butLeft))    t_butLeft    = 0x01;         
  if(!digitalRead(butRight))    t_butRight    = 0x01;          
   
   if(digitalRead(butLeft) && t_butLeft)                      
   {                                                   
      t_butLeft = 0x00;                                    
      
      subMenu3++;                                       
      
      if(subMenu3 > 3) subMenu3 = 0x01;                 
      
   
   } //end butP
   
   if(digitalRead(butRight) && t_butRight)                      
   {                                                    
      t_butRight = 0x00;                                    
      
      subMenu3--;                                      
      
      if(subMenu3 < 1) subMenu3 = 0x03;   
    
   
   } //end butM
  
  
  
    switch(subMenu3)                                    
    {
       case 0x01:                                       
             disp.setCursor(1,1);                         
             disp.print("Temperatura");  
             readEnter(1);                                          
             
             break;                                     
       case 0x02:                                       
             disp.setCursor(1,1);                          
             disp.print("Umidade");                 
             readEnter(2);                              
             
             break;                                     
       case 0x03:                                       
             disp.setCursor(1,1);                          
             disp.print("Pressao");   
             readEnter(3);                                         
             
             break;                                     

       case 0x04:                                       
             disp.setCursor(1,1);                          
             disp.print("Indice UV");   
             readEnter(3);                                           
             
             break;
    } 
} 

 
void readEnter(char option)                            //Leitura do botão enter para seleção de subMenus
{
    
   if(!digitalRead(butEnter))     t_butEnter    = 0x01;      //Botão enter pressionado? Seta flag
   
   
   if(digitalRead(butEnter) && t_butEnter)                  //Botão enter solto e flag setada?
   {                                                   
        t_butEnter = 0x00;                                //Limpa flag        
        switch(option)
        {
           case 0x01: subMenuSaidas_temperatura(); break;
           
           case 0x02: subMenuSaidas_umidade(); break;
           
           case 0x03: subMenuSaidas_pressao(); break;

           case 0x04: subMenuSaidas_indiceUv(); break;        
        }    
   } 
} 


void subMenuSaidas_temperatura()
{
  t_butEnter = 0x00;
  while(1)
  {
   if(!digitalRead(butEnter)) return; //botao enter pressionada? flag setada
   disp.setCursor(0,0);                                 
   disp.print("Valor do timer");                              
   disp.setCursor(0,1);
   disp.print("Formato: H:M:S");                                                         
   disp.setCursor(0,2);
   if(!digitalRead(butUp)) caracter += 0x01; 
   if(!digitalRead(butDown)) caracter -= 0x01;
   disp.print(caracter);
   
  }
  //t_butEnter = 0x00;
  //if(digitalRead(butEnter) && t_butEnter) 

}

void subMenuSaidas_umidade()
{
  while(1)
  {
   disp.setCursor(0,0);                                 
   disp.print("Tarefa 2");                              
   disp.setCursor(0,1);                                  
   disp.print("Em funcionamento");                      
  }

}


void subMenuSaidas_pressao()
{
  while(1)
  {
   disp.setCursor(0,0);                                 
   disp.print("Tarefa 3");                              
   disp.setCursor(0,1);                                    
   disp.print("Em funcionamento");                      
  }

} 

void subMenuSaidas_indiceUv()
{
  while(1)
  {
   disp.setCursor(0,0);                                 
   disp.print("Tarefa 3");                              
   disp.setCursor(0,1);                                    
   disp.print("Em funcionamento");                      
  }

} 

void bytesTemperatura(int quantidade_bytes_esperados) { // Esta funcao sera executada quando "quantidade_bytes_esperados" for recebida via I2C
  
    Tbyte1 = Wire.read(); //Temperatura                            
    Tbyte2 = Wire.read(); //Temperatura
    
    indiceUV = Wire.read(); //Indice UV

    Ubyte1 = Wire.read(); //Umidade                            
    Ubyte2 = Wire.read(); //Umidade
    
    Pbyte1 = Wire.read(); //Pressao                           
    Pbyte2 = Wire.read(); //Pressao

    horas = Wire.read();
    minutos = Wire.read();
    segundos = Wire.read();
    dia = Wire.read();
    mes = Wire.read();
    ano = Wire.read();
        
    Serial.println("Indice UV:");
    Serial.println(indiceUV);

    Serial.println("Relogio");
    Serial.println((String)horas + ":" + (String)minutos + ":" + (String)segundos);
    
    // Ajustando os bytes recebidos para obter a temperaturadht
    aux = Tbyte2;                     // Ajusta a parte fracionaria (depois da virgula)
    temperaturadht = (float) (aux*0.01);          // Atribui a parte fracionaria 
    aux = Tbyte1;                      // Ajusta a parte inteira (antes da virgula)
    temperaturadht += aux;                          // Atribui a parte iteira
    Serial.println("Temperatura:");
    Serial.println(temperaturadht);  

     // Ajustando os bytes recebidos para obter a umidadedht
    aux2 = Ubyte2;                     // Ajusta a parte fracionaria (depois da virgula)
    umidadedht = (float) (aux2*0.01);          // Atribui a parte fracionaria 
    aux2 = Ubyte1;                     // Ajusta a parte inteira (antes da virgula)
    umidadedht += aux2;                          // Atribui a parte iteira

    Serial.println("Umidade:");
    Serial.println(umidadedht);

    // Ajustando os bytes recebidos para obter a pAtm
    aux3 = Pbyte2;                     // Ajusta a parte fracionaria (depois da virgula)
    pAtm = (float) (aux3*0.01);          // Atribui a parte fracionaria
    aux3 = Pbyte1;                     // Ajusta a parte inteira (antes da virgula)
    pAtm += aux3;                          // Atribui a parte iteira

    Serial.println("PressÃƒÂ£o atmosferica:");
    Serial.println(pAtm);

/*
  Temperatura, umidade, pressao e indice uv.
  1º descobrir para qual tipo de variavel (sensor) o comando sera dado
    - 1º bit: Indicador de funcao
    - 2º bit: Valor a ser setada
  2º Alocar os valores para as devidas variaveis

  ======= VARIAVEIS DA FUNÇÃO progAcionamento() ===========
  opc: Tipo de programação de acionamento
  tempAc: Tempo pelo qual a saida ficara em estado ativo
  datAcio: Acionamento por data
  tempAcio: Acionamento por intervalo de tempo ou hora, minuto, segundo agendado
*/
    void progAcionamento()
    {
      switch(opc)
      {
        case 1:
        acionaPorTmp();
        break;

        case 2:
        acionaPorUmd();
        break;

        case 3:
        acionaPorPres();
        break;

        case 4:
        acionaPorIndv();
        break;       

      }
      
      void acionaPorTmp()
      {
        
      }

      void acionaPorUmd()
      {
        
      }

      void acionaPorPres()
      {
        
      }

      void acionaPorIndv()
      {
        
      }
      
    }
    
}
