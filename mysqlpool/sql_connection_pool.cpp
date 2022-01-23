//
// Created by john on 2022/1/18.
//
#include <mysql/mysql.h>
#include "sql_connection_pool.h"

using namespace std;

sql_connection_pool::sql_connection_pool() {
    m_CurConn = 0;
    m_FreeConn = 0;
}

sql_connection_pool *sql_connection_pool::GetInstance() {
    static sql_connection_pool connPool;
    return &connPool;
}

void sql_connection_pool::init(string &url, string &User, string &PassWord,
                               string &DataBaseName, int port, int MaxConn,
                               int close_log) {
    m_url = url;
    m_User = User;
    m_PassWord = PassWord;
    m_DataBaseName = DataBaseName;
    m_port = port;
    m_MaxConne = MaxConn;
    m_close_log = close_log;
    for(int i = 0; i < MaxConn; ++i){
        MYSQL* con = NULL;
        //MYSQL对象初始化
        // MYSQL *mysql_init(MYSQL *mysql)
        //如果mysql是NULL指针，该函数将分配、初始化、并返回新对象。
        //否则，将初始化对象，并返回对象的地址。
        //如果mysql_init()分配了新的对象，
        // 应当在程序中调用mysql_close() 来关闭连接，以释放对象
        //初始失败返回NULL
        con = mysql_init(con);

        if(con == NULL){
            //LOG_ERROR("MySQL Error");
            //exit（0）：正常运行程序并退出程序；
            //exit（1）：非正常运行导致退出程序;
            //return是函数的退出(返回)；exit是进程的退出
            exit(1);
        }

        //MYSQL *mysql_real_connect(MYSQL *mysql, const char *host,
        // const char *user, const char *passwd, const char *db, unsigned int port,
        // const char *unix_socket, unsigned long client_flag)
        //连接数据库引擎，通过函数mysql_real_connect()
        // 尝试与运行在主机上的MySQL数据库引擎建立连接。
        //如果连接成功，返回MYSQL*连接句柄。
        //如果连接失败，返回NULL。对于成功的连接，返回值与第1个参数的值相同。
        //c_str()它返回当前字符串的首字符地址

/*mysql：前面定义的MYSQL变量；
host：MYSQL服务器的地址；
  如果“host”是NULL或字符串"localhost"，连接将被视为与本地主机的连接。
  如果操作系统支持套接字（Unix）或命名管道（Windows），将使用它们而不是TCP/IP连接到服务器。
user：登录用户名；
  如果“user”是NULL或空字符串""，用户将被视为当前用户。在UNIX环境下，它是当前的登录名。
passwd：登录密码；
db：要连接的数据库，如果db为NULL，连接会将默认的数据库设为该值。
port：MYSQL服务器的TCP服务端口；
  如果“port”不是0，其值将用作TCP/IP连接的端口号。注意，“host”参数决定了连接的类型。
unix_socket：unix连接方式。
  如果unix_socket不是NULL，该字符串描述了应使用的套接字或命名管道。注意，“host”参数决定了连接的类型。
clientflag：Mysql运行为ODBC数据库的标记，一般取0。*/

        con = mysql_real_connect(con, url.c_str(), User.c_str(), PassWord.c_str(), DataBaseName.c_str(),
                                 port, NULL, 0);
        if(con == NULL){
            //LOG_ERROR("MySQL Error");
            exit(1);
        }
        ConnList.push_back(con);
        ++m_FreeConn;
    }

    //有参构造改变信号量的值
    reserve = sem(m_FreeConn);
    m_MaxConne = m_FreeConn;

}

//当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL *sql_connection_pool::GetConnection() {
    MYSQL* con = NULL;
    reserve.wait();
    lock.lock();

    con = ConnList.front();
    ConnList.pop_front();
    --m_FreeConn;
    ++m_CurConn;

    lock.unlock();
    return con;

}

//释放当前使用的连接
bool sql_connection_pool::ReleaseConnection(MYSQL *conn) {
    if(conn == nullptr){
        return false;
    }

    lock.lock();
    ConnList.push_back(conn);
    ++m_FreeConn;
    --m_CurConn;
    lock.unlock();

    reserve.post();
    return true;
}

int sql_connection_pool::GetFreeConn() {
    return m_FreeConn;
}

void sql_connection_pool::DestroyPool() {
    lock.lock();
    if(!ConnList.empty()){
        list<MYSQL*>::iterator it;
        for(it = ConnList.begin(); it != ConnList.end(); ++it){
            MYSQL* con = *it;
            mysql_close(con);
        }
        ConnList.clear();
        m_FreeConn = 0;
        m_CurConn = 0;
    }
}

sql_connection_pool::~sql_connection_pool() {
        DestroyPool();
}

connectionRAII::connectionRAII(MYSQL **SQL, sql_connection_pool *connPool){
    *SQL = connPool->GetConnection();
    conRAII = *SQL;
    poolRAII = connPool;
}

connectionRAII::~connectionRAII() {
    poolRAII->ReleaseConnection(conRAII);
}