#ifndef		__UDP_CTRL_H__
#define	__UDP_CTRL_H__

#include "lib.h"


#define	EMPTY_FUNC \
	log_err(false, "\n\n\nEEEEEEEE please write function %s \n\n\n", __FUNCTION__)

/*****************************************
	UDP控制协议报文头(CMD_UDP_XXX)
 *****************************************/

// 净载荷
#define	MAX_UC_PKT_SIZE	1500
// UDP CTRL 通用报文的TTL数据大小:object + sub object + attri + param_len
#define UDP_CTRL_TL_SIZE 8
// UDP CTRL 通用报文的TTL数据大小:object + sub object + attri
#define UDP_CTRL_TL_SIZE_NO_LEN 6

#pragma pack(push,1)
/*
	udp ctrl packet header
*/
typedef struct{
#if BYTE_ORDER == BIG_ENDIAN
	u_int8_t ver:3,
		hlen:3,
		request:1,
		encrypt:1;
#elif BYTE_ORDER == LITTLE_ENDIAN
	u_int8_t 	encrypt:1,
		request:1,
		hlen:3,
		ver:3;
#else
# error "Please fix <bits/endian.h>"
#endif
	u_int8_t flags;
	u_int8_t request_id;
	u_int8_t resv;
	// session id
	u_int32_t client_sid;
	u_int32_t device_sid;

	// 下面开始可能加密
	u_int16_t command;
	u_int16_t param_len;
} ucph_t;


#define	ucph_hdr_len(ucph)	(((ucph_t *)ucph)->hlen<<2)
#define ucph_hdr_size (sizeof(ucph_t))
#define	get_ucp_payload(pkt, type) (type *)BOFP((pkt->data), ucph_hdr_len((pkt->data)))
#define	get_net_ucp_payload(hdr, type) (type *)BOFP(hdr, ucph_hdr_len(hdr))

// 头部总是明文的长度
#define	ucph_hdr_plain_size	12

// udp control action
#define	UCA_NONE		0
#define	UCA_REQUEST		1
#define	UCA_REPLY		2
#define	UCA_QUESTION	3
#define	UCA_ANSWER		4
#define	UCA_RESULT		5
#define	UCA_REDIRECT	6
#define	UCA_RESET		7
#define	UCA_GET			8
#define	UCA_SET			9
#define	UCA_DELETE		10
#define UCA_PUSH		11


/*
	会话ID的分配
*/
#define	UC_SID_INVALID	0
// 0 - 0x10000000 保留给设备端用来分配给局域网直连的手机
#define	UC_SID_DEV_RESV_BEGIN	0x00000001
#define	UC_SID_DEV_RESV_END	0x0FFFFFFF

// 给服务器用来分配给设备
#define	UC_SID_SERV_DEV_BEGIN_1	0x80000000
#define	UC_SID_SERV_DEV_END_1	0x8FFFFFFF

#define	UC_SID_SERV_DEV_BEGIN_2	0x90000000
#define	UC_SID_SERV_DEV_END_2	0x9FFFFFFF


// 给服务器用来分配给广域网连接的手机
#define	UC_SID_SERV_CLIENT_BEGIN	0xA0000000
#define	UC_SID_SERV_CLIENT_END	0xAFFFFFFF


// 未认证成功、半连接状态的死亡时间
#define	DFL_TIME_UCS_HALF_DIE	5
// 删除连接的延时
#define	DFL_TIME_UCS_DEL_DELAY	3

// 客户端在IDLE状态等待多少秒后，进入下一次尝试
#define	DFL_UC_IDLE_TIME	3
//如果为克隆机时，时间延长
#define	CLONE_UC_IDLE_TIME	60


#define	UCDS_CHECK_PLEN(s, hdr, min_len) \
	do { \
		if (hdr->param_len < min_len) { \
			log_err(false, "%s recv bad pkt: cmd=%u, param_len=%u, min_len=%u\n", s->name, hdr->command, hdr->param_len, min_len); \
			return RS_ERROR; \
		} \
	} while (0)

#define	UCDS_CHECK_PLEN_VOID(s, hdr, min_len) \
	do { \
		if (hdr->param_len < min_len) { \
			log_err(false, "%s recv bad pkt: cmd=%u, param_len=%u, min_len=%u\n", s->name, hdr->command, hdr->param_len, min_len); \
			return; \
		} \
	} while (0)

/*****************************************
	状态
 *****************************************/

// udp ctrl server status
enum {
	UCSS_QUESTION	= 0,
	UCSS_ESTABLISH	 = 1,
	UCSS_DIE = 2,
	UCSS_MAX = 3
};

typedef void (* ucs_func_t)();

typedef struct {
	char *name;
	// 进入本状态调用的函数
	ucs_func_t on_into;
	// 处理报文
	ucs_func_t proc_pkt;
} ucs_proc_t;


// udp ctrl client status
enum {
	UCCS_IDLE = 0,
	UCCS_AUTH_REQ = 1,
	UCCS_AUTH_ANSWER = 2,
	UCCS_ESTABLISH = 3,
	UCCS_ERROR = 4,
	UCCS_MAX = 5
};

typedef void (* ucc_func_t)();

typedef struct {
	char *name;
	// 进入本状态调用的函数
	ucc_func_t on_into;
	// 离开本状态调用的函数
	ucc_func_t on_out;
	// 处理报文
	ucc_func_t proc_pkt;
} ucc_proc_t;


/*
	加密数据的方法，返回加密后的长度
*/
typedef int (*ucs_enc_func_t)(void *session, u_int8_t *data, int len);
/*
	解密数据的方法，返回解密后的长度
*/
typedef int (*ucs_dec_func_t)(void *session, u_int8_t *data, int len);


/*****************************************
	认证报文格式 (CMD_UDP_AUTH)
 *****************************************/


#define MAX_UUID_BIN_LEN 16

// type define for TLV
#define	UCT_VENDOR			1
#define	UCT_LOCAL_TIME		2
#define	UCT_TIME_PARAM		3
#define	UCT_E_TM_PRICE		4
#define UCT_GATHER_SOUND    5
#define UCT_CURRENT_DETECT   6
#define UCT_CHIP_ATTISTR_SEV 	7 //透传模式下，获取设备属性字符串，通告给服务器
#define UCT_DEV_INFO			8
#define UCT_CHIP_TMP		 	9
#define UCT_IA_DELTA        10
#define UCT_IA_CONTROL_BOARD_TYPE	12	// 当前面板类型
#define UCT_CHIP_ATTISTR_PHONE 	6 //透传模式下，获取设备属性字符串，通告给手机，理想是统一用7，为了兼容手机端代码，这里使用6
#define UCT_PRODUCT_TYPE      11 //透传模式下,设备类别。例如风扇类，暖风机类
#define UCT_SMARTCFG_TIME	  14  //一键配置成功的时间
#define UCT_CLONE_PKT_SEND_ENABLE 15 //服务器下发，是否上报克隆报文
#define UCT_UCDS_TIMEZONE_PUT		16//回个时区给客户端
#define UCT_DEVFUN_BITS		17//设备端功能bit位
#define UCT_PHONE_INDEX 18//手机注册index
#define UCT_PSK		19//通过result给客户端返回的psk
#define UCT_IA_IS_AP_MODE  20//值为1，设备出处于邋AP模式，值为0，设备处于sta模式.value长度为1

//UCT_DEVFUN_BITS	17 bit位差异定义
#define DEVFUN_BITS_SHARE (0) //设备功能bit位之 分享用户登录权限


