#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_NOMBRE 200
#define MAX_FECHA 7

#define BUENA 'B'
#define REGULAR 'R'
#define MALA 'M'
#define VALOR_BUENA 3
#define VALOR_REGULAR 2
#define VALOR_MALA 1

#define ERROR (-1)
#define EXITO 0

#define NOMBRE_IMPERIUS "imperius.dat"
#define NOMBRE_CRUCIO "crucio.dat"
#define NOMBRE_IMPERIUS_AUX "auximperius.dat"
#define NOMBRE_CRUCIO_AUX "auxcrucio.dat"

#define PREFIJO_LIBERADOS "liberados_"
#define EXTENSION_LIBERADOS ".txt"

typedef struct preso{
    char nombre[MAX_NOMBRE];
    unsigned int edad;
    char conducta; // B buena, R regular o M malo
    unsigned int pabellon;
    unsigned int celda;
    char fecha[MAX_FECHA]; // formato aaaamm
    int maldicion_realizada; // 1 crucio, 2 imperius
} preso_t;

/* 
 * Pre: Recibe una variable, una cadena de caracteres, la cual representa el nombre que el usuario le asigna al archivo perdonables.
 * Post: Genera un archivo con los presos que no realizaron ambas maldiciones, Devuelve 0 si la operacion fue exitosa o -1 si hubo algun error entre archivos.
 */
int comando_perdonables(char nombre_perdonables[]){

    preso_t preso_imperius;
    preso_t preso_crucio;

    FILE *archivo_imperius = fopen(NOMBRE_IMPERIUS, "rb");
    if(!archivo_imperius){
        return ERROR;
    }
    FILE *archivo_crucio = fopen(NOMBRE_CRUCIO, "rb");
    if(!archivo_crucio){
        fclose(archivo_imperius);
        return ERROR;
    }
    FILE *archivo_perdonables = fopen(nombre_perdonables, "wb");
    if(!archivo_perdonables){
        fclose(archivo_imperius);
        fclose(archivo_crucio);
        return ERROR;
    }

    size_t leer_imperius = fread(&preso_imperius, sizeof(preso_t), 1, archivo_imperius);
    size_t leer_crucio = fread(&preso_crucio, sizeof(preso_t), 1, archivo_crucio);

    while(leer_imperius && leer_crucio){

        if(strcmp(preso_imperius.nombre, preso_crucio.nombre) == 0){
            leer_imperius = fread(&preso_imperius, sizeof(preso_t), 1, archivo_imperius);
            leer_crucio = fread(&preso_crucio, sizeof(preso_t), 1, archivo_crucio);
        }
        else if(strcmp(preso_imperius.nombre, preso_crucio.nombre) < 0){
            fwrite(&preso_imperius, sizeof(preso_t), 1, archivo_perdonables);
            leer_imperius = fread(&preso_imperius, sizeof(preso_t), 1, archivo_imperius);
        }
        else{
            fwrite(&preso_crucio, sizeof(preso_t), 1, archivo_perdonables);
            leer_crucio = fread(&preso_crucio, sizeof(preso_t), 1, archivo_crucio);
        }

    }

    while(leer_imperius){
        fwrite(&preso_imperius, sizeof(preso_t), 1, archivo_perdonables);
        leer_imperius = fread(&preso_imperius, sizeof(preso_t), 1, archivo_imperius);
    }

    while(leer_crucio){
        fwrite(&preso_crucio, sizeof(preso_t), 1, archivo_perdonables);
        leer_crucio = fread(&preso_crucio, sizeof(preso_t), 1, archivo_crucio);
    }

    fclose(archivo_imperius);
    fclose(archivo_crucio);
    fclose(archivo_perdonables);

    return EXITO;

}

/* 
 * Pre: Recibe una variable, un caracter, el cual representa la conducta.
 * Post: Le asigna un valor al caracter recibido segun los parametros preestablecidos.
 */
