#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define LENGTH 2048

// Global variables
volatile sig_atomic_t flag = 0;//exit*****************************************
int sockfd = 0;
char name[32];

void choisir_level(){
	printf("==> Choisissez le niveau : <== \n");
	printf("[+] Niveau 1 : Facile \n");
	printf("[+] Niveau 2 : Moyen \n");
	printf("[+] Niveau 3 : Difficile \n");
}
/*
void Qniveaux1(int i){
	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("niveau2.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

   if (i == 0) {
	   read = getline(&line, &len, fp);
        printf("%s", line);
   }
   if (i == 1) {
	   read = getline(&line, &len, fp);
        printf("%s", line);
   }
   if (i == 2) {
	   read = getline(&line, &len, fp);
        printf("%s", line);
   }
   if (i == 3) {
	   read = getline(&line, &len, fp);
        printf("%s", line);
   }
fclose(fp);
    if (line)
        free(line);
}

void Qniveaux2(int i){
   if (i == 0) // 3
      printf("Browres qui sont disponibles pour Android sont bases sur \nA)google chrome \nB)firefox\nC)open source webkit\nD)IE\n");
   if (i == 1) // 3
      printf("laquelle de la societe suivante a achete Android \nA) Microsoft \nB) Goole \nC) Apple\n D) aucun des option\n");
   if (i == 2) // 4
      printf("laquelle de(s) la fonctionnalité(s) suivante(s) est/sont pris en charge par Android \nA) multitache\nB) Bluetooth\nC) chat video\nD) tous\n");
   if (i == 3) // 1
      printf("est-ce-que Android support AWT et SPRING \nA) Oui\nB) Nom\n");
}

void Qniveaux3(int i){
   if (i == 0) // 3
      printf("Browres qui sont disponibles pour Android sont bases sur \nA)google chrome \nB)firefox\nC)open source webkit\nD)IE\n");
   if (i == 1) // 3
      printf("laquelle de la societe suivante a achete Android \nA) Microsoft \nB) Goole \nC) Apple\n D) aucun des option\n");
   if (i == 2) // 4
      printf("laquelle de(s) la fonctionnalité(s) suivante(s) est/sont pris en charge par Android \nA) multitache\nB) Bluetooth\nC) chat video\nD) tous\n");
   if (i == 3) // 1
      printf("est-ce-que Android support AWT et SPRING \nA) Oui\nB) Nom\n");
}

void lvl_question(int lvl, int i){
	if ((lvl = 1))
		Qniveaux1(i);
}*/


//appeler par send_answers
void free_buffer() {
  printf("%s", "> ");
  fflush(stdout);//libération des données dans le buffer
}

void str_trim_lf (char* arr, int length) {//chaine de caractére + strlen
  int i;
  for (i = 0; i < length; i++) { // trim \n
    if (arr[i] == '\n') {
      arr[i] = '\0';
      break;
    }
  }
}

void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

//appeler par la fonction pthread_create
void send_answers() {//Récupérer choix niveau et envoie du nom et niveau au serveur (envoie via socket)
  	char message[LENGTH] = {};
	char buffer[LENGTH + 32] = {};
	int j, quiz_num = 0;
	int cnt = 0;

	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	fp = fopen("niveau2.txt", "r");
	if (fp == NULL)
   		exit(EXIT_FAILURE);

	free_buffer();
	if ((cnt = 0)){
		free_buffer();//libérer les données dans la fonction
		while (1){//Spécifier le nombre du level (quiz)
			fgets(message, LENGTH, stdin);
			if (strcmp(message, "1")){
				quiz_num = 1;
				break;
			}else if (strcmp(message, "2")){
				quiz_num = 2;
				break;
			}else if (strcmp(message, "3")){
				quiz_num = 3;
				break;
			}else
				printf("il y a que 3 options 1,2 ou 3 !!!");
		}
		cnt++;
    	str_trim_lf(message, LENGTH);
      	sprintf(buffer, "%s: %s\n", name, message);
      	send(sockfd, buffer, strlen(buffer), 0);
	}

  while(1) {
  	free_buffer();
    fgets(message, LENGTH, stdin);
    str_trim_lf(message, LENGTH);

    if (strcmp(message, "EXIT") == 0) {
			break;
    }else{
      	sprintf(buffer, "%s: %s\n", name, message);
		//lvl_question(quiz_num, j++);
		read = getline(&line, &len, fp);
        	printf("%s", line);
      	send(sockfd, buffer, strlen(buffer), 0);
    }
	bzero(message, LENGTH);
    bzero(buffer, LENGTH + 32);
  }
  catch_ctrl_c_and_exit(2);
}

