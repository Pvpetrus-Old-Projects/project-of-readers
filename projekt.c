#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>

pthread_mutex_t blokadaCzytelnikow;
pthread_mutex_t blokadPisarzy;


int main(int arg,char **argv)
{
<<<<<<< HEAD
	//deklaracja zmiennych zaleznych od parametrow
    char readers;
    char writers;
    int liczba_czytelnikow;
    int liczba_pisarzy;
    //
    //kod na sprawdzenie parametrow
    bool czy_parametry_poprawne=true;

    if(argc!=2) {
        czy_parametry_poprawne = false;
        printf("\n Zla liczba parametrow");
	    retrun 0;
    }
    else
    {
	    if(atoi(argv[0]) != 0)
	    {
		    liczba_czytelenikow = atoi(argv[0]);
		    printf("Ilośc czytleników wynosi: %d.", ilosc_czytlenikow); 
	    }
	    else if(argv[0]) == 0)
	    {
		    printf("Bledny parametr nr 1!");
		    return 0;
	    }
	    else if(atoi(argv[1]) != 0)
	    {
		    liczba_pisarzy = atoi(argv[1]);
		    printf("Ilośc pisarzy wynosi: %d.", ilosc_pisarzy); 
	    }
	    else
	    {
		    printf("Bledny parametr nr 2!");
		    return 0;
	    }
	    
	    
	    
    }
        
	return 0;
	
}
