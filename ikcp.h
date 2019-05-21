//=====================================================================
//
// KCP - A Better ARQ Protocol Implementation
// skywind3000 (at) gmail.com, 2010-2011
//  
// Features:
// + Average RTT reduce 30% - 40% vs traditional ARQ like tcp.
// + Maximum RTT reduce three times vs tcp.
// + Lightweight, distributed as a single source file.
//
//=====================================================================
#ifndef __IKCP_H__
#define __IKCP_H__

#include <stddef.h>
#include <stdlib.h>
#include <assert.h>


//=====================================================================
// 32BIT INTEGER DEFINITION 
//=====================================================================
#ifndef __INTEGER_32_BITS__
#define __INTEGER_32_BITS__
#if defined(_WIN64) || defined(WIN64) || defined(__amd64__) || \
	defined(__x86_64) || defined(__x86_64__) || defined(_M_IA64) || \
	defined(_M_AMD64)
	typedef unsigned int ISTDUINT32;
	typedef int ISTDINT32;
#elif defined(_WIN32) || defined(WIN32) || defined(__i386__) || \
	defined(__i386) || defined(_M_X86)
	typedef unsigned long ISTDUINT32;
	typedef long ISTDINT32;
#elif defined(__MACOS__)
	typedef UInt32 ISTDUINT32;
	typedef SInt32 ISTDINT32;
#elif defined(__APPLE__) && defined(__MACH__)
	#include <sys/types.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif defined(__BEOS__)
	#include <sys/inttypes.h>
	typedef u_int32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#elif (defined(_MSC_VER) || defined(__BORLANDC__)) && (!defined(__MSDOS__))
	typedef unsigned __int32 ISTDUINT32;
	typedef __int32 ISTDINT32;
#elif defined(__GNUC__)
	#include <stdint.h>
	typedef uint32_t ISTDUINT32;
	typedef int32_t ISTDINT32;
#else 
	typedef unsigned long ISTDUINT32; 
	typedef long ISTDINT32;
#endif
#endif


//=====================================================================
// Integer Definition
//=====================================================================
#ifndef __IINT8_DEFINED
#define __IINT8_DEFINED
typedef char IINT8;
#endif

#ifndef __IUINT8_DEFINED
#define __IUINT8_DEFINED
typedef unsigned char IUINT8;
#endif

#ifndef __IUINT16_DEFINED
#define __IUINT16_DEFINED
typedef unsigned short IUINT16;
#endif

#ifndef __IINT16_DEFINED
#define __IINT16_DEFINED
typedef short IINT16;
#endif

#ifndef __IINT32_DEFINED
#define __IINT32_DEFINED
typedef ISTDINT32 IINT32;
#endif

#ifndef __IUINT32_DEFINED
#define __IUINT32_DEFINED
typedef ISTDUINT32 IUINT32;
#endif

#ifndef __IINT64_DEFINED
#define __IINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef __int64 IINT64;
#else
typedef long long IINT64;
#endif
#endif

#ifndef __IUINT64_DEFINED
#define __IUINT64_DEFINED
#if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 IUINT64;
#else
typedef unsigned long long IUINT64;
#endif
#endif

#ifndef INLINE
#if defined(__GNUC__)

#if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define INLINE         __inline__ __attribute__((always_inline))
#else
#define INLINE         __inline__
#endif

#elif (defined(_MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__))
#define INLINE __inline
#else
#define INLINE 
#endif
#endif

#if (!defined(__cplusplus)) && (!defined(inline))
#define inline INLINE
#endif


//=====================================================================
// QUEUE DEFINITION                                                  
//=====================================================================
#ifndef __IQUEUE_DEF__
#define __IQUEUE_DEF__

struct IQUEUEHEAD {
	struct IQUEUEHEAD *next, *prev;
};

typedef struct IQUEUEHEAD iqueue_head;


//---------------------------------------------------------------------
// queue init                                                         
//---------------------------------------------------------------------
#define IQUEUE_HEAD_INIT(name) { &(name), &(name) }
#define IQUEUE_HEAD(name) \
	struct IQUEUEHEAD name = IQUEUE_HEAD_INIT(name)

#define IQUEUE_INIT(ptr) ( \
	(ptr)->next = (ptr), (ptr)->prev = (ptr))

#define IOFFSETOF(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define ICONTAINEROF(ptr, type, member) ( \
		(type*)( ((char*)((type*)ptr)) - IOFFSETOF(type, member)) )

