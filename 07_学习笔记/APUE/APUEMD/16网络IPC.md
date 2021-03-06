# 套接字描述符
套接字是通信端点的抽象，正如使用文件描述符访问文件，应用程序用套接字描述符访问套接字。套接字描述符在UNIX系统中被当作是一种文件描述符。事实上，许多处理文件描述符的函数可以用于处理套接字描述符。
为创建一个套接字，调用socket函数。
```c
#include<sys/socket.h>
int socket(int domain,int type,int protocol);
```
返回值：若成功，返回套接字描述符，若出错，返回-1
参数domain（域）确定通信特性，包括地址格式。各个域都有自己表示地址的格式，而表示各个域的常数都是以AF_开头，意指地址族。
* AF_INET		IPv4因特网域
* AF_INET6	IPv6因特网域
* AF_UNIX		UNIX域
* AF_UNSPEC	未指定
参数type确定套接字的类型，进一步确定通信特征。
* SOCK_DGRAM		固定长度的、无连接的、不可靠的报文传递
* SOCK_RAW			IP协议的数据报接口
* SOCK_SEQPACKET	固定长度的、有序的、可靠的、面向连接的报文传递
* SOCK_STREAM		有序的、可靠的、双向的、面向连接的字节流
参数protocol通常是0，表示为给定的域和套接字类型选择默认协议。当对同一域和套接字类型支持多个协议时，可以使用protocol选择一个特定协议。
* IPPROTO_IP			IPv4国际协议
* IPPROTO_IPV6		IPv6国际协议
* IPPROTO_ICMP		因特网控制报文协议
* IPPROTO_RAW		原始IP数据包协议
* IPPROTO_TCP		传输控制协议
* IPPROTO_UDP		用户数据报协议
对于数据报接口，两个对等进程之间通信时不需要逻辑连接。只需要向对等进程所使用的套接字送出一个报文。因此数据报提供了一个无连接的服务。另一方面，字节流要求在交换数据报之前，在本地套接字和通信的对等进程的套接字之间建立一个逻辑连接。
数据报是自包含报文。发送数据报近似于给某人邮寄信件，相反，使用面向连接的协议通信就像与对方通电话。
SOCK_STREAM套接字提供字节流服务，所以应用程序分辨不出报文的界限。这意味着从SOCK_STREAM套接字读数据时，他也许不会返回所有由发送进程所写的字节数。最终可以获得发送过来的所有数据，但也许要通过若干次函数调用才能得到。
SOCK_SEQPACKET套接字和SOCK_STREAM套接字很类似，只是从该套接字得到的是基于报文的服务而不是字节流服务。这意味着从SOCK_SEQPACKET套接字接收的数据量与对方所发送的一致。流控制传输协议提供了因特网域上的顺序数据包服务。
SOCK_RAW套接字提供一个数据报接口，用于直接访问下面的网络层。使用这个接口时，应用程序负责构造自己的协议头部，这是因为传输协议被绕过了。当创建一个原始套接字时，需要有超级用户特权，这样可以防止恶意应用程序绕过内建安全机制来创建报文。
调用socket与调用open相类似。在两种情况下，均可获得用于I/O的文件描述符。当不在需要该文件描述符时，调用close来关闭文件或套接字的访问，并且释放该描述符以便重新使用。
套接字通信是双向的，可以采用shutdown函数来禁止一个套接字I/O。
# 寻址
如何标识一个目标通信进程？进程标识由两部分组成，一部分是计算机网络地址，他可以帮助标识网络上我们想与之通信的计算机；另一部分是该计算机上用端口号表示的服务，他可以帮助标识特定的进程。
## 字节序
在同一台计算机上的进程进行通信时，一般不用考虑字节序。字节序是一个处理器架构特性，用于指示像整数这样的大数据类型内部的字节如何排序。
网络协议指定了字节序，因此异构计算机系统能够交换协议信息而不会被字节序所混淆。TCP/IP协议栈使用大端字节序。应用程序交换格式数据时，字节序问题就会出现。对于TCP/IP，地址用网络字节序来表示，所以应用程序有时需要在处理器的字节序与网络字节序之间转换它们。
## 地址格式
一个地址标识一个特定通信域的套接字端点，地址格式与这个特定的通信域相关。为使不同格式的地址能够传入到套接字函数，地址会被强制转换成一个通用的地址结构sockaddr。
```c
struct sockaddr 
{
	as_family_t		sa_family;
	char			sa_data[];
	...
}
```
套接字现实可以自由地添加额外的成员并且定义sa_data成员的大小。
## 地址查询
如果一个程序简单地传递一个类似于sockaddr结构的套接字地址，并且不依赖于任何协议相关的特性，那么可以与提供相同类型服务的许多不同协议协作。
通过调用gethostent，可以找到给定计算机系统的主机信息。
## 将套接字与地址关联
使用bind函数来关联地址和套接字。
```c
#include<sys/socket.h>
int bind( int sockfd , const struct sockaddr *addr , socklen_t len );
```
对于使用的地址有以下一些限制：
* 在进程正在运行的计算机上，指定的地址必须是有效；不能指定一个其他机器的地址
* 地址必须和创建套接字时的地址族所支持的格式相匹配
* 地址中的端口号必须不小于1024，除非该进程具有相应的特权
* 一般只能将一个套接字端点绑定到一个给定的地址上，尽管有些协议允许多重绑定
可以调用getsockname函数来发现绑定到套接字上的地址。
如果套接字已和对方连接，可以调用getpeername函数来找到对方的地址。
# 建立连接
如果要处理一个面向连接的网络服务，那么在开始交换数据以前，需要在请求服务的进程套接字（客户端）和提供服务的进程套接字（服务器）之间建立一个连接。
使用connect函数来建立连接。
```c
#include<sys/socket.h>
int connect(int sockfd, const struct sockaddr *addr, socklen_t len);
```
在connect中指定的地址是我们想与之通信的服务器地址，如果sockfd没有绑定到一个地址，connect会给调用者绑定一个默认地址。
当尝试连接服务器时，出于一些原因，连接可能会失败。想要一个连接请求成功，要连接的计算机必须是开启的，并且正在运行，服务器必须绑定到一个想与之连接的地址上，并且服务器的等待连接队列要有足够的空间。因此，应用程序必须能够处理connect返回的错误，这些错误可能是由一些瞬间条件引起的。
服务器调用listen函数来宣告它愿意接受连接请求。
一旦服务器调用了listen，所有的套接字就能接收连接请求，使用accept函数获得连接请求并建立连接。
# 数据传输
既然一个套接字端点表示为一个文件描述符，那么只要建立连接，就可以使用read和write来通过套接字通信。
在套接字描述符上使用read和write是非常有意义的，因为这意味着可以将套接字描述符传递给那些原先为处理本地文件而设计的函数。而且还可以安排将套接字描述符传递给子进程，而该子进程执行的程序并不了解套接字。
通过read和write仅能做到交换数据。
函数send可以指定标志来改变传输数据的方式，类似于write。
函数sendto和send类似，但可以在无连接的套接字上指定一个目标地址。
函数sendmsg可调用msghdr结构指定多重缓冲区传输数据，类似于writev。
函数recv和read类似，但是recv可以指定标志来控制如何接收数据。
函数recvfrom可得到数据发送者的源地址。
函数recvmsg可将接收到的数据送入多个缓冲区，类似于readv。
# 套接字选项
套接字机制提供了两个套接字选项接口来控制套接字行为。一个接口用来设置选项，另一个接口可以用来查询选项的状态。
* 通用选项，工作在所有套接字类型上
* 在套接字层次管理的选项，但是依赖于下层协议的支持
* 特定于某协议的选项，每个协议独有的
可以用setsockopt函数来设置套接字选项。
可以用getsockopt函数来查看选项的当前值。
# 带外数据
带外数据是一些通信协议所支持的可选功能，于普通数据相比，他允许更高优先级的数据传输。带外数据先行传输，即使传输队列已经有数据。TCP支持带外数据，UDP不支持，套接字接口对带外数据的支持很大程度上受TCP带外数据具体实现的影响。
TCP将带外数据称为紧急数据。TCP仅支持一个字节的紧急数据，但是允许紧急数据在普通数据传递机制数据流之外传输。为了产生紧急数据，可以在3个send函数中的任何一个里指定MSG_OOB标志。如果带MSG_OOB标志发送的字节数超过一个时，最后一个字节将被视为紧急数据字节。
TCP支持紧急标志的概念，即在普通数据流中紧急数据所在的位置。如果采用套接字选项SO_OOBINLINE，那么可以在普通数据中接收紧急数据。为帮助判断是否已经到达紧急标志，可以使用函数sockatmark。
# 非阻塞和异步I/O
通常，recv函数没有数据可用时会阻塞等待，同样的，当套接字输出队列没有足够空间来发送消息时，send函数会阻塞。
在套接字非阻塞模式下，行为会改变。在这种情况下，这些函数不会阻塞而是会失败，将errno设置为EWOULDBLOCK或者EAGAIN。当这种情况发生时，可以使用poll或select来判断能否接收或者传输数据。
套接字机制有其自己的处理异步I/O的方式。

