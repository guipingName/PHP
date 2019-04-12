#ifndef __UDP_PARSE_H__
#define __UDP_PARSE_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include <netinet/in.h>

#pragma pack(push,1)

#ifdef WIN32
static __inline u_int64_t ntoh_ll(u_int64_t n)
#else
static inline u_int64_t ntoh_ll(u_int64_t n)
#endif
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    return (((n>>56)&0xFFULL)
            |((n>>40)&0xFF00ULL)
            |((n>>24)&0xFF0000ULL)
            |((n>>8)&0xFF000000ULL)
            |((n<<8)&0xFF00000000ULL)
            |((n<<24)&0xFF0000000000ULL)
            |((n<<40)&0xFF000000000000ULL)
            |((n<<56)&0xFF00000000000000ULL));
#elif __BYTE_ORDER == __BIG_ENDIAN
    return n;
#else
# error "Please fix <bits/endian.h>"
#endif
}


/* memcache查询接口相关 */
#define MC_KEY_TYPE_DEVINFO     (3) /*mc_devinfo_t*/
#define MC_DEFAULT_PORT         (11211) /* 默认的memcache端口 */
#define MEMCACHED_MAX_KEY       251 /* We add one to have it null terminated */
#define PROTO_MAX               PROTO_VER2
#define MAX_PARAM_LEN           (1024*1024*64)
#define SN_LEN                  12    /*DS007设备序列号字符串长度*/
#define MAX_NICKNAME            16  /*用户昵称最大长度*/
#define MAX_PLUG_TIMER_NAME     64    /* 定时开关名称最大长度 */
#define MAX_EMAIL               32
#define MAX_PHONE_SINGLE        16
#define MAX_PHONE_NUM           10
#define MAX_PHONE               (MAX_PHONE_SINGLE * MAX_PHONE_NUM)
#define MAX_WEB_ROOT            64
#define MAX_HANDLE              0xFFFFFFFF
#define MAX_SERVER_HANDLE       0xEE6B2800 /*保证设备端与服务器产生的handle不能重复*/
#define MAX_SSIDPW_SSID_LEN     33
#define MAX_SSIDPW_PW_LEN       64

#define MAX_DATE 20
#define UCKEY_MAC_LEN 6
#define UCKEY_SN_LEN 6
#define UCKEY_VENDOR_LEN 16
#define UCKEY_DFPWD_LEN 8
#define UCKEY_PRESHAREKEY_LEN 8
#define MAX_UUID_BIN_LEN 16


#define tlv_next(tlv) (uc_tlv_t *)((u_int8_t *)(tlv + 1) + tlv->len)
#define tlv_type(tlv) (tlv->type)
#define tlv_len(tlv) (tlv->type)
#define tlv_n_next(tlv) (uc_tlv_t *)((u_int8_t *)(tlv + 1) + ntohs(tlv->len))
#define tlv_n_type(tlv) (ntohs(tlv->type))
#define tlv_n_len(tlv) (ntohs(tlv->len))

#define tlv_val(tlv) ((u_int8_t *)((uc_tlv_t *)tlv + 1))

typedef struct {
    // UCT_XX
    u_int16_t type;
    // 数据长度，不包括头部
    u_int16_t len;
} uc_tlv_t;

/******************debug info********************************/
#define DBG_TLV_HD_LEN      sizeof(uc_tlv_t)
//SVN号
#define DBG_TYPE_SVN        1
//电流
#define DBG_TYPE_CUR        2
//电流对应AD值
#define DBG_TYPE_CUR_AD     3
//电流k值
#define DBG_TYPE_CUR_K      4
//电流b值
#define DBG_TYPE_CUR_B      5
//电压
#define DBG_TYPE_VOL        6
//电压对应AD值
#define DBG_TYPE_VOL_AD     7
//电压k值
#define DBG_TYPE_VOL_K      8
//电压b值
#define DBG_TYPE_VOL_B      9
//光敏AD值
#define DBG_TYPE_LIGHT_AD   10
//连接服务器域名
#define DBG_TYPE_SERVER_DONAME  11
//连接服务器时间
#define DBG_TYPE_SERVER_CONNTIME 12
//当前局域网手机接入数量
#define DBG_TYPE_CLIENTS        13
//设备uptime
#define DBG_TYPE_UPTIME         14
//光感学习
#define DBG_TYPE_LIGHT_STUDY        15
//cpu使用百分比
#define DGB_TYPE_CPU_PERCENTAGE     16
//mem使用百分比
#define DBG_TYPE_MEM_PERCENTAGE     17

//待机平均电流
#define DBG_TYPE_AVG_AD     18
//待机最大电流
#define DBG_TYPE_MAX_AD     19
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
#define DBG_TYPE_IR_LIB_ID     25

#define DBG_TYPE_COLD_DELAY_PN_TIME 26
#define DBG_TYPE_COLD_DELAY_PF_TIME 27
#define DBG_TYPE_HOT_DELAY_PN_TIME  28
#define DBG_TYPE_HOT_DELAY_PF_TIME  29

//单片机编译信息
#define DGB_TYPE_STM32_INFO     30

//单片机等待校正
#define DBG_TYPE_STM_WAIT_ADJUST    31
//dev server ip
#define DBG_TYPE_DEVSERVERIP        32
//产测状态查询
#define DBG_TYPE_DEV_STATUS         33



//debug2
//光敏设置
#define DBG_TLV_LIGHT           100
//功率设置
#define DBG_TLV_POWER           101
//温度设置
#define DBG_TLV_TMP             102


/********** udp_ctrl.h header **********/


/*****************************************
    控制报文格式 (CMD_UDP_CTRL)
 *****************************************/

#define UCOT_GET_ALL_ATTRI (0xFFFF)

/*****************************************
    UDP Control Object Type 
    UDP Control Sub Object Type
    UDP Control Attribute Type
 ****************************************/
 
/*
    system. (系统)
*/
#define UCOT_SYSTEM 1

// system.support.
#define UCSOT_SYS_SUPPORT   1
// system.support.support
#define UCAT_SUPPORT_SUPPORT    1

// system.hostname.
#define UCSOT_SYS_HOSTNAME  2
// system.hostname.hostname
#define UCAT_HOSTNAME_HOSTNAME  1

// system.software.
#define UCSOT_SYS_SOFTWARE  3
#define UCAT_SYS_REBOOT         1   /* system.software.reboot */
#define UCAT_SYS_UPTIME         2   /* system.software.dev_uptime */
#define UCAT_SYS_IP             3   /* system.software.ip */
#define UCAT_SYS_SVN            4   /* system.software.svn */
#define UCAT_SYS_SSID           5   /* system.software.ssid */
#define UCAT_SYS_PASSWD         6   /* system.software.passwd */
#define UCAT_SYS_DEVSTATUS      7   /* system.software.devstatus */
#define UCAT_SYS_COM_DATE       8   /* system.software.compile_date */
#define UCAT_SYS_COM_TIME       9   /* system.software.compile_time */
#define UCAT_SYS_SYSTIME        10  /* system.software.systime */
#define UCAT_SYS_SET_SYSTIME    11  /* system.software.setsystime */
#define UCAT_SYS_DEBUGINFO      12  /* system.software.debuginfo */
// system.software.version
#define UCAT_SYS_VERSION        100 /* 可能被多个子对象用的，从大一点的定义 */
// system.software.upgrade
#define UCAT_SYS_UPGRADE        101 /*升级映像*/
#define UCAT_SYS_STM_UPGRADE    102 /*stm单片机升级*/


// system.upgrade.
#define UCSOT_SYS_UPGRADE   4

