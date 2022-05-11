#include <stdio.h>
#include <stdlib.h>




int main(int arg,char **argv)
{
	//deklaracja zmiennych zaleznych od parametrow
    char *plik_zrodlowy;
    char *plik_docelowy;
    int czas=3;
    //
    //kod na sprawdzenie parametrow
    bool czy_parametry_poprawne=true;

    if(argc<=2 || argc>6) {
        czy_parametry_poprawne = false;
        printf("\nzla liczba parametrow");
    }
    else {
        //analiza i przypisywanie parametrow
        for(int i = 1; i <  argc; i++)
        {
            printf("\nparametr %d-ty to: %s",i, argv[i]);
            if(argv[i][0] == '-')
            {
                if(argv[i][1] == 'R')
                {
                    rekurencja = true;
                }
                else if(argv[i][1] == 'i')
                {
                    if(i+1 < argc )
                    {
                        czas=atoi(argv[i+1]);
                        i++;
                        printf("\nparametr %d-ty to: %s",i, argv[i]);
                    }
                    else
                    {
                        printf("\nnie podano czasu");
                    }
                }
                else
                {
                    printf("\nzly parametr z myslnikiem na poczatku");
                    czy_parametry_poprawne=false;
                    break;
                }
            }
            else if(i+1<argc)
            {
                plik_zrodlowy=argv[i];
                i++;
                plik_docelowy=argv[i];
                printf("\nparametr %d-ty to: %s",i, argv[i]);
            }
            else
            {
                printf("\nzle parametry wejsciowe");
                czy_parametry_poprawne=false;
                break;
            }



        }
        //
    }
    if(!czy_parametry_poprawne)
    {
        printf("\n Parametry niepoprawne");
        printf("\nPoprawne parametry to: ");
        printf("\nsynchronizacja_katalogow plik_zrodlowy plik_docelowy [-i czas(liczba calkowita)] [-R] ");


        return -1;
    }
    else
    {
        printf("\nParametry sa poprawne.");
    }
}