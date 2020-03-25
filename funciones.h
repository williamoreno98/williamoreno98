// sube cuando esta listo para leer 
extern volatile int readFlag;
// Almacena el valor leido
extern volatile int analogVal;


//Filtro FIR frecuencia de muestreo 100 Hz orden 7  fc= 10Hz
//const int BL = 8;
//const int B[BL] = {
//      -3 ,   20  ,  47  ,  64  ,  64  ,  47  ,  20  ,  -3
//};


//Filtro FIR frecuencia de muestreo 100 Hz orden 11  fc= 10 Hz
//const int BL = 12;
//const int B[BL] = { -24  ,  -9  ,  21  ,  59  ,  94  , 114  , 114  ,  94  ,  59  ,  21  ,  -9  , -24
//     
//};

//Filtro FIR frecuencia de muestreo 100 Hz orden 17  fc= 10 Hz
//const int BL = 18;
//const int B[BL] = { -9 ,  -22  , -27  , -19  ,   2  ,  35  ,  72  , 103  , 121  , 121  , 103  ,  72  ,  35  ,   2  , -19  , -27 ,  -22  ,  -9
//     
//};

//Filtro FIR frecuencia de muestreo 100 Hz orden 30  fc= 10Hz
const int BL = 31;
const int B[BL] = { -7  ,  -1  ,   7  ,  13  ,  13   ,  8  ,  -3  , -15  , -23  , -21  ,  -8 ,  
                    16  ,  47  ,  77  ,  99  , 107   , 99  ,  77  ,  47  ,  16  ,  -8  , -21 , 
                    -23 ,  -15 ,  -3  ,   8  ,  13  ,  13 ,    7  ,  -1  ,  -7
};


/*Inicio de variables volatiles para pasar datos entre funciones*/
volatile int x[BL];
volatile int k;
volatile int analogVal;
volatile int readFlag;

/*Variables especificas del ejemplo:*/
volatile int walk;
volatile int cont;
uint32_t ts1,ts2;
unsigned long previousMillis=0;

/*Definiciones del ejemplo*/
#define RUIDO 1
#define PASO 2
#define IMPULSO 3
#define PASOANALOGO 4
#define PERIODOTEST 100

#define FIR1 1
#define FIR2 2
#define FIR3 3
#define IIR 4

void  Inicializar_ADC_PWM_Serial(){
  cont=0;
  walk=0;
  k=0;
  previousMillis=0;
  pinMode(LED_BUILTIN,OUTPUT);//LED_BUILTIN

  Serial.begin(1000000);/*velocidad maxima para que no interfiera en tiempos*/
  while (!Serial) {
    ; // Esperar a que el puerto inicie
  }
  Serial.println("Setup");
}


void enviardato(){
  Serial.print(analogVal);
  Serial.print(" ");
  Serial.print(cont);
  Serial.println("");
}
 
/*INICIAN FUNCIONES DE EJEMPLO*/

long filtrarFIR1(int in)
{
  int i=1;
  x[k] = (int)in;
  long y=0; 
  for (i=1; i<=BL; i++)// NOTA, DEBE INICIAR EN 1. EJERCICIO: haga una prueba de escritorio con una respuesta impulso y compruebe...
  {
      y += B[i-1] * x[(i + k) % BL];// verifique que para su filtro no exista overflow. 
  }
  
    k = (k+1) % BL;
    return y>>8; //si no es multiplo de 2^n divida por el factor de normalización adecuado a su filtro. 
}

long filtrarFIR2(int in)
{
  int i=0;
  x[k] = in;
  long y=0;
  int inx=k;
  for(i = 0; i < BL; ++i) {
    y += x[inx] * B[i];// verifique que para su filtro no exista overflow. 
    inx = inx != 0 ? inx-1 : BL-1;
  }
  k++;
  k = (k>=BL) ? 0:k;
  return y>>8; //si no es multiplo de 2^n divida por el factor de normalización adecuado a su filtro. 
  
}

long filtrarFIR3(int in)
{
  int i=0;
  x[k] = in;
  int inx=k;
  const int *apuntadorcoef=&B[0];
  volatile int *apuntadorarrc=&x[inx];
  long y=0;
  for(i = 0; i < BL; ++i) {
    y += (long)(*apuntadorarrc) * (long)(*apuntadorcoef);// verifique que para su filtro no exista overflow. 
    apuntadorcoef++;
    if (inx != 0){
      apuntadorarrc--;
      inx--;
    }
    else{
      apuntadorarrc=&x[BL-1];
      inx=BL-1;
    }
  }
  k++;
  k = (k>=BL) ? 0:k; 
  return y>>8; //si no es multiplo de 2^n divida por el factor de normalización adecuado a su filtro. 
}

/*Parametros del filtro IIR*/
const int NL=3;
//const float NUM[3]={3.844633284e-06,7.689266567e-06,3.844633284e-06};
//const float NUM[3]={0.036574835843928, 0.073149671687856, 0.036574835843928};
//const float NUM[3]={0.10598800495854646, -0.1580936232020776, 0.06284028813992219};
//const float NUM[3]={0.39133577250659896, -0.7826715450131979, 0.39133577250659896};
float NUM[3]={1.0,-2.0,1.0};
const int DL=3;
//const float DEN[3]={1,   -1.994446397,   0.9944617748};
//const float DEN[3]={1,   1.3908952814253899, -0.5371946248011019};
//const float DEN[3]={1,   0.8939931253174443, 0.2209};
//const float DEN[3]={1,   -0.36952737735124147, 0.1958157126558331};
const float DEN[3]={1,   0, -1};



float w[NL]={0,0,0};


float filtrarIIR(float in){
float y;
w[0]=(DEN[0]*in)-(DEN[1]*w[1])-(DEN[2]*w[2]);// OJO QUE EL MENOS YA ESTA EN LA ECUACION
y=((NUM[0]*w[0])+(NUM[1]*w[1])+(NUM[2]*w[2]));
w[2]=w[1];
w[1]=w[0];
return y;
}
