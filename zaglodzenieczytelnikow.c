#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>


pthread_mutex_t blokadaCzytelnikow;
pthread_mutex_t blokadaPisarzy;
pthread_mutex_t kolejkaCzytelnikow;
pthread_mutex_t kolejkaPisarzy; 

int aktualnieczytajacy=0;
int aktualniepiszacy=0;
int aktualniepiszacylubczekajacy=0;
int liczba_czytelnikow=5;
int liczba_pisarzy=1;

void* czytelnik(void *argument);
void* pisarz(void *argument);
void wypiszKomunikat();
int generatorCzasuCzekania();
int generatorCzasuCzekania()
{
	return rand()%900000  + 100000;//losowa ilosc czasu z przedzialu: [0.1s,1s)
}
void wypiszKomunikat()
{
	//funkcja wypisuje komunikat w logu na temat aktualnej ilości czekających oraz będących w czytelni
	syslog(LOG_NOTICE, "READERQ: %d WriterQ: %d [in: R: %d W: %d]", (liczba_czytelnikow-aktualnieczytajacy),
	(liczba_pisarzy-aktualniepiszacy), aktualnieczytajacy, aktualniepiszacy);
}
void* czytelnik(void *argument)
{
	//funkcja wątku czytelnika
	srand(time(NULL));
	while(1==1)
	{
        //kolejka
        pthread_mutex_lock(&blokadaCzytelnikow);
        pthread_mutex_lock(&kolejkaCzytelnikow);
		//Czytelnik próbuje wejść do czytelni
		if(aktualnieczytajacy==0)
		{
			pthread_mutex_lock(&blokadaPisarzy);
		}
		aktualnieczytajacy+=1;
        pthread_mutex_unlock(&kolejkaCzytelnikow);
        pthread_mutex_unlock(&blokadaCzytelnikow);
		usleep(generatorCzasuCzekania());//czytelnik robi coś w czytelni przez losową ilość czasu
		wypiszKomunikat();
		aktualnieczytajacy-=1;//czytelnik wychodzi
		if(aktualnieczytajacy==0)
		{
			pthread_mutex_unlock(&blokadaPisarzy);
		}
		
        usleep(generatorCzasuCzekania());
	}
	return 0;
}

void* pisarz(void *argument)
{
	//funkcja wątku pisarza
	srand(time(NULL));
	while(1==1)
	{
        //kolejka
        
        pthread_mutex_lock(&kolejkaPisarzy);
        aktualniepiszacylubczekajacy+=1;
        if(aktualniepiszacylubczekajacy==1)
        {
            pthread_mutex_lock(&blokadaCzytelnikow);

        }
        pthread_mutex_unlock(&kolejkaPisarzy);

		//pisarz próbuje wejść do czytelni
		pthread_mutex_lock(&blokadaPisarzy);
        aktualniepiszacy+=1;
		usleep(generatorCzasuCzekania());//pisarz korzysta z czytelni
		wypiszKomunikat();
		aktualniepiszacylubczekajacy-=1;//pisarz wychodzi
        if(aktualniepiszacylubczekajacy==0)
        {
            pthread_mutex_unlock(&blokadaCzytelnikow);
        }
		pthread_mutex_unlock(&blokadaPisarzy);
        aktualniepiszacy-=1;
        usleep(generatorCzasuCzekania());
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int nr_blednego_parametru = 0;
	pthread_mutex_init(&blokadaCzytelnikow, NULL);
	pthread_mutex_init(&blokadaPisarzy, NULL);
    pthread_mutex_init(&kolejkaCzytelnikow, NULL);
    pthread_mutex_init(&kolejkaPisarzy, NULL);
	
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
		//tu należy dać funkcję czekająca pewną ilość czasu, aby zagłodzić pisarzy, gdyż po takim zabiegu, przy odpowiednim dobraniu czasu 
		//odczekania i ilości czytelników, gdy jeden czytelnik skończy, do biblioteki przyjdzie kolejny, a biblioteka nigdy nie będzie pusta
		//i pisarze nie będą mogli wejść
	}
	for(i=0;i<liczba_pisarzy;i++)
	{
		pthread_create(&pisarze[i],NULL,pisarz,NULL);
	}

	//czekanie aż wątki się zakończą
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