// system.hardware.
#define UCSOT_SYS_HARDWARE       5
// system.hardware.LED
#define UCAT_HARDWARE_LED      1
// system.hardware.ledcolor
#define UCAT_HARDWARE_LEDCOLOR 2
// system.hardware.stm_version
#define UCAT_HARDWARE_STM_VERSION 3
// system.hardware.stm_upgrade
#define UCAT_HARDWARE_STM_UPGRADE 4



// system.vendor.
#define UCSOT_SYS_VENDOR    6
// system.vendor.oem_id
#define UCAT_VENDOR_OEM_ID  1

// system.server.
#define UCSOT_SYS_SERVER 7
// system.server.connect_time
#define UCAT_SERVER_CONNECT_TIME 1
//system.server.domainname
#define UCAT_SERVER_DONAME 2
//system.server.ip
#define UCAT_SERVER_IP      3



// system.user
#define UCSOT_SYS_USER      8
// system.user.passwd
#define UCAT_USER_PASSWD    1
// system.user.location
#define UCAT_USER_LOCATION  2

//system.licence
#define UCSOT_SYS_LICENCE 9
//system.licence.active
#define UCAT_SYS_LICENCE_ACTIVE 1

//system.share
#define UCSOT_SYS_SHARE 10
//system.share.code
#define UCSOT_SYS_SHARE_CODE 1
//system.share.phone
#define UCSOT_SYS_SHARE_PHONE 2


/*
    IA(智能电器)
*/
// IA.
#define UCOT_IA 2

//IA.RFGW
//RF网关
#define UCSOT_IA_RFGW		(21)
enum _UCSOT_IA_RFGW_ATTR_ {
	UCAT_IA_RFGW_JOIN 		 = 1,
	UCAT_IA_RFGW_JOIN_FIND 	 = 2,
	UCAT_IA_RFGW_JOIN_ACTION = 3,
	UCAT_IA_RFGW_GROUP 		 = 4,
	UCAT_IA_RFGW_LIST 		 = 5,
	UCAT_IA_RFGW_TT 		 = 6,
	UCAT_IA_RFGW_DEV_SYNC 	 = 7,
	UCAT_IA_RFGW_PARAM       = 12,
	UCAT_IA_RFGW_TT_CACHE	 = 13,
	UCAT_IA_RFGW_LIST_DIGEST = 14,
	UCAT_IA_RFGW_LIST_DIGEST_ACK = 15,
	UCAT_IA_RFGW_LIST_WITH_ID = 16,
};

typedef struct _IA_RFGW_TT_ {
	u_int64_t sn;
	u_int16_t length;
}IA_RFGW_TT;

//
/*#define RF_PACKET_TYPE_TT (?)
typedef _RF_PACK_TT_ {
	u_int8_t cmd;
	u_int8_t num;
	u_int8_t resv;
	u_int8_t appid;
}RF_PACK_TT;*/
//RF网关透传命令头
typedef struct _RF_TT_TLV_HEAD_ {
	u_int8_t type;
	u_int8_t len;
}RF_TT_TLV_HEAD;
//命令类型
enum _DOOR_CONTACT_CONTRL_ {
	SMTDLOCK_CMD_GET_SUMMARY	= 1,
	SMTDLOCK_CMD_PUSH_SUMMARY	= 2,
	SMTDLOCK_CMD_GET_ONOFFTIME 	= 33,
	SMTDLOCK_CMD_PUSH_ONOFFTIME = 34,
	SMTDLOCK_CMD_SET_DEFENSE	= 37,
	SMTDLOCK_CMD_SET_AUTO_DEFENSE = 45,
};
//获取设备状态信息参数
typedef struct _CMD_GET_SUMMARY_ {
	u_int8_t getindex;
	u_int8_t resv;
}CMD_GET_SUMMARY;
//软件版本
typedef struct _CMD_SOFT_VER_ {
#if __BYTE_ORDER == __BIG_ENDIAN
    u_int8_t 	rev:3,
        		min_v:3,
        		mar:1;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
    u_int8_t    mar:1,
        		min_v:3,
        		rev:3;
#else
# error "Please fix <bits/endian.h>"
#endif
}CMD_SOFT_VER;
//设备状态标志
typedef struct _CMD_STAUS_FLAG_ {
#if __BYTE_ORDER == __BIG_ENDIAN
    u_int32_t 	d_onoff:1,
        		b_warn:1,
        		pry_d:1,
        		arlming:1,
        		resv_bit:28;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
    u_int32_t   resv_biy:28,
        		arlming:1,
        		pry_d:1,
        		b_warn:1,
        		d_onoff:1;
#else
# error "Please fix <bits/endian.h>"
#endif
}CMD_STATUS_FLAG;
//设备状态信息上报
typedef struct _CMD_PUSH_SUMMARY_ {
	u_int8_t 	hw_ver;
	CMD_SOFT_VER    soft_ver;
	u_int8_t	abc_battery;
	u_int8_t	resv;
 	CMD_STATUS_FLAG   flagbits;
}CMD_PUSH_SUMMARY;

//获取设备开关门时间点
typedef struct _CMD_GET_ONOFFTIME_ {
	u_int32_t timestamp;
}CMD_GET_ONOFFTIME;
//开关门状态
typedef struct _CMD_DOOR_ONOFF_STATUS_ {
#if __BYTE_ORDER == __BIG_ENDIAN
    u_int8_t 	time_flag:1,
        		valve:1,
        		type:6;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
    u_int8_t    type:6,
        		value:1,
        		time_flag:1;
#else
# error "Please fix <bits/endian.h>"
#endif	
}CMD_DOOR_ONOFF_STATUS;
//开门时间点上报
typedef struct _CMD_PUSH_ONOFFTIME_ {
	CMD_DOOR_ONOFF_STATUS on_off_1;
	CMD_DOOR_ONOFF_STATUS on_off_2;
	CMD_DOOR_ONOFF_STATUS on_off_3;
	CMD_DOOR_ONOFF_STATUS on_off_4;
	u_int32_t			  timestamp_1;
	u_int32_t			  timestamp_2;
	u_int32_t			  timestamp_3;
	u_int32_t			  timestamp_4;
}CMD_PUSH_ONOFFTIME;

//布防撤防
typedef struct _CMD_SET_DEFENSE_ {
	u_int8_t is_defense;
	u_int8_t resv;
}CMD_SET_DEFENSE;
//自动布防与撤防
typedef struct _CMD_SET_AUTO_DEFENSE_ {
	u_int8_t status;
	u_int8_t enable;
	u_int8_t starthour;
	u_int8_t endhour;
}CMD_SET_AUTO_DEFENSE;
//自动撤防与布防上报
typedef struct _CMD_PUSH_AUTO_DEFENSE_ {
	u_int8_t don_enable;
	u_int8_t doff_enable;
	u_int8_t don_starthour;
	u_int8_t don_endhour;
	u_int8_t doff_starthour;
	u_int8_t doff_endhour;
}CMD_PUSH_AUTO_DEFENSE;

