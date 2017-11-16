//
// Created by mac on 16/8/17.
//

#include "ThreadPool.h"
#include "GLThreadUtilJni.h"

void CTask::SetData(void * data)
{
    m_ptrData = data;
}

CTask* CThreadPool::m_vecTaskList = NULL;         //任务列表
bool CThreadPool::shutdown = false;

int list_size(CTask *root) {
    int count = 0;
    while(root != NULL)
    {
        count++;
        root = root->next;
    }
    return count;
}

CTask * list_erase_head(CTask *task) {
    if (NULL == task) {
        return NULL;
    }
    CTask *next = task->next;
    CTask *prev = task->prev;
    if (NULL != next) {
    next->prev = prev;
    }
    if(NULL != prev) {
        prev->next = next;
    }

    task->next = NULL;
    task->prev = NULL;
    return next;
}

void list_push_back(CTask **root, CTask *t) {
    if(NULL == root) return;
    if(NULL == *root)
    {
        *root = t;
        return;
    }

    CTask *now = *root;
    while(now->next != NULL)
    {
        now = now->next;
    }

    now->next = t;
    t->prev = now;
    t->next = NULL;
}

pthread_mutex_t CThreadPool::m_pthreadMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t CThreadPool::m_pthreadCond = PTHREAD_COND_INITIALIZER;
bool CThreadPool::mbNeedMakeCurrent = true;

/**
 * 线程池管理类构造函数
 */
CThreadPool::CThreadPool(int threadNum)
{
    mbCreate = false;
    m_iThreadNum = threadNum;
//    Create();
}

CThreadPool::~CThreadPool()
{
    StopAll();
}

/**
 * 线程回调函数
 */
void* CThreadPool::ThreadFunc(void* threadData)
{

    while (1)
    {
        pthread_mutex_lock(&m_pthreadMutex);
        while (list_size(m_vecTaskList) == 0 && !shutdown)
        {
            pthread_cond_wait(&m_pthreadCond, &m_pthreadMutex);
        }

        if (shutdown)
        {
            pthread_mutex_unlock(&m_pthreadMutex);
            pthread_exit(NULL);
        }

        /**
        * 取出一个任务并处理之
        */
        CTask* task = m_vecTaskList;
        if (task != NULL)
        {
            m_vecTaskList = list_erase_head(m_vecTaskList);
        }

        pthread_mutex_unlock(&m_pthreadMutex);

        if( mbNeedMakeCurrent ){
            if( !eglMakeCurrent( gDisplay, gAuxSurface, gAuxSurface, gShareContext )){
                LOGI("make current error");
            }
            mbNeedMakeCurrent = false;
        }

        task->Run(); /** 执行任务 */
        delete task;
    }
    return (void*)0;
}

/**
 * 往任务队列里边添加任务并发出线程同步信号
 */
int CThreadPool::AddTask(CTask *task)
{
    pthread_mutex_lock(&m_pthreadMutex);
    list_push_back(&m_vecTaskList, task);
    pthread_mutex_unlock(&m_pthreadMutex);
    pthread_cond_signal(&m_pthreadCond);
    return 0;
}

/**
 * 创建线程
 */
int CThreadPool::Create()
{
    if( mbCreate ){
        mbNeedMakeCurrent = true;
        return 0;
    }
    mbCreate = true;
    pthread_id = (pthread_t*)malloc(sizeof(pthread_t) * m_iThreadNum);
    for(int i = 0; i < m_iThreadNum; i++)
    {
        pthread_create(&pthread_id[i], NULL, ThreadFunc, NULL);
    }
    return 0;
}

/**
 * 停止所有线程
 */
int CThreadPool::StopAll()
{
    /** 避免重复调用 */
    if (shutdown)
    {
        return -1;
    }
//    printf("Now I will end all threads!!/n");
    /** 唤醒所有等待线程，线程池要销毁了 */
    shutdown = true;
    pthread_cond_broadcast(&m_pthreadCond);

    /** 阻塞等待线程退出，否则就成僵尸了 */
    for (int i = 0; i < m_iThreadNum; i++)
    {
        pthread_join(pthread_id[i], NULL);
    }

    free(pthread_id);
    pthread_id = NULL;

    /** 销毁条件变量和互斥体 */
    pthread_mutex_destroy(&m_pthreadMutex);
    pthread_cond_destroy(&m_pthreadCond);

    return 0;
}

/**
 * 获取当前队列中任务数
 */
int CThreadPool::getTaskSize()
{
    return list_size(m_vecTaskList);
}

