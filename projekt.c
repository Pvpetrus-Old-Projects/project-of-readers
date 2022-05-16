#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>


pthread_mutex_t blokadaCzytelnikow;
pthread_mutex_t blokadPisarzy;


int main(int argc, char *argv[])
{

    int liczba_czytelnikow;
    int liczba_pisarzy;
	int nr_blednego_parametru = 0;
	
    if(argc!=3) {
        printf("Zla liczba parametrow\n");
		printf("Wprowadz parametry jako liczby calkowite oddzielone spacja, np: 1 2 ");
	    return 0;
    }
    else
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
		if(nr_blednego_parametru != 0)
		{
			if(nr_blednego_parametru == 1)
			{
				printf("Bledny parametr nr 1!");
				return 0;
			}
			else if(nr_blednego_parametru == 2)
			{
				printf("Bledny parametr nr 2!");
				return 0;
			}
			else
			{
				printf("Niepoprawne oba parametry!");
				return 0;
			}
		}
    
    }
	printf("Czytelnicy: %d, pisarze %d", liczba_czytelnikow, liczba_pisarzy);
        
	return 0;
	
}
