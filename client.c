#include <stdio.h>     //printf
#include <string.h>    //strlen
#include <sys/socket.h>//socket
#include <arpa/inet.h> //inet_addr
#include <fcntl.h>     //for open
#include <unistd.h>    //for close
#include <time.h>      //vremenska biblioteka za delay
#include <stdlib.h>
#include <termios.h>   //za input sa terminala
#include <unistd.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

int validanUnos;
int mestoPolja;
int zauzetoPolje;
int susednoJe;
int krajIgre;
int idIgraca = 0;
//promenjive za polja
unsigned char mojaPolja[9];
unsigned char protivnikovaPolja[9];
unsigned char dobijeniKarakterProtivnikovogPolja;
char unos[2];

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
 
    // Storing start time
    clock_t start_time = clock();
 
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds);
}

void whileProveraValidnostiUnosa(){
    while(1){//proverava se validnost, dokle god se ne unese polje dobrog formata xy (x=A,B,C ; y=1,2,3)
        scanf("%s", unos);
        if(proveraValidnostiUnosa() == -1){ //provera validnosti
            printf("NEVALIDAN UNOS! PROBAJ PONOVO: \n");
            continue;
        }
        break;
    }
}

int proveraValidnostiUnosa(){ //1 ako validan unos, -1 ako nije
    validanUnos = 1;
    if(unos[0] != 'A' && unos[0] != 'B' && unos[0] != 'C'){
        validanUnos = -1;
    }
    if(unos[1] != '1' && unos[1] != '2' && unos[1] != '3'){
        validanUnos = -1;
    }
    if(validanUnos == 1){
        izracunajMestoPolja();
    }
    return validanUnos;
}

void ispisTabele(){
    //ispis tabele
    printf("  A B C\n");
    printf(" -------\n");
    printf("1|%c|%c|%c|\n", mojaPolja[0], mojaPolja[1],mojaPolja[2]);
    printf(" -------\n");
    printf("2|%c|%c|%c|\n", mojaPolja[3], mojaPolja[4],mojaPolja[5]);
    printf(" -------\n");
    printf("3|%c|%c|%c|\n", mojaPolja[6], mojaPolja[7],mojaPolja[8]);
    printf(" -------\n\n");
}

void ispisTabeleProtivnika(){
    //ispis tabele
    printf("  A B C\n");
    printf(" -------\n");
    printf("1|%c|%c|%c|\n", protivnikovaPolja[0], protivnikovaPolja[1],protivnikovaPolja[2]);
    printf(" -------\n");
    printf("2|%c|%c|%c|\n", protivnikovaPolja[3], protivnikovaPolja[4],protivnikovaPolja[5]);
    printf(" -------\n");
    printf("3|%c|%c|%c|\n", protivnikovaPolja[6], protivnikovaPolja[7],protivnikovaPolja[8]);
    printf(" -------\n\n");
}

void izracunajMestoPolja(){ //nakon pravilnog unosa ovo vraca kom mestu u mojaPolja nizu pripada polje
    mestoPolja = 0;
    if(unos[0] == 'B'){//mesto polja A1 je 0,0
        mestoPolja += 1;
    } else if(unos[0] == 'C'){
        mestoPolja += 2;
    }
    if(unos[1] == '2'){
        mestoPolja += 3;
    } else if(unos[1] == '3'){
        mestoPolja += 6;
    }
}

void whileDaLiJeZauzetoPolje(){
    while(1){
        whileProveraValidnostiUnosa();
        if(daLiJeZauzetoPolje() == 1){
            printf("\nPOLJE VEC ZAUZETO. PROBAJ OPET:");
            continue;
        }
        break;
    }
}

int daLiJeZauzetoPolje(){ //ako -1 nije zauzeto, ako 1 jeste zauzeto
    zauzetoPolje = -1;
    if(mojaPolja[mestoPolja] == 'X' || mojaPolja[mestoPolja] == 'Y'){ //ako da, polje je vec zauzeto
        zauzetoPolje = 1;
    }
    return zauzetoPolje;
}

void whileDaLiJeSusednoOdY(){
    while(1){
        whileDaLiJeZauzetoPolje();
        if(daLiJeSusednoOdY() == -1){
            printf("\nUNETO POLJE NIJE SUSEDNO PRVOM DELU. PROBAJ OPET:");
            continue;
        }
        break;
    }
}

int daLiJeSusednoOdY(){ //-1 nije susedno, 1 susedno je
    susednoJe = -1;
    if(unos[0] == 'A' && mojaPolja[mestoPolja+1] == 'Y'){ //provera po horizontali sa desne strane
        susednoJe = 1;
    } else if(unos[0] == 'B'){ //provera po horizontali sa leve i desne strane
        if(mojaPolja[mestoPolja+1] == 'Y' || mojaPolja[mestoPolja-1] == 'Y'){
            susednoJe = 1;
        }
    } else if(unos[0] == 'C' && mojaPolja[mestoPolja-1] == 'Y'){ //provera po horizontali sa leve strane
        susednoJe = 1;
    }

    if(unos[1] == '1' && mojaPolja[mestoPolja+3] == 'Y'){ //provera po vertikali sa donje strane
        susednoJe = 1;
    } else if(unos[1] == '2'){ //provera po vertikali sa donje i gornje strane
        if(mojaPolja[mestoPolja+3] == 'Y' || mojaPolja[mestoPolja-3] == 'Y'){
            susednoJe = 1;
        }
    } else if(unos[1] == '3' && mojaPolja[mestoPolja-3] == 'Y'){ //provera po vertikali sa gornje strane
        susednoJe = 1;
    }
    return susednoJe;
}