int asignar_valor_char(char caracter){

	if(caracter == BUENA)
		return VALOR_BUENA;
	else if(caracter == REGULAR)
		return VALOR_REGULAR;
	else
		return VALOR_MALA;

}

/* 
 * Pre: Recibe 3 variables, la es una cadena de caracteres que representa la fecha enviada por el usuario, la segunda un caracter enviado por el usuario que representa la conducta y la tercera una cadena de caracteres la cual representa el nombre que el usuario le asigna al archivo perdonables.
 * Post: Crea un archivo de liberados los cuales son presos que cumplen los requisitos de fecha y conducta previamente enviados, Devuelve 0 si la operacion fue exitosa o -1 si hubo algun error entre archivos.
 */
int comando_liberar(char fecha[], char conducta, char nombre_perdonables[]){

    char nombre_liberados[MAX_NOMBRE] = PREFIJO_LIBERADOS;
    strcat(nombre_liberados, fecha);
    strcat(nombre_liberados, EXTENSION_LIBERADOS);
    preso_t preso_perdonable;

    FILE *archivo_perdonables = fopen(nombre_perdonables, "rb");
    if(!archivo_perdonables){
        return ERROR;
    }

    FILE *file_check;
    if((file_check = fopen(nombre_liberados, "r"))){
        fclose(archivo_perdonables);
        fclose(file_check);
        return ERROR;
    }

    FILE *archivo_liberados = fopen(nombre_liberados, "w");
    if(!archivo_liberados){
        fclose(archivo_perdonables);
        return ERROR;
    }

    size_t leer_binario = fread(&preso_perdonable, sizeof(preso_t), 1, archivo_perdonables);

    while(leer_binario){
        
        if(((strcmp(preso_perdonable.fecha, fecha) == 0) || (strcmp(preso_perdonable.fecha, fecha) < 0)) && (asignar_valor_char(preso_perdonable.conducta) >= asignar_valor_char(conducta))){
            fprintf(archivo_liberados, "%s\n", preso_perdonable.nombre);
        }
        leer_binario = fread(&preso_perdonable, sizeof(preso_t), 1, archivo_perdonables);

    }

    fclose(archivo_perdonables);
    fclose(archivo_liberados);

    return EXITO;

}

/* 
 * Pre: Recibe una variable, una cadena de caracteres, que representa la fecha enviada por el usuario.
 * Post: Actualiza los archivos que contienen a los presos elimiando aquellos que estan en el archivo de liberados de la fecha enviada, Devuelve 0 si la operacion fue exitosa o -1 si hubo algun error entre archivos.
 */
