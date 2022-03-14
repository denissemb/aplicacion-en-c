#define _CRT_SECURE_NO_WARNINGS

#include "p3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAXLEN 255
#define MAXSIZE 256


int main() {
    char archivo[MAXLEN];
    char interface[MAXLEN];
    
    // deleteFiles();
    readFile(archivo);
    showConfigAdapter(&interface);
    showAdapterData("adapterInfo.txt");
    readFileAndPing(archivo);
    printf("\nComparamos velocidades medias de los que han dado correctamente\n");
    readFileAndDNS("pingCorrecto.txt");
    mergeTwoFiles("dnsInterfaz.txt", "fastestDNS.txt", "finalBattle.txt");
    printf("\nComparamos velocidades medias del ganador con el de la configuracion actual del adaptador\n");
    readFileAndDNS("finalBattle.txt");
    readFileAndChangeAdapter("finalBattle.txt", interface);
    deleteFiles();

    return 0;
}
// añadir comentario//
