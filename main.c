#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>

#include "basicUtilities.h"
#include "unboundedqueue.h"

#define MAX 80
#define PORT 6398
#define SA struct sockaddr
#define socket_buffer_length 1024
#define max_filename_length 255

// Struct per i parametri del thread worker generico
typedef struct {
  Queue_t *q1; // coda1, qui inserirò i nomi dei file da elaborare
  // int N;			//dimensione file
  char *basepath; // path iniziale per il main thread
  int W;          // numero worker
  int socketfd;
} arg_t;

int socketConnection() {
  int sockfd;
  struct sockaddr_in servaddr;

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  IFERROR(sockfd,"socket creation failed...\n");
  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);
  if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0) {
    printf("connection with the server failed...\n");
    exit(0);
  }
  return sockfd;
}

void *workerFun(void *arg) {
  arg_t *args = (arg_t *)arg;
  while (1) {
    char *path = (char *) pop(args->q1);
    if(path == NULL){ perror(" Unable to pop from queue"); exit(-1); }
    if (strcmp(path, "fine") == 0) {
      break;
    }
    if (strstr(path, ".dat") != NULL) {
      fileElaborato f = elaboraFile(path);
      char buffer[socket_buffer_length];
      snprintf(buffer, sizeof(char) * 1024, "%d\t%f\t%f\t%s\n", f.dimensione,
               f.media, f.variazioneStandard, f.nomeFile);
      IFERROR(write(args->socketfd, &buffer, sizeof(buffer)), "Unable to write on socket");
      free(f.nomeFile);
    }
    free(path);
  }
  IFERROR(push(args->q1, "fine"),"Error pushing into queue");
  return NULL;
}

void scanDirForDatFiles(char *path, Queue_t *q1) {
  DIR *dir;
  struct dirent *entry;
  struct stat fileStat;

  dir = opendir(path);
  if (dir == NULL) {
    perror("opendir");
    exit(EXIT_FAILURE);
  }

  while ((entry = readdir(dir)) != NULL) {
    // Ignore "." and ".." entries
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);

    if (stat(fullPath, &fileStat) < 0) {
      perror("stat");
      exit(EXIT_FAILURE);
    }

    if (S_ISDIR(fileStat.st_mode)) {
      scanDirForDatFiles(fullPath, q1);
    } else {
      // If the entry is a file, check if it ends with ".dat"
      if (strstr(entry->d_name, ".dat") != NULL) {
        IFERROR(push(q1, strdup(fullPath)),"Unable to push into queue");
      }
    }
  }
  closedir(dir);
}

int main(int argc, char *argv[]) {
  if (argc != 3)
    exit(1);

  // Cartella principale
  char path[max_filename_length];
  strcpy(path, argv[1]);

  // Ora mi leggo quanti W ho
  int W;
  W = atoi(argv[2]);

  if (fork() == 0) {
    // inizializzo arg_t
    arg_t args;
    args.q1 = initQueue();
    args.basepath = path;
    args.W = W;
    args.socketfd =
        socketConnection(); // questo mi connette al socket e mi restituisce il
                            // file descriptor che mi è stato assegnato
    pthread_t thread[W];
    // starto i thread, che per costruzione di unboundedqueue saranno in attesa
    // che ci venga inserito qualcosa
    for (int i = 0; i < W; i++) {
      IFERROR(pthread_create(&thread[i], NULL, workerFun, &args),"Error creating threads...\n");
    }

    // MASTER START
    // Non sto creando un thread aggiuntivo per il master in quanto già di suo
    // questo pezzo di codice è un thread e per semplicità mi sta bene che resti
    // così, in quanto ho scritto una funzione ricorsiva e aggiungerei
    // complessità, senza effettivamente guardagnare alcun che
    scanDirForDatFiles(path, args.q1);

    char *fine = "fine";

    for (int i = 0; i < W; i++)
      IFERROR(push(args.q1, fine),"Error pushing into queue...\n");
    // MASTER END

    for (int i = 0; i < W; i++) {
      IFERROR(pthread_join(thread[i], NULL),"Error joining from thread...\n");
    }
    IFERROR(write(args.socketfd, fine, sizeof(fine)),"Error writing into socket...");
    deleteQueue(args.q1);
    close(args.socketfd);
    return 0;
  } else {
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // creo il socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    IFERROR(sockfd,"Socket creation failed...\n");

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET; // sto usando AF_INET
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0) {
      printf("socket bind failed...\n");
      exit(0);
    }

    if ((listen(sockfd, 10)) != 0) {
      printf("Listen failed...\n");
      exit(0);
    }
    len = sizeof(cli);

    connfd = accept(sockfd, (SA *)&cli, &len);
    if (connfd < 0) {
      printf("server accept failed...\n");
      exit(0);
    }
    printf("n\tavg\t\tstd\t\tfile\n");
    printf("-------------------------------------------------------------------"
           "---------------\n");
    while (1) {
      char buffer[socket_buffer_length];
      IFERROR(read(connfd, buffer, sizeof(buffer)),"Error reading from socket...\n");
      if (strcmp(buffer, "fine") == 0) {
        printf("Termine programma...");
        break;
      }
      printf("%s", buffer);
    }
    close(sockfd);
    IFERROR(wait(NULL),"Error wait...\n");
    return 0;
  }
}
