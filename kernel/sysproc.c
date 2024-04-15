#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  // int pgaccess(void *base, int len, void *mask);
  int n;  // number of pages
  uint64 base_addr, mask_addr;
  uint32 mask = 0;  // at most 32 pages
  pagetable_t upt = myproc()->pagetable;
  pte_t *upte;

  // parse arguments
  argint(1, &n);
  if(n > 32) {  // define upper limit on the number of pages
    return -1;
  }
  argaddr(0, &base_addr);
  argaddr(2, &mask_addr);

  upte = walk(upt, base_addr, 0);

  for (int i = 0;i < n;i++) {
    // see upte as array to traverse current pte to pte + n
    if ((upte[i] & PTE_V) && (upte[i] & PTE_A)) {
      mask = mask | (1 << i);
      upte[i] = upte[i] ^ PTE_A;
    }
  }

  copyout(upt, mask_addr, (char *)&mask, sizeof(uint32));

  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
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
