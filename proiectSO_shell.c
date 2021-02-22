#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h> 
#include <readline/readline.h>
#include <readline/history.h>


void run_shell(void);
char **my_split_line(char*);
int my_help();
int run(char**);
int my_mkdir(char**);
int my_rmdir(char**);
int my_cd(char**);
int my_ls();
int my_pwd();
int my_rm(char**);
int my_clear();
int my_cp(char**);
int my_exit(char**);
int nr_cmnds();
int execute(char**);

char* cmnds[]=
{
	"my_help",
  	"my_mkdir",
  	"my_rmdir",
  	"my_cd",
  	"my_ls",
  	"my_pwd",
  	"my_rm",
  	"my_clear",
  	"my_cp",
  	"my_exit"
};

int (*cmnds_action[]) (char**) =
{
 	&my_help,
  	&my_mkdir,
  	&my_rmdir,
  	&my_cd,
  	&my_ls,
  	&my_pwd,
  	&my_rm,
  	&my_clear,
  	&my_cp,
  	&my_exit
};



int nr_cmnds()
{
  return sizeof(cmnds) / sizeof(char*);         //dimensiunea fiecarei comenzi
}

void run_shell(void)
{
  char* line;
  char** args;
  int state;
    do {             // citim linia
    line = readline("$ ");
    add_history(line);
    args = my_split_line(line);       // separam linia in argumente
    if(strcmp(args[0],"history")==0){ //daca args[0] e egal cu history, afiseaza istoricul
        register HIST_ENTRY **my_history;
        register int i;
        my_history=history_list();
        for (i = 0; my_history[i]; i++)
              printf ("command %d: %s\n", i + history_base, my_history[i]->line);}
    else{
    	state = execute(args);       // variabila folosita pentru iesirea din executie a functiei execute
    }
    free(line);
    free(args);
  } while(state);
}


char **my_split_line(char *line)      //functie pentru impartirea liniei in lista de argumente
{
  int buffer_size = 64;
  int poz = 0;
  char **arguments = malloc(buffer_size * sizeof(char*));
  char *argument;
  if(!argument)
  {
    fprintf(stderr, "Error found at the allocation process\n");    //printeaza cod de eroare
    exit(EXIT_FAILURE);
  }
  argument = strtok(line, " \n");         // variabila argument  memoreaza argumentele date(cd, ls, rmdir, mkdir + argumentele lor)
  while(argument != NULL)
  {
    arguments[poz] = strdup(argument);    //punem in vectorul de argumente un pointer la argumentul curent
    poz++;
    if(poz >= buffer_size)
    {
      buffer_size += 64;
      arguments = realloc(arguments,  buffer_size * sizeof(char*));
      if(!arguments)
      {
        fprintf(stderr, "Error found at the allocation process\n");
        exit(EXIT_FAILURE);
      }
    }
    argument = strtok(NULL, " \n");
  }
  arguments[poz] = NULL;
  return arguments;
}

int my_help()
{
  	int i;
  	printf("This project was developed by de Mariana Barbu, Valentin Balu & Marinel Arsene.\n");
  	printf("Basic commands in linux:\n");
  	for(i = 0; i < nr_cmnds(); i++)
    		printf("  %s\n", cmnds[i]);
  	printf("my_mkdir = creates new directory\n");
  	printf("my_rmdir = deletes the directory which is named as the given argument\n");
  	printf("my_cd = changes director into the directory given as an argument\n");
  	printf("my_ls = displays directory's files\n");
  	printf("my_pwd = displays current directory\n"); 
  	printf("my_rm = deletes file\n");
  	printf("my_clear = clears the terminal\n");
 	printf("my_cp = copies a file\n");
	printf("my_exit = exit function\n");
}