int comando_actualizar(char fecha[]){

    char nombre_liberados[MAX_NOMBRE] = PREFIJO_LIBERADOS;
    strcat(nombre_liberados, fecha);
    strcat(nombre_liberados, EXTENSION_LIBERADOS);

    preso_t preso_imperius;
    preso_t preso_crucio;
    char nombre_liberado[MAX_NOMBRE];

    FILE *archivo_imperius = fopen(NOMBRE_IMPERIUS, "rb");
    if(!archivo_imperius){
        return ERROR;
    }
    FILE *archivo_crucio = fopen(NOMBRE_CRUCIO, "rb");
    if(!archivo_crucio){
        fclose(archivo_imperius);
        return ERROR;
    }
    FILE *aux_imperius = fopen(NOMBRE_IMPERIUS_AUX, "wb");
    if(!aux_imperius){
        fclose(archivo_imperius);
        fclose(archivo_crucio);
        return ERROR;
    }
    FILE *aux_crucio = fopen(NOMBRE_CRUCIO_AUX, "wb");
    if(!aux_crucio){
        fclose(archivo_imperius);
        fclose(archivo_crucio);
        fclose(aux_imperius);
        return ERROR;
    }
    FILE *archivo_liberados = fopen(nombre_liberados, "r");
    if(!archivo_liberados){
        fclose(archivo_imperius);
        fclose(archivo_crucio);
        fclose(aux_imperius);
        fclose(aux_crucio);
        return ERROR;
    }

    size_t leer_imperius = fread(&preso_imperius, sizeof(preso_t), 1, archivo_imperius);
    size_t leer_crucio = fread(&preso_crucio, sizeof(preso_t), 1, archivo_crucio);
    int leer_liberados = fscanf(archivo_liberados, "%[^\n]\n", nombre_liberado);

    while(leer_imperius && leer_crucio && (leer_liberados > 0)){

        if(strcmp(preso_imperius.nombre, preso_crucio.nombre) < 0){
            if(strcmp(nombre_liberado, preso_imperius.nombre) == 0){
                leer_imperius = fread(&preso_imperius, sizeof(preso_t), 1, archivo_imperius);
                leer_liberados = fscanf(archivo_liberados, "%[^\n]\n", nombre_liberado);
            }
            else{
                fwrite(&preso_imperius, sizeof(preso_t), 1, aux_imperius);
                leer_imperius = fread(&preso_imperius, sizeof(preso_t), 1, archivo_imperius);
            }
        }
        else{
            if(strcmp(nombre_liberado, preso_crucio.nombre) == 0){
                leer_crucio = fread(&preso_crucio, sizeof(preso_t), 1, archivo_crucio);
                leer_liberados = fscanf(archivo_liberados, "%[^\n]\n", nombre_liberado);
            }
            else{
                fwrite(&preso_crucio, sizeof(preso_t), 1, aux_crucio);
                leer_crucio = fread(&preso_crucio, sizeof(preso_t), 1, archivo_crucio);
            }
        }

    }

    while(leer_imperius && (leer_liberados > 0)){

        if(strcmp(nombre_liberado, preso_imperius.nombre) == 0){
            leer_imperius = fread(&preso_imperius, sizeof(preso_t), 1, archivo_imperius);
            leer_liberados = fscanf(archivo_liberados, "%[^\n]\n", nombre_liberado);
        }
        else{
            fwrite(&preso_imperius, sizeof(preso_t), 1, aux_imperius);
            leer_imperius = fread(&preso_imperius, sizeof(preso_t), 1, archivo_imperius);
        }

    }

    while(leer_crucio && (leer_liberados > 0)){

        if(strcmp(nombre_liberado, preso_crucio.nombre) == 0){
            leer_crucio = fread(&preso_crucio, sizeof(preso_t), 1, archivo_crucio);
            leer_liberados = fscanf(archivo_liberados, "%[^\n]\n", nombre_liberado);
        }
        else{
            fwrite(&preso_crucio, sizeof(preso_t), 1, aux_crucio);
            leer_crucio = fread(&preso_crucio, sizeof(preso_t), 1, archivo_crucio);
        }

    }

    while(leer_crucio && leer_imperius){

        if(strcmp(preso_imperius.nombre, preso_crucio.nombre) < 0){
            fwrite(&preso_imperius, sizeof(preso_t), 1, aux_imperius);
            leer_imperius = fread(&preso_imperius, sizeof(preso_t), 1, archivo_imperius);
        }
        else{
            fwrite(&preso_crucio, sizeof(preso_t), 1, aux_crucio);
            leer_crucio = fread(&preso_crucio, sizeof(preso_t), 1, archivo_crucio);
        }

    }

    while(leer_imperius){
        fwrite(&preso_imperius, sizeof(preso_t), 1, aux_imperius);
        leer_imperius = fread(&preso_imperius, sizeof(preso_t), 1, archivo_imperius);
    }

    while(leer_crucio){
        fwrite(&preso_crucio, sizeof(preso_t), 1, aux_crucio);
        leer_crucio = fread(&preso_crucio, sizeof(preso_t), 1, archivo_crucio);
    }

    fclose(archivo_imperius);
    fclose(archivo_crucio);
    fclose(aux_imperius);
    fclose(aux_crucio);
    fclose(archivo_liberados);

    remove(NOMBRE_IMPERIUS);
    remove(NOMBRE_CRUCIO);
    rename(NOMBRE_IMPERIUS_AUX, NOMBRE_IMPERIUS);
    rename(NOMBRE_CRUCIO_AUX, NOMBRE_CRUCIO);

    return EXITO;

}

