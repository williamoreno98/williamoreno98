#include"funciones.h"
/*Instrucciones de las siguientes 3 lineas:
1. Cambie MEDIRTIEMPOS a 1 o 0 para sacar en 2 columna el tiempo de funcion en microsegundos
2. Cambie entrada a {RUIDO,PASO,IMPULSO} para simular una señal de entrada
3. Cambie salida a {FIR1,FIR2,FIR3, IIR} para comparar diferentes implementaciones de funciones
4. Abra en: ->Herramientas -> monitor serie. o ->Herramientas -> "Serial Plotter"
*/
//Senos muestreados a 100 Hz
const int sin2[50]={ 512 ,576 ,639 ,700 ,758, 812 ,862, 906, 943, 974 ,998 ,1014 , 1022  ,1022 , 1014 , 998 ,974, 943 ,906 ,862 ,812 ,758 ,700, 639, 576 ,512 ,448 ,385 ,324 ,266, 212, 162, 118 ,81 , 50 , 26 , 10 , 2 ,2, 10 , 26 , 50 , 81 , 118, 162, 212, 266, 324, 385, 448};
const int sin4[25]={512 ,  639 ,758 ,862, 943, 998 ,1022 , 1014 , 974 ,906 ,812 ,700 ,576 ,448 ,324 ,212 ,118 ,50 , 10 , 2 ,26 , 81 , 162, 266 ,385};
const int sin6[16]={512 , 700 ,862 ,974 ,1022 , 998 ,906 ,758 ,576 ,385 ,212 ,81  ,10 , 10 , 81 , 212};
const int sin8[12]={512 , 758, 943, 1022 , 974 ,812 ,576 ,324 ,118 ,10 , 26 , 162};
const int sin10[10]={  512  , 812 ,  998 ,  998 ,  812 ,  512  , 212 ,   26 ,   26 ,  212};
const int sin20[5]={ 512 ,  998  , 812 ,  212 ,   26};
const int sin30[3]={ 512 ,  998 ,  212};

#define SIN2 1
#define SIN4 2
#define SIN6 3
#define SIN8 4
#define SIN10 5
#define SIN20 6
#define SIN30 7


volatile byte index=0; //Index to keep tracking of Sinus signal
#define MEDIRTIEMPOS 0//Cambiar a 1 y abrir el monitor serial "de la grafica no se aprecia mucho ya que la columna del centro está en microsegundos"
#define MOSTRARENTRADA 1// cambiar a 1 para mostrar la entrada
#define MUESTRASPASOIMPULSO 100// numero de muestras antes de resetear la respuesta paso e impulso, subir en sistemas lentos



int entrada=SIN2;// Seleccione PASO, RUIDO, IMPULSO
int salida=FIR1;// Seleccione FIR1,FIR2,FIR3, o IIR
const long interval = 10;           // Intervalo a medir periodico en milisegundos 100Hs=10ms
void setup() {
  // put your setup code here, to run once:
Inicializar_ADC_PWM_Serial();
previousMillis=0;
}


void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    long sal;// para los FIR cambiar sal a long
    float saliir;
    switch(entrada)
    { 
       case PASO: 
          if (cont>MUESTRASPASOIMPULSO)
          {
             cont=0;
            if (walk)
              walk=0;
            else
            if (salida==IIR)
              walk=1;// para el filtro IIR la respuesta impulso es de una unidad
            else
              walk=256;// para el filtro FIR la respuesta impulso es de 256 ya que esta escalizada, mire al final de las funciones FIR# que se hace un corrimiento de 8 bits 2^8-1=256
          }
          cont++;
          break; 
       case RUIDO: 
          walk+=random(-3, 4);//numero aleatorio entre -3 y (4-1)=3
          break; 
       case IMPULSO: 
          if (cont>MUESTRASPASOIMPULSO)
          {
            cont=0;
            if (salida==IIR)
              walk=1;// para el filtro IIR la respuesta paso es de una unidad
            else
              walk=256; //para el filtro FIR la respuesta paso es de 256 ya que esta escalizada, mire al final de las funciones FIR# que se hace un corrimiento de 8 bits 2^8-1=256
          }
          else{
              walk=0;
          }
          cont++;
          break; 

          case SIN2:
              walk=sin2[index];
              index= index==49 ? 0 : index+1;
              break;
          case SIN4:
              walk=sin4[index];
              index= index==24 ? 0 : index+1;
              break;
          case SIN6:
              walk=sin6[index];
              index= index==15 ? 0 : index+1;
              break;
          case SIN8:
              walk=sin8[index];
              index= index==11 ? 0 : index+1;
              break;
          case SIN10:
              walk=sin10[index];
              index= index==9 ? 0 : index+1;
              break;
          case SIN20:
              walk=sin20[index];
              index= index==4 ? 0 : index+1;
              break;
          case SIN30:
              walk=sin30[index];
              index= index==2 ? 0 : index+1;
              break;
       default: 
          Serial.println("no selecciono entrada");
          walk=-1; 
    }
    //Serial.print("Dato de entrada:");
    if (MOSTRARENTRADA)
    {
      Serial.print(walk);
      Serial.print(" ");
    }
    
    switch(salida) 
    { 
       case FIR1:
        ts1 = micros();
        sal=filtrarFIR1(walk);
        ts2 = micros();
        if (MEDIRTIEMPOS){
          Serial.print(ts2-ts1);//timepo en microsegundos
          Serial.print(" ");
        }
        Serial.print(sal);
        Serial.print(" ");
      break;
      case FIR2:
        ts1 = micros();
        sal=filtrarFIR2(walk);
        ts2 = micros();
        if (MEDIRTIEMPOS){
          Serial.print(ts2-ts1);//timepo en microsegundos
          Serial.print(" ");
        }
        Serial.print(sal);
        Serial.print(" ");
      
      break;
      case FIR3:
        ts1 = micros();
        sal=filtrarFIR3(walk);
        ts2 = micros();
        if (MEDIRTIEMPOS){
          Serial.print(ts2-ts1);//timepo en microsegundos
          Serial.print(" ");
        }
        Serial.print(sal);
        Serial.print(" ");
      break;
      case IIR:
        ts1 = micros();
        saliir=filtrarIIR((float)walk);
        ts2 = micros();
        if (MEDIRTIEMPOS){
          Serial.print(ts2-ts1);//timepo en microsegundos
          Serial.print(" ");
        }
        Serial.print(saliir,10);//pinta 10 decimales
        Serial.print(" ");
      break;
    }
    Serial.println("");
}
  delay(10);//delay de 10 ms para que haga alrededor de 100 muestras por segundo
}
