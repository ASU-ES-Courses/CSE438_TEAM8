#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
 
#define Num_Periodic_Threads 5

struct threadID
{
int ptid;
};

void *thread_func(void *a){
struct threadID *point = (struct threadID *)a;
printf(" Thread ID of the running thread is %d\n", point->ptid);
return NULL;
}

int main()
{
struct threadID ID[Num_Periodic_Threads];

pthread_t threads[Num_Periodic_Threads];

int i;
for(i=0;i<Num_Periodic_Threads;i++)
    {
     ID[i].ptid = i;
     pthread_create(&threads[i], NULL, &thread_func,&ID[i]); 
    }

for(i=0;i<Num_Periodic_Threads;i++)
    pthread_join(threads[i],  NULL);

return 0;
 
}