/* 
 * Pre: Recibe una variable, una cadena de caracteres, que representa la fecha enviada por el usuario
 * Post: Imprime en pantalla los presos que han sido liberados segun la fecha recibida, Devuelve 0 si la operacion fue exitosa o -1 si hubo algun error entre archivos.
 */
int comando_mostrar_liberados(char fecha[]){

    char nombre_liberados[MAX_NOMBRE] = PREFIJO_LIBERADOS;
    strcat(nombre_liberados, fecha);
    strcat(nombre_liberados, EXTENSION_LIBERADOS);

    char nombre[MAX_NOMBRE];

    FILE *archivo_liberados = fopen(nombre_liberados, "r");
    if(!archivo_liberados){
        return ERROR;
    }

    printf("\n");
    printf("Presos liberados:\n");
    printf("\n");

    int leidos = fscanf(archivo_liberados, "%[^\n]\n", nombre);
    while(leidos > 0){
        printf("Nombre: %s\n", nombre);
        leidos = fscanf(archivo_liberados, "%[^\n]\n", nombre);
    }

    printf("\n");

    fclose(archivo_liberados);

    return EXITO;

}

void comando_ayuda();

/* 
 * Pre: -
 * Post: Imprime en pantalla la ayuda relacionada sobre el comando perdonables.
 */
void comando_ayuda_perdonables(){

    printf("\n********************************************************\n");
    printf("COMANDO PERDONABLES:\n\n");
    printf("Funcionalidad: Dado 2 archivos binarios, imperius y crucio, genera un tercer archivo nombrado por el usuario el cual contiene a los presos de ambos archivos previamente mencionados.\n");
    printf("Nota: Si un preso esta en ambos archivos NO sera ingresado al archivo perdonables, ya que solo debe haber sido condenado por una maldicion para ser apto.\n\n");
    printf("Requisitos Previos: Archivo imperius.dat ; Archivo crucio.dat.\n\n");
    printf("Argumentos:\n\n");
    printf("1. ./azkaban : Indica el nombre del ejecutable y lo ejecuta.\n");
    printf("2. perdonables : Es el comando identificador, indica al programa que funcionalidad usted desea utilizar, en este caso la funcion perdonables.\n");
    printf("3. <nombre_archivo> : Aqui debe ingresar el nombre deseado, seguido de la extension, del archivo destino.\n\n");
    printf("Ejemplo de ejecucion: ./azkaban perdonables ejemplo.dat\n\n");
    printf("********************************************************\n");
    comando_ayuda();

}

/* 
 * Pre: -
 * Post: Imprime en pantalla la ayuda relacionada sobre el comando liberar.
 */
void comando_ayuda_liberar(){

    printf("\n********************************************************\n");
    printf("COMANDO LIBERAR:\n\n");
    printf("Funcionalidad: Se creara un archivo de texto con los nombres de los presos que cumplan las condiciones necesarias, es decir, fecha de liberacion menor o igual a la ingresada (cuarto argumento) y conducta igual o mejor a la ingresada (quinto argumento).\n\n");
    printf("Requisitos Previos: Previa ejecucion del comando perdonables o el archivo generado del mismo.\n\n");
    printf("Argumentos:\n\n");
    printf("1. ./azkaban : Indica el nombre del ejecutable y lo ejecuta.\n");
    printf("2. liberar : Es el comando identificador, indica al programa que funcionalidad usted desea utilizar, en este caso la funcion liberar.\n");
    printf("3. <archivo_perdonables.dat> : Indica el nombre y la extension del archivo perdonables a utilizar.\n");
    printf("4. fecha : Ingrese la fecha de liberacion minima que los presos deben cumplir (formato aaaamm)\n");
    printf("5. conducta : Ingrese la conducta necesaria que los presos deban cumplir para su liberacion (caracteres validos 'B';'R';'M')\n\n");
    printf("Ejemplo de ejecucion: ./azkaban liberar perdonables.dat 200012 B\n\n");
    printf("********************************************************\n");
    comando_ayuda();

}