#define	tlv_next(tlv) (uc_tlv_t *)BOFP((uc_tlv_t *)tlv + 1, tlv->len)
#define	tlv_n_next(tlv) (uc_tlv_t *)BOFP((uc_tlv_t *)tlv + 1, ntohs(tlv->len))
#define	tlv_val(tlv) ((u_int8_t *)((uc_tlv_t *)tlv + 1))

typedef struct {
	// UCT_XX
	u_int16_t type;
	// 数据长度，不包括头部
	u_int16_t len;
} uc_tlv_t;

/******************debug info********************************/
#define DBG_TLV_HD_LEN		sizeof(uc_tlv_t)
//SVN号
#define DBG_TYPE_SVN		1
//电流
#define DBG_TYPE_CUR		2
//电流对应AD值
#define DBG_TYPE_CUR_AD		3
//电流k值
#define DBG_TYPE_CUR_K		4
//电流b值
#define DBG_TYPE_CUR_B		5
//电压
#define DBG_TYPE_VOL		6
//电压对应AD值
#define DBG_TYPE_VOL_AD		7
//电压k值
#define DBG_TYPE_VOl_K		8
//电压b值
#define DBG_TYPE_VOL_B		9
//光敏AD值
#define DBG_TYPE_LIGHT_AD	10
//连接服务器域名
#define DBG_TYPE_SERVER_DONAME	11
//连接服务器时间
#define DBG_TYPE_SERVER_CONNTIME 12
//当前局域网手机接入数量
#define DBG_TYPE_CLIENTS		13
//设备uptime
#define DBG_TYPE_UPTIME			14
//光感学习
#define DBG_TYPE_LIGHT_STUDY		15
//cpu使用百分比
#define DGB_TYPE_CPU_PERCENTAGE		16
//mem使用百分比
#define DBG_TYPE_MEM_PERCENTAGE		17

//待机平均电流
#define DBG_TYPE_AVG_AD		18
//待机最大电流
#define DBG_TYPE_MAX_AD		19
//开机电流突变延迟时间
#define DBG_TYPE_PN_TIME   20
//关机电流突变时间
#define DBG_TYPE_PF_TIME   21
//空载电流ad值
#define DBG_TYPE_NO_LOAD_AD 22
//单片机软件版本
#define DBG_TYPE_SMT_SOFT_VER  23
//单片机硬件版本
#define DBG_TYPE_SMT_HARD_VER  24
//红外编码库的id
#define DBG_TYPE_IR_LIB_ID	   25

#define DBG_TYPE_COLD_DELAY_PN_TIME 26
#define DBG_TYPE_COLD_DELAY_PF_TIME 27
#define DBG_TYPE_HOT_DELAY_PN_TIME  28
#define DBG_TYPE_HOT_DELAY_PF_TIME  29

//单片机编译信息
#define DGB_TYPE_STM32_INFO		30

//单片机等待校正
#define DBG_TYPE_STM_WAIT_ADJUST	31
//dev server ip
#define DBG_TYPE_DEVSERVERIP		32
//产测状态查询
#define DBG_TYPE_DEV_STATUS			33



//debug2
//光敏设置
#define DBG_TLV_LIGHT			100
//功率设置
#define DBG_TLV_POWER			101
//温度设置
#define DBG_TLV_TMP				102


/**************************************************/


typedef struct {
	u_int16_t year;
	u_int8_t month;
	u_int8_t day;
	u_int8_t hour;
	u_int8_t minute;
	u_int8_t second;
	u_int8_t timezone;
} uc_time_t;

#if 1
//电价数据结构
enum {
	EP_TYPE_HIGH = 1,
	EP_TYPE_LOW = 2,
	EP_TYPE_LEVEL = 3,
};

//阶梯电价:起止时间-电价
typedef struct {
	u_int8_t begin_hour;
	u_int8_t begin_minute;
	u_int8_t end_hour;
	u_int8_t end_minute;
	u_int8_t tm_type;//峰1 谷2 平3 EP_TYPE_XXX
	u_int8_t pad;
	u_int16_t price;//电价，单位为分，如168表示1元6毛8
} uc_e_tm_t;

typedef struct {
	u_int8_t type;	//民用(缺省):0  工业:1
	u_int8_t count;	//uc_e_tm_t个数
	u_int8_t pad[2];
//	uc_e_tm_t item[0];//uc_e_tm_t
} uc_e_price_t;

#endif

/*
	重传时间、保活时间、死亡时间
*/
typedef struct {
	// 请求报文重传时间间隔，第三次后面的重传时间都与第三次一样了。单位100ms
	u_int8_t retry_100ms[3];
	// 发送保活请求的时间间隔，单位秒
	u_int8_t keeplive;
	// 死亡的时间间隔，单位秒
	u_int16_t die;
} uc_time_param_item_t;

typedef struct {
	// 设备与服务器的
	uc_time_param_item_t dev;
	// 手机与设备局域网直连
	uc_time_param_item_t lan;
	// 手机与设备广域网连接，APP在前端运行
	uc_time_param_item_t wan_front;
	// 手机与设备广域网连接，APP在后端运行
	uc_time_param_item_t wan_background;
} uc_time_param_all_t;


typedef struct {
	// UAA_XXX
	u_int8_t action;
	u_int8_t flag_bits;
	u_int8_t version;
	u_int8_t reserved;
	// 随机数
	u_int8_t rand1[4];
	u_int64_t sn;
	// 客户端唯一标识
	u_int8_t my_uuid[MAX_UUID_BIN_LEN];

	// 当前本地时钟，用于客户端给设备同步时间。不可靠来源
	uc_time_t time;
} uc_auth_request_t;

typedef struct {
	// UAA_XXX
	u_int8_t action;
	u_int8_t reserved[3];

	u_int32_t ip;
	u_int16_t port;
	u_int16_t pad;
} uc_auth_redirect_t;

typedef struct {
	// UAA_XXX
	u_int8_t action;
	u_int8_t reserved[3];
	// 随机数
	u_int8_t rand2[4];
	u_int8_t your_uuid[MAX_UUID_BIN_LEN];
} uc_auth_question_t;

#define	UC_ENC_NONE	0x0000
#define	UC_ENC_AES128	0x0001

typedef struct {
	// UAA_XXX
	u_int8_t action;
	u_int8_t reserved;
	// UC_ENC_xxx
	u_int16_t support_enc;

	u_int8_t answer[16];
	// 当前本地时钟，用于客户端给设备同步时间。可靠来源
	uc_time_t time;
	/*设备需要上报设备属性字符串，引入这个TLV,方便后面扩展*/
	//uc_tlv_t *tlv;
} uc_auth_answer_t;

typedef struct {
	// UAA_XXX
	u_int8_t action;
	u_int8_t reserved1;
	u_int16_t select_enc;

	u_int16_t err_num;
	u_int16_t reserved2;

	// 应答方为设备端时有效，为服务器时忽略
	u_int8_t dev_type;
	u_int8_t ext_type;
	u_int8_t login_type; /* LT_NORMAL, LT_BIND, LT_UNBIND */
	u_int8_t net_type; /* NT_SERVER, NT_DEVICE, NT_DEVICE_OFFLINE */

	// UCT_VENDOR, UCT_LOCAL_TIME, UCT_TIME_PARAM
	//后面紧跟tlv
	//uc_tlv_t *tlv;
} uc_auth_result_t;

/*****************************************
	保活报文格式 (CMD_UDP_KEEPLIVE)
 *****************************************/
