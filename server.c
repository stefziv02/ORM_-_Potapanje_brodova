#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#define DEFAULT_PORT   30000

#define IGRACI 2

int mestoPolja;
int krajIgre = 0;
unsigned char poljaPrvogIgraca[9];
int preostalaPoljaPrvogIgraca = 3;
unsigned char poljaDrugogIgraca[9];
int preostalaPoljaDrugogIgraca = 3;
int id1 = 1;
int id2 = 2;
int trenutniIgrac = 0;

void daLiJeKrajIgre(){
    krajIgre = 0;
    if(preostalaPoljaPrvogIgraca == 0){
        krajIgre = 2; //drugi pobedio
    } else if(preostalaPoljaDrugogIgraca == 0){
        krajIgre = 1; //prvi pobedio
    }
}

typedef struct {
    int sock;
    int protivnik;
    pid_t klijent_pid;
    unsigned char poljaProtivnika[9];
    int id;

} KLIENT;

static pthread_mutex_t mutex;

void* Igrac(void* arg) {
    pthread_mutex_lock(&mutex);
    KLIENT klient = *(KLIENT*) arg;

    while(1){ //prvi treba da dobija signal da igra dok ne promasi
        if(krajIgre != 0){
            break; //gotova igra, prekini izvrsavanje
        }
        trenutniIgrac = klient.id;
        printf("SALJEM INFORMACIJU %d. DA KRENE DA IGRA\n", klient.id);//server salje prvom da krene da pogadja

        if (kill(klient.klijent_pid, SIGUSR1) == 0) {
            printf("Signal SIGUSR1 poslat klijentu sa ID-em %d da nastavi sa radom.\n", klient.id);
        } else {
            perror("Greška pri slanju signala klijentu\n");
        }
        
        printf("CEKAM MESTOPOLJA OD %d. IGRACA\n", klient.id);//dobija mestoPolja od igraca
        sleep(1);
        
        recv(klient.sock, &mestoPolja, sizeof(mestoPolja), 0);
        printf("PRIMIO SAM MESTOPOLJA OD %d. IGRACA\n", klient.id);
        
        if(mestoPolja == -1){ //ovo je u slucaju da je igrac izabrao vec igrano polje
            printf("KLIJENT %d IZABRAO VEC IGRANO POLJE.\n", klient.id);
            break; //kod klijenta kad izabere igrano polje i posalje mestoPolja -1, nema provere daLiJeKrajIgre, i zato if stoji pre daLiJeKrajIgre na serveru
        } else if(klient.poljaProtivnika[mestoPolja] == 'X' || klient.poljaProtivnika[mestoPolja] == 'Y'){
            if(klient.id == 1){ //ovo je prvi igrac u tom slucaju sam pogodio brod drugog igraca, smanji njegova polja{
                preostalaPoljaDrugogIgraca--;
            } else {
                preostalaPoljaPrvogIgraca--;
            }
        }
        printf("SALJI NAZAD VREDNOST POLJA NA MESTOPOLJA %d IGRACU\n", klient.id);
        //salje poljaProtivnika[mestoPolja] trenutnom igracu
        if( send(klient.sock, &klient.poljaProtivnika[mestoPolja], sizeof(klient.poljaProtivnika[mestoPolja]), 0) < 0)
        {
            puts("Send failed\n");
            break;
        }

        printf("KRAJ IGRE?\n");
        daLiJeKrajIgre(); //server vraca 0 ako se i dalje igra, 1 ako je prvi pobedio, 2 ako je drugi pobedio
        printf("SALJEM KLIJENTU %d DA LI JE KRAJ IGRE\n", klient.id);//ovde server salje informaciju klijentu da li je kraj igre
        if( send(klient.sock, &krajIgre, sizeof(krajIgre), 0) < 0)
        {
            puts("Send failed\n");
            break;
        }

        if(klient.poljaProtivnika[mestoPolja] == ' '){
            break; //treba suprotan igrac da igra sad
        }
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc , char *argv[])
{
    ////1. INICIJALIZACIJA////

    int socket_desc , client_sock[IGRACI], c;
    struct sockaddr_in server , client[IGRACI];

    KLIENT prvi;
    KLIENT drugi;

    pthread_t hprvi;
    pthread_t hdrugi;

    //Kreiranje socketa
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket\n");
    }
    puts("Socket created\n");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(DEFAULT_PORT);

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error\n");
        return 1;
    }
    puts("bind done\n");

    //Listen
    listen(socket_desc , 3);
    c = sizeof(struct sockaddr_in);

    printf("CEKANJE NA IGRACA 1...\n");
    client_sock[0] = accept(socket_desc, (struct sockaddr *)&client[0], (socklen_t*)&c);
    if (client_sock[0] < 0)
    {
        perror("accept failed\n");
        return 1;
    }
    printf("IGRAC 1 POVEZAN\n"); //prvi povezan
    //server salje signal prvom igracu da je prvi igrac  
    
    if( send(client_sock[0], &id1, sizeof(int), 0) < 0) //salje se soket igracu
    {
        puts("Send failed\n");
        return 1;
    }

    printf("CEKANJE NA IGRACA 2...\n");
    client_sock[1] = accept(socket_desc, (struct sockaddr *)&client[1], (socklen_t*)&c);
    if (client_sock[1] < 0)
    {
        perror("accept failed\n");
        return 1;
    }
    printf("IGRAC 2 POVEZAN\n"); //drugi povezan
    //server salje signal drugom igracu da je drugi igrac

    if( send(client_sock[1], &id2, sizeof(int), 0) < 0) //salje se soket drugom igracu
    {
        puts("Send failed\n");
        return 1;
    }

    pid_t klijent_pid1;
    if (recv(client_sock[0], &klijent_pid1, sizeof(pid_t), 0) == -1) {
        perror("Greška pri prijemu PID-a od servera\n");
        return 1;
    }

    // Slanje signala SIGUSR1 klijentu
    if (kill(klijent_pid1, SIGUSR1) == 0) {
        printf("Signal SIGUSR1 poslat klijentu sa ID-em %d da nastavi sa radom.\n", id1);
    } else {
        perror("Greška pri slanju signala klijentu\n");
    }

    sleep(1);

    pid_t klijent_pid2;
    if (recv(client_sock[1], &klijent_pid2, sizeof(pid_t), 0) == -1) {
        perror("Greška pri prijemu PID-a od servera\n");
        return 1;
    }
    // Slanje signala SIGUSR1 klijentu
    if (kill(klijent_pid2, SIGUSR1) == 0) {
        printf("Signal SIGUSR1 poslat klijentu sa ID-em %d da nastavi sa radom.\n", id2);
    } else {
        perror("Greška pri slanju signala klijentu\n");
    }

    sleep(1);

    ////2. BIRANJE POLJA////

    //server ceka dok ne primi polja oba igraca, kad klijent salje niz serveru paziti na Id
    printf("CEKAM DA OBA IGRACA IZABERU POLJA\n");
    for(int i = 0; i < 9; i++) {
        recv(client_sock[0] , &poljaPrvogIgraca[i] , sizeof(poljaPrvogIgraca[i]) , 0);
    }
    printf("PRIMLJENE KOORDINATE PRVOG IGRACA\n");
    sleep(1);
    for(int i = 0; i < 9; i++) {
        recv(client_sock[1] , &poljaDrugogIgraca[i] , sizeof(poljaDrugogIgraca[i]) , 0);
    }
    printf("PRIMLJENE KOORDINATE DRUGOG IGRACA\n");
    sleep(1);

    //parametri za nit
    prvi.klijent_pid = klijent_pid1;
    prvi.sock = client_sock[0];
    prvi.protivnik = client_sock[1];
    for(int i = 0; i < 9; i++) {
        prvi.poljaProtivnika[i] = poljaDrugogIgraca[i];
    }
    prvi.id = id1;

    drugi.klijent_pid = klijent_pid2;
    drugi.sock = client_sock[1];
    drugi.protivnik = client_sock[0];
    for(int i = 0; i < 9; i++) {
        drugi.poljaProtivnika[i] = poljaPrvogIgraca[i];
    }
    drugi.id = id2;
    
    ////3. POCINJE IGRA////

    preostalaPoljaPrvogIgraca = 3;
    preostalaPoljaDrugogIgraca = 3;
    while(preostalaPoljaPrvogIgraca != 0 && preostalaPoljaDrugogIgraca != 0){ 
        //prekidaju se runde, kad jedan igrac prvi pogodi sva 3 polja

        pthread_create(&hprvi, NULL, Igrac, (void*) &prvi);
        pthread_join(hprvi, NULL);

        pthread_create(&hdrugi, NULL, Igrac, (void*) &drugi);
        pthread_join(hdrugi, NULL);      
    }

    ////4. KRAJ IGRE////

    //saljem signal klijentu koji ceka na potez da je izgubio i da je kraj igre
    printf("SALJEM INFORMACIJU %d. DA JE IZGUBIO\n", trenutniIgrac);
    if(trenutniIgrac == 1){ //dajem suprotnom igracu potez
        if (kill(drugi.klijent_pid, SIGUSR2) == 0) {
            printf("Signal SIGUSR2 poslat klijentu\n");
        } else {
            perror("Greška pri slanju signala klijentu\n");
        }
    } else {
        if (kill(prvi.klijent_pid, SIGUSR2) == 0) {
            printf("Signal SIGUSR2 poslat klijentu\n");
        } else {
            perror("Greška pri slanju signala klijentu\n");
        }
    }

    if(krajIgre == 1){
        printf("PRVI IGRAC POBEDIO. SERVER SE GASI.\n");
    } else if(krajIgre == 2){
        printf("DRUGI IGRAC POBEDIO. SERVER SE GASI.\n");
    }
    
    close(client_sock[0]);
    close(client_sock[1]);
    close(socket_desc);

    return 0;
}