/* 
 * Pre: -
 * Post: Imprime en pantalla la ayuda relacionada sobre el comando actualizar.
 */
void comando_ayuda_actualizar(){

    printf("\n********************************************************\n");
    printf("COMANDO ACTUALIZAR:\n\n");
    printf("Funcionalidad: Al ejecutar este comando el programa actualizara los archivos crucio e imperius quitando aquellos presos que esten en el archivo liberados el cual variara segun la fecha ingresada.\n\n");
    printf("Requisitos Previos: Archivo imperius.dat ; Archivo crucio.dat ; Archivo liberados_aaaamm.txt.\n\n");
    printf("Argumentos:\n\n");
    printf("1. ./azkaban : Indica el nombre del ejecutable y lo ejecuta.\n");
    printf("2. actualizar : Es el comando identificador, indica al programa que funcionalidad usted desea utilizar, en este caso la funcion actualizar.\n");
    printf("3. fecha : Aqui debe ingresar la fecha deseada la cual coincida con algun archivo liberados del formato liberados_aaamm.txt existente.\n\n");
    printf("Ejemplo de ejecucion: ./azkaban actualizar 20012\n\n");
    printf("********************************************************\n");
    comando_ayuda();

}

/* 
 * Pre: -
 * Post: Imprime en pantalla la ayuda relacionada sobre el comando mostrar liberados.
 */
void comando_ayuda_mostrar_liberados(){

    printf("\n********************************************************\n");
    printf("COMANDO MOSTRAR LIBERADOS:\n\n");
    printf("Funcionalidad: Al ejecutar este comando el programa mostrara aquellos presos que han sido liberados segun la fecha ingresada.\n\n");
    printf("Requisitos Previos: Archivo liberados_aaaamm.txt.\n\n");
    printf("Argumentos:\n\n");
    printf("1. ./azkaban : Indica el nombre del ejecutable y lo ejecuta.\n");
    printf("2. mostrar_liberados : Es el comando identificador, indica al programa que funcionalidad usted desea utilizar, en este caso la funcion mostrar liberados.\n");
    printf("3. fecha : Aqui debe ingresar la fecha deseada la cual coincida con algun archivo liberados del formato liberados_aaamm.txt existente.\n\n");
    printf("Ejemplo de ejecucion: ./azkaban mostrar_liberados 20012\n\n");
    printf("********************************************************\n");
    comando_ayuda();

}

/* 
 * Pre: -
 * Post: Imprime en pantalla el apartado de ayuda general, conteniendo en si una interfaz interactuable que incluye las instrucciones de cada comando especifico.
 */
void comando_ayuda(){

    int opcion;
    printf("\nBienvenido al apartado de ayuda, seleccione un comando para obtener informacion:\n\n");
    printf("[1] Comando Perdonables\n");
    printf("[2] Comando Liberar\n");
    printf("[3] Comando Actualizar\n");
    printf("[4] Comando Mostrar Liberados\n");
    printf("[5] Salir\n\n");
    printf("Ingrese el numero correspondiente por favor: ");
    scanf("%i", &opcion);

    switch (opcion){

        case 1:
            comando_ayuda_perdonables();
            break;
        case 2:
            comando_ayuda_liberar();
            break;
        case 3:
            comando_ayuda_actualizar();
            break;
        case 4:
            comando_ayuda_mostrar_liberados();
            break;
        default:
            break;

    }

}