// IA.stat
#define UCSOT_IA_STAT       (1)
enum _UCOST_IA_STAT_E_ {
    UCAT_STAT_TOTAL_PEAK            = 1,  /* IA.stat.total_peak */
    UCAT_STAT_TOTAL_VALLEY          = 2,  /* IA.stat.total_valley */
    UCAT_STAT_PERIOD_TIME           = 3,  /* IA.stat.period_time */
    UCAT_STAT_PERIOD_PEAK           = 4,  /* IA.stat.period_peak */
    UCAT_STAT_PERIOD_VALLEY         = 5,  /* IA.stat.period_valley */
    UCAT_STAT_MONTH_PEAK            = 6,  /* IA.stat.month_peak 月峰电统计 */
    UCAT_STAT_MONTH_VALLEY          = 7,  /* IA.stat.month_valley 月谷电统计 */
    UCAT_STAT_CUR_POWER             = 8,  /* IA.stat.cur_power */
    UCAT_STAT_PEAK_TIME             = 9,  /* IA.stat.peak_time 峰电时间 */
    UCAT_STAT_PEAK_PRICE            = 10, /* IA.stat.peak_price */
    UCAT_STAT_VALLEY_PRICE          = 11, /* IA.stat.valley_price */
    UCAT_STAT_MONTH_FLAT            = 12, /* 月平电统计 */
    UCAT_STAT_FLAT_TIME             = 13, /* 平电时间段 */
    UCAT_STAT_FLAT_PRICE            = 14, /* 平电价格 */
    UCAT_STAT_VALLEY_TIME           = 15, /* IA.stat.valley_time 谷电 */
    UCAT_STAT_TOTAL_ELE             = 16, /* 总电量 */
    UCAT_STAT_PHASE_ELE             = 17, /* 阶段电量 */
    UCAT_STAT_ON_ELE                = 18, /* 开机以来的电量 */
    UCAT_STAT_PUSH2SERV             = 20, /* IA.stat.push2serv 状态上报服务器 */
    UCAT_STAT_ELE_DAYBYDAY          = 21, /* 最近一年每天的用电量 */
    UCAT_STAT_CURR_POWER_MW         = 22, /* 当前功率，精确到毫瓦 */
    UCAT_STAT_MAX
};


//商业悟空状态上报
#define UCT_STATE_ORI_ELE              1  //从服务端获取初始电量累计值， 单位W.H。
#define UCT_STATE_PUSH_TIMER_FAST      2  //实时上报间隔，如果和服务器不一致，服务器重新设置一下
#define UCT_STATE_PUSH_TIMER_HISTORY   3  //历史上报间隔，如果和服务器不一致，服务器重新设置一下，      默认是5分钟
#define UCT_STATE_PUSH_AC_RUN          4  //空调实时运行状态
#define UCT_STATE_PUSH_POWER_ONEDAY    5  //空调上报电量信息
#define UCT_STATE_PUSH_AC_ALARM        6  //空调报警信息上报
//统一配置下发
#define UCT_STATE_SET_PERSON_INDUCTION      20 //人体探测相关设置
#define UCT_STATE_SET_AUTO_AJUST_TEMP       21 //智能调节温度
#define UCT_STATE_SET_ACOPEN_CURRENT_CHECK  22 //空调开机电流检测
#define UCT_STATE_SET_AC_CURRENT_DEFALUT    23 //空调出厂额定电流值
#define UCT_STATE_SET_ACTEMP_RANGE          24 //空调控制温度阀值，默认16-30




// IA.AIRCONDITION.
#define UCSOT_IA_AC         (2)
enum _UCSOT_IA_AC_E_ {
    UCAT_AC_WORK                   = 1,  /* IA.AIRCONDITION.WORK */
    UCAT_AC_CUR_TEMP               = 2,  /* IA.AIRCONDITION.CUR_TEMP */
    UCAT_AC_TIMER                  = 3,  /* IA.AIRCONDITION.TIMER */
    UCAT_AC_SMART_ON               = 4,  /* IA.AIRCONDITION.SMART_ON */
    UCAT_AC_SMART_OFF              = 5,  /* IA.AIRCONDITION.SMART_OFF */
    UCAT_AC_SMART_SLEEP            = 6,  /* IA.AIRCONDITION.SMART_SLEEP */
    UCAT_AC_SOUND_MATCH_REQUEST    = 7,  /* IA.AIRCONDITION.SOUND_MATCH_REQUEST */
    UCAT_AC_SOUND_DATA             = 8,  /* IA.AIRCONDITION.SOUND_DATA */
    UCAT_AC_SOUND_MATCH_RESULT     = 9,  /* IA.AIRCONDITION.SOUND_MATCH_RESULT */
    UCAT_AC_ATTRI                  = 10, /* 红外匹配结果 */
    UCAT_AC_MATCH_INFO             = 11,
    UCAT_AC_AUTO_CTRL			   = 12, /*智能恒温*/
    UCAT_AC_ON_USER_SET		       = 13, //用户设置开关机状态(不是发红外，只改设备空调状态)
    UCAT_AC_CUR_RH                 = 14, /* 湿度，加强版悟空才有这个功能 */
    UCAT_AC_TMP_SAMPLE_CURVE       = 15,
    UCAT_AC_RH_SAMPLE_CURVE        = 16,
    UCAT_AC_LOCK                   = 17, /* 童锁功能 */
    UCAT_AC_MAX
};


typedef enum IA_AC_MODE {
    IA_AC_MODE_AUTO         = 0, /* 自动 */
    IA_AC_MODE_COOL         = 1, /* 制冷 */
    IA_AC_MODE_DEHUMIDIFY   = 2, /* 除湿 */
    IA_AC_MODE_WIND         = 3, /* 送风 */
    IA_AC_MODE_HEATING      = 4, /* 制热 */
} IA_AC_MODE_E;

typedef enum IA_AC_POWER {
    IA_AC_POWER_ON         = 0, /* 开机 */
    IA_AC_POWER_OFF        = 1, /* 关机 */
} IA_AC_POWER_E;


typedef struct temp_ctrl {
    u_int8_t  enable;
    u_int8_t  mode;
    u_int8_t  temp_min;
    u_int8_t  temp_max;
    u_int8_t  week;
    u_int8_t  begin_hour;
    u_int8_t  end_hour;
    u_int8_t  pad[1];
} temp_ctrl_t;


enum _UCAT_AC_LOCK_E_{
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


// IA.CODE
#define UCSOT_IA_CODE       (3)
enum _UCSOT_IA_CODE_E_ {
    UCAT_CODE_PROCESS       = 1,
    UCAT_CODE_MATCH         = 2,
    UCAT_CODE_LIST          = 3,
    UCAT_CODE_INFO          = 4,
    UCAT_CODE_DATA          = 5,
    UCAT_CODE_DATA2         = 6,
    
    UCAT_CODE_RC_INFO   	= 16, // 遥控器信息
    UCAT_CODE_RC_KEY    	= 17, // 遥控器按键信息
    UCAT_CODE_RC_LEARN  	= 18, // 遥控器学习
    UCAT_CODE_RC_CTRL   	= 19, // 遥控器控制
    UCAT_CODE_RC_MATCH  	= 20, // 遥控器匹配
    UCAT_CODE_RC_MATCH_STATE = 21, // 遥控器匹配状态查询
    