#define IQUEUE_ENTRY(ptr, type, member) ICONTAINEROF(ptr, type, member)


//---------------------------------------------------------------------
// queue operation                     
//---------------------------------------------------------------------
#define IQUEUE_ADD(node, head) ( \
	(node)->prev = (head), (node)->next = (head)->next, \
	(head)->next->prev = (node), (head)->next = (node))

#define IQUEUE_ADD_TAIL(node, head) ( \
	(node)->prev = (head)->prev, (node)->next = (head), \
	(head)->prev->next = (node), (head)->prev = (node))

#define IQUEUE_DEL_BETWEEN(p, n) ((n)->prev = (p), (p)->next = (n))

#define IQUEUE_DEL(entry) (\
	(entry)->next->prev = (entry)->prev, \
	(entry)->prev->next = (entry)->next, \
	(entry)->next = 0, (entry)->prev = 0)

#define IQUEUE_DEL_INIT(entry) do { \
	IQUEUE_DEL(entry); IQUEUE_INIT(entry); } while (0)

#define IQUEUE_IS_EMPTY(entry) ((entry) == (entry)->next)

#define iqueue_init		IQUEUE_INIT
#define iqueue_entry	IQUEUE_ENTRY
#define iqueue_add		IQUEUE_ADD
#define iqueue_add_tail	IQUEUE_ADD_TAIL
#define iqueue_del		IQUEUE_DEL
#define iqueue_del_init	IQUEUE_DEL_INIT
#define iqueue_is_empty IQUEUE_IS_EMPTY

#define IQUEUE_FOREACH(iterator, head, TYPE, MEMBER) \
	for ((iterator) = iqueue_entry((head)->next, TYPE, MEMBER); \
		&((iterator)->MEMBER) != (head); \
		(iterator) = iqueue_entry((iterator)->MEMBER.next, TYPE, MEMBER))

#define iqueue_foreach(iterator, head, TYPE, MEMBER) \
	IQUEUE_FOREACH(iterator, head, TYPE, MEMBER)

#define iqueue_foreach_entry(pos, head) \
	for( (pos) = (head)->next; (pos) != (head) ; (pos) = (pos)->next )
	

#define __iqueue_splice(list, head) do {	\
		iqueue_head *first = (list)->next, *last = (list)->prev; \
		iqueue_head *at = (head)->next; \
		(first)->prev = (head), (head)->next = (first);		\
		(last)->next = (at), (at)->prev = (last); }	while (0)

#define iqueue_splice(list, head) do { \
	if (!iqueue_is_empty(list)) __iqueue_splice(list, head); } while (0)

#define iqueue_splice_init(list, head) do {	\
	iqueue_splice(list, head);	iqueue_init(list); } while (0)


#ifdef _MSC_VER
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#pragma warning(disable:4996)
#endif

#endif


//---------------------------------------------------------------------
// WORD ORDER
//---------------------------------------------------------------------
#ifndef IWORDS_BIG_ENDIAN
    #ifdef _BIG_ENDIAN_
        #if _BIG_ENDIAN_
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #if defined(__hppa__) || \
            defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
            (defined(__MIPS__) && defined(__MIPSEB__)) || \
            defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
            defined(__sparc__) || defined(__powerpc__) || \
            defined(__mc68000__) || defined(__s390x__) || defined(__s390__)
            #define IWORDS_BIG_ENDIAN 1
        #endif
    #endif
    #ifndef IWORDS_BIG_ENDIAN
        #define IWORDS_BIG_ENDIAN  0
    #endif
#endif