typedef struct {
	u_int8_t action;
	u_int8_t count;
	u_int8_t reserved[2];
	//uc_tlv_t *tlv;
}uc_misc_hd_t;

typedef struct {
	u_int8_t action;
	u_int8_t flags;
	u_int16_t reserved;
	uc_tlv_t *tlv;
} uc_keeplive_request_t;

typedef struct {
	u_int8_t action;
	u_int8_t reserved[3];
	//后面紧跟tlv
	//uc_tlv_t *tlv;
} uc_keeplive_reply_t;

typedef struct {
	u_int8_t action;
	u_int8_t reserved[3];
} uc_keeplive_reset_t;


/* 透传相关Transparent transmission */

/* Transparent transmission ability */
#define TT_ABLILITY BIT(0)

/*透传数据使用TLV格式方便扩展*/

#define UCT_TT_RAW_CHIPDATA 1 //发送给控制芯片的原始数据
#define UCT_TT_DEVINFO      2 //设备信息



/*****************************************
	控制报文格式 (CMD_UDP_CTRL)
 *****************************************/


/*****************************************
	UDP Control Object Type
	UDP Control Sub Object Type
	UDP Control Attribute Type
 ****************************************/

/*
	system. (系统)
*/
#define	UCOT_SYSTEM	1

// system.support.
#define	UCSOT_SYS_SUPPORT	1
// system.support.support
#define	UCAT_SUPPORT_SUPPORT	1

// system.support.
#define	UCSOT_SYS_HOSTNAME	2
// system.hostname.hostname
#define	UCAT_HOSTNAME_HOSTNAME	1

// system.software.
#define	UCSOT_SYS_SOFTWARE	3
// system.software.version
#define	UCAT_SYS_VERSION	100 /* 可能被多个子对象用的，从大一点的定义 */
//system.software.upgrade
#define UCAT_SYS_UPGRADE	101	/*升级镜像*/
//system.softwate.stm_upgrade
#define UCAT_SYS_STM_UPGRADE 102 /*stm单片机升级*/
//system.softwate.evm_upgrade
#define UCAT_SYS_EVM_UPGRADE 103 /* 虚拟机镜像包升级*/

//system.softwate.evm_erase
#define UCAT_SYS_EVM_ERASE 104 /* 虚拟机镜像包擦除*/

//system.softwate.evm_info
#define UCAT_SYS_EVM_INFO 105 /* 虚拟机镜像包查询*/

//system.software.reboot
#define UCAT_SYS_REBOOT		1
//system.software.dev_uptime
#define UCAT_SYS_UPTIME		2
//system.software.ip
#define UCAT_SYS_IP			3
//system.software.svn
#define UCAT_SYS_SVN		4
//system.software.ssid
#define UCAT_SYS_SSID		5
//system.software.passwd
#define UCAT_SYS_PASSWD		6
//system.software.devstatus
#define UCAT_SYS_DEVSTATUS	7
//system.software.compile_date
#define UCAT_SYS_COM_DATE	8
//system.software.compile_time
#define UCAT_SYS_COM_TIME	9
//system.software.systime
#define UCAT_SYS_SYSTIME	10
//system.software.setsystime
#define UCAT_SYS_SET_SYSTIME	11
//system.software.debuginfo
#define UCAT_SYS_DEBUGINFO	12
//system.software.reset
#define UCAT_SYS_RESET	    13
//system.software.tlbdebug
#define UCAT_SYS_DEBUG2		14



// system.upgrade.
#define	UCSOT_SYS_UPGRADE	4

// system.hardware.
#define	UCSOT_SYS_HARDWARE	5
// system.hardware.LED
#define	UCATT_HARDWARE_LED	1
// system.hardware.ledcolor
#define	UCATT_HARDWARE_LEDCOLOR	2
// system.hardware.stm_version
#define UCAT_HARDWARE_STM_VERSION 3
// system.hardware.stm_upgrade
#define UCAT_HARDWARE_STM_UPGRADE 4

// system.vendor.
#define	UCSOT_SYS_VENDOR	6
// system.vendor.oem_id
#define	UCAT_VENDOR_OEM_ID	1

// system.server.
#define	UCSOT_SYS_SERVER	7
// system.server.connect_time
#define	UCAT_SERVER_CONNECT_TIME	1
//system.server.domainname
#define UCAT_SERVER_DONAME	2
//system.server.ip
#define UCAT_SERVER_IP		3

// system.user
#define UCSOT_SYS_USER		8 /*一些用户配置信息*/
// system.user.passwd
#define UCAT_USER_PASSWD	1
// system.user.location
#define UCAT_USER_LOCATION	2

//system.licence
#define UCSOT_SYS_LICENCE	9
//system.licence.active
#define UCAT_SYS_LICENCE_ACTIVE 1

//system.share
#define UCSOT_SYS_SHARE 10
//system.share.code
#define UCSOT_SYS_SHARE_CODE 1
//system.share.phone
#define UCSOT_SYS_SHARE_PHONE 2

/*------------------------------UCOT_IA	2---------------------------*/
/*
	IA(智能电器)
*/
// IA.
#define	UCOT_IA	2

// IA.stat.
#define	UCSOT_IA_STAT	1
// IA.stat.total_peak
#define	UCAT_STAT_TOTAL_PEAK	1
// IA.stat.total_valley
#define	UCAT_STAT_TOTAL_VALLEY	2
// IA.stat.period_time
#define	UCAT_STAT_PERIOD_TIME	3
// IA.stat.period_peak
#define	UCAT_STAT_PERIOD_PEAK	4
// IA.stat.period_valley
#define	UCAT_STAT_PERIOD_VALLEY	5
// IA.stat.month_peak
#define	UCAT_STAT_MONTH_PEAK	6
// IA.stat.month_valley
#define	UCAT_STAT_MONTH_VALLEY	7
// IA.stat.cur_power
#define	UCAT_STAT_CUR_POWER	8
// IA.stat.peak_time
#define	UCAT_STAT_PEAK_TIME	9
// IA.stat.peak_price
#define	UCAT_STAT_PEAK_PRICE	10
// IA.stat.valley_price
#define	UCAT_STAT_VALLEY_PRICE	11
// IA.stat.month_flat
#define	UCAT_STAT_MONTH_FLAT	12
// IA.stat.month_flat
#define UCAT_STAT_FLAT_TIME		13
// IA.stat.flast_price
#define	UCAT_STAT_FLAT_PRICE	14
// IA.stat.valley_time
#define	UCAT_STAT_VALLEY_TIME	15
//IA.stat.total_ele
#define UCAT_STAT_TOTAL_ELE		16
//IA.stat.phase_ele
#define UCAT_STAT_PHASE_ELE		17
//IA.stat.on_ele
#define UCAT_STAT_ON_ELE		18

//IA.stat.push2serv 状态上报服务器
#define UCAT_STAT_PUSH2SERV		20

// IA.stat.ele_daybyday 最近一年每天的用电情况
#define UCAT_STAT_ELE_DAYBYDAY 21
// IA.stat.cur_power_milliwatt 当前功率，单位毫瓦
#define	UCAT_STAT_CUR_POWER_MILLIWATT	22

