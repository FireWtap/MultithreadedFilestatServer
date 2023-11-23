//
// Created by Francesco on 21/02/2023.
//

#ifndef ESAME_BASH_POSIX_BASICUTILITIES_H
#define ESAME_BASH_POSIX_BASICUTILITIES_H
#define IFERROR(s, m)                                                          \
  if ((s) == -1) {                                                             \
    perror(m);                                                                 \
    exit(errno);                                                               \
  }
typedef struct fileelaborato{
    int dimensione;
    double media;
    double variazioneStandard;
    char* nomeFile;
}fileElaborato;

int readFile(char* file, double** values);
double calcolaMedia(double* values, int values_n);
double calcolaVariazioneStandard(double* values, int values_n);
char* basename(char* path);
fileElaborato elaboraFile(char* path);
void SYSERROR(int result, const char* msg);


#endif //ESAME_BASH_POSIX_BASICUTILITIES_H