int main(int argc , char *argv[])
{
    /*
    int sock;
    struct sockaddr_in server;
    char *message = "this is a test";

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(DEFAULT_PORT);

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    //Send some data
    if( send(sock , message , strlen(message), 0) < 0)
    {
        puts("Send failed");
        return 1;
    }

    puts("Client message:");
    puts(message);

    close(sock);
    */

    ////1. INICIJALIZACIJA////
    printf("POVEZUJEM SE SA SERVEROM...\n"); //povezivanje
    printf("USPESNO POVEZAN.\n");
    //i prima informaciju koji je igrac po redu 1 - prvi, 2 - drugi i smesta u idIgraca
    printf("JA SAM IGRAC %d\n", idIgraca);

    printf("CEKAM DA SVI BUDU SPREMNI\n"); //cekanje na signal

    //klijent prima signal da su oba igraca spremna
    printf("POCINJE IGRA.\n");

    ////2. BIRANJE POLJA////
    delay(1000); //temporary

    for(int i = 0;i < 9;i++){ //postavljanje promenjivih na prazna polja
        mojaPolja[i] = ' ';
        protivnikovaPolja[i] = ' ';
    }

    //1x1
    system("clear"); //brise se sve sa terminala
    ispisTabele();
    printf("IGRACU %d, IZABERI POLJE ZA BROD 1x1: ", idIgraca);
    whileProveraValidnostiUnosa();
    mojaPolja[mestoPolja] = 'X'; //X je 1x1 brod

    //uspesno unet 1x1, unosim prvi deo 2x1 broda
    system("clear"); //brise se sve sa terminala
    ispisTabele();
    printf("IZABERI PRVO POLJE ZA BROD 2x1: ");
    whileDaLiJeZauzetoPolje();
    mojaPolja[mestoPolja] = 'Y'; //Y je 2x1 brod, treba da budu 2 y

    //uspesno unet prvi deo 2x1 broda, unosim drugi deo 2x2 broda
    system("clear"); //brise se sve sa terminala
    ispisTabele();
    printf("IZABERI DRUGO POLJE ZA BROD 2x1: ");
    whileDaLiJeSusednoOdY();
    mojaPolja[mestoPolja] = 'Y';

    //Klijent ovde salje vrednosti polja serveru
    printf("SALJEM SERVERU POLJA...\n");
    printf("USPESNO POSLATA POLJA SERVERU...\n");

    ////3. POCINJE IGRA////

    delay(2000); //temporary

    while(1){
        printf("CEKAM SVOJ POTEZ\n");//klijent prima signal da je na redu, mora da ceka na signal
        //Prvi igrac ce prvi i nagadjati, drugi mora da ceka da prvi zavrsi potez
        
        system("clear"); //brise se sve sa terminala
        printf("MOJA POLJA:\n"); 
        ispisTabele();
        printf("PROTIVNIKOVA POLJA:\n"); 
        ispisTabeleProtivnika();
        printf("POGODI POLJE PROTIVNIKOVOG BRODA: "); 

        whileProveraValidnostiUnosa();
        if(protivnikovaPolja[mestoPolja] == 'X' || protivnikovaPolja[mestoPolja] == 'Y' || protivnikovaPolja[mestoPolja] == '-'){
            //nakon sto unese pravilan unos proverava se da li je
            //to polje vec birano, ako jeste igrac gubi potez i pogadja drugi igrac
            printf("IZABRAO SAM BIRANO POLJE. SALJEM SERVERU DA SAM IZGUBIO RUNDU\n");
            //klijent ovde salje serveru mestoPolja = -1
            continue;
        }

        //klijent ovde salje serveru mestoPolja
        printf("SALJEM IZABRANO POLJE SERVERU\n");

        printf("PRIMAM OD SERVERA VREDNOST IZABRANOG POLJA");//klijent prima polje protivnika na mestoPolja lokaciji: x y ili [space],
        //i stavlja to u dobijeniKarakterProtivnikovogPolja

        if(dobijeniKarakterProtivnikovogPolja == 'X' || dobijeniKarakterProtivnikovogPolja == 'Y'){
            printf("BRAVO. POGODIO SI LOKACIJU BRODA.\n");
        } else if(dobijeniKarakterProtivnikovogPolja == ' '){
            dobijeniKarakterProtivnikovogPolja = '-';//probao lokaciju, nije pogodio
        }
        protivnikovaPolja[mestoPolja] = dobijeniKarakterProtivnikovogPolja;

        printf("DA LI JE KRAJ IGRE?");
        //ovde klijent prima krajIgre
        if(krajIgre == 0){
            continue;
        } else if(krajIgre == idIgraca){ //ako se krajIgre poklapa sa brojem igraca onda je pobedio
            printf("CESTITAM. POBEDILI STE");
            break;
        } else {
            printf("IZGUBILI STE");
            break;
        }
        delay(2000);//temporary
    }

    ////4. KRAJ IGRE////

    printf("KRAJ IGRE");
    printf("KLIJENT SE GASI");

    return 0;
}