/* 
 * Pre: Recibe 2 variables las cuales son enviadas por el usuario via consola en el momento de la ejecucion del programa, la primera representa la cantidad de argumentos enviados mientras la segunda es un vector de caracteres que almacena los argumentos en si.
 * Post: Segun el comando ejecutado mostrara al usuario una de las funciones previamente comentadas. Devuelve el valor de 0 si la ejecucion se finalizo con exito o imprime la ayuda al usuario si este cometio algun error durante la ejecucion del mismo.
 */
int main(int argc, char *argv[]){

    if(argc < 2){
        printf("\nERROR. Faltan argumentos o los mismos no son validos , por favor lea el apartado de ayuda necesario y vuelvalo a intentar.\n");
        comando_ayuda();
    }

    char nombre_perdonables[MAX_NOMBRE];
    int estado = EXITO;

    if(argc == 3 && (strcmp(argv[1], "perdonables") == 0)){
        strcpy(nombre_perdonables, argv[2]);
        estado = comando_perdonables(nombre_perdonables);
        if(estado != EXITO){
            printf("\nERROR. Los archivos necesarios no existen, por favor lea el apartado de ayuda necesario y vuelvalo a intentar.\n");
            comando_ayuda_perdonables();
        }
    }
    else if(argc < 3 && (strcmp(argv[1], "perdonables") == 0)){
        printf("\nERROR. Faltan argumentos o los mismos no son validos , por favor lea el apartado de ayuda necesario y vuelvalo a intentar.\n");
        comando_ayuda_perdonables();
    }

    if(argc == 5 && (strcmp(argv[1], "liberar") == 0)){
        strcpy(nombre_perdonables, argv[2]);
        estado = comando_liberar(argv[3], argv[4][0], nombre_perdonables);
        if(estado != EXITO){
            printf("\nERROR. Los archivos necesarios no existen o el archivo que intenta crear ya existe, por favor lea el apartado de ayuda necesario y vuelvalo a intentar.\n");
            comando_ayuda_liberar();
        }
    }
    else if(argc < 5 && (strcmp(argv[1], "liberar") == 0)){
        printf("\nERROR. Faltan argumentos o los mismos no son validos , por favor lea el apartado de ayuda necesario y vuelvalo a intentar.\n");
        comando_ayuda_liberar();
    }

    if(argc == 3 && (strcmp(argv[1], "actualizar") == 0)){
        estado = comando_actualizar(argv[2]);
        if(estado != EXITO){
            printf("\nERROR. Los archivos necesarios no existen, por favor lea el apartado de ayuda necesario y vuelvalo a intentar.\n");
            comando_ayuda_actualizar();
        }        
    }
    else if(argc < 3 && (strcmp(argv[1], "actualizar") == 0)){
        printf("\nERROR. Faltan argumentos o los mismos no son validos , por favor lea el apartado de ayuda necesario y vuelvalo a intentar.\n");
        comando_ayuda_actualizar();
    }

    if(argc == 3 && (strcmp(argv[1], "mostrar_liberados") == 0)){
        estado = comando_mostrar_liberados(argv[2]);
        if(estado != EXITO){
            printf("\nERROR. Los archivos necesarios no existen, por favor lea el apartado de ayuda necesario y vuelvalo a intentar.\n");
            comando_ayuda_mostrar_liberados();
        }
    }
    else if(argc < 3 && (strcmp(argv[1], "mostrar_liberados") == 0)){
        printf("\nERROR. Faltan argumentos o los mismos no son validos , por favor lea el apartado de ayuda necesario y vuelvalo a intentar.\n");
        comando_ayuda_mostrar_liberados();
    }

    if(argc == 2 && (strcmp(argv[1], "ayuda") == 0)){
        comando_ayuda();
    }

    return EXITO;

}