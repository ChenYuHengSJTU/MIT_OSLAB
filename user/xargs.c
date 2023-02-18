#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "kernel/fs.h"
#include "user/user.h"
#include <stddef.h>

#define MAXARGLEN 20

// static char exe_path[DIRSIZ+2]="./";

static char* Argv[MAXARG];
// static char buffer[MAXARG*MAXARGLEN]="";
static char tmp[MAXARGLEN];
static char* p;

static uint32 Argc,init_argc;

void Debug(){
    char** argv = (char**)Argv;
    for(int i=0;i<Argc;++i){
        printf("%s\n",argv[i]);
    }
    printf("\n");
}

void Init(){
    for(int i=0;i<MAXARG;++i){
        Argv[i]=malloc(MAXARGLEN);
        memset(Argv[i],0,MAXARGLEN);
    }
}

void Clear(){
    for(int i=init_argc;i<Argc;++i){
        memset(Argv[i],0,MAXARGLEN);
    }
    Argc=init_argc;
}

void Exec(){
    Debug();
    if(fork()==0){
        exec(Argv[0],Argv);
        exit(0);
    }
    else{
        wait((int*)0);
    }
}

// Doubt life!!!
void Test(){
    char* argv[4]={"grep","hello","./a/b",NULL};
    if(fork()==0){
        exec(argv[0],argv);
        exit(0);
    }
    else wait((int*)0);
}

int main(int argc,char* argv[]){
    // test();

    if(argc < 2){
        printf("Xargs: too few arguments\n");
        exit(1);
    }

    Init();

    init_argc = argc - 1;
    Argc=init_argc;

    // printf("%d\n",argc);
    // for(int i=0;i<argc;++i) printf("%s\n",argv[i]);

    for(int i=1;i<argc;++i){
        strcpy(Argv[i-1],argv[i]);
    }

    memset(tmp,0,MAXARGLEN);
    p = &tmp[0];
    
    char ch;
    while(read(0,&ch,1)){
        // printf("%c",ch);
        if(ch == ' ' || ch == '\n'){
            *p++ = '\0';
            // printf("tmp: %s\n",tmp);
            strcpy(Argv[Argc],tmp);
            memset(tmp,0,MAXARGLEN);
            p = &tmp[0];
            Argc++;
        }
        if(ch == '\n'){
            // strcpy(exe_path+2,argv[1]);
            // strcpy(Argv[0],argv[0]);
            // printf("%s\n",exe_path);
            // printf("%s\n",Argv[0]);
            // Exec();
            
            // Important!!!
            char* argvs[32];
            for(int i=0;i<Argc;++i) argvs[i]=Argv[i];
            argvs[Argc]=NULL;

            if(fork()==0){
                exec(Argv[0],argvs);
                exit(0);
            }
            else{
                wait((int*)0);
            }
            Clear();
        }
        else{
            *p++ = ch;
        }
    }
    exit(0);
}