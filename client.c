#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

int buff_size = 256;
int send, receive, server_pid;

static void sighandler(int signo) {
    if (signo == SIGINT) {
        //experimented with a lot of ways, but sending a SIGPIPE signal to server most reliably reset it
        kill(server_pid, SIGPIPE);
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

    printf("Server PID: %d\n", atoi(input));
    server_pid = atoi(input);

    printf("Sending Message Back to Server to Complete Handshake \n");
    write(send, "Handshake Complete", buff_size);
    //program doesn't continue if i don't flush stdin for some reason?
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
