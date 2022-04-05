#pragma once
#include<queue>
#include<pthread.h>

using namespace std;

using callback=void(*)(void* arg);

//task struct
template <typename T>
struct Task{
Task(){
function=nullptr;
arg=nullptr;
}
Task(callback f,void* arg){
    this->arg=(T*)arg;
    function=f;
}
void (*function)(void* arg);
T* arg;
};

template<typename T>
class TaskQueue{
public:
TaskQueue();
~TaskQueue();

void addTask(Task<T> task);
void addTask(callback f,void* arg);

Task<T> takeTask();

inline int taskNumber(){
return m_taskQ.size();
}
private:
pthread_mutex_t m_mutex;
std::queue<Task<T>>  m_taskQ;
};