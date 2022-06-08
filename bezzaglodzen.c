#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>

//poniższe zmienne warunkowe umożliwiają zawieszenie i zwolnienie czasu wątku, do momentu spełnienia określonego warunku
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
	//ustawienie blokady na mutex kolejka
        pthread_mutex_lock(&kolejka);
		
	//jeżeli ktoś z czekających pisarzy jest w kolejce
        if (czekajacypisacze > 0)
        {
	    //tworzony nowo czytelnik dodaje się do kolejki czekających czytelników
            czekajacyczytacze+=1;
	    //funkcja zwalnia mutex kolejka i czeka na sygnał spełnienia warunku gotowyByCzytac
	    //wykonanie wątku zawieszone do czasu odebrania sygnału
            pthread_cond_wait(&gotowyByCzytac, &kolejka);
        }
	//jeżeli w czytelni znajduję się pisarz
        else if(aktualniepiszacy>0)
        {
	    //tworzony nowo czytelnik dodaje się do kolejki czekających czytelników
            czekajacyczytacze+=1;
	    //wykonanie wątku zawieszone do czasu odebrania sygnału
            pthread_cond_wait(&gotowyByCzytac, &kolejka);
        }
	//w innym wypadku
        else
        {
	    //nowo utworzony czytelnik może wejść do czytelni
            aktualnieczytajacy+=1;
            wypiszKomunikat();
        }
	//odblokowanie mutexu kolejka, czytelnik wchodzi do środka
        pthread_mutex_unlock(&kolejka);

	//uspanie wątku(czytelnik czyta)
	usleep(generatorCzasuCzekania());

        //zablokowanie mutexu kolejka(czytelnik wychodzi, więc nikt nie może wejść póki 
	//nie zostanie sprawdzone, kto czeka w kolejce)
        pthread_mutex_lock(&kolejka);
        aktualnieczytajacy-=1;
	//jeżeli nie ma w czytelni żadnego czytelnika
        if (aktualnieczytajacy == 0)
        {
	    //dajemy sygnał dla czekającego wątku, że może się uruchomić
            pthread_cond_signal(&gotowyByPisac);
        }
	//odblokowanie kolejki
        pthread_mutex_unlock(&kolejka);

        usleep(generatorCzasuCzekania());
	}
	return 0;
}

//wątek pisarza
void* pisarz(void *argument)
{
	srand(time(NULL));
	while(1==1)
	{
        //ustawienie blokady na mutex kolejka
        pthread_mutex_lock(&kolejka);
	//jeżeli w czytelni znajdują się czytelnicy
        if (aktualnieczytajacy > 0) 
        {
	    //zwiększa się ilość czekających pisarzy
            czekajacypisacze+=1; 
	    //wykonanie wątku zawieszone do czasu odebrania sygnału, że pisarz może pisać
            pthread_cond_wait(&gotowyByCzytac, &kolejka);
	    //pisarz wchodzi do czytelni, zatem już nie czeka w kolejce
            czekajacypisacze-=1;
        }
	//jeżeli w czytelni znajduję się jakiś pisarz
        else if(aktualniepiszacy > 0)
        {
	    //zwiększa się ilość czekających pisarzy
            czekajacypisacze+=1; 
	    //wykonanie wątku zawieszone do czasu odebrania sygnału, że pisarz może pisać
            pthread_cond_wait(&gotowyByCzytac, &kolejka);
	    //pisarz wchodzi do czytelni, zatem już nie czeka w kolejce
            czekajacypisacze-=1;
        }
	// w innym wypadku
        else
        {
	    //w czytelni znajduje się tylko jeden pisarz
            aktualniepiszacy = 1; 
            wypiszKomunikat(); 
        }
	//odblokowanie mutexu kolejka
        pthread_mutex_unlock(&kolejka);
        
        //uspanie wątku
        usleep(generatorCzasuCzekania());

        //zablokowanie kolejki
        pthread_mutex_lock(&kolejka); 
	//pisarz wychodzi z czytelni
        aktualniepiszacy = 0; 
	//jeżeli istnieją czytelnicy, którzy czekają
        if (czekajacyczytacze == 0)
        {
	    //dajemy sygnał dla czekającego wątku, że może się uruchomić
            pthread_cond_signal(&gotowyByPisac);
        }
	//w innym przypadku 
        else
        {
	    //wznów wszystkie oczekujące wątki
            pthread_cond_broadcast(&gotowyByCzytac); 
	    //wpuść wszystkich czekających czytelników
            aktualnieczytajacy += czekajacyczytacze; 
	    //wyzeruj czekających czytelników(wszyscy już weszli)
            czekajacyczytacze = 0; 
            
            wypiszKomunikat(); 
        }
	//odblokowanie mutexu kolejka
        pthread_mutex_unlock(&kolejka); 
        //uśpij wątek
        usleep(generatorCzasuCzekania());
	}
	return 0;
}

int main(int argc, char *argv[])
{
    int nr_blednego_parametru = 0;
    //inicjalizacja zmiennych warunkowych
    pthread_cond_init(&gotowyByCzytac, NULL);
    pthread_cond_init(&gotowyByPisac, NULL);
    pthread_mutex_init(&kolejka, NULL);
	
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
		sleep(1);
	}
	for(i=0;i<liczba_pisarzy;i++)
	{
		pthread_create(&pisarze[i],NULL,pisarz,NULL);
        sleep(1);
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
