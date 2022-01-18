//
// Created by john on 2022/1/18.
//

#ifndef MYWEBSERVER_LOCKER_H
#define MYWEBSERVER_LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>


//sem_init函数用于初始化一个未命名的信号量
//sem_destory函数用于销毁信号量
//sem_wait函数将以原子操作方式将信号量减一,信号量为0时,sem_wait阻塞
//sem_post函数以原子操作方式将信号量加一,信号量大于0时,唤醒调用sem_post的线程
//把对信号的所有操作封装在一个类里,提供接口,方便管理
class sem{
public:
    //无参构造函数
    sem(){
        //sem_init该函数初始化由 sem 指向的信号对象，并给它一个初始的整数值 value。
        //pshared 控制信号量的类型，值为 0 代表该信号量用于多线程间的同步，值如果大于 0 表示可以共享，用于多个相关进程间的同步
        if(sem_init(&m_sem, 0, 0) != 0){
            throw std::exception();
        }
    }
    //有参构造函数,初始化信号量的初始值为num
    sem(int num){
        if(sem_init(m_sem, 0, num) != 0){
            throw std::exception();
        }
    }

    ~sem(){
        sem_destroy(m_sem);
    }

//    sem_wait 是一个阻塞的函数，测试所指定信号量的值，它的操作是原子的。
//    若 sem value > 0，则该信号量值减去 1 并立即返回。若sem value = 0，
//    则阻塞直到 sem value > 0，此时立即减去 1，然后返回。
    bool wait(){
        return sem_wait(&m_sem) == 0;
    }

    //sem_post函数以原子操作方式将信号量加一,信号量大于0时,唤醒调用sem_post的线程
    bool post(){
        return sem_post(&m_sem) == 0;
    }
private:
    //信号量
    sem_t m_sem;
};

//初始化互斥锁,同时提供给互斥锁上锁的locker接口
class locker{
public:
    locker(){
        //　pthread_mutex_init()函数是以动态方式创建互斥锁的，
        // 参数attr指定了新建互斥锁的属性。如果参数attr为NULL，
        // 则使用默认的互斥锁属性，默认属性为快速互斥锁
        if(pthread_mutex_init(&m_mutex,NULL) != 0){
            throw std::exception();
        }
    }

    ~locker(){
        pthread_mutex_destroy(&m_mutex);
    }

    //pthread_mutex_lock函数以原子操作方式给互斥锁加锁
    bool lock(){
        return pthread_mutex_lock(&m_mutex) == 0;
    }

    bool unlock(){
        return pthread_mutex_unlock(&m_mutex) == 0;
    }

    pthread* get(){
        return (&m_mutex);
    }

private:
    pthread_mutex_t m_mutex;
};

//条件变量提供了一种线程间的通知机制,当某个共享数据达到某个值时,
// 唤醒等待这个共享数据的线程
//pthread_cond_init函数用于初始化条件变量
//pthread_cond_destory函数销毁条件变量
//pthread_cond_broadcast函数以广播的方式唤醒所有等待目标条件变量的线程
//pthread_cond_wait函数用于等待目标条件变量.该函数调用时需要传入 mutex参数(加锁的互斥锁) ,函数执行时,
//  先把调用线程放入条件变量的请求队列,然后将互斥锁mutex解锁,当函数成功返回为0时,互斥锁会再次被锁上.
//  也就是说函数内部会有一次解锁和加锁操作.
class cond{
public:
    //初始化一个条件变量。当参数cattr为空指针时，函数创建的是一个缺省的条件变量。
    // 否则条件变量的属性将由cattr中的属性值来决定
    cond(){
        if(pthread_cond_init(&m_cond, NULL) != 0){
            throw std::exception();
        }
    }
    ~cond(){
        pthread_cond_destroy(&m_cond);
    }
    bool wait(pthread_mutex_t* m_mutex){
        return pthread_cond_wait(m_cond, m_mutex) == 0;
    }
    bool broadcast(){
        return pthread_cond_broadcast(&m_cond) == 0;
    }
private:
    pthread_cond_t m_cond;
};



#endif //MYWEBSERVER_LOCKER_H
