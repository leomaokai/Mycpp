# I/O复用--epoll

epoll把用户关心的文件描述符上的事件放在内核里的一个事件表中,从而无须像select和poll那样每次调用都要重复传入文件描述符集或事件集.减少了用户空间到内核空间的开销.

epoll需要使用一个额外的文件描述符来唯一标识内核中的事件表

```cpp
#include<sys/epoll.h>
int epoll_create(int size);//创建额外的文件描述符
int epoll_ctl(int epfd,int op,int fd,struct epoll_event *event);
//操作epoll内核事件表
int epoll_wait(int epfd,struct epoll_event *events,int maxevents,int timeout);
//等待一组文件描述符上的事件,并将检测到的事件复制到events数组中,返回检测到的个数
/*
通过epoll_wait的返回个数,我们可以直接知道就绪的文件描述符的个数,不需要每次遍历判断是否就绪,而select和poll仅仅是告诉我们有事件就绪.
*/
```

```cpp
struct epoll_event
{
    _uint32_t events;//需要检测的事件
    epoll_data_t data;//用户数据
}
typedef union epoll_data
{
    void *ptr;
    int fd;
    uint32_t u32;
    uint64_t u64;
}epoll_data_t;
```



```cpp
//code12.cpp code_tree
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/epoll.h>
#define MAX_USER 2
#define BUFFER_SIZE 64

struct epoll_event events[MAX_USER];//epoll_wait需要的事件数组
int epfd = epoll_create(10);//额外的文件描述符epfd,标识内核中的事件表
int main()
{
    //第一步:创建服务端的socket
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(socket >= 0);

    //第二步:绑定地址与端口号
    int ret = 0;
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(54321);
    ret = bind(sockfd, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);

    //第三步:把socket设置为监听模式
    ret = listen(sockfd, 5);
    assert(ret != -1);

    //第四步:接受客户端连接
    for (int i = 0; i < MAX_USER; ++i)
    {
        struct epoll_event ev;//需要epoll_ctl操作的事件
        struct sockaddr_in client;
        socklen_t clientlen = sizeof(client);
        ev.data.fd = accept(sockfd, (struct sockaddr *)&client, &clientlen);
        printf("connected with ip %s\n", inet_ntoa(client.sin_addr));
        ev.events = EPOLLIN;
        epoll_ctl(epfd, EPOLL_CTL_ADD, ev.data.fd, &ev);//将ev事件添加到内核事件表
    }

    //第五步:与客户端通信
    char buffer[BUFFER_SIZE];
    while (1)
    {
        ret = epoll_wait(epfd, events, 2, -1);
        //检测到事件后,将所有就绪的事件从内核事件表中复制到events指向的数组中
        for (int i = 0; i < ret; ++i)
        {
            memset(buffer, 0, BUFFER_SIZE);
            int recvret = recv(events[i].data.fd, buffer, BUFFER_SIZE, 0);
            if (recvret > 0)
                printf("recv the data from the user \n");
            else
                continue;
        }
    }
}
```

