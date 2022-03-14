#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXLEN 255
#define MAXSIZE 256

void readFile(char* path);
bool readFileAndPrint(const char* archivo);
void readFileAndPing(const char* archivo);
void readFileAndDNS(const char* archivo);
int  readTextWithSpaces(char* text, int n);
void showConfigAdapter(char* interface);
void showAdapter(char* interface);
void showAdapterData(char* rutaFicheroRespuestaComandoRed);
void makePing(const char* ip, FILE* fichero);
void pingDNS(char* dns, char* file);
int getPingSpeedResult(char* file, char* dns);
void mergeTwoFiles(char* file1, char* file2, char* file3);
void readFileAndChangeAdapter(const char* archivo, char* adapterName);
void changeAdapterConfig(char* name, char* dns);
void deleteFiles();


/*  Pide la ruta del archivo y llama a readFileAndPrint pasandole la ruta */
void readFile(char* path) {
    bool ask = false;
    printf("\n");
    do {
        printf("\nRuta del archivo? \n");
        scanf("%s", path);
        printf("\n");
        ask = readFileAndPrint(path);
    } while (!ask);
}

/*  Lee el archivo txt que recibe por parametro y pinta por pantalla cada linea */
bool readFileAndPrint(const char* archivo)
{
    FILE* f;
    char line[MAXLEN] = { 0 };

    f = fopen(archivo, "rt");
    if (f == NULL)
    {
        printf("Parece que el archivo no existe o no puede abrirse");
        return false;
    }
    else {
        while (fgets(line, MAXLEN, f))
        {
            printf(line);
        }
        fclose(f);
    }
    return true;
}

/*  Pide el nombre del adaptador de Red y llama al ShowAdapter pasandole el nombre */
void showConfigAdapter(char* interface) {
    printf("\n");
    printf("Introduzca la interfaz de red \n");
    printf("\n");
    readTextWithSpaces(interface, 100);
    showAdapter(interface);
    printf("\n");
}

/*  Guarda la configuracion del adaptador de red y lo mete en un fichero llamado adapterInfo.txt */
void showAdapter(char* interface) {
    /* Muestra por pantalla la configuración de red del adaptador */
    char order[MAXSIZE];
    sprintf(order,
        "netsh interface ip show config %s > adapterInfo.txt", interface);
    int err = system(order);
    if (err) {
        printf("Ha habido un error\n");
       // exit(-1);
    }
}

/*
    Lee la configuracion del adaptador que está en un archivo que se le pasa por parametro y
    guarda en otro archivo llamado dnsInterfaz.txt la dns del adaptador
*/
void showAdapterData(char* rutaFicheroRespuestaComandoRed) {
    FILE* interfazRedP;
    char respuesta[100];
    char* res;
    int i = 0;
    bool interfazEncontrada = false;

    interfazRedP = fopen(rutaFicheroRespuestaComandoRed, "r");

    if (interfazRedP == NULL) {
        printf("Error abriendo el fichero de datos de la interfaz de red \n");
    }
    else {
        while (feof(interfazRedP) == 0) {
            fgets(respuesta, 100, interfazRedP);
            // Condición donde se comprueba la existencia de las palabras claves a buscar
            char* strMedia = strstr(respuesta, "Servidores DNS configurados");
            if (strMedia != NULL) {
                res = strrchr(respuesta, ': ') + 1;

                // Creamos un archivo que contiene el dns de la interfaz
                FILE* fichero;
                fichero = fopen("dnsInterfaz.txt", "wt");
                fputs(res, fichero);
                fclose(fichero);

                interfazEncontrada = true;
            }
        }
        fclose(interfazRedP);
        if (!interfazEncontrada) {
            printf("Error - No se ha encontrado la interfaz seleccionada \n");
        }
    }
}

/*  Lee el archivo txt que recibe por parametro y llama a la funcion makePing para cada una de sus lineas */
void readFileAndPing(const char* archivo)
{
    FILE* f;
    FILE* fichero;
    char line[MAXLEN] = { 0 };
    f = fopen(archivo, "rt");

    if (f == NULL)
    {
        printf("Error al abrir el archivo .txt\n");
    }
    else
    {
        fichero = fopen("pingCorrecto.txt", "wt");
        if (fichero == NULL)
        {
            printf("Error al crear el archivo pingCorrecto.txt\n");
            return;
        }
        else {
            while (fgets(line, MAXLEN, f))
                
            {
                makePing(line, fichero);
            }
            fclose(fichero);
        }
        fclose(f);
    }

}

/*
    Hace ping al pasarle una direccion ip y guarda las ips cuyo ping hayan sido satisfactorios,
    en un archivo que se le pasa por parametro
*/
void makePing(const char* ip, FILE* fichero)
{
    char line[MAXLEN] = { 0 };
    char command[MAXLEN] = { 0 };
    FILE* fp;

    snprintf(command, MAXLEN, "ping %s", ip);
    fp = _popen(command, "r");
    if (fp == NULL)
    {
        /*Ocuparse del error */
        return;
    }
    bool isSuccess = false;
    printf("\n%s", ip);
    while (fgets(line, MAXLEN, fp) != NULL)
    {
        /*Verifica aqui la respuesta del host */
        printf(".");
        if (!isSuccess) {
            isSuccess = strstr(line, "perdidos = 0");
        }
    }
    if (isSuccess) {
        printf(" PING CORRECTO!\n");
        fputs(ip, fichero);
    }
    else {
        printf(" PING INCORRECTO!\n");
    }
    _pclose(fp);
}