/*----------------------UCOT_IA	2  UCSOT_IA_AC 2----------------------*/
// IA.AIRCONDITION.
#define	UCSOT_IA_AC	2
// IA.AIRCONDITION.WORK
#define	UCAT_AC_WORK	1
// IA.AIRCONDITION.CUR_TEMP
#define	UCAT_AC_CUR_TEMP	2
// IA.AIRCONDITION.TIMER
#define	UCAT_AC_TIMER	3
//IA.AIRCONDITION.SMART_ON
#define UCAT_AC_SMART_ON		4
//IA.AIRCONDITION.SMART_OFF
#define UCAT_AC_SMART_OFF		5
//IA.AIRCONDITION.SMART_SLEEP
#define UCAT_AC_SMART_SLEEP		6
//IA.AIRCONDITION.SOUND_MATCH_REQUEST
#define UCAT_SOUND_MATCH_REQUEST 7
//IA.AIRCONDITION.SOUND_DATA
#define UCAT_SOUND_DATA			 8
//IA.AIRCONDITION.SOUND_MATCH_RESULT
#define UCAT_SOUND_MATCH_RESULT	 9
// IA.AIRCONDITION.ATTRI
#define UCAT_AC_ATTRI		10
// IA.AIRCONDITION.MATCH_INFO
#define UCAT_AC_MATCH_INFO	11
//// IA.AIRCONDITION.AUTO_CTL
 #define UCAT_IA_AUTO_CTRL			12
//IA. AIRCONDITION.ON_USER 用户设置开关机状态(不是发红外，只改设备空调状态)
 #define UCAT_IA_ON_USER_SET		13
// IA.AIRCONDITION.CUR_RH
#define	UCAT_AC_CUR_RH	14
//IA.AIRCONDITION.TMP_SAMPLE_CURVE
#define 	UCAT_AC_TMP_SAMPLE_CURVE    15
//IA.AIRCONDITION.RH_SAMPLE_CURVE
#define 	UCAT_AC_RH_SAMPLE_CURVE    16

// IA.AIRCONDITION.LOCK 空调锁
#define UCAT_AC_LOCK	17

// IA.AIRCONDITION.POWERCHECK	电流检测开关机的开关
#define UCAT_AC_POWERCHECK 18

//// IA.AIRCONDITION.AUTO_CTL_MARGIN
 #define UCAT_IA_AUTO_CTRL_MARGIN			19

 #define UCAT_IA_AUTO_CTRL_UTC			20


// IA.CODE
#define	UCSOT_IA_CODE		3
#define UCAT_CODE_PROCESS		1
#define UCAT_CODE_MATCH		2
#define UCAT_CODE_LIST		3
#define UCAT_CODE_INFO		4
#define UCAT_CODE_DATA		5
#define UCAT_CODE_DATA2		6
#define UCAT_CODE_CONTROL_BOARD  7
#define UCAT_CODE_KEY            8
#define UCAT_CODE_LEARN          9
#define UCAT_CODE_CONTROL        10
// 带类型的匹配、下载
#define UCAT_CODE_MATCH_V2  11
#define UCAT_CODE_LIST_V2   12
#define UCAT_CODE_INFO_V2   13
#define UCAT_CODE_DATA_V2   14
#define UCAT_CODE_DATA2_V2  15
// 遥控器信息
#define UCAT_CODE_RC_INFO   16
// 遥控器按键信息
#define UCAT_CODE_RC_KEY    17
// 遥控器学习
#define UCAT_CODE_RC_LEARN  18
// 遥控器控制
#define UCAT_CODE_RC_CTRL   19
// 遥控器匹配
#define UCAT_CODE_RC_MATCH  20

/*
*HEATER_LCYT 油灯支持
*/
//IA.HEATER_LCYT
#define UCOT_IA_HEATER_LCYT 			4
//IA.HEART_LCYT.WORK
#define UCAT_HEATER_LCYT_WORK			1
//IA.HEART_LCYT.MODE
#define UCAT_HEATER_LCYT_MODE			2
//IA.HEART_LCYT.GEAR
#define UCAT_HEATER_LCYT_GEAR			3
//IA.HEART_LCYT.CUR_TEMP
#define UCAT_HEATER_LCYT_CUR_TEMP		4
//IA.HEART_LCYT.SET_TEMP
#define UCAT_HEATER_LCYT_SET_TEMP		5
//IA.HEART_LCYT.SET_TIME
#define UCAT_HEATER_LCYT_SET_TIME		6
//IA.HEART_LCYT.REMAIN_TIME
#define UCAT_HEATER_LCYT_REMAIN_TIME	7

/*
*联创快热炉添加
*/
//IA.FAST_HOT_FURNACE
#define UCOT_IA_FHF					5
//IA.FAST_HOT_FURNCE.WORK
#define UCAT_FHF_WORK				1
//IA.FAST_HOT_FURNCE.SET_TIMER
#define UCAT_FHF_SET_TIMER			2
//IA.FAST_HOT_FURNCE.GET_TIME
#define UCAT_FHF_GET_TIMER			3
//IA.FAST_HOT_FURNCE.POWER
#define UCAT_FHF_GET_POWER			4
//IA.FAST_HOT_FURNCE.SET_TEMP
#define UCAT_FHF_SET_TEMP			5
//IA.FAST_HOT_FURNCE.GET_TEMP
#define UCAT_FHF_GET_TEMP			6
//IA.FAST_HOT_FURNCE.GET_SN
#define UCAT_FHF_GET_SN				7

/*
海科空气净化器
*/
//IA.AIR_CLEANER
#define UCOT_IA_HKAC					6
//IA.HK_AIR_CLEANER.WORK
#define UCAT_HKAC_WORK				1
//IA.HK_AIR_CLEANER.MODE
#define UCAT_HKAC_MODE				2
//IA.HK_AIR_CLEANER.WIND
#define UCAT_HKAC_WIND				3
//IA.HK_AIR_CLEANER.TEMP
#define UCAT_HKAC_TEMP				4
//IA.HK_AIR_CLEANER.PM25
#define UCAT_HKAC_PM25				5
//IA.HK_AIR_CLEANER.HUMIDITY
#define UCAT_HKAC_HUMIDITY				6
//IA.HK_AIR_CLEANER.ANION_UVL
#define UCAT_HKAC_ANION_UVL			7
//IA.HK_AIR_CLEANER.TIMER
#define UCAT_HKAC_TIMER				8

// 高讯调光灯
#define UCOT_IA_GX_LED		7
#define UCAT_GX_LED_STATUS	1

/*
 * E宝
 */
//IA.EB
#define UCOT_IA_EB			8
//IA.EB.WORK
#define UCAT_EB_WORK		1
//IA.EB.TIMER
#define UCAT_EB_TIMER		2



/* 电热毯subobject和attribute值定义 */
#define UCSOT_IA_CH_BLANKET 10
//状态
#define UCAT_CH_STATUS 1
#define UCAT_CH_QUERY_LEFT 2
#define UCAT_CH_QUERY_RIGHT 3


//智能电器之透明传输: Transparent transmission
//IA.TT
#define UCSOT_IA_TT  11

//IA.TT. ALLSTATE 查询或push所有的状态，该属性方便手机统一处理，反馈WIFI模块定时查询维护的值
#define UCAT_IA_TT_ALLSTATE 1
//IA.TT.command ctrl 设置与查询命令
#define UCAT_IA_TT_CMD 2
//IA.TT.command result 最后一条命令的结果
#define UCAT_IA_TT_CMDRET 3
//IA.TT.uart_cmd 设备透传的客户端串口命令
#define UCAT_IA_TT_UARTCMD 4
//IA.TT.ir_notify 设备上报的红外编码
#define UCAT_IA_TT_IR_NOTIFY 5
//IA.TT.reset 设备处理客户端来的reset命令
#define UCAT_IA_TT_RESET 6
//IA.TT.test_flag	产测标志
#define UCAT_IA_TT_TESTFLAG 7
//IA.TT.ir_sound_notify 设备上报声音采集数据
#define UCAT_IA_TT_IR_SOUND_NOTIFY 8
//透传模式下，定时器的开的串口命令
#define UCAT_TT_ON_CMD			9
//透传模式下，定时器的关命令
#define UCAT_TT_OFF_CMD         10


