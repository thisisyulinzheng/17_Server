#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

//did not add a run target for the makefile like the other
//this program only works if server is run first, then client 


int buff_size = 256;
char input[256]; 
int send, receive;
int connection_made = 0;

void handshake() {
    
    mkfifo("WKP", 0666); 
    printf("Opened Well Known Pipe\n");
    printf("Waiting For Handshake...\n");

    receive = open("WKP", O_RDONLY); 
    read(receive, input, buff_size); 
    remove("WKP");
    printf("Received Information From Process %s\n", input);

    send = open(input, O_WRONLY);
    write(send, "Message Received", buff_size);

    read(receive, input, buff_size);
    printf("%s\n", input);
    connection_made = 1;
}

char *count(char *string) {
    char *c = malloc(sizeof(char));
    c[0] = -1;
    int i;
    for (i = 0; i < strlen(string); i++) {
        if (string[i] != ' ') {
            (c[0])++;
        }
    }
    return c;
}

static void sighandler(int signo) {
    if (signo == SIGINT) {
        printf("I am here\n");
        connection_made = 0;
    }
}

int main() {    
    signal(SIGINT, sighandler);
    while(1) {
        if (connection_made) {
            read(receive, input, buff_size);
            write(send, count(input), buff_size);
        }
        else {
            handshake();
        }
    }
    
    //write(send, count(input), buff_size); 
    
}
