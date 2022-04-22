//
// Created by john on 2022/1/25.
//

#ifndef MYWEBSERVER_BLOCK_QUEUE_H
#define MYWEBSERVER_BLOCK_QUEUE_H

//exit的头文件
#include <stdlib.h>
#include "../lock/locker.h"

template<class T>
class block_queue{
public:
    block_queue(int max_size = 1000){
        if(max_size <= 0){
            exit(-1);
        }

        m_max_size = max_size;
        //创建大小为max_size的T类型数组
        m_array = new T[max_size];
        m_size = 0;
        m_front = -1;
        m_back = -1;
    }
        void clear(){
        m_mutex.lock();
        m_size = 0;
        m_front = -1;
        m_back = -1;
        m_mutex.unlock();
    }

    ~block_queue(){
        m_mutex.lock();
        if(m_array != nullptr){
            delete [] m_array;
        }
        m_mutex.unlock();
    }

    //判断队列是否满了
    bool full()
    {
        m_mutex.lock();
        if (m_size >= m_max_size)
        {

            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();
        return false;
    }

    bool empty(){
        m_mutex.lock();
        if(m_size == 0){
            m_mutex.unlock();
            return true;
        }
        m_mutex.unlock();
        return false;
    }

    //引用获取队首的值,类似回调函数
    bool front(T& value){
        m_mutex.lock();
        if(m_size == 0){
            m_mutex.unlock();
            return false;
        }
        value = m_array[m_front];
        m_mutex.unlock();
        return true;
    }

    bool back(T& value){
        m_mutex.lock();
        if(m_size == 0){
            m_mutex.unlock();
            return false;
        }
        value = m_array[m_back];
        m_mutex.unlock();
        return true;
    }

    int size(){
        int tem = 0;
        m_mutex.lock();
        tem = m_size;
        m_mutex.unlock();
        return tem;
    }

    int max_size(){
        int tem = 0;
        m_mutex.lock();
        tem = m_max_size;
        m_mutex.unlock();
        return tem;
    }

    bool push(const T &item){
        m_mutex.lock();
        if(m_size >= m_max_size){
            m_cond.broadcast();
            m_mutex.unlock();
            return false;
        }
        m_back = (m_back + 1) % m_max_size;
        m_array[m_back] = item;
        ++m_size;

        m_mutex.unlock();
        return true;

    }

    //类似回调函数获取元素值
    bool pop(T &item){
        m_mutex.lock();
        //m_cond.wait一直堵塞自己不返回,当pthread_cond_signal()或者是pthread_cond_broadcast()函数唤醒
        //线程竞争到锁是返回1,因为不可能永远能竞争到锁,所以当被唤醒的时候会返回非0的值
        while(m_size <= 0){
            if(!m_cond.wait(m_mutex.get())){
                m_mutex.unlock();
                return false;
            }
        }
        --m_size;
        m_front = (m_front + 1) % m_max_size;
        item = m_array[m_front];
        m_mutex.unlock();
        return true;

    }

private:
    locker m_mutex;     //locker类实例
    cond m_cond;

    T* m_array;
    int m_size;
    int m_max_size;
    int m_front;
    int m_back;
};


#endif //MYWEBSERVER_BLOCK_QUEUE_H