    UCAT_CODE_MAX
};


// IA.PUBLIC.
#define UCSOT_IA_PUBLIC     12
/* IA.PUBLIC.EXCEPTION */
enum _UCSOT_IA_PUBLIC_E_ {
    UCAT_IA_PUBLIC_PERIOD_TIMER     = 1,
    UCAT_IA_PUBLIC_EXCEPTION        = 2,
    UCAT_IA_PUBLIC_WIFI_SET         = 3,
    UCAT_IA_PUBLIC_EXTENDED_TIMER   = 4,
    UCAT_IA_PUBLIC_TMP_CURVE        = 5,
    UCAT_IA_PUBLIC_TMP_CURVE_UTC	= 6,
};

/* IA.TBHEATER */
#define UCSOT_IA_TBHEATER   13
/* IA.TBHEATER.BINDSN_S */
#define UCAT_TBHEATER_BINDSN_S 8
/* IA.TBHEATER.FAULTREPORT_S */
#define UCAT_TBHEATER_FAULTREPORT_S 9
/* IA.TBHEATER.DEVINFO_S */
#define UCAT_TBHEATER_DEVINFO_S 10




/*
    STORAGE(存储)
*/
// storage.
#define    UCOT_STORAGE    3

// storage.string.
#define    UCSOT_STORAGE_STRING    1
// storage.binary.
#define    UCSOT_STORAGE_BINARY    2
// storage.packet.
#define    UCSOT_STORAGE_PACKET    3

// storage.packet.receive
#define    UCAT_PACKET_RECEIVE    1
// storage.packet.send
#define    UCAT_PACKET_SEND    2



/*
    UPGRADE
 */
#define UCOT_UPGRADE (4)

#define UCSOT_UPGRADE_FLASH (1)
enum _UCSOT_UPGRADE_FLASH_E_ {
    UCAT_FLASH_FLASHINFO    = 1,
    UCAT_FLASH_ERASE        = 2,
    UCAT_FLASH_UPGRADE      = 3,
    UCAT_FLASH_COPY         = 4,
    UCAT_FLASH_ERASE_STM    = 5,
};


/*此结构体用ucp_code_process_request_t代替*/
/**进入匹配红外编码*/
typedef struct{
    u_int8_t action;                /**Action = 0取消匹配Action = 1云匹配，Action = 2全匹配*/
    u_int8_t timeout;               /**超时时间(云匹配用，单位秒)，表示接受红外信号超时时间*/
    u_int8_t resv0;                 /**保留*/
    u_int8_t resv1;                 /**保留*/
}ucp_ia_matchcode_t;

/*此结构体用ucp_code_process_reply_t代替*/
/**匹配红外编码结果*/
typedef struct{
    u_int8_t action;                /**Action = 0取消匹配Action = 1云匹配，Action = 2全匹配*/
    u_int8_t step_type;             /**Step Type = 0 云匹配用，表示进入云匹配模式*/
    u_int8_t step_count;            /**Step Count，总共需要进行的步骤*/
    u_int8_t step_index;            /**Step index，当前进行到第几步*/
    u_int8_t error_num;             /**Error Number：0无错误，?网络错误*/
    u_int8_t resv0;                 /**保留*/
    u_int16_t resv1;                /**保留*/
}ucp_matchcode_result_t;

/*增强悟空通知设备进入红外匹配*/
typedef struct{
	u_int8_t	action;		/*Action = 0取消匹配Action = 1云匹配*/
	u_int8_t	timeout;	/*超时时间(云匹配用，单位秒)，表示接受红外信号超时时间*/
	u_int16_t	id;         /*红外匹配时填0*/
	u_int8_t	type;		/*遥控板类型*/
	u_int8_t	devid;		/*遥控板id*/
	u_int16_t	reserved; 	/*保留字段*/
}ucp_ucat_code_match_s;

/*红外匹配结果*/
typedef struct{
	u_int8_t	action;		/*Action = 7*/
	u_int8_t	timeout;	/*超时时间(云匹配用，单位秒)，表示接受红外信号超时时间*/
	u_int16_t	id;         /*红外匹配时填0*/
	u_int8_t	type;		/*遥控板类型*/
	u_int8_t	devid;		/*遥控板id*/
	u_int16_t	reserved; 	/*保留字段*/
	u_int8_t	skey;		/*继续匹配的按键*/
	u_int8_t	reverved1;	/*保留字段*/
	u_int16_t	reverved2;	/*保留字段*/
}ucp_ucat_code_match_r;

/*设备push到ice的红外匹配状态*/
typedef struct{
	u_int8_t	action;				  /*3 匹配成功 7 继续匹配*/ 
    u_int8_t 	tep_type;             /**Step Type = 0 云匹配用，表示进入云匹配模式*/
    u_int8_t 	tep_count;            /**Step Count，总共需要进行的步骤*/
    u_int8_t 	tep_index;            /**Step index，当前进行到第几步*/
    u_int8_t 	error_num;            /**Error Number：0无错误，?网络错误*/
	u_int8_t	flagbits;		
	u_int16_t	reverved;
	u_int8_t	skey;				 /*建议继续匹配的按键*/
	u_int8_t	reverved1;
	u_int16_t	reverved2;
}ucp_ucat_code_match_push;


/*遥控器查询接口*/
typedef struct{
	u_int8_t count;				/*遥控器数量*/
	u_int8_t length;			/*单个遥控器信息的长度*/
	u_int16_t reserved;         /*保留字段*/
}ucp_ucat_code_g;

/*每个遥控器的详细信息*/
typedef struct{
	u_int8_t	type;			/*遥控器类型*/
	u_int8_t	devid;			/*遥控器id*/
	u_int16_t	irid;			/*红外编码库id*/
	u_int8_t	flags;			/*标志位*/
	u_int8_t	reverted0;      /*保留位*/
	u_int16_t	reverted1;		/*保留位*/
	u_int8_t    name[16];		/*遥控器名称*/
}ucp_ucat_code_info_g;

/*自定义遥控器按键设置和删除*/
typedef struct {
	u_int8_t	type;
	u_int8_t	key;
	u_int16_t	irid;
	u_int8_t	devid;
	u_int8_t	flags;
	u_int16_t	reserved;
	u_int8_t    name[16];	
}ucp_ucat_key_c;

/*遥控器查询*/
typedef struct {
	u_int8_t	type;
	u_int8_t	flags;
	u_int16_t	irid;
	u_int8_t	devid;
	u_int8_t	reserved;
	u_int8_t	fixkey_len;
	u_int8_t	fixkey_num;
	u_int8_t	defkey_len;
	u_int8_t	defkey_num;
	u_int16_t	reserved1;
}ucp_ucat_key_g;

/*固定按键返回数据*/
typedef struct {
	u_int8_t	key;
	u_int8_t	flags;
	u_int16_t	reverved;
}ucp_ucat_fix_key_g;

/*自定义按键返回数据*/
typedef struct {
	u_int8_t	key;
	u_int8_t	flags;
	u_int16_t	reverved;
	u_int8_t	name[16];
}ucp_ucat_def_key_g;

/*按键学习*/
typedef struct {
	u_int8_t	type;
	u_int8_t 	key;
	u_int8_t	devid;
	u_int8_t	action;
	u_int32_t	reserved;
}ucp_ucat_key_learn;
	
/*按键控制*/
typedef struct {
	u_int8_t 	type;
	u_int8_t	key;
	u_int16_t	irid;
	u_int8_t	devid;
	u_int8_t	flags;
	u_int16_t	reserved;
	u_int32_t	reserved1;
}ucp_ucat_key_ctrl;
	

/*
*产测对象
*/
//dev_product
#define UCOT_DEVPRO             5
//devproduct.ir 整机产测子对象红外产测
#define UCSOT_DEVPRO_IR         1
//devpro.ir.val_kb 电压 get时ad/ad2,set时k/ad/ad2值
#define UCAT_IR_VALKADKB        2
//devpro.ir.cur_kb 电流 get时ad,set时b/k
#define UCAT_IR_CURKB           3
//devpro.ir.adjust
#define UCAT_IR_ADJUST          4
//悟空2.0计量芯片HLW8012
#define UCAT_IR_HLW8012         5



// udp control action
#define UCA_NONE        0
#define UCA_REQUEST     1
#define UCA_REPLY       2
#define UCA_QUESTION    3
#define UCA_ANSWER      4
#define UCA_RESULT      5
#define UCA_REDIRECT    6
#define UCA_RESET       7
#define UCA_GET         8
#define UCA_SET         9
#define UCA_DELETE      10
#define UCA_PUSH        11

/** @brief   控制报文定义*/ 
/* proto version control */
enum {
    PROTO_VER1 = 1,
    PROTO_VER2 = 2,
    /* further version add here */
};

/*role type of DS007 system*/
enum {
    TP_CENTER = 0,      /*生产中心*/
    TP_DISPATCHER = 1,  /*分配服务器*/
    TP_DEV_SRV = 2,     /*设备服务器*/
    TP_WEB_SRV = 3,     /*web服务器*/
    TP_USER = 4,        /*手机或流量器用户*/
    TP_DS007 = 5,       /* DS007设备*/
    TP_CHARGE_SRV = 6,      /* 充值服务器 */
    TP_CHARGE_CLI_RO = 7,   /* 充值客户端，只读 */
    TP_CHARGE_CLI_WR = 8,   /* 充值客户端，读写  */
    TP_MAX
};
/*通信协议命令
禁止在已有命令中间插入新命令
只能顺序添加到后面*/
enum {
    CMD_OK = 0,
    CMD_FAIL = 1,
    CMD_ECHO_Q = 2,
    CMD_ECHO_A = 3,
    CMD_AUTH_Q = 4,
    CMD_AUTH_A = 5,
    CMD_AUTH_K = 6,
    CMD_EXCHANG_Q = 7,
    CMD_EXCHANG_A = 8,
    CMD_USER_BIND_Q = 9,
    CMD_USER_BIND_A = 10,
    CMD_USER_AUTH_Q = 11,
    CMD_USER_AUTH_A = 12,
    CMD_USER_AUTH_K = 13,
    CMD_SERVER_Q = 14,
    CMD_SERVER_A = 15,
    CMD_DEV_Q = 16,
    CMD_DEV_A = 17,
    CMD_URL_Q = 18,
    CMD_URL_A = 19,
    CMD_DEV_CONFIG_Q = 20,
    CMD_DEV_CONFIG_A = 21,
    CMD_DEV_STAT_Q = 22,
    CMD_DEV_STAT_A = 23,
    CMD_DEV_STAT_CYCLE = 24,
    CMD_ELECT_MASTER_Q = 25,
    CMD_ELECT_MASTER_A = 26,
    CMD_ELECT_MASTER_C = 27,
    CMD_NICKNAME = 28,
    CMD_PASSWD = 29,
    CMD_LINK_OPTION_Q = 30,
    CMD_LINK_OPTION_A = 31,
    CMD_WEB_AUTH_Q = 32,
    CMD_WEB_AUTH_A = 33,
    CMD_NICKNAME_Q = 34,
    CMD_NICKNAME_A = 35,
    CMD_FTP_Q = 36,
    CMD_FTP_A = 37,
    CMD_YW_SERVER_Q = 38, /*运维查询服务器请求*/
    CMD_KEEP_DATA = 39,
    CMD_NICKNAME_CONFIRM = 40,  /*修改昵称成功确认命令，服务器内部使用*/
    CMD_NICKNAME_FAIL = 41,     /*修改昵称失败命令，服务器内部使用*/
    CMD_LOAD_USER_INFO = 42,    /*加载用户基本信息*/
    CMD_UDP_KEEP_DATA = 43,
    CMD_UDP_DEV_STAT = 44,
    CMD_SERVICE_DATE_Q = 45,    /*服务套餐查询请求*/
    CMD_SERVICE_DATE_A = 46,    /*服务套餐查询响应*/
#if 0   
    CMD_SERVICE_CHARGE = 47,    /* 服务套餐更新*/
#endif  
    CMD_URL_HIT_Q = 48,         /*查询URL 命中请求*/
    CMD_URL_HIT_A = 49,         /*查询URL 命中响应*/
    CMD_IPLOCATION_Q    = 50,   /*查询客户端IP地址范围*/
    CMD_IPLOCATION_A = 51,  /*dispatcher 查询客户端IP地址范围*/
    CMD_SELLER_LOGIN_Q  = 52,   /*销售人员登录认证请求*/
    CMD_SELLER_LOGIN_A  = 53,   /*销售人员登录认证应答*/
    CMD_SELLER_USER_Q   = 54,   /*用户账号查询请求*/
    CMD_SELLER_USER_A   = 55,   /*用户账号查询应答*/
    CMD_SELLER_USER_ADD = 56,   /*添加账号*/
    CMD_SELLER_USER_MODIFY = 57,/*修改账号*/
    CMD_SELLER_USER_DEL = 58,   /*删除账号*/
    CMD_VEDIO_Q = 59, /* 请求查看摄像头视频 */
    CMD_VEDIO_A = 60, /* 摄像头视频信息应答 */
    CMD_SELLER_PWD = 61,    /*修改账号密码*/
    CMD_VIDEO_SYN_Q = 62,
    CMD_VIDEO_SYN_A = 63,
    CMD_VIDEO_HELLO_REQ = 64,
    CMD_VIDEO_HELLO =65,
    CMD_VIDEO_JPG = 66,
    CMD_VIDEO_ACK = 67,
    CMD_VIDEO_QUALITY = 68,
    CMD_VIDEO_STOP = 69,
    CMD_VEDIO_AGENT_A = 70,
    CMD_VEDIO_AGENT_Q = 71,
    CMD_VEDIO_AGENT_SETUP = 72,
    CMD_MISC_Q = 73,
    CMD_MISC_A = 74,
    CMD_UDP_YW_DISPATCH_Q = 78,//运维--在分配服务器下-向设备服务器发送-取得设备服务器下详细设备列表
    CMD_LOAD_PACKAGE_LOG_Q = 79,//查询充值日志
    CMD_LOAD_PACKAGE_LOG_A = 80,//查询充值日志
    CMD_UDP_YW_DISPATCH_A = 81,////运维--在分配服务器下-向设备服务器发送
    CMD_ADD_RC_CARD_Q = 83,  /*添加充值卡请求*/
    CMD_ADD_RC_CARD_A = 84,  /*添加充值卡响应*/
    #if 0
    CMD_MOD_RC_CARD_Q = 85 , /*更新充值卡状态请求*/
    CMD_MOD_RC_CARD_A = 86,  /*更新充值卡状态响应*/
    CMD_DEL_RC_CARD_Q = 87,  /*删除充值卡请求*/
    CMD_DEL_RC_CARD_A = 88, /*删除充值卡响应*/
    #endif
    CMD_TRANS_RC_CARD_Q = 89, /*转移充值卡请求*/
    CMD_TRANS_RC_CARD_A = 90, /*转移充值卡响应*/
    CMD_QUERY_RC_CARD_Q = 91, /*查询充值卡状态请求*/
    CMD_QUERY_RC_CARD_A = 92, /*查询充值卡状态响应*/
    CMD_CARD_CHARGE_Q = 93, /*充值请求*/
    CMD_CARD_CHARGE_A = 94, /*充值响应*/
    CMD_DEV_CHARGE = 95,  /*充值成功同步通知命令*/
    CMD_YW_SERVER_A = 96, /*运维查询服务器响应*/
    CMD_VEDIO_AGENT_OK = 97,/*代理成功*/
    CMD_ADD_CARD_TYPE_Q = 98,/*添加卡类型请求*/
    CMD_ADD_CARD_TYPE_A = 99,/*添加卡类型响应*/
    CMD_QUERY_CARD_TYPE_Q = 100,/*查询卡类型请求*/
    CMD_QUERY_CARD_TYPE_A = 101,/*查询卡类型响应*/
    CMD_QUERY_USER_PWD_Q = 102,/*取得当前登录DEV密码*/
    #if 0
    CMD_CARD_CHARGE_SYN_Q = 103,/*充值信息同步请求*/
    CMD_CARD_CHARGE_SYN_A = 104,/*充值信息同步应答*/
    CMD_CARD_TYPE_SYN_Q = 105,/*充值卡类型同步请求*/
    CMD_CARD_TYPE_SYN_A = 106,/*充值卡类型同步应答*/
    CMD_CARD_STATUS_SYN_Q = 107,/*充值卡状态同步请求*/
    CMD_CARD_STATUS_SYN_A = 108,/*充值卡状态同步应答*/
    CMD_DEV_CHARGE_CONFIRM = 109,/*充值成功同步通知确认命令*/
    #endif
    CMD_YW_DEVSERVER_Q = 110,/*运维管理--设备服务器查询*/
    CMD_YW_DEVSERVER_A = 111,/*运维管理--设备服务器查询*/
    CMD_BIND = 112, /* 主从设备命令 */
    CMD_SET_NAME = 113, /* 主从设备命令 */
    CMD_MS_CTRL = 114,  /* 主从设备命令，服务器不需要处理 */
    CMD_VIDEO_ROLL = 115, /* 控制云台转动 */
    CMD_FM_CONFIG_Q = 116,  /*配置家庭测量人员请求*/
    CMD_FM_CONFIG_A = 117,  /*配置家庭测量人员响应*/
    CMD_FM_Q = 118,  /*查询家庭测量人员请求*/
    CMD_FM_A = 119,  /*查询家庭测量人员响应*/
    CMD_MESURE_TRANS = 120, /*传输测量结果*/
    CMD_MESURE_Q = 121,      /*查询测量结果请求*/
    CMD_MESURE_A = 122,      /*查询测量结果响应*/
    CMD_MESURE_DEL = 123,   /*删除测量结果*/
    CMD_MESURE_TRANS_CONFIRM = 124, /*设备服务器间同步测量数据确认*/
    CMD_PLUG_TIMER_Q = 125, /* 定时开关的请求 */
    CMD_PLUG_TIMER_A = 126, /* 定时开关的回应 */
    CMD_USER_DEBUG = 127,     /*手机上传调试信息*/
    CMD_IR_LIST = 128,      /*手机请求支持列表*/
    CMD_IR_CONFIG = 129,    /*手机配置和获取已配置家电*/
    CMD_IR_DB = 130,        /*设备从服务器获取编码库*/
    CMD_IR_URL = 131,       /*设备获取电器遥控列表的URL */
    CMD_IR_CTRL= 132,       /* 手机测试红外编码库是有有效 */
    CMD_REMOTE_CONFIG = 133, /* CMD_REMOTE_CONFIG */
    CMD_REMOTE_KEY_CONFIG = 134, /* 手机为家庭电器配置一个按键 */
    CMD_REMOTE_CODE = 135, /* 手机为按键配置一个编码 */
    CMD_REMOTE_CTRL= 136, /* 手机向家庭电器发送控制命令 */
    CMD_REMOTE_STATE = 137, /* 手机对单个电器状态的操作 */
    CMD_VIDEO_TEMP_QUALITY = 138, /* 用于云台控制时，临时调试画质，不保存配置 */
    CMD_VIDEO_RECOR_CTRL = 139,/*设备端本地录像命令------已经取消此报文*/
    CMD_ALARM_BIND_PHONE = 140, /*报警短信绑定手机*/
    CMD_ALARM_MSG = 141,    /*报警信息发送请求*/
    CMD_FAIL_EX = 142,      /* cmd_fail扩展命令 */
    CMD_ALARM_CONFIG_PHONE = 143,
    CMD_SLAVE_HIS = 144, /*从设备通告视屏信息给主设备*/
    CMD_GET_CMDOK = 145,    /* 设备端向服务器查询手机认证成功后需要的相关信息 */
    CMD_ALARM_LOG = 146,/*报警日志相关操作*/
    CMD_ALARM_LOG_SYNC = 147,/*报警日志的同步*/
    CMD_TIME_SYNC = 148,/*设备向手机查询时间同步*/
    CMD_VTAP_LIST_Q = 149,/*查询设备录像列表*/
    CMD_VTAP_LIST_A = 150,/*响应设备录像列表的查询*/
    CMD_VTAP_Q = 151,/*请求设备录像文件*/
    CMD_VTAP_KEEP = 152,/*录像观看保活*/
    CMD_VTAP_A = 153,/*录像请求结果*/
    CMD_VTAP_TIMESTAP = 154, /*请求播放指定时戳*/

