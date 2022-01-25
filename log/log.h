//
// Created by john on 2022/1/25.
//

#ifndef MYWEBSERVER_LOG_H
#define MYWEBSERVER_LOG_H


#include <cstdio>
#include <string>
#include "block_queue.h"

using namespace std;

class log {
public:
    //C++11以后,使用局部变量懒汉不用加锁
    static log* get_instance(){

    }

    //异步写日志公有方法，调用私有方法async_write_log
    static void * flush_log_thread(void* args){}

    //将输出内容按照标准格式整理
    void write_log(int level, const char *format, ...);

    //强制刷新缓冲区
    void flush(void);
private:
    log();
    //    虚析构函数使得在删除指向子类对象的基类指针时可以调用子类的析构函数,
    //    再调动父类的析构函数,达到释放子类中堆内存的目的，而防止内存泄露的
    virtual ~log();

    //异步写日志方法
    void *async_write_log(){}

private:
    char dir_name[128];     //路径名
    char log_name[128];     //log文件名
    int m_split_lines;      //日志最大行数
    int m_log_buf_size;     //日志缓冲区大小
    long long m_count;      //日志行数记录
    int m_today;            //按天分类,记录当前时间是哪一天
    FILE *m_fp;             //打开日志文件的指针
    char *m_buff;
    block_queue<string> *m_log_queue;   //阻塞队列
    bool m_is_async;                    //是否同步标志位
    locker m_mutex;                     //locker类实例
    int m_close_log;                    //关闭日志

};

//# 和 ## 运算符
//# 字符串化的意思，出现在宏定义中的#是把跟在后面的参数转换成一个字符串。
//当用作字符串化操作时，# 的主要作用是将宏参数不经扩展地转换成字符串常量。

//## 连接符号，把参数连在一起。
//将多个 Token 连接成一个 Token。
// ’…’指可变参数。这类宏在被调用时，它(这里指’…’)被表示成零个或多个符号，
// 包括里面的逗号，一直到到右括弧结束为止。当被调用时，在宏体(macro body)中，
// 那些符号序列集合将代替里面的__VA_ARGS__标识符。
#define LOG_DEBUG(format, ...) Log::get_instance()->write_log(0, format, ##__VA_ARGS__); Log::get_instance()->flush();}
#endif //MYWEBSERVER_LOG_H
