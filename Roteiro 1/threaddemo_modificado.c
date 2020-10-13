/* threaddemo.c */
/* Thread demonstration program. Note that this program uses a shared variable
   in an unsafe manner (eg mutual exclusion is not attempted!) */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

cond = 0;   /* a global (shared) variable */
const clock_t MAXDELAY = 2000000;

void delay(clock_t ticks) { /* a "busy" delay */
  clock_t start = clock();
  do
    ; while (clock() < start + ticks);
}

void * adjustX(void *n)
{  int i = (int)n;
   int x = 50;
   while (1)   /* loop forever */
    { 
      
      if (cond == 1){
        printf("child: %d\n", x);
        x += i;
        cond = 0;
        delay(rand()%MAXDELAY);
      }
      if(cond == 0){
        printf("parent: %d\n", x);
        x += i;
        cond = 1;
        delay(rand()%MAXDELAY);
      }
   }
   delay(rand()%MAXDELAY);
   return(n);
}

main()
{  int a;
   srand(time(NULL));
   pthread_t  up_thread, dn_thread;

   pthread_attr_t *attr;  /* thread attribute variable */
   attr=0;  
   int g = 50;
   printf("creating threads:\n");
   pthread_create(&up_thread,attr, adjustX, (void *)1);
   pthread_create(&dn_thread,attr, adjustX, (void *)-1);
   delay(rand()%MAXDELAY);
   while (1) /* loop forever */
   { ;} 
}
