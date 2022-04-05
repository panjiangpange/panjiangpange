#pragma once
#include "TaskQueue.hpp"
#include <pthread.h>


template <typename T>
class ThreadPool{
public:
ThreadPool(int min,int max);
~ThreadPool();
void addTask(Task<T> task);
int getbusyNum();
int getliveNum();

private:
static void* worker(void *arg);
static void* manager(void* arg);
void threadExit();


TaskQueue<T>* TQ;

pthread_t managerID;
pthread_t *threadIDs;
int minNum;
int maxNum;
int liveNum;
int busyNum;
int exitNum;

pthread_mutex_t mutexPool;
pthread_cond_t noteempty;

bool shutdown;
};