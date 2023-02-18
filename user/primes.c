#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

static int fd[2];



void primes(){
    // printf("primes:\n");

    int first_num = 0,num;
    int buffer[35];
    int sz=0;

    close(fd[1]);

    if(read(fd[0],&first_num,sizeof(int))){
        printf("prime %d\n",first_num);
        while(read(fd[0],&num,sizeof(int))){
            // if(num == 0) break;
            if(num % first_num != 0){
                buffer[sz++] = num;
            }
        }
    }
    close(fd[0]);
    
    // It is important to create new pipe,otherwise the process will keep 
    // writing and read what it has write.
    pipe(fd);
    write(fd[1],buffer,sz * sizeof(int));

    if(fork() == 0){
        primes();
    }
    else{
        // important to close fd in parent process
        close(fd[0]);
        close(fd[1]);
        wait((int*)0);
    }
    exit(0);
}

int main(int argc,char * argv[]){
    pipe(fd);

    int i;
    for(i=2;i<=35;++i){
        write(fd[1],&i,sizeof(int));
    }

    // i = 0;
    // write(fd[1],&i,sizeof(int));

    primes();

    exit(0);
}