/*----------------------UCOT_IA	2  UCSOT_IA_PUBLIC 12----------------------*/
//智能电器公共属性
//IA.PUBLIC
#define UCSOT_IA_PUBLIC 12
//IA.PUBLIC. PERIOD_TIMER 时间段定时器
#define UCAT_IA_PUBLIC_PERIOD_TIMER  1
// IA.PUBLIC.EXCEPTION_QUERY 异常报告
#define UCAT_IA_PUBLIC_EXCEPTION 2
//通过手机配置无线路由器的ssid和password
#define UCAT_IA_PUBLIC_WIFI_SET            3

//IA.PUBLIC. EXTENDED_TIMER 扩展的时间段定时器
#define UCAT_IA_PUBLIC_EXTENDED_TIMER  4
#define UCAT_IA_TMP_CURVE			5
#define UCAT_IA_TMP_CURVE_UTC			6
// 金长信智能配电箱私有属性
// IA.JCX
#define UCSOT_IA_JCX 16

// 电力参数
#define UCAT_IA_JCX_VOL 1
#define UCAT_IA_JCX_CURRENT 2
#define UCAT_IA_JCX_ACTIVE_POWER 3
#define UCAT_IA_JCX_REACTIVE_POWER 4
#define UCAT_IA_JCX_POWER_FACTOR 5
#define UCAT_IA_JCX_FREQ 6
#define UCAT_IA_JCX_ACTIVE_DEGREE 7
#define UCAT_IA_JCX_REACTIVE_DEGREE 8
#define UCAT_IA_JCX_CHANNEL_NAME 9
#define UCAT_IA_JCX_CHANNEL_INFO 10
#define UCAT_IA_JCX_SN 11
#define UCAT_IA_JCX_SOFT_VER 12
#define UCAT_IA_JCX_HARWARE_VER 13


// 通用的用电量统计
#define UCSOT_PUBLIC_ELE 17
// 属性同IA STAT完全一致

//亿林温控器，先占坑
//IA.YL 亿林子对象
#define UCSOT_IA_THERMOSTAT_YL			18
//亿林温控器开关
#define UCAT_IA_THERMOSTAT_YL_WORK		1
//亿林温控器模式
#define UCAT_IA_THERMOSTAT_YL_MODE		2
//亿林温控器温度设置
#define UCAT_IA_THERMOSTAT_YL_SET_TMP	3
//亿林温控器室温
#define UCAT_IA_THERMOSTAT_YL_ROOM_TMP	4
//亿林温控器功能菜单
#define UCAT_IA_THERMOSTAT_YL_FUNC_MENU	5
//亿林温控器补偿温度
#define UCAT_IA_THERMOSTAT_YL_OFFSET_TMP 6


#define UCSOT_IA_JNB_THERMOSTAT 20	/* 杰能宝温控器 */
#define UCAT_IA_JNB_STAT 1	/* 杰能宝温控器属性 */

/* 特林温控器 */
/* IA.TELIN */
#define UCSOT_IA_TELIN (23)
/* IA.TELIN.WORK */
#define UCAT_IA_TELIN_BASE  (1)
/* IA.TELIN.ADVANCE */
#define UCAT_IA_TELIN_ADV   (2)
/* IA.TELIN.STAT */
#define UCAT_IA_TELIN_STAT  (3)
/* IA.TELIN.TIMER */
#define UCAT_IA_TELIN_TIMER (4)
/* IA.TELIN.SYNC_TIME */
#define UCAT_IA_TELIN_SYNC_TIME (5)


//海迅破壁机
#define UCSOT_IA_BREAK_MACHINE                 22
enum{
	//设置海迅破壁机的工作模式
	UCAT_IA_BREAK_MACHINE_MODE = 1,
	//获取设备已工作时间
	UCAT_IA_BREAK_MACHINE_ON_TIME,
	//获取温度
	UCAT_IA_BREAK_MACHINE_TEMP,
	//获取转速
	UCAT_IA_BREAK_MACHINE_SPEED,
	//工作完成
	UCAT_IA_BREAK_MACHINE_FINISH,
	//工作异常终止
	UCAT_IA_BREAK_MACHINE_STOP,
	//diy 名字
	UCAT_IA_BREAK_MACHINE_DIYNAME,
	//工作暂停
	UCAT_IA_BREAK_MACHINE_PAUSE,
	//工作空闲
	UCAT_IA_BREAK_MACHINE_IDLE,
	//保持温度
	UCAT_IA_BREAK_MACHINE_KEEP_WARM,
	UCAT_IA_BREAK_MACHINE_MAX
};


//千帕智能茶盘
//IA.TEA_TRAY_QP 千帕智能茶盘子对象
#define UCSOT_IA_TEA_TRAY_QP			24
//千帕智能茶盘开关
#define UCAT_IA_TEA_TRAY_QP_WORK		1
//千帕智能茶盘手动加水
#define UCAT_IA_TEA_TRAY_QP_WATER		2
//千帕智能茶盘手动工作
#define UCAT_IA_TEA_TRAY_QP_CTRL	3
//千帕智能茶盘方案
#define UCAT_IA_TEA_TRAY_QP_PLAN	4
//千帕智能茶盘方案ID查询
#define UCAT_IA_TEA_TRAY_QP_PLAN_ID	5
//千帕智能茶盘方案执行
#define UCAT_IA_TEA_TRAY_QP_PLAN_EXECUTE	6
//千帕智能茶盘故障复位
#define UCAT_IA_TEA_TRAY_QP_RESET	7

/*千帕锅*/
#define   UCSOT_IA_QPG   	   25
//设置千帕锅的烹饪流程
#define UCAT_IA_QPG_SET_PROC    1
//设置自定义烹饪流程
 #define UCAT_IA_QPG_PLAN       2
//查询锅的状态信息
 #define UCAT_IA_QPG_QUERY       3
//查询设备保存的所有方案id
 #define UCAT_IA_QPG_PLAN_ID       4
//修改方案后单独push报文
 #define UCAT_IA_QPG_MODIFYED       5


/* IA.statforshare社交分享排行 */
#define UCSOT_IA_SHARE 26
/* IA.statforshare.tempsetcount各个温度手动设置次数统计，用于习惯统计 */
#define UCAT_IA_SHARE_TEMP_SET_COUNT 1
/* IA.statforshare.acontime累计空调开机时间 */
#define UCAT_IA_SHARE_AC_ON_HOUR 2
/* IA.statforshare.coldfast采样周期内温度下降变化的最大值，用于速率排行 */
#define UCAT_IA_SHARE_COLD_TOP 3
/* IA.statforshare.warmfast采样周期内温度上升变化的最大值，用于速率排行 */
#define UCAT_IA_SHARE_WARM_TOP 4
/* IA.statforshare.powertoday当天的空调功率，用于功耗排行 */
#define UCAT_IA_SHARE_POWER_TODAY 5
/* IA.statforshare.powertoday配置温度变化采样周期，用于速率排行 */
#define UCAT_IA_SHARE_CONFIG_SAMPLE_PERIOD 6

