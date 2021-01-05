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
int send, receive;

static void sighandler(int signo) {
    //not entirely sure why ctrl + c on the client kills the server (on a separate terminal too!),
    //so I had to put this here to catch it
    if (signo == SIGINT) {
        close(send);
        close(receive);
        exit(0);
    }
}

int main() {
    signal(SIGINT, sighandler);
    char input[buff_size]; 
    char output[buff_size]; 

    char private_pipe[128];
    sprintf(private_pipe, "%d", getpid());
    mkfifo(private_pipe, 0666);

    send = open("WKP", O_WRONLY); 
    write(send, private_pipe, buff_size);  

    printf("Waiting For Response From Server\n");

    
    receive = open(private_pipe, O_RDONLY);
    read(receive, input, buff_size); 
    remove(private_pipe);
    printf("%s\n", input);

    printf("Sending Message Back to Server to Complete Handshake \n");
    write(send, "Handshake Complete", buff_size);
    fflush(stdin);

    while(1) {
        printf("\nInput: \n");
        fgets(input, buff_size, stdin);
        write(send, input, buff_size);
        read(receive, output, buff_size);
        printf("\nOutput:\n"); 
        printf("Number of Characters: %d\n", output[0]);
    }
    
    return 0;
    
}
