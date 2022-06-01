#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>


pthread_cond_t gotowyByCzytac; 
pthread_cond_t gotowyByPisac;
pthread_mutex_t kolejka;

int aktualnieczytajacy=0;
int aktualniepiszacy=0;
int czekajacyczytacze=0;
int czekajacypisacze=0;
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
        pthread_mutex_lock(&kolejka);
        if (czekajacypisacze > 0)
        {
            czekajacyczytacze+=1;
            pthread_cond_wait(&gotowyByCzytac, &kolejka);
        }
        else if(aktualniepiszacy>0)
        {
            czekajacyczytacze+=1;
            pthread_cond_wait(&gotowyByCzytac, &kolejka);
        }
        else
        {
            aktualnieczytajacy+=1;
            wypiszKomunikat();
        }
        pthread_mutex_unlock(&kolejka);


		usleep(generatorCzasuCzekania());

        
        pthread_mutex_lock(&kolejka);
        aktualnieczytajacy-=1;
        if (aktualnieczytajacy == 0)
        {
            pthread_cond_signal(&gotowyByPisac);
        }
        pthread_mutex_unlock(&kolejka);
        

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
        
        
        pthread_mutex_lock(&kolejka);  
        if (aktualnieczytajacy > 0 || aktualniepiszacy > 0) 
        {
        czekajacypisacze+=1; 
        pthread_cond_wait(&gotowyByCzytac, &kolejka); 
        
        czekajacypisacze-=1;
        }
        aktualniepiszacy = 1; 
        wypiszKomunikat(); 
        pthread_mutex_unlock(&kolejka);
        
        
        usleep(generatorCzasuCzekania());

        
        pthread_mutex_lock(&kolejka); 
        aktualniepiszacy = 0; 
        if (czekajacyczytacze == 0)
        pthread_cond_signal(&gotowyByPisac); 
        else 
        {
        pthread_cond_broadcast(&gotowyByCzytac); 
        aktualnieczytajacy += czekajacyczytacze; 
        czekajacyczytacze = 0; 
        
        wypiszKomunikat(); 
        }
        pthread_mutex_unlock(&kolejka); 
        

        usleep(generatorCzasuCzekania());
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int nr_blednego_parametru = 0;
	pthread_cond_init(&gotowyByCzytac, NULL);
    pthread_cond_init(&gotowyByPisac, NULL);
    pthread_mutex_init(&kolejka, NULL);
	
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
		sleep(1);
		//tu należy dać funkcję czekająca pewną ilość czasu, aby zagłodzić pisarzy, gdyż po takim zabiegu, przy odpowiednim dobraniu czasu 
		//odczekania i ilości czytelników, gdy jeden czytelnik skończy, do biblioteki przyjdzie kolejny, a biblioteka nigdy nie będzie pusta
		//i pisarze nie będą mogli wejść
	}
	for(i=0;i<liczba_pisarzy;i++)
	{
		pthread_create(&pisarze[i],NULL,pisarz,NULL);
        sleep(1);
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
