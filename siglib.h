#include<stdio.h>
#include<pthread.h>

struct sig_thread{
	pthread_t threadId;
	int sig_set_flag;
};


struct sig_thread * register_thread(pthread_t thread_id){

	struct sig_thread *s = (struct sig_thread*)malloc(sizeof(struct sig_thread));
	s->threadId = thread_id;
	s->sig_set_flag = 1;
	printf("Thread registed with id %lu\n",thread_id);

	return s;
}



struct sig_thread * de_register_thread(pthread_t thread_id){

	struct sig_thread *s = (struct sig_thread*)malloc(sizeof(struct sig_thread));
	s->threadId = thread_id;
	s->sig_set_flag = 0;
	printf("Thread de-registed with id %lu\n",thread_id);

	return s;
}