    CMD_REMOTE_BROADCAST = 155, /* 广播控制命令给从设备 */
    CMD_VIDEO_SONIX = 156, /*对sonix 设置*/
    CMD_REC_TIMER_Q = 157,/* 视频录制定时开关的请求 */
    CMD_REC_TIMER_A  = 158,/* 视频录制定时开关的回应 */
    CMD_VIDEO_CONTROL_ALARM_CFG = 159, /*侦测报警配置信息*/
    CMD_MS_DEV_INFO = 160, /*主从设备通信告知相互的版本，时区，支持的功能 */

    CMD_DEV_SAHRD_INFO = 161, /*设备共享命令，请求与响应相同*/
    CMD_DEV_SAHRD_COUNT = 162, /*设备分享次数增加命令*/
    CMD_OPEN_TELNET = 163, /*开启从设备telnet服务*/
    CMD_ALARM_SWITCH_CTL = 164, /*安防报警总开关控制*/
    CMD_VTAP_END = 165,/*设备通知客户端，录像播放完毕*/
    CMD_VIDEO_QUALITY_V2 = 166,/* 自定义分辨率设置 */
    CMD_PLUG_ELECTRIC_STAT = 167,/*电量统计功能支持查询命令*/
    CMD_SCENE_CONFIG = 168,/*情景模式操作命令*/
    CMD_OPT_SCAN_Q = 169, /*扫描刷机设备请求*/
    CMD_OPT_SCAN_A = 170, /*扫描刷机设备响应*/
    CMD_DEV_REG_Q = 171,  /*刷机设备注册请求*/
    CMD_DEV_REG_A = 172, /*刷机设备注册响应*/
    CMD_ARIA2C_Q = 173, /*aria2c控制请求*/
    CMD_ARIA2C_A = 174, /*aria2c控制响应*/
    CMD_NETWORK_DETECT_Q = 175, /* 视频网络探测请求包 */
    CMD_NETWORK_DETECT_A = 176, /* 视频网络探测响应包 */
    CMD_SSIDPW = 177, /*手机修改设备wifi SSID和密码*/
    CMD_DEVSERVER_LIST_Q = 178, /*获取设备服务器列表请求*/
    CMD_DEVSERVER_LIST_A = 179, /*获取设备服务器列表响应*/
    CMD_NET_PROBE_Q = 180,  /*设备到服务器网络探测请求*/
    CMD_NET_PROBE_A = 181,  /*设备到服务器网络探测响应*/
    CMD_LOCATION_Q = 182,  /*设备位置请求*/
    CMD_LOCATION_A = 183,  /*设备位置响应*/
    CMD_MACDENY = 184, /*添加wifi mac禁止列表*/
    CMD_MACALLOW = 185, /*删除wifi mac禁止列表*/
    CMD_BIND_SLAVE_INFO = 186,/*获取从设备绑定信息*/
    CMD_MASTER_SLAVE_CTRL = 187,/*主从之间的控制命令，不通过服务器*/
    CMD_REBOOT = 188,/*远程重启命令*/
    CMD_AREA_CONFIG = 189,/*区域操作命令*/    
    CMD_STATIC_PIC_Q = 190,/*最近静态图片请求*/
    CMD_STATIC_PIC_A = 191,/*最近静态图片响应*/
    CMD_POSITION_Q = 192, /*定位信息请求*/
    CMD_POSITION_A = 193, /*定位信息响应*/
    CMD_SPEED_MAX_Q = 194,/*限速阀值请求*/
    CMD_SPEED_MAX_A = 195,/*限速阀值响应*/
    CMD_SCHLBUS_BIND = 196,/*校车绑定*/
    CMD_VOICE = 197,
    CMD_VOICE_ACK = 198,
    CMD_SPEEK_Q = 199,
    CMD_SPEEK_A = 200,
    CMD_VOICE_REG = 201,
    CMD_REMOTE_CODE_UPLOAD = 202,/*上传本地设备学习到的控制编码到服务器*/
    CMD_RECORD_QUALITY_V2 = 203,/*录像质量配置*/
    CMD_NOTIFY_HELLO = 204, /* 设备与小区服务器握手请求 */
    CMD_NOTIFY_HELLO_ACK = 205, /* 设备与小区服务器握手响应 */
    CMD_NOTIFY = 206, /* 消息推送命令，如报警、健康、公告 */
    CMD_NOTIFY_RESULT = 207, /* 消息推送应答 */
    CMD_NOTIFY_EXPECT = 208, /* 消息ID 同步命令*/
    CMD_NOTIFY_CENTER_LIST = 209, /* 小区服务器把自己ip/port上报给服务器，服务器推送给设备*/
    CMD_CMT_OP_DEVICE = 210, /* 对小区所管理的设备进行操作，如添加。参数net_cmt_op_device_t*/
    CMD_VOICE_PROMPT  = 211, /* 播放本地音乐文件 */
    CMD_REMOTE_BD_BIND = 212,   /* 双向RF协议绑定解绑定 */
    CMD_REMOTE_CONFIG_SOUNDLIGHT = 213, /* 配置安防报警器关联的声光报警器 */
    CMD_REMOTE_TD_CODE = 214, /* 电器二维码添加 */
    CMD_PHONE_BIND_Q = 215, /*绑定手机申请提交，参数net_phone_bind_t*/
    CMD_PHONE_REQUESTLIST_Q =216, /*绑定手机申请列表查询*/
    CMD_PHONE_REQUESTLIST_A =217, /*绑定手机申请列表响应，参数net_phone_bind_list_t*/
    CMD_PHONE_BIND_OPERATION = 218, /*已绑定手机对绑定申请操作，参数net_phone_bind_operation_t*/
    CMD_PHONE_BIND_RESULT = 219, /*已绑定手机对绑定申请操作结果，参数net_phone_bind_result_t*/
    CMD_PHONE_BIND_DEL = 220, /*删除绑定手机，net_phone_bind_uuid_t*/
    CMD_PHONE_UNBINDLOGIN_ALLOW = 221, /*允许未绑定手机登录*/
    CMD_PHONE_UNBINDLOGIN_DENY = 222, /*禁止未绑定手机登录*/
    CMD_PHONE_BINDLIST_Q = 223, /*绑定手机列表查询请求*/
    CMD_PHONE_BINDLIST_A = 224, /*绑定手机列表查询响应，参数net_phone_bind_list_t*/
    CMD_SCENE_TIMER_Q  =  225,  /*配置情景模式的定时器请求命令*/
    CMD_SCENE_TIMER_A  =  226,  /*配置情景模式的定时器的应答命令*/
    CMD_SCENE_LINKAGE = 227,    /* 联动情景配置命令 */
    CMD_PHONE_APN_OPERATION = 228, /*配置手机推送命令，参数net_phone_push_t*/
    CMD_STATIC_PIC_Q_V2 = 229,/*第二版本最近静态图片请求*/
    CMD_STATIC_PIC_A_V2 = 230,/*第二版本最近静态图片响应*/
    CMD_REMOTE_VTY = 231,/* 启动远程telnetd服务 */
    CMD_IA = 232,       /* 智能家居单品控制和查询 */
    CMD_NEWUPVER_Q = 233,   /*手机获取设备最新升级版本请求*/
    CMD_NEWUPVER_A = 234,   /*手机获取设备最新升级版本应答*/
    CMD_SET_NEWUPVER = 235, /*手机设置设备升级版本*/
    CMD_NOTICE_DEVUP = 236, /*手机通知设备立即升级*/
    CMD_V4L2_COLOR = 237, /* v4l2 color参数查询和控制 */
    CMD_NOTIFY_QUERY = 238, /* 查询推送消息请求，参数net_notify_query_t，响应CMD_NOTIFY */
    CMD_MOTO_ATTRIBUTE = 239, /* IPC云台属性相关命令 */
    CMD_MOTO_PRE_POSITION = 240, /* IPC云台预置位命令 */
    CMD_MOTO_CRUISE = 241, /* IPC云台巡航命令 */
    CMD_UDP_AUTH = 242,
    CMD_UDP_KEEPLIVE = 243,
    CMD_UDP_CTRL = 244,
    CMD_UDP_NOTIFY = 245,
    CMD_UDP_BIND_PHONE = 246,
    CMD_RF2_CTRL = 247,
    CMD_CLOUD_MATCH = 248,  /* 客户端和设备进行云匹配交互的报文 */
    CMD_CLOUD_MATCH_RESULT = 249,   /* 客户端向服务器请求云匹配结果 */
    CMD_CLOUD_AC_LIB_INFO = 250,    /* 设备向服务器获取云空调编码摘要部分 */
    CMD_CLOUD_AC_LIB_DATA = 251,    /* 设备向服务器获取云空调编码数据部分 */
    CMD_RF_PRODUCTION_TEST = 252,   /* RF单向产测 */
    CMD_807_SYS_LED_CTRL = 253,     /* 807指示灯控制 */
    CMD_805_CONFIG = 254,       /*805相关操作*/
    CMD_UDP_MISC = 255, /*udp控制设备杂项命令*/
    CMD_UDP_LUA_REQ=260,      /** @brief   lua主发送的请求，特定转发到wuUdpServer处理*/ 

