/*
 * Temat: Pomieszczenie ma dwoje drzwi: wejsciowe i wyjsciowe i moze pomiescic 20 osob.
 *          W drzwiach znajduja sie czujniki, ktore zliczaja ilosc wchodzacych
 *          i wychodzacych osob. W przypadku proby wejscia 21 osoby zablokowac drzwi
 *          wejsciowe wystawiajac 1 na dowolnym wyjsciu portu i utrzymywac ten stan
 *          dopoki liczba osob w pomieszczeniu nie spadnie do 19.
 * 
 * Authorzy:    Adrian Chmielowiec
 *              Mateusz Kubas
 * 
 */

#pragma config WDTE = OFF //uk?ad czasowy oczekujacy na potwierdzenie poprawnej pracy przez urzadzenie, co okreslony przedzial czasu. 

#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include "C:\Program Files\Microchip\xc8\v2.32\pic\include\legacy\pic16f7x_legacy.h"

unsigned char counter = 18; //volatile bo uzywane w przerwaniu - info dla kompilatora ze moze sie zmienic niespodziewanie
unsigned char lastStatEntry = 1; //poczatkowo 1 bo pull up - przechowuje ostatni stan dal drzwi wejsciowych
unsigned char lastStatExit = 1; //poczatkowo 1 bo pull up - przechowuje ostatni stan dal drzwi wyjsciowych

void __interrupt() ISR(void);

void main(void)
 {
    //czujniki wejscia/wyjscia - zakladamy ze gdy drzwi sa otwierane to port jest zwierany przez czujnik do GND czyli pojawia sie stan niski na porcie
    TRISBbits.TRISB6 = 1; //ustawienie pinu B6 jako wejscie - czujnik wejscia
    TRISBbits.TRISB7 = 1; //ustawienie pinu B7 jako wejscie - czujnik wyjscia
    TRISBbits.TRISB5 = 0; //nó?ka nie pracuje jako interrupt on change
    TRISBbits.TRISB4 = 0; //^^
    OPTION_REGbits.nRBPU = 0; //ustawienie pull'up na porty B  
    
    //zamek drzwi wejsciaowych
    TRISCbits.TRISC0 = 0; //ustawienie pinu C0 jako wyjscie
    PORTCbits.RC0 = 0; //stan niski na C0 - drzwi otwarte
     
    //ustawienia przerwan
    INTCONbits.RBIF = 0;        //reset przerwania zewnetrznego
    INTCONbits.RBIE = 1;        //wlaczenie przerwania gdy zmiana na porcie B 4:7
    INTCONbits.PEIE = 1;        //wlaczenie zewnetrznych przerwan
    INTCONbits.GIE = 1;         //globalne zezwolenie na przerwania
    
     while(1)
     {
         ;
     }
 }
 
void __interrupt() ISR(void) {
    if(INTCONbits.RBIF == 1 )   //przydatny do odroznienia przerwan
    {                           //RBIF ustawia sie na 1 gdy jest zmiana stanu na pinach B4:7 (zmiana z 1 na 0 lub z 0 na 1)
        
        if(PORTBbits.RB6 == 0 && lastStatEntry == 1){ //jezeli stan niski z czujnika wejsciowego, a wczesniej byl wysoki - ktos wchodzi
            ++counter; 
        }
        
        if(PORTBbits.RB7 == 0 && lastStatExit == 1){ //jezeli stan niski z czujnika wejsciowegeo, a wczesniej byl wysoki - ktos wychodzi
            --counter; 
        }
        lastStatEntry = PORTBbits.RB6;  //zapisanie obecnego stanu
        lastStatExit = PORTBbits.RB7;
        
        if(counter>=20){
            PORTCbits.RC0=1; //stan wysoki na C0 - drzwi zamkniete
        }
        else{
            PORTCbits.RC0=0; //stan niski na C0 - drzwi otwarte
        }
        
        INTCONbits.RBIF = 0; //reset przerwania
    }
}