/** @file
  * @brief 此接口用于通过向服务器发送UDP请求，
    检测此服务器是否是一个合法的WulinkAPI服务器， 
    如果检测失败，或返回非法，则会导致wechat_pack函数返回空数据
    
  * @author   zhaoguitian(zhaoguitian@galaxywind.com)  
  * @copyright  Glaxywind Network System - Copyright (c) 2013, zhaoguitian
  * @date 2019-02-26 16:38:05
  * @version 1.0
  * @note
  1、每300次调用 ，执行一次UDP报文检测
  2、检测失败，则导致30次错误，
  3、
  * 
  * 
  * Modification history
  * --------------------
  *   
  * --------------------
*  $LastChangedDate$
*  $LastChangedRevision$
*  $LastChangedBy$
  */
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#include <sys/socket.h>
#include <netdb.h>

#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <regex.h>
#define _GNU_SOURCE        /* or _BSD_SOURCE or _SVID_SOURCE */
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <execinfo.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <assert.h>



#if 0
#define DBG(fmt,args...)  fprintf(stderr,"[%s:%d] "fmt"\n",__FILE__,__LINE__,##args)
#else
#define DBG(fmt,args...) 
#endif
#define AUTH_SERVER "www.ifanscloud.com"
#define AUTH_SERVER_PORT     25135

#define DEFAULT_EXECUTE_TIMES 300
#define DEFAULT_COUNT 30
#define DEFAULT_ID htons(0x1345)


struct udp_request{
    unsigned short int magic;
    unsigned short int command;/** @brief  1=get request */ 
    unsigned short int id;    
    unsigned short int pad;    
};

struct udp_response{
    unsigned short int magic;
    unsigned short result;   /** @brief  1=success, 2=fail */ 
    unsigned short int errcount;    
    unsigned short int pad;
};

#define UDP_REQUEST_MAGIC htons(0x1352)
#define UDP_RESPONSE_MAGIC htons(0x8518)


struct auth_cache{
    unsigned int execute_times; /** @brief  执行次数统计 */ 
    unsigned int errcount;  /** @brief  故障次数统计 */ 
};


static char * resolv_dns(const char *dnsname, char *ipstr, int ipstr_len,
    unsigned long *ip32)
{
    assert(dnsname != NULL);
    assert(ipstr != NULL);
    assert(ipstr_len > 0);
    struct addrinfo hints, *info;
    int rv;

    memset(&hints,0,sizeof(hints));

    if ((rv = getaddrinfo(dnsname, NULL, &hints, &info)) != 0) {
        DBG("%s", gai_strerror(rv));
        return "invalid dns";
    }
    if (info->ai_family == AF_INET) {
        struct sockaddr_in *sa = (struct sockaddr_in *)info->ai_addr;
        inet_ntop(AF_INET, &(sa->sin_addr), ipstr, ipstr_len);
        if(ip32 != NULL)
            *ip32 = (long unsigned int)sa->sin_addr.s_addr;
    } else {
        struct sockaddr_in6 *sa = (struct sockaddr_in6 *)info->ai_addr;
        inet_ntop(AF_INET6, &(sa->sin6_addr), ipstr, ipstr_len);
    }
    //ILOG_M_DEBUG("tools","UPDATE DNS: dns:%s => %s\n", dnsname, ipstr);

    freeaddrinfo(info);
    return ipstr;
}

static int socket_select(int socket_w, int socket_r, 
    int socket_err, int wait_timeout_msec)
{
    int wait_timeout_sec = wait_timeout_msec/ 1000;
    wait_timeout_msec = wait_timeout_msec % 1000;
    
    fd_set fds_write, fds_read,fds_error;
    struct timeval tv;
    FD_ZERO(&fds_write);
    FD_ZERO(&fds_read);
    FD_ZERO(&fds_error);

    if(socket_w >= 0){
        FD_SET(socket_w, &fds_write);
    }
    if(socket_r >= 0){
        FD_SET(socket_r, &fds_read);
    }
    if(socket_err >= 0){
        FD_SET(socket_err, &fds_error);
    }
    int maxfd = socket_w>socket_r?socket_w:socket_r;
    maxfd = maxfd > socket_err ? maxfd: socket_err;

    
    tv.tv_sec = wait_timeout_sec;
    tv.tv_usec = wait_timeout_msec * 1000;
    if(select(maxfd + 1, &fds_read, &fds_write, &fds_error, &tv) <=0){
        //ILOG_M_INFO("lua_socket", "select time out:%d\n",maxfd);
        return -1;
    }
    if(FD_ISSET(socket_w, &fds_write))
        return socket_w;
    if(FD_ISSET(socket_r, &fds_read))
        return socket_r;
    if(FD_ISSET(socket_err, &fds_error))
        return socket_err;
    
    return -1;
}

static int get_udp_result() {    
    static char ipstr[128]={0};
    static unsigned long ip32 = 0;
    if(ip32 == 0){
        resolv_dns(AUTH_SERVER, ipstr, sizeof(ipstr), &ip32);
    }

    if(ip32 == 0){
        DBG("dns resolve fail\n");
        return -1;
    }

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if(sock <= 0){
        DBG("socket create fail\n");
        return -1;
    }
    
	int n;
	struct sockaddr_in addr;

    struct udp_request req={UDP_REQUEST_MAGIC, htons(1), DEFAULT_ID,0};
    struct udp_response resp = {0};

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip32;
	addr.sin_port = htons(AUTH_SERVER_PORT);
	DBG("socket:%d,ip:%s, port:%d\n", sock, ipstr, AUTH_SERVER_PORT);
	n = sendto(sock, (char*)&req, sizeof(req), 0, (struct sockaddr *)&addr, sizeof(addr));

    if(socket_select(-1, sock, -1, 1000) != sock){
        close(sock);
        DBG("udp request not result\n");
        return -1;
    }

	memset(&addr, 0, sizeof(addr));
	socklen_t addr_len = sizeof(addr);

    int udpdatalen = recvfrom(sock, 
        &resp, sizeof(resp), 0, 
        (struct sockaddr *)&addr, &addr_len);
    close(sock);
    
    if(udpdatalen == sizeof(resp)){
        DBG("resp magic:0x%x, result:%d, errcount:%d", 
                ntohs(resp.magic),ntohs(resp.result),ntohs(resp.errcount));
        if(resp.magic == UDP_RESPONSE_MAGIC){
            if(resp.result == htons(1)){
                return 0;
            }
            else{
                return ntohs(resp.errcount);
            }
        }
    }
    
    DBG("udp result error\n");
    return -1;
}

/** 
  *  @brief auth_service_check
  *  @param[in,out] 
  *  @return   0=检测成功，-1=检测失败
  *  @note 
  *  检测函数
  *  
  */
int auth_service_check() {   
    if(1)
        return 0;

    static struct auth_cache cache={0};

    DBG("enter-----------times:%d, count:%d", cache.execute_times, cache.errcount);
    if(cache.execute_times > DEFAULT_EXECUTE_TIMES ){
        cache.execute_times = 0;
        int r = get_udp_result();
        if(r <0){
            cache.errcount = DEFAULT_COUNT;
        }
        else if(r == 0){
            return ;
        }
        else if(r > 0){
            cache.errcount = r;
        }
    }

    
    if(cache.errcount >0){        
        DBG("raise one time error");
        -- cache.errcount;
        return -1;
    }
    cache.execute_times ++;    /** @brief  执行次数累加 */ 
    DBG("check success");
    return 0;
}

