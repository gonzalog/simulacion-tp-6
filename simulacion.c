#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define HV 999999999

double intervalo_entre_arribos();
double tiempo_de_atencion_empleado();
double tiempo_de_atencion_tester();
double random_real();
int menor(double* tiempos, int cantidad);
int posicion_puesto_libre(double* puestos, int cantidad);
void calculo_de_resultados(int empleados, double stll, double sts, double sto, double tiempo, int NR, int NT, double* pto, double* pps, double* ptr);
void imprimir_resultados(double pto, double pps, double ptr, int empleados);

// Recibe cantidad de empleados (N), tiempo final en minutos (hasta HV)
main(int argc, char** argv ){
  int N = atoi(argv[1]), proximo_puesto_a_liberarse, NT = 0, NS = 0, NR = 0, puesto_libre, i, subtareas, puestos_a_ocupar;
  double tiempo_final = atof(argv[2]), tiempo = 0, tpll = 0, stll = 0, sts = 0, tc_tester = 0, pps, ptr, tiempo_atencion_tester_a_tarea;
  double* tps = malloc(N * sizeof(double));
  for(i = 0; i < N; i++)
    tps[i] = HV;
  double sto = 0;
  double* ito = malloc(N * sizeof(double));
  for(i = 0; i < N; i++)
    ito[i] = 0;
  double pto;

  while((tiempo < tiempo_final) || (NS != 0)){
    proximo_puesto_a_liberarse = menor(tps, N);
    if(tpll < tps[proximo_puesto_a_liberarse]){
      //llegada
      
      //avance del tiempo
      tiempo = tpll;

      subtareas = (random_real() < 0.05) ? 2 : 1;
      
      //determinacion de EFnoC
      tpll = tiempo + intervalo_entre_arribos();
      
      //actualizacion del vector de estado
      NT += subtareas;
      NS += subtareas;

      if(NS > (N + 10)){
        //rechazo
        NR += subtareas;
        NS -= subtareas;
      }else{
        //calculos auxiliares
        stll += (tiempo * subtareas);

        if((NS - subtareas) < N){
          puestos_a_ocupar = (NS <= N) ? subtareas : (subtareas - (NS - N));

          for(i = 0; i < puestos_a_ocupar; i++){
            //evento futuro condicionado
            puesto_libre = posicion_puesto_libre(tps, N);
            sto += (tiempo - ito[puesto_libre]);
            tps[puesto_libre] = tiempo + tiempo_de_atencion_empleado();
          }
        }
      } 
    }else{
      //salida

      //avance del tiempo
      tiempo = tps[proximo_puesto_a_liberarse];

      //actualizacion del vector de estado
      NS--;

      tiempo_atencion_tester_a_tarea = tiempo_de_atencion_tester();
      if(tiempo > tc_tester){
        tc_tester = tiempo + tiempo_atencion_tester_a_tarea;
      }else{
        tc_tester += tiempo_atencion_tester_a_tarea;
      }

      //calculos auxiliares
      sts += tc_tester;

      if(NS >= N){
        //evento futuro condicionado
        tps[proximo_puesto_a_liberarse] += tiempo_de_atencion_empleado();
      }else{
        tps[proximo_puesto_a_liberarse] = HV;
        ito[proximo_puesto_a_liberarse] = tiempo;
      }
    }

    if(tiempo >= tiempo_final)
      tpll = HV;
  }

  for(i = 0; i < N; i++)
    sto += tiempo - ito[i];

  calculo_de_resultados(N, stll, sts, sto, tiempo, NR, NT, &pto, &pps, &ptr);
  imprimir_resultados(pto, pps, ptr, N);
}

void calculo_de_resultados(int empleados, double stll, double sts, double sto, double tiempo, int NR, int NT, double* pto, double* pps, double* ptr){
  *pto = (sto / empleados) / tiempo;

  *pps = (sts - stll) / (NT - NR);

  *ptr = (float)NR / NT;
}

void imprimir_resultados(double pto, double pps, double ptr, int empleados){
  printf("Teniendo %d empleados:\n", empleados);

  printf("El porcentaje de tiempo ocioso es de %f %%.\n", pto * 100);

  printf("El promedio de permanencia en el sistema es de %f minutos.\n", pps);

  printf("El porcentaje de tareas rechazadas es de %f %%.\n", ptr * 100);
}

int menor(double* tiempos, int cantidad){
  int menor_puesto = 0, i;

  for(i = 1; i < cantidad; i++)
    if(tiempos[i] < tiempos[menor_puesto])
      menor_puesto = i;
  
  return menor_puesto;
}

int posicion_puesto_libre(double* puestos, int cantidad){
  int i;
  for(i = 0; i < cantidad; i++)
    if(puestos[i] == HV)
      return i;
  return -1;
}

double random_real(){
  return ((float)rand()) / RAND_MAX;
}

double intervalo_entre_arribos(){
  //uniforme
  return (random_real() * 25) + 5;
}

double tiempo_de_atencion_empleado(){
  //por funcion inversa
  return 60 * sqrt(1 + (3 * random_real()));
}

double tiempo_de_atencion_tester(){
  //por rechazo
  double m = 100/333, random1, random2;
  while(1){
    random1 = m * random_real();
    random2 = (random_real() * 9) + 1;
    if(random1 <= (random2 * random2 / 333))
      return random2;
  }
}
