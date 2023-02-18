#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc,char* argv[]){
    // int ptoc_p[2],ctop_p[2];
    // pipe(ptoc_p);
    // pipe(ctop_p);
    char byte[2];
    int p[2];
    pipe(p);
    byte[1]='\0';
    if(fork()==0){
        // read(ptoc_p[0],byte,1);
        // printf("%d: received ping\n",getpid());
        // write(ctop_p[1],byte,1);
        read(p[0],byte,1);
        printf("%d: received ping\n",getpid());
        write(p[1],byte,1);
        exit(0);
    }
    else{
        // write(ptoc_p[1],"c",1);
        // wait((int*)(0));
        // read(ctop_p[0],byte,1);
        write(p[1],"c",1);
        wait((int*)(0));
        read(p[0],byte,1);
        printf("%d: received pong\n",getpid());
        exit(0);
    }
    exit(0);
}