/*  Lee el archivo que se le pasa por parametro, y pasa cada linea del archivo, llama a la funcion
    getPingSpeedResult que devuelve la velocidad del ping que previamente se ha hecho llamando a la funcion pingDNS.
    Se mira cual es más rapido y se guarda en un fichero llamado fastestDNS
*/
void readFileAndDNS(char* archivo) {
    FILE* f;

    char line[MAXLEN] = { 0 };
    int currentSpeed = 999999999;
    int minSpeed = 1000000;
    char minDNS[MAXLEN] = { 0 };
    f = fopen(archivo, "rt");

    if (f == NULL)
    {
        printf("Error al abrir el archivo\n");
    }
    else
    {
        while (fgets(line, MAXLEN, f))
        {
            line[strcspn(line, "\n")] = 0;
            pingDNS(line, "dnsPing.txt");
            currentSpeed = getPingSpeedResult("dnsPing.txt", line);
            if (currentSpeed < minSpeed) {
                minSpeed = currentSpeed;
                strcpy(minDNS, line);
            }
            //remove("dnsPing.txt");
        }
        printf("\n\nDNS mas rapida: %s .\nSu velocidad media es de: %d ms.\n\n", minDNS, minSpeed);

        // Creamos un archivo que contiene el dns más rapido
        FILE* fichero;
        fichero = fopen("fastestDNS.txt", "w");
        fputs(minDNS, fichero);
        fclose(fichero);
        fclose(f);
    }
}

/*  Hace ping de la DNS que se le pasa por parametro y guarda los resultados en el archivo que se le pasa */
void pingDNS(char* dns, char* file) {
    
    char comando[MAXLEN];
    printf("Haciendo ping a %s...\n", dns);
    snprintf(comando, MAXLEN, "ping %s > %s", dns, file);
    system(comando);
}

/*  Se le pasa un archivo y una direccion dns y retorna la velocidad media de los pings */
int getPingSpeedResult(char* file, char* dns) {
    FILE* fp = fopen(file, "r");
    printf("\n- Resultado de %s\n", dns);
    char order[MAXLEN];
    char* line;
    char* value = "100000";
    int currentSpeed = 0;
    if (fp == NULL) {
        printf("%s", file);
        printf("Archivo no encontrado\n");
    }
    else {
        while (fgets(order, MAXLEN, fp)) {
            line = strstr(order, "Media ");
            if (line != NULL) {
                char* separate = strtok(line, "ms");
                value = strtok(separate, "Media = ");

                if (value) {
                    printf("Velocidad media: %s ms\n\n", value);
                    //Convertir de String a entero
                    currentSpeed = (int)strtol(value, (char**)NULL, 10);
                    fclose(fp);
                }
            }
        }
        fclose(fp);
    }
    return currentSpeed;
}

/*  Se le pasan dos archivos y se mergean sus contenidos en uno nuevo */
void mergeTwoFiles(char* file1, char* file2, char* file3) {
    FILE* f1 = fopen(file1, "r");
    FILE* f2 = fopen(file2, "r");
    FILE* f3 = fopen(file3, "w");

    char c;

    if (f1 == NULL || f2 == NULL || f3 == NULL)
    {
        puts("Could not open files");
        exit(0);
    }
    while ((c = fgetc(f1)) != EOF) {
        fputc(c, f3);
    }
    while ((c = fgetc(f2)) != EOF) {
        fputc(c, f3);
    }

    fclose(f1);
    fclose(f2);
    fclose(f3);
}

/*  Sirve para obtener un texto con espacios */
int readTextWithSpaces(char* text, int n) {
    int i, r;

    r = getchar();
    if (r == EOF) {
        text[0] = '\0';
        return 0;
    }
    if (r == '\n') {
        i = 0;
    }
    else {
        text[0] = r;
        i = 1;
    }
    for (; i < n - 1 && (r = getchar()) != EOF && r != '\n'; i++) {
        text[i] = r;
    }
    text[i] = '\0';
    if (r != '\n' && r != EOF) {
        while ((r = getchar()) != '\n' && r != EOF);
    }
    return 1;
}

/*  Lee un archivo que contiene una direccion dns y llama a changeAdapterConfig */
void readFileAndChangeAdapter(const char* archivo, char* adapterName)
{
    FILE* f;
    FILE* fa;
    char line[MAXLEN] = { 0 };
    char aline[MAXLEN] = { 0 };

    f = fopen(archivo, "rt");
    if (f == NULL)
    {
        printf("Error al abrir el archivo\n");
        return;
    }
    else {
        fa = fopen("dnsInterfaz.txt", "rt");
        if (fa != NULL) {
            fgets(aline, MAXLEN, fa);
            aline[strcspn(aline, "\n")] = 0;
            fgets(line, MAXLEN, f);
            line[strcspn(line, "\n")] = 0;
            if (strcmp(line, aline) != 0) {
                printf("\nVamos a cambiar el dns del adaptador\n");
                changeAdapterConfig(adapterName, line);
            }
            else {
                printf("\nLa mas rapida ya estaba configurada\n");
            }
            fclose(fa);
        }
        fclose(f);
    }
}

/*  Se le pasa un nombre de adaptador y una direccion dns y se cambia su configuracion */
void changeAdapterConfig(char* name, char* dns) {
    char order[MAXSIZE];
    sprintf(order,
        "netsh interface ipv4 add dnsservers \"%s\" address=%s index=1", name, dns);
    system(order);

    int er = system(order);
    if (er) {
        printf("Command failed!\n");
    }
    else {
        printf("\nConfiguracion óptima!\n");
    }
}

/*  Borra los archivos txt creados por el programa */
void deleteFiles() {
    remove("dnsInterfaz.txt");
    remove("fastestDNS.txt");
    remove("finalBattle.txt");
    remove("pingCorrecto.txt");
    remove("adapterInfo.txt");
    remove("dnsPing.txt");
}