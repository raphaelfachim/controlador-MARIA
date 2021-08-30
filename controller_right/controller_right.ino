/*
    Curso de Arduino Aula 033 - WR Kits Channel
    
    Lendo Rotary Encoder
    
    Autor: Eng. Wagner Rambo   Setembro de 2015
    Editor: Raphael Siller Fachim
    
    www.wrkits.com.br | facebook.com/wrkits | youtube.com/user/canalwrkits

*/


// --- Mapeamento de Hardware  e  Constantes---
#define    encoder_C1   2                     //Conexão C1 do encoder
#define    encoder_C2   3                     //Conexão C2 do encoder
#define    R_en         6
#define    L_en         7

// Variáveis da interrupção do encoder

byte            Encoder_C1Last;
long            pulse_number =     0;           // Contador de pulsos do encoder
uint8_t         PWM_OUT;           // Porta de saída PWM
float           raio =             0.1;         // Raio da roda (10cm)
uint16_t        time_counter;                   // Contador do timer
boolean         direction_m;                    // Direção de giro do motor
int             s1 =               0;           // Distância 1
int             s2 =               0;           // Distância 2
float           v_l =              0;           // Velocidade linear
float           distancia_per =    0.0;         // Distancia percorrida
float           rpm =              0.0;         // Velocidade angular atual
float           prev_rpm =         0.0;         // Velocidade angular anterior

// Variaveis do Controlador
              
uint8_t         output =           0;           // Byte de saída da velocidade estimada
float           setpoint =         0.0,         // Setpoint em RPM
                erro =             0.0,         // Erro --> entrada do controlador 
                P =                0,           // = Erro*kp
                I =                0,           // += Erro*ki*(1/deltaTEMPO)
                D =                0,           // =(PRESENTE - PASSADO)*kd
                myPID =            0;           // = P + I + D
boolean         en_PID =           true;
uint8_t         c_inercia;
// Variáveis da comunicação com raspberry pi

// Valores de ajuste
float           k_amostragem =     1;           // Tempo de amostragem (1 = 100ms)
String          direcao =          "cw";        // Sentido de giro do motor 'ccw' ou 'cw'                              
float           setpoint_v =       0.0,         // Setpoint de velocidade
                kp =               1.3,         // Constante proporcional do controlador
                ki =               0.8,         // Constante integrativa do controlador
                kd =               0,           // Constante derivativa do controlador
                dist =             (2*PI*raio)*4;

ISR(TIMER2_OVF_vect)
{
    uint8_t sinal_controle;
    TCNT2=178;                                            // Reinicializa o registrador do Timer2 (~5ms)
    time_counter ++;
    if(time_counter == 20*k_amostragem){                  // Amostragem k = 1 = 100ms
      time_counter = 0;
      s1 = s2;
      s2 = pulse_number;
      if(s1 == s2){                                       // Não há variação na leitura do encoder
        v_l = 0.0;
        rpm = 0.0;
        } else {                                          // Há variação na leitura do encoder
            prev_rpm = rpm;                               // Variável recebe valor anterior de 'Velocidade Angular' para a parte derivativa do PID
            rpm = abs((s2-s1)/(12*(0.1*k_amostragem)));   // Cál culo da Velocidade Angular atual - Valor absoluto --> não importa o sentido de giro
            v_l = to_linear(rpm, raio, k_amostragem);     // 1 rpm = 0,104 719 755 119 66 rad/s
            //distancia_per += v_l*(0.1*k_amostragem);
          }


       if(en_PID){
          erro = setpoint - rpm; 
          P = erro*kp;
          I += erro*ki*(0.1*k_amostragem);
          //D = (prev_rpm - rpm)*kd;
          myPID = c_inercia + P + I + D;
          if(myPID>255){
              myPID = 255;
            }
          if(myPID<0){
              myPID = 0;
            }
          analogWrite(PWM_OUT, myPID); 
        } else {
        P = 0;
        I = 0;
        }
       }
      }

