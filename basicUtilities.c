//
// Created by Francesco on 21/02/2023.
//

#include "basicUtilities.h"
#include <ctype.h>
#include <dirent.h>
#include <errno.h> /* serve per errno */
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdio.h>  /* serve per la perror */
#include <stdlib.h> /* serve per la exit */
#include <string.h> /* serve per strlen */
#include <sys/stat.h>
#include <unistd.h> /* serve per la write */


#define BUFFER_SIZE 1024

void SYSERROR(int result, const char *msg) {
  if (result == -1) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
  }
}

char *strstrip(char *s) {
  size_t size;
  char *end;

  size = strlen(s);

  if (!size)
    return s;

  end = s + size - 1;
  while (end >= s && isspace(*end))
    end--;
  *(end + 1) = '\0';

  while (*s && isspace(*s))
    s++;

  return s;
}

int readFile(char *file, double **values) {
  int values_n = 0;
  FILE *fp;
  char line[256];
  fp = fopen(file, "r");
  if (fp == NULL) {
    printf("Failed to open the file.\n");
    exit(1);
  }
  while (fgets(line, sizeof(line), fp)) {

    char *strippedLine = strstrip(line);
    if(strcmp(strippedLine,"") == 0) break;
    double new_element = atof(strippedLine);
    values_n++;
    size_t new_size = values_n * sizeof(double);
    *values = (double *)realloc(*values, new_size);
    (*values)[values_n - 1] = new_element;
  }
  fclose(fp);
  return values_n;
} /*
 int readFile(char *file, double **values) {
 //    printf("Leggendo il file:%s", file);
     int fd; //File Descriptor
     char buffer[BUFFER_SIZE];
     int bytes_read;
     int values_n = 0;
     char fileName[256];
     IFERROR(fd = open(file, O_RDWR ^ O_APPEND), "Apro il file...");
     while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
         int line_lenght = 0; //Dimensione della riga che sto leggendo ( inizia
 da 0 ) char *line = calloc(1, sizeof(char)); //Linea che sto leggendo for (int
 i = 0; i < bytes_read + 1; i++) { if (buffer[i] == '\n') { if (strcmp(line, "")
 != 0) { char *strippedLine = strstrip(line); if (strcmp(strippedLine, "") != 0)
 { double new_element = atof(strippedLine); values_n++; size_t new_size =
 values_n * sizeof(double); *values = (double *) realloc(*values, new_size);
                         (*values)[values_n - 1] = new_element;
                     }
                     free(line);
                     line = calloc(1, sizeof(char));
                     line_lenght = 0;
                 }

             } else {
                 line_lenght++;
                 char *new_line = realloc(line, line_lenght * sizeof(char));

                 if (new_line == NULL) {
                     exit(1);
                 }
                 line = new_line;

                 line[line_lenght - 1] = buffer[i];
             }
         }
         free(line);
     }
     close(fd);
     return values_n;
 }
 */

double calcolaMedia(double *values, int values_n) {
  double sum = 0.0;
  for (int i = 0; i < values_n; i++) {
    sum += values[i];
  }
  return sum / (double)values_n;
}

double calcolaVariazioneStandard(double *values, int values_n) {
  double media = calcolaMedia(values, values_n);
  double sum = 0.0;
  for (int i = 0; i < values_n; i++) {
    sum += pow((values[i] - media), 2);
  }
  sum = sum / values_n;
  return sqrt(sum);
}

fileElaborato elaboraFile(char *path) {
  fileElaborato f;
  double *values = NULL;
  f.nomeFile = strdup(path);
  int num_values = readFile(path, &values);
  f.dimensione = num_values;
  f.media = calcolaMedia(values, num_values);
  f.variazioneStandard = calcolaVariazioneStandard(values, num_values);
  free(values);
  return f;
}
char *basename(char *path) {
  char *base = strrchr(path, '/');
  return base == NULL ? path : base + 1;
}
