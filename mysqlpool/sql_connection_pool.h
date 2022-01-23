//
// Created by john on 2022/1/18.
//

#ifndef MYWEBSERVER_SQL_CONNECTION_POOL_H
#define MYWEBSERVER_SQL_CONNECTION_POOL_H
#include <mysql/mysql.h>
#include <string>
#include <list>

#include "../lock/locker.h"

using namespace  std;

class sql_connection_pool {
public:
    //获取数据库连接
    //当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
    MYSQL* GetConnection();
    //释放资源,释放当前使用的连接
    bool ReleaseConnection(MYSQL* conn);
    //获取空闲连接
    int GetFreeConn();
    //销毁所有连接
    void DestroyPool();

    //单例模式获取实例
    static sql_connection_pool* GetInstance();

    void init(string& url, string& User, string& PassWord, string& DataBaseName,int port, int MaxConn, int close_log);

private:
    sql_connection_pool();
    ~sql_connection_pool();

    int m_MaxConne;     //最大连接数
    int m_CurConn;      //当前已使用连接数
    int m_FreeConn;     //当前可用(空闲连接数)
    locker lock;
    list<MYSQL*> ConnList;   //list做连接池
    sem reserve;

public:
    string m_url ;      //主机地址
    string m_User;      //登录数据库用户名
    string m_PassWord;  //登录数据库密码
    string m_DataBaseName;//使用数据库名
    int m_port;         //数据库端口号
    //int m_MaxConn;      //最大连接数(上面已经定义)
    int m_close_log;    //日志开关

};

class connectionRAII{
public:
    connectionRAII(MYSQL** SQL, sql_connection_pool* connPool);
    ~connectionRAII();

private:
    MYSQL *conRAII;
    sql_connection_pool* poolRAII;
};


#endif //MYWEBSERVER_SQL_CONNECTION_POOL_H