int run(char **args)                // functie pentru folosirea functiilor de sistem linux   
{
  pid_t pid, wait_pid;
  int state;
  pid = fork();
  if(pid == 0)
  {
    if(execvp(args[0], args) == -1)     //verifcam daca duplicarea unei actiuni in shell, in cautarea unei file executabile, este posibila
      perror("Error found in executing child process");
  exit(EXIT_FAILURE);
 }
  else if(pid < 0)
    perror("Error found while creating this process");
  else
  {
    do {
      wait_pid = waitpid(pid, &state, WUNTRACED);         //WUNTRACED => Daca nu specificam argumentul WUNTRACED, functia va astepta continuu un semnal
    } while(!WIFEXITED(state) && !WIFSIGNALED(state));    //returneaza true daca procesul copil s-a terminat cu succes si daca procesul copil s-a terminat la un semnal
  }
  return 1;
}

int my_mkdir(char** args)
{
  if( mkdir(args[1], 0777) == -1)                     //verificam daca se poate crea directorul si daca nu, afisam un mesaj de eroare
        perror("Cannot create directory\n");
  else
        printf("The directory has been created\n");
}


int my_rmdir(char** directory)
{
  if( rmdir(directory[1]) == -1)		      //verificam daca se poate sterge directorul si daca nu, afisam un mesaj de eroare
    perror("Cannot delete directory\n");
  else
    printf("The directory has been deleted\n");
  return 1;
}


int my_cd(char **args)
{
  if(args[1] == NULL)					//verificam  daca my_cd primeste un argument pt a modifica directorul curent, daca nu afisam un mesaj de eroare
    fprintf(stderr, "Did not receive expected argument to cd\n");
  else
  {
    if(chdir(args[1]) != 0)				//daca  exista directorul primit ca parametru ne mutam acolo, altfel afisam un mesaj de eroare
      perror("Error found while executing cd command");
  }
  return 1;
}


int my_ls()
{
  int nr;
  struct dirent** content_list;                          //luam o variabila de tip struct dirent  si numarul de elemente din directorul curent
  nr = scandir(".", &content_list, NULL, alphasort);     //daca este mai mic decat 0, afisam eroare
  if(nr < 0)
    perror("Error found while executing scandir command");
  else
  {
    while(nr--)
    {
      printf("%s\n", content_list[nr]->d_name);		//altfel afisam numele componentelor lui
      free(content_list[nr]);
    }
  }
    free(content_list);
return 1;
}

int my_pwd()
{
    char path[1024];					//afisam calea curenta cu ajutorul getcwd
    getcwd(path, sizeof(path));
    printf("Current path: %s\n", path);
    return 1;
}

int my_rm(char** file_name)				//daca gasim fila cu numele primit ca parametru o stergem, altfel afisam un mesaj de eroare
{
  if(remove(file_name[1]) == 0)
    printf("The file has been deleted\n");
  else
    printf("Cannot delete this file\n");
  return 1;
}

int my_clear()						//curatam consola
{
  write(1, "\33[H\33[2J", 7);
  return 1;
}

int my_cp(char** files)					//copiem continutul primei file in a doua
{
  FILE *file_1, *file_2;
  char chr;
  if((file_1 = fopen(files[1], "r")) == NULL)
  {
    printf("This file cannot be opened\n");
    return 1;
  }
  file_2 = fopen(files[2], "w");
  chr = fgetc(file_1);				//luam textul din file_1 caracter cu caracter si copiem in file_2
  while(chr != EOF)
  {
    fputc(chr, file_2);
    chr = fgetc(file_1);
  }
  fclose(file_1);
  fclose(file_2);
  return 1;
}


int my_exit(char **args)
{
  return 0;
}

int execute(char **args)			//primeste ca parametru o lista de argumente reprezentand comanda curenta si argumentele sale
{
  int i;
  if(args[0] == NULL)
    return 1;
  for(i = 0; i < nr_cmnds(); i++)
  {
    if(strcmp(args[0], cmnds[i])== 0)
      return (*cmnds_action[i])(args);        //verificam daca comanda curenta se afla in lista de comenzi definite si apeleaza functia corespunzatoare cand gaseste un match
  }
  return run(args);				//apelam run pt efectuarea comenzii linux corespunzatoare
}

int main()
{
  run_shell();
  return EXIT_SUCCESS;
}

