#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>

/*

    Por causa da rapidez no código, ele serializa a execução (primeiro faz
    tudo no thread 1, depois tudo no thread2, e assim por diante).

    Usei o "sched_yield();", que em teoria deveria dar a vez para outro
    thread, mas não fez isso. Acho que o motivo está explicado na seguinte
    página de manual (man sched_yield).

    "Nota: Se o processo atual é o único processo sendo executado na fila de
    mais  alta  prioridade naquele instante, este processo continuará a sua
    execução depois da chamada a sched_yield".

*/

int var_global = 0;
sem_t mutex;

void *soma1 (void *th_number){
	int temp,i;
        int* th = (int*) th_number;
        

	for (i=0; i < 200; i++){
          sem_wait (&mutex);
	      ++var_global;
          sched_yield();
          --var_global;
          sleep(1);
          printf ("thread %d. global: %d\n", *th,var_global);
          sem_post (&mutex); //signal
	}
}

int main(){

	pthread_t th_p1, th_p2, th_p3;

	int p1=1, p2=2, p3=3, erro;

	sem_init(&mutex, 0 ,1);

	erro = pthread_create (&th_p1, NULL, soma1, &p1);
	erro = pthread_create (&th_p2, NULL, soma1, &p2);
	erro = pthread_create (&th_p3, NULL, soma1, &p3);

	pthread_join (th_p1, NULL);
	pthread_join (th_p2, NULL);
	pthread_join (th_p3, NULL);

	return 0;
}


