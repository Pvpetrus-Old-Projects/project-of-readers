#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>

//poniższe zmienne umożliwiają zawieszenie i zwolnienie czasu wątku, do momentu spełnienia określonego warunku
pthread_mutex_t blokadaCzytelnikow;
pthread_mutex_t blokadaPisarzy;

int aktualnieczytajacy=0;
int aktualniepiszacy=0;
int liczba_czytelnikow=5;
int liczba_pisarzy=1;

void* czytelnik(void *argument);
void* pisarz(void *argument);
void wypiszKomunikat();
int generatorCzasuCzekania();

//funkcja generująca czas czekania dla wątku
int generatorCzasuCzekania()
{
	return rand()%900000  + 100000;//losowa ilosc czasu z przedzialu: [0.1s,1s)
}

//funkcja wypisuje komunikat w logu na temat aktualnej ilości czekających oraz będących w czytelni
void wypiszKomunikat()
{
	syslog(LOG_NOTICE, "READERQ: %d WriterQ: %d [in: R: %d W: %d]", (liczba_czytelnikow-aktualnieczytajacy),
	(liczba_pisarzy-aktualniepiszacy), aktualnieczytajacy, aktualniepiszacy);
}

//wątek czytelnika
void* czytelnik(void *argument)
{
	srand(time(NULL));
	while(1==1)
	{
		//czytelnik próbuje wejść do czytelni
		//jeżeli w czytelni nie ma czytelników
		if(aktualnieczytajacy==0)
		{
			//zablokuj mutex blokadaPisarzy
			pthread_mutex_lock(&blokadaPisarzy);
		}
		//czytelnik wszedł do czytelni
		aktualnieczytajacy+=1;
		//czytelnik czyta - uspanie wątku
		usleep(generatorCzasuCzekania());
		wypiszKomunikat();
		//czytelnik wychodzi
		aktualnieczytajacy-=1;
		//jeżeli w czytelni nie ma czytelników
		if(aktualnieczytajacy==0)
		{
			//odblokuj mutex blokadaPisarzy(umożliwienie pisarzom wejścia)
			pthread_mutex_unlock(&blokadaPisarzy);
		}
		
	}
	return 0;
}

//wątek pisarza
void* pisarz(void *argument)
{
	//funkcja wątku pisarza
	srand(time(NULL));
	while(1==1)
	{
		//zablokowanie mutexu blokadaPisarzy(żaden pisarz nie może wejść)
		pthread_mutex_lock(&blokadaPisarzy);
		//ustawienie nowo utworzonego pisarza jako aktualnie piszącego
		aktualniepiszacy+=1;
		//pisarz korzysta z czytelni
		usleep(generatorCzasuCzekania());
		wypiszKomunikat();
		//pisarz wychodzi
		aktualniepiszacy-=1;
		//mutex blokadaPisarzy się odblokowuje
		pthread_mutex_unlock(&blokadaPisarzy);
	}
	return 0;
}

int main(int argc, char *argv[])
{
    int nr_blednego_parametru = 0;
	
    //inicjalizacja zmiennych warunkowych
    pthread_mutex_init(&blokadaCzytelnikow, NULL);
    pthread_mutex_init(&blokadaPisarzy, NULL);
	
    //sprawdzenie poprawności wprowadzonych argumentów
    if(argc>3 || argc<1) {
        printf("Zla liczba parametrow\n");
		printf("Wprowadz komendę w postaci: ./nazwa_programu [liczba_czytelnikow] [liczba_pisarzy], liczby w nawiasi nie są wymagane");
	    return -1;
    }
    else
    {	
		if(argc==2)
		{
			if(atoi(argv[1])>= 0)
			{
				liczba_czytelnikow = atoi(argv[1]);
			}
			else
			{
				nr_blednego_parametru = nr_blednego_parametru + 1;
			}
		}
		if(argc==3)
		{	
			if(atoi(argv[1])>= 0)
			{
				liczba_czytelnikow = atoi(argv[1]);
			}
			else
			{
				nr_blednego_parametru = nr_blednego_parametru + 1;
			}
			if(atoi(argv[2])>= 0)
			{
				liczba_pisarzy = atoi(argv[2]);
			}
			else
			{
				nr_blednego_parametru = nr_blednego_parametru + 2;
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
	//utworzenie logu pozwalającego na śledzenie działania programu
	openlog("Threads", LOG_PID, LOG_DAEMON);
	syslog(LOG_NOTICE, "===---The program has started working---===");
	syslog(LOG_NOTICE,"Czytelnicy: %d, pisarze: %d", liczba_czytelnikow, liczba_pisarzy);
	
	//tworzenie wątków
	pthread_t pisarze[liczba_pisarzy];
	pthread_t czytelnicy[liczba_czytelnikow];
	int i=0;
	for(i=0;i<liczba_czytelnikow;i++)
	{
		pthread_create(&czytelnicy[i],NULL,czytelnik,NULL);
		sleep(2);
	}
	for(i=0;i<liczba_pisarzy;i++)
	{
		pthread_create(&pisarze[i],NULL,pisarz,NULL);
	}

	//oczekiwanie na zakończenie działania innych wątków
	for(i=0;i<liczba_czytelnikow;i++)
	{
		pthread_join(czytelnicy[i],NULL);
	}
	for(i=0;i<liczba_pisarzy;i++)
	{
		pthread_join(pisarze[i],NULL);
	}
	syslog(LOG_NOTICE, "===---The program has ended working---===");
	return 0;
	
}
