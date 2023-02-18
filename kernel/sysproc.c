#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

#define MASK 0x00000001
char* syscall_name[32]={
  "syscall fork","syscall exit","syscall wait","syscall pipe","syscall read","syscall kill",
  "syscall exec","syscall fstat","syscall chdir","syscall dup","syscall getpid","syscall sbrk",
  "syscall sleep","syscall uptime","syscall open","syscall write","syscall mknod",
  "syscall unlink","syscall link","syscall mkdir","syscall close","syscall trace","syscall sysinfo"
};

int traced_syscall[32]={0};
// added
uint64
sys_trace(void){
  int mask;
  argint(0, &mask);
  // printf("mask: %d\n",mask);
  for(int i = 1;i <= 22;++i){
    if((mask >> i) & MASK){
      traced_syscall[i] = 1;
      // printf("traced %d\n",i);
    }
  }
  return 0;
}

extern int bytes_unused();
extern int count_proc();

uint64
sys_sysinfo(void){
  printf("called\n");

  struct sysinfo info;
  // argaddr(0,(uint64*)(&info));
  info.nproc = count_proc();
  info.freemem = bytes_unused();
  struct proc *p=myproc();

  uint64 addr;
  if(argaddr(0,&addr) < 0) return -1;

  if(copyout(p->pagetable,addr,(char*)&info,sizeof(info)) < 0) return -1;

  printf("freemem: %d\tnproc: %d\n",info.freemem,info.nproc);

  return 0;
}