void recv_msg_handler() {
	char message[LENGTH] = {};
  while (1) {
	int receive = recv(sockfd, message, LENGTH, 0);
    if (receive > 0) {
      printf("%s", message);
      free_buffer();
    } else if (receive == 0) {
			break;
    } else {
			// -1
	}
	memset(message, 0, sizeof(message));
  }
}

int main(int argc, char **argv){
	if(argc != 2){
		printf("Usage: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}
	char *ip = "127.0.0.1";//adress of localhost (local)
	int port = atoi(argv[1]);//Cette fonction permet de transformer une chaîne de caractères, représentant une valeur entière, en une valeur numérique de type int.

	signal(SIGINT, catch_ctrl_c_and_exit); // si client tape Ctr-C ===> exit the execution

	printf("Merci de tapez votre nom : ");
  	fgets(name, 32, stdin);//stdin ==> entrée standard = clavier
  	str_trim_lf(name, strlen(name));//tapez entrer ===> take name
	// testez si la taille du nom est inf à 2 ou sup à 32
	if (strlen(name) > 50 || strlen(name) < 2){
		printf("Le nom doit avoir entre 2 et 50 caractères !! .\n");
		return EXIT_FAILURE;//quitter l'exécution
	}

	struct sockaddr_in server_addr;

	/* Socket settings */
	//remplir les infos de server_addr
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
  	server_addr.sin_family = AF_INET;
  	server_addr.sin_addr.s_addr = inet_addr(ip);
  	server_addr.sin_port = htons(port);


  // Connect to Server
  	int connection = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  	if (connection == -1) {
		printf("Erreur dans la connexion\n");
		exit(1);
	}

	// Send name
	send(sockfd, name, 32, 0);

	printf("\n\n ************************* Avant de commancer le quizz *************************");
	printf("\n   -------------------------------------------------------------------------------");
	printf("\n                       ........... Directives ...........\n");
	printf("\n [+] Vous aurez un total de 3 questions, les questions sont choisis aléatoirement");
	printf("\n [+] Vous devez choisir la réponse juste en choisissanr A, B ou C");
	printf("\n [+] Un bonus de 5 points pour chaque réponse juste");
	printf("\n [+] le score ne sera pas débité en cas de fausse réponse");

 printf("\n\n ******************************* BONNE CHANCE ***********************************\n\n");
	choisir_level();///===> afficher des messages pour choisir un level
	pthread_t msg_in_thread_from_client;// Création de la variable qui va contenir le thread
	if(pthread_create(&msg_in_thread_from_client, NULL, (void *)send_answers, NULL) != 0){//create the thread
			printf("Erreur dans la création du thread\n");
		return EXIT_FAILURE;
	}

	pthread_t msg_in_thread_from_server;
  	if(pthread_create(&msg_in_thread_from_server, NULL, (void *) recv_msg_handler, NULL) != 0){
		printf("Erreur dans la création du thread\n");
		return EXIT_FAILURE;
	}

	while (1){
		if(flag){
			printf("\nSignal de quittance CTRL+C Reçu, au revoir...\n");
			break;
    	}
	}
	close(sockfd);
	return EXIT_SUCCESS;
}