    CMD_UDP_ERROR = 300, /*错误命令*/
    CMD_MAX

};

#define net_header_size (sizeof(net_header_t))

#define set_net_proto_hd(is_compress, is_encrypt, type, cmd, paramlen, hd) \
do{\
    (hd)->ver = PROTO_VER1;\
    (hd)->hlen = (net_header_size>>2);\
    (hd)->compress = (is_compress);\
    (hd)->encrypt = (is_encrypt);\
    (hd)->ds_type = (type);\
    (hd)->command = htons(cmd);\
    (hd)->param_len = htonl(paramlen);\
}while(0)
#define IP_SHOW(ip) (((ip) >> 24)&0xFF), (((ip) >> 16)&0xFF), (((ip) >> 8)&0xFF), ((ip)&0xFF)
#define IP_FORMART "%u.%u.%u.%u"

typedef struct {
    // DISP_TLV_XXX
    u_int16_t type;
    // 数据长度，不包括头部
    u_int16_t len;
} tcp_tlv_t;


typedef struct{
#if __BYTE_ORDER == __LITTLE_ENDIAN
    u_int8_t    encrypt:1,
        compress:1,
        hlen:3,
        ver:3;
#elif __BYTE_ORDER == __BIG_ENDIAN
    u_int8_t ver:3,
        hlen:3,
        compress:1,
        encrypt:1;
#else
# error "Please fix <bits/endian.h>"
#endif
    u_int8_t ds_type;
    u_int16_t command;
    u_int32_t param_len;
    u_int32_t handle;
}net_header_t;


typedef struct{
    u_int64_t sn;
    u_int32_t devserver_id;
    u_int32_t devserver_ip;
    u_int16_t devserver_port;
    u_int8_t major_ver;
    u_int8_t minor_ver;
    u_int8_t is_udp;    /*是否走udp ctrl protocol*/
    u_int8_t reserverd2;
    tcp_tlv_t tlv[0];
}net_device_location_t;

typedef struct{
    u_int8_t username[MAX_NICKNAME];
    u_int8_t major_ver;
    u_int8_t minor_ver;
    u_int8_t client_id; /*CID_IPHONE*/
    u_int8_t isp; /*client isp, available between servers*/
}net_para_user_bind_q;




typedef struct{
#if __BYTE_ORDER == __BIG_ENDIAN
    u_int8_t ver:3,
        hlen:3,
        request:1,
        encrypt:1;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
    u_int8_t    encrypt:1,
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
} ucph_t;//16

typedef struct {
    u_int16_t objct;
    u_int16_t sub_objct;
    u_int16_t attr;
    u_int16_t param_len;
    // u_int8_t param[0];
} ucp_obj_t;//8

typedef struct {
    // UCA_XXX
    u_int8_t action;
    u_int8_t count;
    u_int16_t reserved;
    // ucp_obj_t objs[0];
} ucp_ctrl_t;//4//4


typedef struct uc_trans_hdr{
    u_int64_t sn;
    u_int32_t errorcode;
    u_int8_t  cfg_id;
    u_int8_t  pad[3];
}__attribute__((packed)) uc_trans_hdr_t;//16

#define ucph_hdr_len(ucph)  (((ucph_t *)ucph)->hlen<<2)
#define ucph_hdr_size (sizeof(ucph_t))

typedef struct{
    struct sockaddr_in addr;
    socklen_t addr_len;
    u_int32_t ip;
    u_int16_t port;
    u_int16_t resved;
    u_int32_t total;
    u_int32_t fragment_total_length;

    
    // 加密后不好知道命令，在这放一个便于调试
    int cmd;
    u_int8_t action;
    u_int8_t resved2;
    u_int16_t type;

    /** @brief  记录一些lua脚本推送的相关信息，用于在脚本推送成功后，生成对应sn的推送历史记录 */ 
    int is_lua_update_strategy;     /** @brief   标识当前是否是一个lua脚本推送策略的末尾报文*/ 
    u_int64_t sn;                    /** @brief   推送对应的sn*/ 
    long strategy_id;             /** @brief   推送的策略的id*/ 
    long strategy_version;         /** @brief   推送的策略的数据版本号*/ 
    /** @brief   *************************************************************************/ 

      /** @brief   是否是一个sn列表信息*/ 
    int is_reset_new_query;   /** @brief   当进行新的一天的查询时，发送一个空折报文到处理线程，使处理线程能够清空其缓存信息，执行全新的查询请求*/ 
    int is_sn_list;
    int sn_count;     /** @brief   sn总数，sn在buf中以8byte一个的数组排列*/ 
    u_int64_t *sn_offset_ptr;     /** @brief   可写入sn的buf偏移*/ 

    char buf[1024];
}ucs_in_pkt_t;

/* IA.statforshare社交分享排行 */
#define UCSOT_IA_SHARE 26

/** @brief   控制报文定义--end*/ 


typedef struct tmp_header_s{
    u_int8_t id;
    u_int8_t enable;
    u_int8_t week;
    u_int8_t begin_hour;
    u_int8_t end_hour;
    u_int8_t time_period;
    u_int8_t count;
    u_int8_t pad;
}tmp_header_t;

typedef  struct curve_point_s{
    u_int8_t  action;   //动作类型
    u_int8_t  tmp;  //温度
    u_int8_t  wind; //风量
    u_int8_t  dir; //风向
} tmp_curve_t;

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
    u_int8_t id;
    u_int8_t hour;
    u_int8_t minute;
    u_int8_t week;
    u_int8_t enable;
    u_int8_t onoff;
    u_int16_t duration;
} ucp_ac_extended_timer_item_t;