/*
 *[> IA.TBHEATER.RTC_SYNC <]
 *#define UCAT_TBHEATER_RTC_SYNC       3
 *
 */

/*晴乐、沙特插座*/
#define UCSOT_IA_EPLUG_OEM      27
//开关状态
#define UCAT_IA_EP_OEM_ONOFF    0x1
//室温
#define UCAT_IA_EP_OEM_ROOM_TEMP   0x2
//恒温
#define UCAT_IA_EP_OEM_TEMP_RANGE    0x3
//最高温度范围
#define UCAT_IA_EP_OEM_MAX_TEMP    0x4
//离线保护
#define UCAT_IA_EP_OEM_OFFLINE_PROTECT    0x5
//人体探测
#define UCAT_IA_EP_OEM_PERSION_DETECT   0x6


//鑫源温控器
//IA_THERMOSTAT.XY
#define UCSOT_IA_THERMOSTAT_XY			28
//透传控制命令
#define UCAT_IA_THERMOSTAT_CTRL			1
//push命令
#define UCAT_IA_THERMOSTAT_PUSH			2
//智能模式时间段设置
#define UCAT_IA_THERMOSTAT_TIME			3
//外部温度设置
#define UCAT_IA_THERMOSTAT_EX_TMP		4
//智能回家开关
#define UCAT_IA_THERMOSTAT_SMART_HOME	5


/*车载悟空*/
//IA.CARWUKONG 车载悟空子对象
#define UCSOT_IA_CAR_WUKONG				29
//开关
#define UCAT_IA_CAR_WUKONG_ON			1
//寻车控制
#define UCAT_IA_CAR_WUKONG_SERCH		2
//车内温度查询
#define UCAT_IA_CAR_WUKONG_TEMP			3
//车电瓶安全电压检测开关
#define UCAT_IA_CAR_WUKONG_VALCHECK		4
//设备节能开关
#define UCAT_IA_CAR_WUKONG_POWERSAVE	5
//报警push命令
#define UCAT_IA_CAR_WUKONG_ALARM		6

//海迅养生壶
#define UCSOT_IA_HX_YS_POT     31
//状态
#define UCAT_IA_HX_YS_POT_WORK_STAT 0x1
//情景执行
#define UCAT_IA_HX_YS_POT_EXEC_SCENE   0x2
//自定义情景
#define UCAT_IA_HX_YS_POT_EDIT_SCENE   0x3
//情景ID列表
#define UCAT_IA_HX_YS_POT_SCENE_LIST   0x4


//月兔空调
#define UCSOT_IA_YUETU     32

//IA.YUETU.WORK
#define UCAT_IA_YUETU_WORK 1
//IA.YUETU.TIMER
#define UCAT_IA_YUETU_TIMER 2
//IA.YUETU.STAT
#define UCAT_IA_YUETU_STAT 3
//IA.YUETU.ERROR_PROTECT
#define UCAT_IA_YUETU_ERROR_PROTECT 4
//IA.YUETU.ELE
#define UCAT_IA_YUETU_ELE 5


//澳德绅热水器
#define UCSOT_IA_ADS	33

//控制命令
#define UCAT_IA_ADS_CTRL		1
//配置命令
#define UCAT_IA_ADS_CONF		2


//晶石微波炉
#define UCSOT_IA_JS_WEIBOLU     34
enum{
//状态
UCAT_IA_JS_WEIBOLU_WORK_STATE  = 0x01,
// 设置属性，没有查询参数，这个在状态报文中有
UCAT_IA_JS_WEIBOLU_PARAM_SET = 0x02,
//开始，暂停，取消
//UCAT_IA_JS_WEIBOLU_START_PAUSE_CANCEL,
//童锁
UCAT_IA_JS_WEIBOLU_TONGSUO = 0x04
};


//中山电壁炉
#define  UCSOT_IA_ELEC_HEATER  36
//WIFI参数设置
#define  UCAT_IA_ELEC_HEATER_WIFI_SET      0x1
//命令参数设置
#define  UCAT_IA_ELEC_HEATER_PARAM_PRO   0x2

/*--------------------------------UCOT_STORAGE 3-----------------------------------*/

/*
	STORAGE(存储)
*/
// storage.
#define	UCOT_STORAGE	3

// storage.string.
#define	UCSOT_STORAGE_STRING	1
// storage.binary.
#define	UCSOT_STORAGE_BINARY	2
// storage.packet.
#define	UCSOT_STORAGE_PACKET	3

// storage.packet.receive
#define	UCAT_PACKET_RECEIVE	1
// storage.packet.send
#define	UCAT_PACKET_SEND	2

/*--------------------------------UCOT_UPGRADE 4-----------------------------------*/
/*
*升级相关
*/
//upgrade
#define UCOT_UPGRADE	4
//upgrade.flash
#define UCSOT_UPGRADE_FLASH 	1
//upgrade.flash.flashinfo
#define UCAT_FLASH_FLASHINFO	1
//upgrade.flash.erase
#define UCAT_FLASH_ERASE		2
//upgrade.flash.upgrade
#define UCAT_FLASH_UPGRADE		3
//upgrade.flash.copy
#define UCAT_FLASH_COPY			4
//upgrade.flash.erasestm
#define UCAT_FLASH_ERASE_STM	5

/*--------------------------------UCOT_DEVPRO 5-----------------------------------*/
/*
*产测对象
*/
//dev_product
#define UCOT_DEVPRO 			5
//devproduct.ir 整机产测子对象红外产测
#define UCSOT_DEVPRO_IR			1
//devpro.ir.val_kb 电压 get时ad/ad2,set时k/ad/ad2值
#define UCAT_IR_VALKADKB		2
//devpro.ir.cur_kb 电流 get时ad,set时b/k
#define UCAT_IR_CURKB			3
//devpro.ir.adjust
#define UCAT_IR_ADJUST			4
//悟空2.0计量芯片HLW8012
#define UCAT_IR_HLW8012			5

/*--------------------------------UCOT_PLUG_DEVPRO 7-----------------------------------*/
/*
*产测对象
*/
//plug_product
#define UCOT_PLUG_DEVPRO 			7
//plug_product.av
#define UCSOT_PLUG_DEVPRO_AV        1
//plug_product.av.adjust
#define UCAT_AV_ADJUST	1


/*--------------------------------UCOT_MISC 6-----------------------------------*/
/*
*misc对象
*/
//misc
#define UCOT_MISC				6
//misc.debug
#define UCSOT_MISC_DEBUG		1
//misc.debug.monthstat
#define UCAT_DEBUG_MONTH		1


/*
 * 拓邦热水泵
 */
/* IA.TBHEATER */
#define UCSOT_IA_TBHEATER            13
/* IA.TBHEATER.USERCONFIG */
#define UCAT_TBHEATER_USERCONFIG     1
/* IA.TBHEATER.WORKCONFIG */
#define UCAT_TBHEATER_WORKCONFIG     2
/* IA.TBHEATER.HEATER_TEMP */
#define UCAT_TBHEATER_HEATER_TEMP    3
/* IA.TBHEATER.STATE */
#define UCAT_TBHEATER_STATE          4
/* IA.TBHEATER.SYSINFO */
#define UCAT_TBHEATER_SYSINFO        5
/* IA.TBHEATER.UPGRADE_SLAVE */
#define UCAT_TBHEATER_UPGRADE_SLAVE  6
/* IA.TBHEATER.BINDSN */
#define UCAT_TBHEATER_BINDSN         7
/* IA.TBHEATER.BINDSN_S */
#define UCAT_TBHEATER_BINDSN_S       8
/* IA.TBHEATER.FAULTREPORT_S */
#define UCAT_TBHEATER_FAULTREPORT_S  9
/* IA.TBHEATER.DEVINFO_S */
#define UCAT_TBHEATER_DEVINFO_S     10
/* IA.TBHEATER.FAULTLOG_S */
#define UCAT_TBHEATER_FAULTLOG_S    11