//=====================================================================
// SEGMENT  报文被拆分后的段
// 详解 https://light0457.github.io/2016/10/18/Network/KCP/KCPSourceCodeRead/
//=====================================================================
struct IKCPSEG
{
	struct IQUEUEHEAD node;    // 多个报文段通过双向链表存储，node记录前一个和后一个Seg
	IUINT32 conv;    // 用于两端匹配连接，相同的conv被认为是同一个连接的数据
	IUINT32 cmd;     // command, 报文类型，一共有四种:IKCP_CMD_PUSH : 数据包,IKCP_CMD_ACK : ACK包,IKCP_CMD_WASK : 询问远端窗口大小,IKCP_CMD_WINS : 告诉远端自己的窗口大小
	IUINT32 frg;     // fragment id, 用于报文拆分与重组，注意该编号并非从0开始，而且倒序。
	IUINT32 wnd;     // 发送端当前可用窗口大小，用于流量控制
	IUINT32 ts;      // 当前Segment发送时间戳，Seg发送的时候的时间
	IUINT32 sn;      // sequence number, segment序号，表示一个连接中发送的所有seg的编号，ack中的对应的也是sn。sn表示的是一个连接中的seg的先后允许。而frg表示的一条消息的顺序，frg用于组装业务消息，而sn用于实现有序及重发。
	IUINT32 una;     // unacknowledged, 表示期望对端发送的最小的Segment编号，也就是说小于该una的所有Segment都已经收到了。这个编号对应的是sn而不是frg.
	IUINT32 len;     // 数据长度，及后面data长度
	IUINT32 resendts; // 重发时间戳，如果当前时间超过了该时间戳，并且对端没有收到该报文，则重发该包。
	IUINT32 rto;      // retransmission timeout, 超时重传时间，超过该时间如果对端没有收到，则重发报文，与上面的resendts区别？该值在发送出去时根据之前的网络情况进行设置？
	IUINT32 fastack;  // 快速ack，用于快速重传，记录该包被ack跳过的次数，当超过配置的值时，该包将被重传
	IUINT32 xmit;     // 该Segment发送次数，每次重发会加一。目前如果该值超过了配置的deadLink值，会返回EAGAIN?
	char data[1];     // 有效数据载荷
};


//---------------------------------------------------------------------
// IKCPCB   KCP协议控制块，用于存储协议收发过程中使用的变量及数据
//---------------------------------------------------------------------
struct IKCPCB
{
	IUINT32 conv, mtu, mss, state;  //mtu:最大传输单元,mss:最大分片大小，state:连接状态,0表示正常
	IUINT32 snd_una, snd_nxt, rcv_nxt;   // rcv_nxt (receive_next)待接收的包编号, snd_nxt:待发送的包编号, snd_una:第一个未确认的包
	IUINT32 ts_recent, ts_lastack, ssthresh;  //ssthresh:拥塞窗口的阈值
	IINT32 rx_rttval, rx_srtt, rx_rto, rx_minrto; //rx_rttval：ack接收rtt浮动值，rx_srtt：ack接收rtt平滑值(smoothed)，rx_rto：由ack接收延迟计算出来的重发时间，rx_minrto：最小重发时间
	IUINT32 snd_wnd, rcv_wnd, rmt_wnd, cwnd, probe; //snd_wnd本地发送窗口，rcv_wnd 本地接收窗口，rmt_wnd远端接收窗口， cwnd:拥塞窗口大小，根据流控计算得到的窗口，probe表示当前是否需要探测窗口
	IUINT32 current, interval, ts_flush, xmit;  //currunt：当前的时间戳，interval：内部flush刷新间隔，ts_flush：下次flush刷新时间戳，xmit当前连接已经重传包次数
	IUINT32 nrcv_buf, nsnd_buf;    // nrcv_buf尚未接收的缓冲区buffer seg数量，nsnd_buf:尚未发送的缓冲区seg数量。当数据从snd_queue通过flush调用转移到snd_buf时，nsnd_buf++，nsnd_que--
	IUINT32 nrcv_que, nsnd_que;    //not received queue count, not send queue count, 本地缓存的未被应用层接收的seg数量，本地缓存的没有发送到缓冲区的seg数量
	IUINT32 nodelay, updated; //nodelay：是否启动无延迟模式，update：是否调用过update函数的标识(kcp需要上层通过不断的ikcp_update和ikcp_check来驱动kcp的收发过程)
	IUINT32 ts_probe, probe_wait; //ts_probe：下次探查窗口的时间戳，probe_wait：探查窗口需要等待的时间
	IUINT32 dead_link, incr; //dead_link：最大重传次数，incr：可发送的最大数据量

	struct IQUEUEHEAD snd_queue;   // 发送队列，应用层调用 ikcp_send 后，数据将会进入到 snd_queue 中，而下层函数 ikcp_flush 将会决定将多少数据从 snd_queue 中移到 snd_buf 中，进行发送
	struct IQUEUEHEAD rcv_queue;   // 接收队列
	struct IQUEUEHEAD snd_buf;    // 发送缓冲区
	struct IQUEUEHEAD rcv_buf;    // 接收缓冲区

	IUINT32 *acklist;  // 当收到一个数据报文时，将其对应的 ACK 报文的 sn 号以及时间戳 ts 同时加入到acklist 中，即形成如 [sn1, ts1, sn2, ts2 …] 的列表；
	IUINT32 ackcount;  // 记录 acklist 中存放的 ACK 报文的数量；
	IUINT32 ackblock;  // acklist 数组的可用长度，当 acklist 的容量不足时，需要进行扩容；