typedef struct {
    u_int8_t on_valid;
    u_int8_t on_day;
    u_int8_t on_hour;
    u_int8_t on_min;
    u_int16_t on_duration;

    u_int8_t off_valid;
    u_int8_t off_day;
    u_int8_t off_hour;
    u_int8_t off_min;
    u_int16_t off_duration;
} ucp_ac_extended_timer_t;

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
	u_int8_t timeout;
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


typedef struct userrepair_s{
	int8_t vl[4];
}userrepair_t;

enum {
UCT_IA_USERSET_ONOFFSAME      = 1, //开关同码
UCT_IA_USERSET_WINDDIRSAME    = 2,//扫风同码
UCT_IA_USERSET_WIND_REVERSE   = 3,//风力反了
UCT_IA_USERSET_ENVTEMP_REPAIR = 4,//修正环境温度因子
UCT_IA_USERSET_ELE_REPAIR     = 5,//修正电量因子
UCT_IA_USERSET_ELE_CLEAR      = 6,//清零电量
};

typedef struct {
    u_int32_t yesterday;
    u_int32_t today;
    u_int32_t sample[24];
}light_smp_t;


#define UC_GLOBAL_DEBUG   1

#define UC_REQUEST   (1<<6)
#define UC_ANSWER    (1<<7)

