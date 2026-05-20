#include <AFMotor.h>
#include <Stepper.h>

#define S0 22
#define S1 24
#define S2 26
#define S3 28
#define sensorOut 30
#define OE 32

int rojo = 0, verde = 0, azul = 0;
int R = 0, G = 0, B = 0;
const int UMBRAL_COLOR = 50; 

const int PASOS_POR_VUELTA = 2048; 
const int PASOS_UNA_DIVISION = 410; 

const int PIN_A = 41;
const int PIN_B = 42;
const int PIN_C = 43;
const int PIN_D = 44;


Stepper miStepper(PASOS_POR_VUELTA, PIN_A, PIN_C, PIN_B, PIN_D);

const int DIV_ROJO = 1;      
const int DIV_VERDE = 2;     
const int DIV_AZUL = 3;      
const int DIV_AMARILLO = 4;   
const int DIV_DESCARTE = 5;   

int posicionActual = DIV_AZUL; 

int pasosRestantes_PorMover = 0;
int direccionPaso = 1; 

AF_DCMotor banda(1); 
const int velocidadBanda = 255; 

unsigned long ultimoTiempoSensor = 0;
unsigned long ultimoTiempoMotor = 0; 
unsigned long tiempoBloqueoClasificador = 0; 
const int TIEMPO_ESPERA_DESCARTE = 1000;     

void setup() {
  Serial.begin(115200);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(sensorOut, INPUT);

  digitalWrite(OE, LOW);  
  digitalWrite(S0, HIGH); 
  digitalWrite(S1, LOW);

  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT);
  pinMode(PIN_D, OUTPUT);


  miStepper.setSpeed(15); 
  apagarBobinasStepper(); 

  banda.setSpeed(velocidadBanda);
  banda.run(FORWARD); 
  
  delay(1000); 
}

void loop() {
  unsigned long tiempoActual = millis();


  if (pasosRestantes_PorMover != 0 && (tiempoActual - ultimoTiempoMotor >= 4)) {
    ultimoTiempoMotor = tiempoActual;
    
    miStepper.step(direccionPaso); 
    if (pasosRestantes_PorMover > 0) pasosRestantes_PorMover--;
    else pasosRestantes_PorMover++;
    if (pasosRestantes_PorMover == 0) {
      apagarBobinasStepper();
    }
  }
  if (tiempoActual - ultimoTiempoSensor >= 50) {
    ultimoTiempoSensor = tiempoActual;
    leerYEnviarColor();
    if (pasosRestantes_PorMover == 0 && tiempoActual >= tiempoBloqueoClasificador) {
      clasificarYControlarClasificador();
    }
  }
}

void leerYEnviarColor() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  rojo = pulseIn(sensorOut, LOW, 20000);

  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  verde = pulseIn(sensorOut, LOW, 20000);

  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  azul = pulseIn(sensorOut, LOW, 20000);

  if (rojo > 200 || verde > 200 || azul > 200 || rojo == 0 || verde == 0 || azul == 0) {
    R = 0; 
    G = 0; 
    B = 0;
  } else {
    R = map(rojo, 25, 95, 255, 0);
    G = map(verde, 30, 105, 255, 0);
    B = map(azul, 25, 95, 255, 0);

    R = constrain(R, 0, 255);
    G = constrain(G, 0, 255);
    B = constrain(B, 0, 255);
  }
  Serial.print(R);
  Serial.print(",");
  Serial.print(G);
  Serial.print(",");
  Serial.println(B);
}

void clasificarYControlarClasificador() {
  banda.setSpeed(velocidadBanda);
  banda.run(FORWARD);

  if (R < UMBRAL_COLOR && G < UMBRAL_COLOR && B < UMBRAL_COLOR) {
    return;
  }

  int posicionObjetivo = posicionActual; 

  if (R > 130 && G > 120 && B < 120 && abs(R - G) < 65) {
    posicionObjetivo = DIV_AMARILLO; 
  }
  else if (R > G && R > B) {
    posicionObjetivo = DIV_ROJO;     
  }
  else if (G > R && G > B) {
    posicionObjetivo = DIV_VERDE;    
  }
  else if (B > R && B > G) {
    posicionObjetivo = DIV_AZUL;     
  }
  else {
    posicionObjetivo = DIV_DESCARTE; 
  }

  if (posicionObjetivo != posicionActual) {
    int divisionesAMover = posicionObjetivo - posicionActual;

    if (divisionesAMover > 2) {
      divisionesAMover -= 5;
    } else if (divisionesAMover < -2) {
      divisionesAMover += 5;
    }
    pasosRestantes_PorMover = divisionesAMover * PASOS_UNA_DIVISION;
    
    if (pasosRestantes_PorMover > 0) direccionPaso = 1;
    else direccionPaso = -1;

    posicionActual = posicionObjetivo;
    tiempoBloqueoClasificador = millis() + TIEMPO_ESPERA_DESCARTE; 
  }
}

void apagarBobinasStepper() {
  digitalWrite(PIN_A, LOW);
  digitalWrite(PIN_B, LOW);
  digitalWrite(PIN_C, LOW);
  digitalWrite(PIN_D, LOW);
}