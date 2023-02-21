#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include "kernel/fs.h"

#define Bufsize 512

// the char[] path should be defined in main as a char[512] ended with '\0'
void find(char* path,char* target){
    char buf[512],*p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path,0)) < 0){
        fprintf(2,"find: cannot open %s [1]\n",path);
        return;
    }

    if(fstat(fd, &st)<0){
        fprintf(2,"find: cannot stat %s [1]\n", path);
        close(fd);
        return;
    }

    switch(st.type){
        case T_FILE:
            break;
        case T_DIR:
            if(strlen(path)+strlen(target)+1>Bufsize){
                printf("find: path is too long\n");
                break;
            }
            strcpy(buf,path);
            p=buf+strlen(buf);
            *p++ = '/';
            while(read(fd, &de, sizeof(de)) == sizeof(de)){
                if(de.inum == 0)
                    continue;
                if(strcmp(de.name,"..")==0 || strcmp(de.name,".")==0) 
                    continue;
                if(strcmp(de.name,target)==0){
                    printf("%s/%s\n",path,target);
                }
                memmove(p,de.name,DIRSIZ);
                if(stat(buf, &st) < 0){
                    printf("find: cannot stat %s [2]\n", buf);
                    continue;
                }
                find(buf,target);
            }
            break;
    }
    // close() is so important
    // do not forget to close , otherwise some errors may occur
    close(fd);
    return;
}

int main(int argc,char* argv[]){
    if(argc<2){
        printf("Error:Too few arguments for find\n");
        exit(1);
    }
    if(argc == 2)
        find(".",argv[1]);
    else find(argv[1],argv[2]);
    exit(0);
}