#define UC_DATA        "data"
#define UC_RETURN_CODE "return_code"
#define UC_HEX_CODE    "hex_code"

typedef struct {
    u_int32_t ver;
    u_int64_t sn;
    u_int32_t type;
    u_int32_t subtype;
    u_int8_t passwd[16];
    u_int32_t time;
    u_int8_t nickname[MAX_NICKNAME];
    u_int8_t email[MAX_EMAIL];
    u_int8_t phone[MAX_PHONE];
    u_int8_t service_date[MAX_DATE];
    u_int8_t service_type;
    u_int8_t ext_type;
    u_int32_t devserver_id;     /*devserver global id*/
    u_int32_t devserver_id_b; /*backup devserver global id*/
    u_int8_t async_passwd;
    u_int32_t home_id;
}dev_info_t;

typedef struct{
    dev_info_t old;
    u_int8_t vendor[UCKEY_VENDOR_LEN];
    u_int8_t preshare_key[UCKEY_PRESHAREKEY_LEN];
    u_int8_t uuid[MAX_UUID_BIN_LEN];
}uc_dev_info_t;

#pragma pack(pop)

typedef struct __get_dev_info_s_{
    u_int8_t is_online;     //是否在线，1在线，0不在线
    u_int8_t onoff;         //开/关 (0=开，1=关) 
    u_int8_t temp;          //温度 (16-30度  0=16 。。。。 14=30)
    u_int8_t mode;          //模式 (0=自动 ，1=制冷，2=除湿， 3=送风，  4=制热)
    u_int8_t direct;        //风向 (0=自动，1=风向1，2=风向2，3=风向3，4=风向4)
    u_int8_t wind;          //风速 (0=自动，1=风速1，2=风速2,3=风速3)
    u_int8_t key;           //空调key(0=开关，2=其他类型)	
    u_int8_t curtemp;       //当前温度
    u_int32_t curpower;    //当前功率
    uc_dev_info_t dev_info;
}get_dev_info_t;


#endif   /* @END  __UDP_PARSE_H__*/ 
