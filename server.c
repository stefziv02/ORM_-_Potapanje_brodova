#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT   27015

int mestoPolja;
int krajIgre;
unsigned char poljaPrvogIgraca[9];
int preostalaPoljaPrvogIgraca = 3;
unsigned char poljaDrugogIgraca[9];
int preostalaPoljaDrugogIgraca = 3;

void daLiJeKrajIgre(){
    krajIgre = 0;
    if(preostalaPoljaPrvogIgraca == 0){
        krajIgre = 2; //drugi pobedio
    } else if(preostalaPoljaDrugogIgraca == 0){
        krajIgre = 1; //drugi pobedio
    } else {
        krajIgre = 0;
    }
    printf("SALJEM KLIJENTIMA KRAJIGRE\n");//salji krajIgre do oba klijenta;
}

int main(int argc , char *argv[])
{
    /* POSTAVKA 4. VEZBI TCPIP
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[DEFAULT_BUFLEN];
   
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen
    listen(socket_desc , 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    //Receive a message from client
    while( (read_size = recv(client_sock , client_message , DEFAULT_BUFLEN , 0)) > 0 )
    {
        printf("Bytes received: %d\n", read_size);
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    */

    ////1. INICIJALIZACIJA////

    printf("CEKANJE NA IGRACA 1...\n");
    printf("IGRAC 1 POVEZAN\n"); //prvi povezan
    //server salje signal prvom igracu da je prvi igrac

    printf("CEKANJE NA IGRACA 2...\n");
    printf("IGRAC 2 POVEZAN\n"); //drugi povezan
    //server salje signal drugom igracu da je drugi igrac

    //igra zapocinje kad se prikljuce dva igraca
    printf("SALJEM SIGNAL ZA POCETAK IGRE ZA OBA IGRACA\n"); //server salje signal za pocetak igre

    ////2. BIRANJE POLJA////

    //server ceka dok ne primi polja oba igraca, kad klijent salje niz serveru paziti na Id
    printf("CEKAM DA OBA IGRACA IZABERU POLJA\n");

    //kad su oba niza polja stigla
    printf("PRIMLJENE KOORDINATE PRVOG IGRACA\n");
    printf("PRIMLJENE KOORDINATE DRUGOG IGRACA\n");

    ////3. POCINJE IGRA////
    preostalaPoljaPrvogIgraca = 3;
    preostalaPoljaDrugogIgraca = 3;
    while(preostalaPoljaPrvogIgraca != 0 && preostalaPoljaDrugogIgraca != 0){ //prekidaju se runde, kad jedan igrac prvi pogodi sva 3 polja
        while(1){ //prvi treba da dobija signal da igra dok ne promasi
            printf("SALJEM INFORMACIJU PRVOM DA KRENE DA IGRA");//server salje prvom da krene da pogadja

            printf("PRIMAM INFORMACIJU (MESTOPOLJA) OD PRVOG IGRACA");//dobija mestoPolja od igraca
            if(mestoPolja == -1){ //ovo je u slucaju da je igrac izabrao vec igrano polje
                //ide na printf("KRAJ IGRE?\n"), ne menjati nista
            } else if(poljaDrugogIgraca[mestoPolja] == 'X' || poljaDrugogIgraca[mestoPolja] == 'Y'){
                preostalaPoljaDrugogIgraca--;
                printf("SALJI NAZAD VREDNOST POLJA NA MESTOPOLJA PRVOM IGRACU");
                //salje poljaDrugogIgraca[mestoPolja] prvom igracu
            } else {
                printf("SALJI NAZAD VREDNOST POLJA NA MESTOPOLJA PRVOM IGRACU");
                //salje poljaDrugogIgraca[mestoPolja] prvom igracu
            }

            printf("KRAJ IGRE?\n");
            daLiJeKrajIgre(); //server vraca 0 ako se i dalje igra, 1 ako je prvi pobedio, 2 ako je drugi pobedio
            printf("SALJEM PRVOM KLIJENTU");//ovde server salje informaciju klijentu da li je kraj igre

            if(mestoPolja == -1){
                break;
            } 
            if(poljaDrugogIgraca[mestoPolja] == ' '){
                break; //treba suprotan igrac da igra sad
            }
        }
        while(1){ //drugi treba da dobija signal da igra dok ne promasi
            printf("SALJEM INFORMACIJU DRUGOM DA KRENE DA IGRA");//server salje drugom da krene da pogadja

            printf("PRIMAM INFORMACIJU (MESTOPOLJA) OD DRUGOG IGRACA");//dobija mestoPolja od igraca
            if(mestoPolja == -1){ //ovo je u slucaju da je igrac izabrao vec igrano polje
                //ide na printf("KRAJ IGRE?\n"), ne menjati nista
            } else if(poljaPrvogIgraca[mestoPolja] == 'X' || poljaPrvogIgraca[mestoPolja] == 'Y'){
                preostalaPoljaPrvogIgraca--;
                printf("SALJI NAZAD VREDNOST POLJA NA MESTOPOLJA DRUGOM IGRACU");
                //salje poljaPrvogIgraca[mestoPolja] drugom igracu
            } else {
                printf("SALJI NAZAD VREDNOST POLJA NA MESTOPOLJA DRUGOM IGRACU");
                //salje poljaPrvogIgraca[mestoPolja] drugom igracu
            }

            printf("KRAJ IGRE?\n");
            daLiJeKrajIgre();
            printf("SALJEM DRUGOM KLIJENTU");//ovde server salje informaciju klijentu da li je kraj igre

            if(mestoPolja == -1){
                break;
            } 
            if(poljaPrvogIgraca[mestoPolja] == ' '){
                break; //treba suprotan igrac da igra sad
            }
        }
    }

    ////4. KRAJ IGRE////
    if(krajIgre == 1){
        printf("PRVI IGRAC POBEDIO. SERVER SE GASI.");
    } else if(krajIgre == 2){
        printf("DRUGI IGRAC POBEDIO. SERVER SE GASI.");
    }


   return 0;
}