#include "ThreadPool.hpp"
#include "ThreadPool.cpp"
#include "TaskQueue.hpp"
#include "TaskQueue.cpp"
#include <iostream>
#include <unistd.h>
#include <stdio.h>

using namespace std;

void taskFunc(void *arg)
{
    int num = *(int *)arg;
    cout << "thead " << to_string(pthread_self()) << " is working,number=" << num << endl;
    sleep(1);
}

int main()
{
    // create threadpool

    ThreadPool<int> pool(3, 10);
    for (int i = 0; i < 50; i++)
    {
        int *num = (int *)malloc(sizeof(int));
        *num = i + 100;
        pool.addTask(Task<int>(taskFunc, num));
    }
    sleep(20);
    // auto ~ThreadPool
    return 0;
}