//code06.cpp code_tree
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<poll.h>
#include<fcntl.h>
#include<errno.h>
#include<signal.h>
#define USER_LIMIT 5
#define BUFFER_SIZE 64
#define FD_LIMIT 65535

struct client
{
    char* write_buf;
    char buf[BUFFER_SIZE];
};

class Server
{
    public:
    int m_listenfd;
    int m_connfd;
    Server();
    bool initserver();
    bool acceptserver();
    ~Server();
};
Server* server=new Server;
client* users=new client[65535];
void sig_handler(int sig);
int setnonblocking(Server* server);
int main()
{
	signal(SIGINT,sig_handler);
    if(server->initserver()==false) return 1;
    pollfd fds[USER_LIMIT+1];
    fds[0].fd=server->m_listenfd;
    fds[0].events=POLLIN;
    fds[0].revents=0;
    for(int i=1;i<=USER_LIMIT;++i)
    {
        fds[i].fd=-1;
        fds[i].events=0;
    }
    int counter=0;
    while (1)
    {
        int ret=poll(fds,counter+1,-1);
        if(ret<0)
        {
            printf("poll failure\n");
            break;
        }

        for(int i=0;i<counter+1;++i)
        {
            if((fds[i].fd==server->m_listenfd)&&(fds[i].revents & POLLIN))
            {
                if(server->acceptserver()==false) continue;
		        int connfd=server->m_connfd;		
                if(counter>=USER_LIMIT)
                {
                    close(connfd);
                    continue;
                }
                counter++;
                setnonblocking(server);
                fds[counter].fd=connfd;
                fds[counter].events=POLLIN | POLLRDHUP;
                fds[counter].revents=0;
                printf("comes a new user,now have %d users\n",counter);
            }
		else if(fds[i].revents & POLLRDHUP)
		{
			users[fds[i].fd]=users[fds[counter].fd];
			close(fds[i].fd);
			fds[i]=fds[counter];
			i--;
			counter--;
			printf("a user left,now have %d users\n",counter);
		}
		else if(fds[i].revents & POLLIN)
		{
			int connfd=fds[i].fd;
			memset(users[connfd].buf,0,BUFFER_SIZE);
			int ret=recv(connfd,users[connfd].buf,BUFFER_SIZE-1,0);
			printf("get %d bytes of client data %s from %d\n",ret,users[connfd].buf,connfd);
			if(ret<0)
			{
				if(errno!=EAGAIN)
				{
					users[fds[i].fd]=users[fds[counter].fd];
					close(fds[i].fd);
					fds[i]=fds[counter];
					i--;
					counter--;
					printf("a user left,now have %d users\n",counter);			
       				 }
			}	
			else if(ret==0)
			{}
			else		
			{
				for(int j=1;j<=counter;j++)
				{
					if(fds[j].fd==connfd)
						continue;
					fds[j].events|=~POLLIN;
					fds[j].events|=POLLOUT;
					users[fds[j].fd].write_buf=users[connfd].buf;
				}
			}		
		}
		else if(fds[i].revents & POLLOUT)
		{
			int connfd=fds[i].fd;
			if(!users[connfd].write_buf)
				continue;
			int ret=send(connfd,users[connfd].write_buf,strlen(users[connfd].write_buf),0);
			users[connfd].write_buf=NULL;
			fds[i].events|=~POLLOUT;
			fds[i].events|=POLLIN;		
		}
	}
    }
	delete server;
    delete [] users;
    return 0;
}
Server::Server():m_connfd(0),m_listenfd(0)
{
    printf("构造函数调用\n");
}
Server::~Server()
{
    printf("析构函数调用\n");
    if(m_listenfd!=0) close(m_listenfd);
	if(m_connfd!=0) close(m_connfd);
}
bool Server::initserver()
{
    m_listenfd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in servaddr; 
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(54321);
    
    if(bind(m_listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr))!=0)
    { 
        close(m_listenfd);
        m_listenfd=0; 
        return false; 
    }
    
    if (listen(m_listenfd,5) != 0 )
    { 
        close(m_listenfd);
     	m_listenfd=0;
    	return false; 
    }
    
    return true;
}
bool Server::acceptserver()
{
    if((m_connfd=accept(m_listenfd,0,0))<=0)
        return false;
    return true;
}

int setnonblocking(Server* server)
{  
	int fd=server->m_connfd; 
    int old_option=fcntl(fd,F_GETFL);
    int new_option=old_option | O_NONBLOCK;
    fcntl(fd,F_SETFL,new_option);
    return old_option;
}
void sig_handler(int sig)
{
	delete  server;
	delete [] users;   
	printf("收到信号%d,服务器退出\n",sig);
	exit(0);
}