//商用拓邦
//状态数据获取命令
#define UCAT_IA_TB_STATUS				1
//设置命令
//温度设置
#define UCAT_IA_TB_TEMP					2
//模式设置
#define UCAT_IA_TB_MODE					3
//开关设置
#define UCAT_IA_TB_ONOFF				4
//除霜设置
#define UCAT_IA_TB_DEFROST				5
//有无掉电保护等杂项设置
#define UCAT_IA_TB_MISC					6
//一些保护设置,如防冻等
#define UCAT_IA_TB_PROTECT				7
//底盘电加热启动温度设置
#define UCAT_IA_TB_BERT					8
//室温获取
#define UCAT_IA_TB_ROOTTMP				9
//电子膨胀阀操作
#define UCAT_IA_TB_EEV					10


//IA.BREAK_MACHAIN 千帕破壁机
#define UCSOT_IA_BREAK_MACHAIN		30
//操作开关
#define UCAT_IA_QP_BREAK_DO_WORK		(1)
//执行食谱
#define UCAT_IA_QP_BREAK_DO_EXECEUTE		(2)
//增删改食谱
#define UCAT_IA_QP_BREAK_DO_RECIPE		(3)
//获取当前食谱list
#define UCAT_IA_QP_BREAK_GET_RECIPE_LIST	(4)
//清除错误状态信息
#define UCAT_IA_QP_BREAK_RESET_FAULT		(5)

//通配符0xFFFF
#define ATTR_ALL	(0xFFFF)



typedef struct {
	u_int16_t objct;
	u_int16_t sub_objct;
	u_int16_t attr;
	u_int16_t param_len;
	// u_int8_t param[0];
} ucp_obj_t;

typedef struct {
	// UCA_XXX
	u_int8_t action;
	u_int8_t count;
	u_int16_t reserved;
	// ucp_obj_t objs[0];
} ucp_ctrl_t;

typedef struct {
	// 主版本号
	u_int8_t major;
	// 次版本号
	u_int8_t minor;
	// 修订版本号
	u_int8_t revise;
	u_int8_t reserved;
} ucp_version_t;

typedef struct {
	u_int8_t user_enable;
	u_int8_t device_enable;
} ucp_led_t;

typedef struct {
	ucp_version_t soft_ver;
	ucp_version_t stm_ver;
	u_int8_t dev_type;
	u_int8_t ext_type;
	u_int8_t vendor[MAX_VENDER];
	u_int8_t pad[2];
	ucp_version_t hard_ver;
	ucp_version_t stm_hard_ver;
}dev_info_t;


// 开关
#define	AC_POWER_ON	0
#define	AC_POWER_OFF	1

// 模式
#define	AC_MODE_AUTO	0
#define	AC_MODE_COLD	1
#define	AC_MODE_AREFACTION 2
#define	AC_MODE_WIND	3
#define	AC_MODE_HOT	4

#define	AC_TEMP_BASE	16
#define	AC_TEMP_COMFORT 26
#define AC_TEMP_TOHIGH 28
#define AC_TEMP_TOLOW 20
#define AC_TEMP_OFFSET 2
#define	AC_TEMP_MAX	30



// 风速
#define	AC_WIND_AUTO	0
#define	AC_WIND_1	1
#define	AC_WIND_2	2
#define	AC_WIND_3	3

// 风向
#define	AC_DIR_AUTO	0
#define	AC_DIR_1	1
#define	AC_DIR_2	2
#define	AC_DIR_3	3

// 键值
#define	AC_KEY_POWER	0
#define	AC_KEY_MODE	1
#define	AC_KEY_TEMP	2
#define	AC_KEY_WIND	3
#define	AC_KEY_DIR		4
#define AC_KEY 5

// 老式空调的实际按键
#define AC_CKEY_POWER 0
#define AC_CKEY_MODE_AUTO 1
#define AC_CKEY_MODE_COLD 2
#define AC_CKEY_MODE_AREFACTION 3
#define AC_CKEY_MODE_WIND 4
#define AC_CKEY_MODE_HOT 5
#define AC_CKEY_TEMP_UP 6
#define AC_CKEY_TEMP_DOWN 7
#define AC_CKEY_WIND_AUTO 8
#define AC_CKEY_WIND_LOW 9
#define AC_CKEY_WIND_MID 10
#define AC_CKEY_WIND_HIGH 11
#define AC_CKEY_DIR_MANUAL 12
#define AC_CKEY_DIR_AUTO 13

typedef struct {
	// AC_POWER_xxx
	u_int8_t onoff;
	// AC_MODE_XXX
	u_int8_t mode;
	// 实际温度 ＝ temp - AC_TEMP_BASE
	u_int8_t temp;
	// 风速，AC_WIND_xxx
	u_int8_t wind;
	// 风向，AC_DIR_xxx
	u_int8_t direct;
	// 键值，AC_KEY_xxx
	u_int8_t key;
	// 老式空调当前按键, AC_CKEY_xxx
	//u_int8_t ckey;
} ucp_ac_work_t;

typedef struct {
	u_int8_t id;
	u_int8_t enable;
	u_int8_t week;
	u_int8_t hour;
	u_int8_t minute;
	// AC_POWER_ON / AC_POWER_OFF
	u_int8_t onoff;
	u_int16_t reserved;
} ucp_ac_timer_item_t;

typedef struct {
	u_int8_t on_effect;
	u_int8_t reserved1;
	// 下一次定时开机时间离现在还有几分钟
	u_int16_t on_minute;

	u_int8_t off_effect;
	u_int8_t reserved2;
	// 下一次定时关机时间离现在还有几分钟
	u_int16_t off_minute;

	// ucp_ac_timer_item_t item[0];
} ucp_ac_timer_t;


typedef struct {
	// 开始时间，单位：分钟（0:0算起）
	u_int16_t begin_minute;
	// 持续多长，单位分钟
	u_int16_t last_minute;
} ucp_stat_peak_time_t;

// IA.CODE.PROCESS
enum {
	CODE_PROCESS_ACTION_CANCEL = 0,
	CODE_PROCESS_ACTION_MATCH = 1,
	CODE_PROCESS_ACTION_ALL_MATCH = 2,
	CODE_PROCESS_ACTION_COMPLETE = 3,
	CODE_PROCESS_ACTION_ERR = 4,
	CODE_PROCESS_ACTION_SET = 5,
	CODE_PROCESS_ACTION_MATCH_STATE = 6,
	CODE_PROCESS_ACTION_RETRY = 7,
};

enum {
	CODE_PROCESS_TYPE_WAIT_IR = 0,
	CODE_PROCESS_TYPE_MATCHING = 1,
};

enum {
	//云匹配等待红外信号超时
	ERR_CODE_PROCESS_WAIT_IR_TIMEOUT = 1,
	//正在匹配
	ERR_CODE_PROCESS_MATCHING = 2,
	//从云端获取code超时
	ERR_CODE_PROCESS_GET_CODE_TIMEOUT = 3,
	//没有匹配到编码
	ERR_CODE_PROCESS_NO_CODE_MATCH = 4,
	// 服务器没有连接上，外网不通
	ERR_CODE_PROCESS_SERVER_NOT_READY = 5,
};