	void *user; // 语法:无类型指针，可以指向任意数据类型。 字段作用:
	char *buffer;    // 作用？？？
	int fastresend;   //快速重传阈值，大于0表示开启快速重传，值表示被跳过的次数，比如2表示某个包被跳过2次就立即重发
	int nocwnd, stream; // 大于0表示开启快速重传，值表示被跳过的次数，比如2表示某个包被跳过2次就立即重发
	int logmask;  // 日志相关

	int (*output)(const char *buf, int len, struct IKCPCB *kcp, void *user);
	void (*writelog)(const char *log, struct IKCPCB *kcp, void *user);
};


typedef struct IKCPCB ikcpcb;

#define IKCP_LOG_OUTPUT			1
#define IKCP_LOG_INPUT			2
#define IKCP_LOG_SEND			4
#define IKCP_LOG_RECV			8
#define IKCP_LOG_IN_DATA		16
#define IKCP_LOG_IN_ACK			32
#define IKCP_LOG_IN_PROBE		64
#define IKCP_LOG_IN_WINS		128
#define IKCP_LOG_OUT_DATA		256
#define IKCP_LOG_OUT_ACK		512
#define IKCP_LOG_OUT_PROBE		1024
#define IKCP_LOG_OUT_WINS		2048

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------------------------
// interface
//---------------------------------------------------------------------

// create a new kcp control object, 'conv' must equal in two endpoint
// from the same connection. 'user' will be passed to the output callback
// output callback can be setup like this: 'kcp->output = my_udp_output'
ikcpcb* ikcp_create(IUINT32 conv, void *user);

// release kcp control object
void ikcp_release(ikcpcb *kcp);

// set output callback, which will be invoked by kcp
void ikcp_setoutput(ikcpcb *kcp, int (*output)(const char *buf, int len, 
	ikcpcb *kcp, void *user));

// user/upper level recv: returns size, returns below zero for EAGAIN
int ikcp_recv(ikcpcb *kcp, char *buffer, int len);

// user/upper level send, returns below zero for error
int ikcp_send(ikcpcb *kcp, const char *buffer, int len);

// update state (call it repeatedly, every 10ms-100ms), or you can ask 
// ikcp_check when to call it again (without ikcp_input/_send calling).
// 'current' - current timestamp in millisec. 
void ikcp_update(ikcpcb *kcp, IUINT32 current);

// Determine when should you invoke ikcp_update:
// returns when you should invoke ikcp_update in millisec, if there 
// is no ikcp_input/_send calling. you can call ikcp_update in that
// time, instead of call update repeatly.
// Important to reduce unnacessary ikcp_update invoking. use it to 
// schedule ikcp_update (eg. implementing an epoll-like mechanism, 
// or optimize ikcp_update when handling massive kcp connections)
IUINT32 ikcp_check(const ikcpcb *kcp, IUINT32 current);

// when you received a low level packet (eg. UDP packet), call it
int ikcp_input(ikcpcb *kcp, const char *data, long size);

// flush pending data
void ikcp_flush(ikcpcb *kcp);

// check the size of next message in the recv queue
int ikcp_peeksize(const ikcpcb *kcp);

// change MTU size, default is 1400
int ikcp_setmtu(ikcpcb *kcp, int mtu);

// set maximum window size: sndwnd=32, rcvwnd=32 by default
int ikcp_wndsize(ikcpcb *kcp, int sndwnd, int rcvwnd);

// get how many packet is waiting to be sent
int ikcp_waitsnd(const ikcpcb *kcp);

// fastest: ikcp_nodelay(kcp, 1, 20, 2, 1)
// nodelay: 0:disable(default), 1:enable
// interval: internal update timer interval in millisec, default is 100ms 
// resend: 0:disable fast resend(default), 1:enable fast resend
// nc: 0:normal congestion control(default), 1:disable congestion control
int ikcp_nodelay(ikcpcb *kcp, int nodelay, int interval, int resend, int nc);


void ikcp_log(ikcpcb *kcp, int mask, const char *fmt, ...);

// setup allocator
void ikcp_allocator(void* (*new_malloc)(size_t), void (*new_free)(void*));

// read conv
IUINT32 ikcp_getconv(const void *ptr);


#ifdef __cplusplus
}
#endif

#endif


