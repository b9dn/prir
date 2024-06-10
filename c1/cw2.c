#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> /*deklaracje standardowych funkcji uniksowych (fork(), write() itd.)*/

void on_usr1(int signal) {
	printf("Otrzymalem usr1 (potomny)\n");
}

void potomny() {
    sigset_t mask;
    struct sigaction usr1;
	sigemptyset(&mask); /* Wyczyść maskę */
	usr1.sa_handler = (&on_usr1);
	usr1.sa_mask = mask;
	usr1.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &usr1, NULL);

    sigemptyset(&mask); /* Wyczyść maskę */
	sigprocmask(SIG_BLOCK, &mask, NULL); /* Ustaw maskę dla całego procesu */

    sleep(100);
}

int main()
{
    pid_t pid;
    printf("Moj PID = %d\n", getpid());
    switch(pid = fork()){
        case -1:
            fprintf(stderr, "Blad w fork\n");
            return EXIT_FAILURE;
        case 0: /*proces potomny*/
            printf("Jestem procesem potomnym. PID = %d\n \ 
                Wartosc przekazana przez fork() = %d\n", getpid(), pid);
            potomny();
            return EXIT_SUCCESS;
        default:
            printf("Jestem procesem macierzystym. PID = %d\n \ 
                Wartosc przekazana przez fork() = %d\n", getpid(), pid);
            /*Oczekiwanie na zakończenie procesu potomnego*/
            sleep(2);
            printf("Wysylam kill SIGUSR1 (macierzysty)\n");
            kill(pid, SIGUSR1);
            if(wait(0) == -1)
            {
                fprintf(stderr, "Blad w wait\n");
                return EXIT_FAILURE;

            }
            printf("Koncze\n");
            return EXIT_SUCCESS;

    }
}
