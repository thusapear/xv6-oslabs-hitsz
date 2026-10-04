#include "kernel/types.h"
#include "user.h"

int main() {
    int p1[2], p2[2]; //p1:父到子，p2：子到父
    pipe(p1);
    pipe(p2);

    int parent_pid = getpid();
    
    int pid = fork();

    if (pid == 0) {               //子进程
        close(p1[1]);          
        close(p2[0]);   

        char buf[16] = {0};

        int n = read(p1[0], buf, sizeof(buf) - 1);
        if (n > 0){
            buf[n] = '\0';
        }

        printf("%d: received %s from pid %d\n", getpid(), buf, parent_pid);

        write(p2[1], "pong", 4);
        close(p1[0]);
        close(p2[1]);
        exit(0);

         
    } else if (pid > 0) {        //父进程
        close(p1[0]);   
        close(p2[1]);   
        
        write(p1[1], "ping", 4);

        char buf[16] = {0};

        int n = read(p2[0], buf, sizeof(buf)-1);
        if (n > 0){
            buf[n] = '\0';
        }

        wait(0);

        printf("%d: received %s from pid %d\n", getpid(), buf, pid);

        close(p1[1]);
        close(p2[0]);
        exit(0);  

    } else {
        printf("error\n");
        exit(1);
    }
                       
}