typedef struct {
	// CODE_PROCESS_ACTION_XXX
	u_int8_t action;
	u_int8_t time_out;
	u_int16_t id;
} ucp_code_process_request_t;

typedef struct {
	// CODE_PROCESS_ACTION_XXX
	u_int8_t action;
	u_int8_t step_type;
	u_int8_t step_count;
	u_int8_t step_index;
	u_int8_t err;
	u_int8_t flagbits;
	u_int8_t pad[2];
} ucp_code_process_reply_t;

#ifndef HAS_IRLIB_V2
typedef struct {
	u_int8_t err;
	u_int8_t pad;
	u_int16_t lib_id;
	u_int8_t lib_name_len;
} ucp_code_info_reply_head_t;

typedef struct {
	u_int16_t lib_id;
	u_int16_t len;
	u_int32_t idx_begin;
} ucp_code_encode_data_reqeust_head_t;

#else
typedef struct {
	// CODE_PROCESS_ACTION_XXX
	u_int8_t action;
	u_int8_t step_type;
	u_int8_t step_count;
	u_int8_t step_index;
	u_int8_t err;
	u_int8_t flagbits;
	u_int8_t pad[2];
	u_int8_t skey;
	u_int8_t reserved[3];
} ucp_code_process_reply_v2_t;


typedef struct {
	u_int8_t err;
	u_int8_t pad;
	u_int16_t type;
	u_int16_t lib_id;
	u_int8_t lib_name_len;
} ucp_code_info_reply_head_t;

typedef struct {
	u_int16_t type;
	u_int16_t lib_id;
	u_int16_t len;
	u_int16_t reserved;
	u_int32_t idx_begin;
} ucp_code_encode_data_reqeust_head_t;

#endif

typedef struct {
	u_int16_t lib_id;
	u_int16_t idx_begin;
	u_int16_t idx_end;
} ucp_code_data_reqeust_head_t;


/* 声音匹配的数据结构 */
typedef struct {
	/*sample rate, 8000*/
	u_int16_t sample;
	/*sample bit, 8*/
	u_int8_t bit;
	/*sample time*/
	u_int8_t time;
	/* sound type*/
	u_int8_t type;
	u_int8_t pad[3];

} ucp_ac_sound_match_request_t;

typedef struct {
	/*match result. 1:match OK, 0:match failed*/
	u_int8_t result;
	/*next sample time*/
	u_int8_t next_time;
	u_int8_t pad[2];
} ucp_ac_sound_match_result_t;

//智能回家，定位信息
#define LAC_GET 1	//手机获取设备上保存的定为信息（get），设备填写is_east、 is_north、 latitude、 longitude
#define LAC_PUT 2 //手机通知设备保存等位信息（set），设备保存is_east、 is_north、 latitude、 longitude
#define LAC_SMART_ON 3 //手机计算到快要到家了，通知设备触发智能回家（set）
#define LAC_SMART_OFF 4 //手机发现刚才计算执行LAC_SMART_ON有误，通知设备取消智能回家（set）
#define LAC_BACKHOME 5 //手机告知到家(set)


typedef struct{
	u_int8_t is_east;
	u_int8_t is_north;
	u_int8_t action;
	u_int8_t pad;
	u_int16_t distance;
	u_int16_t speed;
	u_int8_t latitude[16];
	u_int8_t longitude[16];
}sa_net_location_t;

enum {
	// 不上锁
	AC_LOCK_NONE = 0,
	// 禁止所以遥控器操作
	AC_LOCK_ALL = 1,
	// 禁止除开关机以外的操作
	AC_LOCK_EXPECT_ONOFF = 2,
};

typedef struct {
	// AC_LOCK_XXX
	u_int8_t type;
	u_int8_t pad[3];
} ac_lock_t;


//鑫源温控器 cmd_ctrl
typedef struct {
	u_int8_t cmd;
	u_int16_t len;
	u_int8_t pad;
	//u_int8_t data;
}ts_xy_cc_t;

//time part data
typedef struct {
	u_int8_t start_index;
	u_int8_t end_index;
	u_int8_t week;
	u_int8_t temp;
}ts_xy_tpda_t;

//time part head
typedef struct {
	u_int8_t sub_mode;
	u_int8_t num;
	u_int16_t pad;
	//ts_xy_tpda_t data;
}ts_xy_tphd_t;

typedef struct {
	u_int8_t temp;
	u_int8_t pad[3];
} ts_xy_tmp_t;

// 海迅养生壶开始
typedef struct {
	u_int8_t work;
	u_int8_t pad[3];
} ucp_hx_hpot_work_set_t;

typedef struct {
	u_int8_t id;
	u_int8_t pad[3];
} ucp_hx_hpot_execute_t;

typedef struct {
	u_int16_t count;
	//u_int16_t id[0];
} ucp_hx_hpot_mode_query_t;

#define HPOT_MODE_METHOD_NONE 0
#define HPOT_MODE_METHOD_ADD 1
#define HPOT_MODE_METHOD_DEL 2
#define HPOT_MODE_METHOD_MODIFY 3

typedef struct {
	u_int16_t id;
	u_int8_t method; // 1: add 2: delete 3: modify
	u_int8_t pad;
	u_int8_t name[32];
	u_int32_t create_time;

	u_int8_t temp;
	u_int8_t work_time;
	u_int8_t power;
	u_int8_t keep_temp;
	u_int8_t keep_time;
	u_int8_t pad1[3];
} ucp_hx_hpot_mode_t;

typedef struct {
	u_int16_t count;
	u_int16_t pad;
//	u_int16_t id[0];
} ucp_hx_hpot_mode_id_t;

typedef struct {
	u_int8_t id;
	u_int8_t pad[3];
} ucp_hx_hpot_timer_t;

typedef struct {
	u_int16_t id;
	u_int16_t pad;

	u_int8_t temp;
	u_int8_t work_time;
	u_int8_t power;
	u_int8_t keep_temp;
	u_int8_t keep_time;
	u_int8_t pad1[3];
} ucp_hx_hpot_timer_node_t;

// 海迅养生壶结束

/*通信协议数据结构1字节对齐，新数据结构添加在本行之前*/
#pragma pack(pop)


/*****************************************
	公用函数
 *****************************************/

extern pkt_t *uc_pkt_new(int cmd, int param_len,
			bool is_request, bool is_enc, u_int8_t flags,
			u_int32_t client_sid, u_int32_t device_sid, u_int8_t request_id);

extern void uc_hdr_order(ucph_t *hdr);

extern void uc_set_time_param(uc_time_param_item_t *it,
		u_int8_t retry0, u_int8_t retry1, u_int8_t retry2,
		u_int8_t keeplive, u_int16_t die);

extern void order_uc_time(uc_time_t *t);

extern int uc_send_pkt_raw(SOCKET sock, u_int32_t ip, u_int16_t port, pkt_t *pkt);
extern int uc_send_data_raw(SOCKET sock, u_int32_t ip, u_int16_t port, void *data, int len);

extern void gen_rand_block(u_int8_t *dst, int size);
extern void uc_set_default_time_param(uc_time_param_all_t *time_param, uc_time_param_all_t *time_param_net);

extern void ucp_obj_order(ucp_obj_t *uobj);


#endif

