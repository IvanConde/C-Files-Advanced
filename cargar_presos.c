#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_NOMBRE 200
#define MAX_FECHA 7

typedef struct preso{
    char nombre[MAX_NOMBRE];
    unsigned int edad;
    char conducta; // B buena, R regular o M malo
    unsigned int pabellon;
    unsigned int celda;
    char fecha[MAX_FECHA]; // formato aaaamm
    int maldicion_realizada; // 1 crucio, 2 imperius
} preso_t;

void cargar_prisioneros(){

    preso_t preso;
    preso_t imperius[100];
    preso_t crucio[100];
    size_t tope_imperius = 0;
    size_t tope_crucio = 0;

    FILE *archivo_presos = fopen("cargar_presos.txt", "r");
    if (!archivo_presos){
        printf("Error");
    }
    FILE *archivo_imperius = fopen("imperius.dat", "wb");
    if(!archivo_imperius){
        fclose(archivo_presos);
        printf("Error");
    }
    FILE *archivo_crucio = fopen("crucio.dat", "wb");
    if(!archivo_crucio){
        fclose(archivo_presos);
        fclose(archivo_imperius);
        printf("Error");
    }

    int leer = fscanf(archivo_presos, "%[^;];%i;%c;%i;%i;%[^;];%i\n", (preso.nombre), &(preso.edad), &(preso.conducta), &(preso.pabellon), &(preso.celda), (preso.fecha), &(preso.maldicion_realizada));

    while(leer > 0){

        if (preso.maldicion_realizada == 1){
            strcpy(crucio[tope_crucio].nombre, preso.nombre);
            crucio[tope_crucio].edad = preso.edad;
            crucio[tope_crucio].conducta = preso.conducta;
            crucio[tope_crucio].pabellon = preso.pabellon;
            crucio[tope_crucio].celda = preso.celda;
            strcpy(crucio[tope_crucio].fecha, preso.fecha);
            crucio[tope_crucio].maldicion_realizada = preso.maldicion_realizada;
            tope_crucio++;
        }
        else{
            strcpy(imperius[tope_imperius].nombre, preso.nombre);
            imperius[tope_imperius].edad = preso.edad;
            imperius[tope_imperius].conducta = preso.conducta;
            imperius[tope_imperius].pabellon = preso.pabellon;
            imperius[tope_imperius].celda = preso.celda;
            strcpy(imperius[tope_imperius].fecha, preso.fecha);
            imperius[tope_imperius].maldicion_realizada = preso.maldicion_realizada;
            tope_imperius++;
        }

        leer = fscanf(archivo_presos, "%[^;];%i;%c;%i;%i;%[^;];%i\n", (preso.nombre), &(preso.edad), &(preso.conducta), &(preso.pabellon), &(preso.celda), (preso.fecha), &(preso.maldicion_realizada));

    }

    preso_t aux;
    for(size_t i = 1; i < tope_imperius; i++){
        for(size_t j = 0; j < (tope_imperius-i); j++){
            if(strcmp(imperius[j].nombre, imperius[j+1].nombre) > 0){
                aux = imperius[j];
                imperius[j] = imperius[j+1];
                imperius[j+1] = aux;
            }
        }
    }
    for(size_t i = 1; i < tope_crucio; i++){
        for(size_t j = 0; j < (tope_crucio-i); j++){
            if(strcmp(crucio[j].nombre, crucio[j+1].nombre) > 0){
                aux = crucio[j];
                crucio[j] = crucio[j+1];
                crucio[j+1] = aux;
            }
        }
    }

    fwrite(&imperius, sizeof(preso_t), tope_imperius, archivo_imperius);
    fwrite(&crucio, sizeof(preso_t), tope_crucio, archivo_crucio);

    fclose(archivo_presos);
    fclose(archivo_imperius);
    fclose(archivo_crucio);

}

int main(){

    cargar_prisioneros();
    return 0;

}