// --------------------------- SETUP ----------------------------
void setup()
{
  Serial.begin(9600);                         //Inicializa comunicação serial
  
  pinMode(encoder_C1,       INPUT);                //Configura entrada C1 para leitura do encoder
  pinMode(encoder_C2,       INPUT);                //Configura entrada C2 para leitura do encoder

  pinMode(R_en,             OUTPUT);
  pinMode(L_en,             OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  digitalWrite(R_en,       HIGH);
  digitalWrite(L_en,       HIGH);

  digitalWrite(encoder_C1, HIGH); //turn pullup resistor on
  digitalWrite(encoder_C2, HIGH); //turn pullup resistor on
  
  attachInterrupt(0, count_pulses, CHANGE);   //Interrupção externa 0 por mudança de estado
  TCCR1B = TCCR1B & B11111000 | B00000001;
  pinMode(9,            OUTPUT);
  pinMode(10,            OUTPUT);



  

//  if(direcao == "ccw"){
//    PWM_OUT = 10;
//      }
//  if(direcao == "cw"){
//    PWM_OUT = 9;
//      }
  
//-- Configura Interrupção de Tempo --

  TCCR2A = 0x00;   //Timer operando em modo normal
  TCCR2B = 0x07;   //Prescaler 1:1024
  TCNT2  = 178;    //~~ 5 ms para o overflow
  TIMSK2 = 0x01;   //Habilita interrupção do Timer2
}

// --------------------------- LOOP ----------------------------

void loop()
{
//      uint16_t cent = uint16_t(v_l);
//      uint16_t dez  = uint16_t(v_l*10)%10;
//      uint16_t un   = uint16_t(v_l*100)%10;
//        
//      Serial.write(cent+48);
//      Serial.write('.');
//      Serial.write(dez+48);
//      Serial.write(un+48);
//      Serial.write(' ');
//      Serial.write('\n');


      if(Serial.available() > 0){
        digitalWrite(LED_BUILTIN, HIGH);
        int data_in = Serial.read() - '0';
        if(data_in == 1){
          en_PID = true;
          PWM_OUT = 9;
          setpoint_v = 0.07;
          c_inercia = 90;
          setpoint = float(to_rpm(setpoint_v, raio));
          //output = map(setpoint, 0, 150, 70, 255);
          //analogWrite(PWM_OUT, output);
          }
        if(data_in == 2){
          en_PID = true;
          PWM_OUT = 10;
          setpoint_v = 0.07;
          c_inercia = 90;
          setpoint = float(to_rpm(setpoint_v, raio));
          //output = map(setpoint, 0, 150, 70, 255);
          //analogWrite(PWM_OUT, output);
          }
        if(data_in == 3){
          c_inercia = 30; 
          setpoint = 0;
          //output = map(setpoint, 0, 150, 70, 255);
          //delay(500);
          //en_PID = false;
          //analogWrite(PWM_OUT, 0);
          }
        if(data_in == 9){
          Serial.write('0' + 2);
          }
        
        }
        delay(100);
}

// --------------------------- FUNCTIONS ----------------------------

float to_rpm(float velocidade, float raio)
{ 
 float rpm = (velocidade*30.0)/(raio*PI);  
 return rpm;
}

float to_linear(int rpm, double raio, uint8_t k_amostragem)
{ 
 float velocidade_linear = float((0.10472*rpm)*raio);  
 return velocidade_linear;
}

void count_pulses()
{
  int Lstate = digitalRead(encoder_C1);       //Lê estado de encoder_C1 e armazena em Lstate
  
  if(!Encoder_C1Last && Lstate)               //Encoder_C1Last igual a zero e Lstate diferente de zero?
  {                                           //Sim...
    int val = digitalRead(encoder_C2);        //Lê estado de encoder_C2 e armazena em val
    
    if(!val && direction_m) direction_m = false;      //Sentido reverso
    
    else if(val && !direction_m) direction_m = true;  //Sentido direto
    
  } //end if 
 
  Encoder_C1Last = Lstate;                    //Encoder_C1Last recebe o valor antigo

  if(!direction_m)  pulse_number++;           //incrementa número do pulso se direction limpa
  else              pulse_number--;           //senão decrementa
} //end count_pulses
