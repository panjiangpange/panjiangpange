#include "ThreadPool.hpp"
#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <pthread.h>
using namespace std;

template <typename T>
ThreadPool<T>::ThreadPool(int min, int max)
{

    TQ = new TaskQueue<T>;
    if (TQ == nullptr)
    {

        cout << "ThreadPool fail..." << endl;
        return;
    }
    // alloc memory
    threadIDs = new pthread_t[max];
    if (threadIDs == nullptr)
    {
        cout << "thread alloc fail...." << endl;
        return;
    }
    if (pthread_mutex_init(&mutexPool, NULL) != 0 || pthread_cond_init(&noteempty, NULL) != 0)
    {

        cout << "init mutex || cond fail..." << endl;
    }

    memset(threadIDs, 0, sizeof(pthread_t) * max);

    for (int i = 0; i < min; i++)
    {
        pthread_create(&threadIDs[i], NULL, worker, this);
    }
    pthread_create(&managerID, NULL, manager, this);

    minNum = min;
    maxNum = max;
    liveNum = min;
    busyNum = 0;
    exitNum = 0;
    shutdown = false;
}

template <typename T>
ThreadPool<T>::~ThreadPool()
{
    shutdown = true;
    // destory manager thread
    pthread_join(managerID, NULL);
    // alive all the consumer thread
    for (int i = 0; i < liveNum; i++)
        pthread_cond_signal(&noteempty);

    if (TQ)
        delete TQ;
    if (threadIDs)
        delete[] threadIDs;

    pthread_mutex_destroy(&mutexPool);
    pthread_cond_destroy(&noteempty);
}

template <typename T>
void ThreadPool<T>::addTask(Task<T> task)
{
    if (shutdown)
        return;
    TQ->addTask(task);
    pthread_cond_signal(&noteempty);
}

template <typename T>
int ThreadPool<T>::getliveNum()
{
    int threadNum = 0;
    pthread_mutex_lock(&mutexPool);
    threadNum = liveNum;
    pthread_mutex_unlock(&mutexPool);
    return threadNum;
}

template <typename T>
int ThreadPool<T>::getbusyNum()
{
    int busyNum = 0;
    pthread_mutex_lock(&mutexPool);
    busyNum = busyNum;
    pthread_mutex_unlock(&mutexPool);
    return busyNum;
}

template <typename T>
void *ThreadPool<T>::worker(void *arg)
{
    ThreadPool *pool = static_cast<ThreadPool *>(arg);
    while (true)
    {
        pthread_mutex_lock(&pool->mutexPool);
        while (pool->TQ->taskNumber() == 0 && !pool->shutdown)
        {
            // block all thread
            pthread_cond_wait(&pool->noteempty, &pool->mutexPool);
            if (pool->exitNum > 0)
            {
                pool->exitNum--;
                if (pool->liveNum > pool->minNum)
                {
                    pthread_mutex_unlock(&pool->mutexPool);
                    pool->threadExit();
                }
            }
        }
        if (pool->shutdown)
        {
            pthread_mutex_unlock(&pool->mutexPool);
            pool->threadExit();
        }

        Task<T> task = pool->TQ->takeTask();
        pool->busyNum++;
        pthread_mutex_unlock(&pool->mutexPool);
        cout << "thread" << to_string(pthread_self()) << "start wordking" << endl;
        task.function(task.arg);
        delete task.arg;
        task.arg = nullptr;

        // task is over
        cout << "thread " << to_string(pthread_self()) << " end working..." << endl;
        pthread_mutex_lock(&pool->mutexPool);
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexPool);
    }
    return nullptr;
}

template <typename T>
void *ThreadPool<T>::manager(void *arg)
{
    ThreadPool *pool = static_cast<ThreadPool *>(arg);
    while (!pool->shutdown)
    {
        sleep(5);

        pthread_mutex_lock(&pool->mutexPool);
        int size = pool->TQ->taskNumber();
        int liveNum = pool->liveNum;
        int busyNum = pool->busyNum;
        pthread_mutex_unlock(&pool->mutexPool);

        const int Number = 2;
        if (size > liveNum - busyNum && liveNum < pool->maxNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            int num = 0;
            for (int i = 0; i < pool->maxNum && num < Number && pool->liveNum < pool->maxNum; i++)
            {
                if (pool->threadIDs[i] == 0)
                {
                    pthread_create(&pool->threadIDs[i], NULL, worker, pool);
                    num++;
                    pool->liveNum++;
                }
            }
        }
        pthread_mutex_unlock(&pool->mutexPool);
        // if

        if (busyNum * 2 < liveNum && liveNum > pool->minNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            pool->exitNum = Number;
            pthread_mutex_unlock(&pool->mutexPool);
            for (int i = 0; i < Number; i++)
                pthread_cond_signal(&pool->noteempty);
        }
    }
    return nullptr;
}

template <typename T>
void ThreadPool<T>::threadExit()
{
    pthread_t tid = pthread_self();
    for (int i = 0; i < maxNum; i++)
    {
        if (threadIDs[i] == tid)
        {
            cout << "threadExit() function:called" << to_string(pthread_self()) << "exiting ..." << endl;
            threadIDs[i] = 0;
            break;
        }
    }
    pthread_exit(NULL);
}