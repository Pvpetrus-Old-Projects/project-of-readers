#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>


pthread_mutex_t blokadaCzytelnikow;
pthread_mutex_t blokadaPisarzy;

void* czytelnik(void *argument);
void* pisarz(void *argument);

void* czytelnik(void *argument)
{

}

void* pisarz(void *argument)
{
	
}

int main(int argc, char *argv[])
{

    int liczba_czytelnikow=5;
    int liczba_pisarzy=1;
	int nr_blednego_parametru = 0;
	pthread_mutex_init(&blokadaCzytelnikow, NULL);
	pthread_mutex_init(&blokadaPisarzy, NULL);
	
    if(argc>3 || argc<1) {
        printf("Zla liczba parametrow\n");
		printf("Wprowadz komendę w postaci: ./nazwa_programu [liczba_czytelnikow] [liczba_pisarzy], liczby w nawiasi nie są wymagane");
	    return -1;
    }
    else
    {	
		if(argc==2)
		{
			if(atoi(argv[1]) != 0)
			{
				liczba_czytelnikow = atoi(argv[1]);
				//printf("Ilośc czytleników wynosi: %d.", liczba_czytelnikow); 
			}
			if(atoi(argv[1]) == 0)
			{
				//printf("Bledny parametr nr 1!");
				nr_blednego_parametru = nr_blednego_parametru + 1;
				//return 0;
			}
		}
		if(argc==3)
		{
			if(atoi(argv[2]) != 0)
			{
				liczba_pisarzy = atoi(argv[2]);
				printf("Ilośc pisarzy wynosi: %d.", liczba_pisarzy); 
			}
			if(atoi(argv[2]) == 0)
			{
				//printf("Bledny parametr nr 2!");
				nr_blednego_parametru = nr_blednego_parametru + 2;
				//return 0;
			}
		}
			
			
	    
		if(nr_blednego_parametru != 0)
		{
			if(nr_blednego_parametru == 1)
			{
				printf("Bledny parametr nr 1!");
				return -2;
			}
			else if(nr_blednego_parametru == 2)
			{
				printf("Bledny parametr nr 2!");
				return -3;
			}
			else
			{
				printf("Niepoprawne oba parametry!");
				return -4;
			}
		}
		else
		{
			printf("Parametry sa poprawne");
		}
    
    }
	openlog("Threads", LOG_PID, LOG_DAEMON);
	syslog(LOG_NOTICE, "The program has started working");
	printf("Czytelnicy: %d, pisarze %d", liczba_czytelnikow, liczba_pisarzy);
	
	pthread_t pisarze[liczba_pisarzy];
	pthread_t czytelnicy[liczba_czytelnikow];
	//tworzenie wątków
	int i=0;
	for(i=0;i<liczba_czytelnikow;i++)
	{
		pthread_create(&czytelnicy[i],NULL,czytelnik,NULL);
	}
	for(i=0;i<liczba_pisarzy;i++)
	{
		pthread_create(&pisarze[i],NULL,pisarz,NULL);
	}
	for(i=0;i<liczba_czytelnikow;i++)
	{
		pthread_join(&czytelnicy[i],NULL);
	}
	for(i=0;i<liczba_pisarzy;i++)
	{
		pthread_join(&pisarze[i],NULL);
	}
	//tworzenie mutexów
	
	return 0;
	
}
