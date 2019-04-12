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


/* memcache��ѯ�ӿ���� */
#define MC_KEY_TYPE_DEVINFO     (3) /*mc_devinfo_t*/
#define MC_DEFAULT_PORT         (11211) /* Ĭ�ϵ�memcache�˿� */
#define MEMCACHED_MAX_KEY       251 /* We add one to have it null terminated */
#define PROTO_MAX               PROTO_VER2
#define MAX_PARAM_LEN           (1024*1024*64)
#define SN_LEN                  12    /*DS007�豸���к��ַ�������*/
#define MAX_NICKNAME            16  /*�û��ǳ���󳤶�*/
#define MAX_PLUG_TIMER_NAME     64    /* ��ʱ����������󳤶� */
#define MAX_EMAIL               32
#define MAX_PHONE_SINGLE        16
#define MAX_PHONE_NUM           10
#define MAX_PHONE               (MAX_PHONE_SINGLE * MAX_PHONE_NUM)
#define MAX_WEB_ROOT            64
#define MAX_HANDLE              0xFFFFFFFF
#define MAX_SERVER_HANDLE       0xEE6B2800 /*��֤�豸���������������handle�����ظ�*/
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
    // ���ݳ��ȣ�������ͷ��
    u_int16_t len;
} uc_tlv_t;

/******************debug info********************************/
#define DBG_TLV_HD_LEN      sizeof(uc_tlv_t)
//SVN��
#define DBG_TYPE_SVN        1
//����
#define DBG_TYPE_CUR        2
//������ӦADֵ
#define DBG_TYPE_CUR_AD     3
//����kֵ
#define DBG_TYPE_CUR_K      4
//����bֵ
#define DBG_TYPE_CUR_B      5
//��ѹ
#define DBG_TYPE_VOL        6
//��ѹ��ӦADֵ
#define DBG_TYPE_VOL_AD     7
//��ѹkֵ
#define DBG_TYPE_VOL_K      8
//��ѹbֵ
#define DBG_TYPE_VOL_B      9
//����ADֵ
#define DBG_TYPE_LIGHT_AD   10
//���ӷ���������
#define DBG_TYPE_SERVER_DONAME  11
//���ӷ�����ʱ��
#define DBG_TYPE_SERVER_CONNTIME 12
//��ǰ�������ֻ���������
#define DBG_TYPE_CLIENTS        13
//�豸uptime
#define DBG_TYPE_UPTIME         14
//���ѧϰ
#define DBG_TYPE_LIGHT_STUDY        15
//cpuʹ�ðٷֱ�
#define DGB_TYPE_CPU_PERCENTAGE     16
//memʹ�ðٷֱ�
#define DBG_TYPE_MEM_PERCENTAGE     17

//����ƽ������
#define DBG_TYPE_AVG_AD     18
//����������
#define DBG_TYPE_MAX_AD     19
//��������ͻ���ӳ�ʱ��
#define DBG_TYPE_PN_TIME   20
//�ػ�����ͻ��ʱ��
#define DBG_TYPE_PF_TIME   21
//���ص���adֵ
#define DBG_TYPE_NO_LOAD_AD 22
//��Ƭ������汾
#define DBG_TYPE_SMT_SOFT_VER  23
//��Ƭ��Ӳ���汾
#define DBG_TYPE_SMT_HARD_VER  24
//���������id
#define DBG_TYPE_IR_LIB_ID     25

#define DBG_TYPE_COLD_DELAY_PN_TIME 26
#define DBG_TYPE_COLD_DELAY_PF_TIME 27
#define DBG_TYPE_HOT_DELAY_PN_TIME  28
#define DBG_TYPE_HOT_DELAY_PF_TIME  29

//��Ƭ��������Ϣ
#define DGB_TYPE_STM32_INFO     30

//��Ƭ���ȴ�У��
#define DBG_TYPE_STM_WAIT_ADJUST    31
//dev server ip
#define DBG_TYPE_DEVSERVERIP        32
//����״̬��ѯ
#define DBG_TYPE_DEV_STATUS         33



//debug2
//��������
#define DBG_TLV_LIGHT           100
//��������
#define DBG_TLV_POWER           101
//�¶�����
#define DBG_TLV_TMP             102


/********** udp_ctrl.h header **********/


/*****************************************
    ���Ʊ��ĸ�ʽ (CMD_UDP_CTRL)
 *****************************************/

#define UCOT_GET_ALL_ATTRI (0xFFFF)

/*****************************************
    UDP Control Object Type 
    UDP Control Sub Object Type
    UDP Control Attribute Type
 ****************************************/
 
/*
    system. (ϵͳ)
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
#define UCAT_SYS_VERSION        100 /* ���ܱ�����Ӷ����õģ��Ӵ�һ��Ķ��� */
// system.software.upgrade
#define UCAT_SYS_UPGRADE        101 /*����ӳ��*/
#define UCAT_SYS_STM_UPGRADE    102 /*stm��Ƭ������*/


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
    IA(���ܵ���)
*/
// IA.
#define UCOT_IA 2

//IA.RFGW
//RF����
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
//RF����͸������ͷ
typedef struct _RF_TT_TLV_HEAD_ {
	u_int8_t type;
	u_int8_t len;
}RF_TT_TLV_HEAD;
//��������
enum _DOOR_CONTACT_CONTRL_ {
	SMTDLOCK_CMD_GET_SUMMARY	= 1,
	SMTDLOCK_CMD_PUSH_SUMMARY	= 2,
	SMTDLOCK_CMD_GET_ONOFFTIME 	= 33,
	SMTDLOCK_CMD_PUSH_ONOFFTIME = 34,
	SMTDLOCK_CMD_SET_DEFENSE	= 37,
	SMTDLOCK_CMD_SET_AUTO_DEFENSE = 45,
};
//��ȡ�豸״̬��Ϣ����
typedef struct _CMD_GET_SUMMARY_ {
	u_int8_t getindex;
	u_int8_t resv;
}CMD_GET_SUMMARY;
//����汾
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
//�豸״̬��־
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
//�豸״̬��Ϣ�ϱ�
typedef struct _CMD_PUSH_SUMMARY_ {
	u_int8_t 	hw_ver;
	CMD_SOFT_VER    soft_ver;
	u_int8_t	abc_battery;
	u_int8_t	resv;
 	CMD_STATUS_FLAG   flagbits;
}CMD_PUSH_SUMMARY;

//��ȡ�豸������ʱ���
typedef struct _CMD_GET_ONOFFTIME_ {
	u_int32_t timestamp;
}CMD_GET_ONOFFTIME;
//������״̬
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
//����ʱ����ϱ�
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

//��������
typedef struct _CMD_SET_DEFENSE_ {
	u_int8_t is_defense;
	u_int8_t resv;
}CMD_SET_DEFENSE;
//�Զ������볷��
typedef struct _CMD_SET_AUTO_DEFENSE_ {
	u_int8_t status;
	u_int8_t enable;
	u_int8_t starthour;
	u_int8_t endhour;
}CMD_SET_AUTO_DEFENSE;
//�Զ������벼���ϱ�
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
    UCAT_STAT_MONTH_PEAK            = 6,  /* IA.stat.month_peak �·��ͳ�� */
    UCAT_STAT_MONTH_VALLEY          = 7,  /* IA.stat.month_valley �¹ȵ�ͳ�� */
    UCAT_STAT_CUR_POWER             = 8,  /* IA.stat.cur_power */
    UCAT_STAT_PEAK_TIME             = 9,  /* IA.stat.peak_time ���ʱ�� */
    UCAT_STAT_PEAK_PRICE            = 10, /* IA.stat.peak_price */
    UCAT_STAT_VALLEY_PRICE          = 11, /* IA.stat.valley_price */
    UCAT_STAT_MONTH_FLAT            = 12, /* ��ƽ��ͳ�� */
    UCAT_STAT_FLAT_TIME             = 13, /* ƽ��ʱ��� */
    UCAT_STAT_FLAT_PRICE            = 14, /* ƽ��۸� */
    UCAT_STAT_VALLEY_TIME           = 15, /* IA.stat.valley_time �ȵ� */
    UCAT_STAT_TOTAL_ELE             = 16, /* �ܵ��� */
    UCAT_STAT_PHASE_ELE             = 17, /* �׶ε��� */
    UCAT_STAT_ON_ELE                = 18, /* ���������ĵ��� */
    UCAT_STAT_PUSH2SERV             = 20, /* IA.stat.push2serv ״̬�ϱ������� */
    UCAT_STAT_ELE_DAYBYDAY          = 21, /* ���һ��ÿ����õ��� */
    UCAT_STAT_CURR_POWER_MW         = 22, /* ��ǰ���ʣ���ȷ������ */
    UCAT_STAT_MAX
};


//��ҵ���״̬�ϱ�
#define UCT_STATE_ORI_ELE              1  //�ӷ���˻�ȡ��ʼ�����ۼ�ֵ�� ��λW.H��
#define UCT_STATE_PUSH_TIMER_FAST      2  //ʵʱ�ϱ����������ͷ�������һ�£���������������һ��
#define UCT_STATE_PUSH_TIMER_HISTORY   3  //��ʷ�ϱ����������ͷ�������һ�£���������������һ�£�      Ĭ����5����
#define UCT_STATE_PUSH_AC_RUN          4  //�յ�ʵʱ����״̬
#define UCT_STATE_PUSH_POWER_ONEDAY    5  //�յ��ϱ�������Ϣ
#define UCT_STATE_PUSH_AC_ALARM        6  //�յ�������Ϣ�ϱ�
//ͳһ�����·�
#define UCT_STATE_SET_PERSON_INDUCTION      20 //����̽���������
#define UCT_STATE_SET_AUTO_AJUST_TEMP       21 //���ܵ����¶�
#define UCT_STATE_SET_ACOPEN_CURRENT_CHECK  22 //�յ������������
#define UCT_STATE_SET_AC_CURRENT_DEFALUT    23 //�յ����������ֵ
#define UCT_STATE_SET_ACTEMP_RANGE          24 //�յ������¶ȷ�ֵ��Ĭ��16-30




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
    UCAT_AC_ATTRI                  = 10, /* ����ƥ���� */
    UCAT_AC_MATCH_INFO             = 11,
    UCAT_AC_AUTO_CTRL			   = 12, /*���ܺ���*/
    UCAT_AC_ON_USER_SET		       = 13, //�û����ÿ��ػ�״̬(���Ƿ����⣬ֻ���豸�յ�״̬)
    UCAT_AC_CUR_RH                 = 14, /* ʪ�ȣ���ǿ����ղ���������� */
    UCAT_AC_TMP_SAMPLE_CURVE       = 15,
    UCAT_AC_RH_SAMPLE_CURVE        = 16,
    UCAT_AC_LOCK                   = 17, /* ͯ������ */
    UCAT_AC_MAX
};


typedef enum IA_AC_MODE {
    IA_AC_MODE_AUTO         = 0, /* �Զ� */
    IA_AC_MODE_COOL         = 1, /* ���� */
    IA_AC_MODE_DEHUMIDIFY   = 2, /* ��ʪ */
    IA_AC_MODE_WIND         = 3, /* �ͷ� */
    IA_AC_MODE_HEATING      = 4, /* ���� */
} IA_AC_MODE_E;

typedef enum IA_AC_POWER {
    IA_AC_POWER_ON         = 0, /* ���� */
    IA_AC_POWER_OFF        = 1, /* �ػ� */
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
    // ������
    AC_LOCK_NONE = 0,
    // ��ֹ����ң��������
    AC_LOCK_ALL = 1,
    // ��ֹ�����ػ�����Ĳ���
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
    
    UCAT_CODE_RC_INFO   	= 16, // ң������Ϣ
    UCAT_CODE_RC_KEY    	= 17, // ң����������Ϣ
    UCAT_CODE_RC_LEARN  	= 18, // ң����ѧϰ
    UCAT_CODE_RC_CTRL   	= 19, // ң��������
    UCAT_CODE_RC_MATCH  	= 20, // ң����ƥ��
    UCAT_CODE_RC_MATCH_STATE = 21, // ң����ƥ��״̬��ѯ
    
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
    STORAGE(�洢)
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


/*�˽ṹ����ucp_code_process_request_t����*/
/**����ƥ��������*/
typedef struct{
    u_int8_t action;                /**Action = 0ȡ��ƥ��Action = 1��ƥ�䣬Action = 2ȫƥ��*/
    u_int8_t timeout;               /**��ʱʱ��(��ƥ���ã���λ��)����ʾ���ܺ����źų�ʱʱ��*/
    u_int8_t resv0;                 /**����*/
    u_int8_t resv1;                 /**����*/
}ucp_ia_matchcode_t;

/*�˽ṹ����ucp_code_process_reply_t����*/
/**ƥ����������*/
typedef struct{
    u_int8_t action;                /**Action = 0ȡ��ƥ��Action = 1��ƥ�䣬Action = 2ȫƥ��*/
    u_int8_t step_type;             /**Step Type = 0 ��ƥ���ã���ʾ������ƥ��ģʽ*/
    u_int8_t step_count;            /**Step Count���ܹ���Ҫ���еĲ���*/
    u_int8_t step_index;            /**Step index����ǰ���е��ڼ���*/
    u_int8_t error_num;             /**Error Number��0�޴���?�������*/
    u_int8_t resv0;                 /**����*/
    u_int16_t resv1;                /**����*/
}ucp_matchcode_result_t;

/*��ǿ���֪ͨ�豸�������ƥ��*/
typedef struct{
	u_int8_t	action;		/*Action = 0ȡ��ƥ��Action = 1��ƥ��*/
	u_int8_t	timeout;	/*��ʱʱ��(��ƥ���ã���λ��)����ʾ���ܺ����źų�ʱʱ��*/
	u_int16_t	id;         /*����ƥ��ʱ��0*/
	u_int8_t	type;		/*ң�ذ�����*/
	u_int8_t	devid;		/*ң�ذ�id*/
	u_int16_t	reserved; 	/*�����ֶ�*/
}ucp_ucat_code_match_s;

/*����ƥ����*/
typedef struct{
	u_int8_t	action;		/*Action = 7*/
	u_int8_t	timeout;	/*��ʱʱ��(��ƥ���ã���λ��)����ʾ���ܺ����źų�ʱʱ��*/
	u_int16_t	id;         /*����ƥ��ʱ��0*/
	u_int8_t	type;		/*ң�ذ�����*/
	u_int8_t	devid;		/*ң�ذ�id*/
	u_int16_t	reserved; 	/*�����ֶ�*/
	u_int8_t	skey;		/*����ƥ��İ���*/
	u_int8_t	reverved1;	/*�����ֶ�*/
	u_int16_t	reverved2;	/*�����ֶ�*/
}ucp_ucat_code_match_r;

/*�豸push��ice�ĺ���ƥ��״̬*/
typedef struct{
	u_int8_t	action;				  /*3 ƥ��ɹ� 7 ����ƥ��*/ 
    u_int8_t 	tep_type;             /**Step Type = 0 ��ƥ���ã���ʾ������ƥ��ģʽ*/
    u_int8_t 	tep_count;            /**Step Count���ܹ���Ҫ���еĲ���*/
    u_int8_t 	tep_index;            /**Step index����ǰ���е��ڼ���*/
    u_int8_t 	error_num;            /**Error Number��0�޴���?�������*/
	u_int8_t	flagbits;		
	u_int16_t	reverved;
	u_int8_t	skey;				 /*�������ƥ��İ���*/
	u_int8_t	reverved1;
	u_int16_t	reverved2;
}ucp_ucat_code_match_push;


/*ң������ѯ�ӿ�*/
typedef struct{
	u_int8_t count;				/*ң��������*/
	u_int8_t length;			/*����ң������Ϣ�ĳ���*/
	u_int16_t reserved;         /*�����ֶ�*/
}ucp_ucat_code_g;

/*ÿ��ң��������ϸ��Ϣ*/
typedef struct{
	u_int8_t	type;			/*ң��������*/
	u_int8_t	devid;			/*ң����id*/
	u_int16_t	irid;			/*��������id*/
	u_int8_t	flags;			/*��־λ*/
	u_int8_t	reverted0;      /*����λ*/
	u_int16_t	reverted1;		/*����λ*/
	u_int8_t    name[16];		/*ң��������*/
}ucp_ucat_code_info_g;

/*�Զ���ң�����������ú�ɾ��*/
typedef struct {
	u_int8_t	type;
	u_int8_t	key;
	u_int16_t	irid;
	u_int8_t	devid;
	u_int8_t	flags;
	u_int16_t	reserved;
	u_int8_t    name[16];	
}ucp_ucat_key_c;

/*ң������ѯ*/
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

/*�̶�������������*/
typedef struct {
	u_int8_t	key;
	u_int8_t	flags;
	u_int16_t	reverved;
}ucp_ucat_fix_key_g;

/*�Զ��尴����������*/
typedef struct {
	u_int8_t	key;
	u_int8_t	flags;
	u_int16_t	reverved;
	u_int8_t	name[16];
}ucp_ucat_def_key_g;

/*����ѧϰ*/
typedef struct {
	u_int8_t	type;
	u_int8_t 	key;
	u_int8_t	devid;
	u_int8_t	action;
	u_int32_t	reserved;
}ucp_ucat_key_learn;
	
/*��������*/
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
*�������
*/
//dev_product
#define UCOT_DEVPRO             5
//devproduct.ir ���������Ӷ���������
#define UCSOT_DEVPRO_IR         1
//devpro.ir.val_kb ��ѹ getʱad/ad2,setʱk/ad/ad2ֵ
#define UCAT_IR_VALKADKB        2
//devpro.ir.cur_kb ���� getʱad,setʱb/k
#define UCAT_IR_CURKB           3
//devpro.ir.adjust
#define UCAT_IR_ADJUST          4
//���2.0����оƬHLW8012
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

/** @brief   ���Ʊ��Ķ���*/ 
/* proto version control */
enum {
    PROTO_VER1 = 1,
    PROTO_VER2 = 2,
    /* further version add here */
};

/*role type of DS007 system*/
enum {
    TP_CENTER = 0,      /*��������*/
    TP_DISPATCHER = 1,  /*���������*/
    TP_DEV_SRV = 2,     /*�豸������*/
    TP_WEB_SRV = 3,     /*web������*/
    TP_USER = 4,        /*�ֻ����������û�*/
    TP_DS007 = 5,       /* DS007�豸*/
    TP_CHARGE_SRV = 6,      /* ��ֵ������ */
    TP_CHARGE_CLI_RO = 7,   /* ��ֵ�ͻ��ˣ�ֻ�� */
    TP_CHARGE_CLI_WR = 8,   /* ��ֵ�ͻ��ˣ���д  */
    TP_MAX
};
/*ͨ��Э������
��ֹ�����������м����������
ֻ��˳����ӵ�����*/
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
    CMD_YW_SERVER_Q = 38, /*��ά��ѯ����������*/
    CMD_KEEP_DATA = 39,
    CMD_NICKNAME_CONFIRM = 40,  /*�޸��ǳƳɹ�ȷ������������ڲ�ʹ��*/
    CMD_NICKNAME_FAIL = 41,     /*�޸��ǳ�ʧ������������ڲ�ʹ��*/
    CMD_LOAD_USER_INFO = 42,    /*�����û�������Ϣ*/
    CMD_UDP_KEEP_DATA = 43,
    CMD_UDP_DEV_STAT = 44,
    CMD_SERVICE_DATE_Q = 45,    /*�����ײͲ�ѯ����*/
    CMD_SERVICE_DATE_A = 46,    /*�����ײͲ�ѯ��Ӧ*/
#if 0   
    CMD_SERVICE_CHARGE = 47,    /* �����ײ͸���*/
#endif  
    CMD_URL_HIT_Q = 48,         /*��ѯURL ��������*/
    CMD_URL_HIT_A = 49,         /*��ѯURL ������Ӧ*/
    CMD_IPLOCATION_Q    = 50,   /*��ѯ�ͻ���IP��ַ��Χ*/
    CMD_IPLOCATION_A = 51,  /*dispatcher ��ѯ�ͻ���IP��ַ��Χ*/
    CMD_SELLER_LOGIN_Q  = 52,   /*������Ա��¼��֤����*/
    CMD_SELLER_LOGIN_A  = 53,   /*������Ա��¼��֤Ӧ��*/
    CMD_SELLER_USER_Q   = 54,   /*�û��˺Ų�ѯ����*/
    CMD_SELLER_USER_A   = 55,   /*�û��˺Ų�ѯӦ��*/
    CMD_SELLER_USER_ADD = 56,   /*����˺�*/
    CMD_SELLER_USER_MODIFY = 57,/*�޸��˺�*/
    CMD_SELLER_USER_DEL = 58,   /*ɾ���˺�*/
    CMD_VEDIO_Q = 59, /* ����鿴����ͷ��Ƶ */
    CMD_VEDIO_A = 60, /* ����ͷ��Ƶ��ϢӦ�� */
    CMD_SELLER_PWD = 61,    /*�޸��˺�����*/
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
    CMD_UDP_YW_DISPATCH_Q = 78,//��ά--�ڷ����������-���豸����������-ȡ���豸����������ϸ�豸�б�
    CMD_LOAD_PACKAGE_LOG_Q = 79,//��ѯ��ֵ��־
    CMD_LOAD_PACKAGE_LOG_A = 80,//��ѯ��ֵ��־
    CMD_UDP_YW_DISPATCH_A = 81,////��ά--�ڷ����������-���豸����������
    CMD_ADD_RC_CARD_Q = 83,  /*��ӳ�ֵ������*/
    CMD_ADD_RC_CARD_A = 84,  /*��ӳ�ֵ����Ӧ*/
    #if 0
    CMD_MOD_RC_CARD_Q = 85 , /*���³�ֵ��״̬����*/
    CMD_MOD_RC_CARD_A = 86,  /*���³�ֵ��״̬��Ӧ*/
    CMD_DEL_RC_CARD_Q = 87,  /*ɾ����ֵ������*/
    CMD_DEL_RC_CARD_A = 88, /*ɾ����ֵ����Ӧ*/
    #endif
    CMD_TRANS_RC_CARD_Q = 89, /*ת�Ƴ�ֵ������*/
    CMD_TRANS_RC_CARD_A = 90, /*ת�Ƴ�ֵ����Ӧ*/
    CMD_QUERY_RC_CARD_Q = 91, /*��ѯ��ֵ��״̬����*/
    CMD_QUERY_RC_CARD_A = 92, /*��ѯ��ֵ��״̬��Ӧ*/
    CMD_CARD_CHARGE_Q = 93, /*��ֵ����*/
    CMD_CARD_CHARGE_A = 94, /*��ֵ��Ӧ*/
    CMD_DEV_CHARGE = 95,  /*��ֵ�ɹ�ͬ��֪ͨ����*/
    CMD_YW_SERVER_A = 96, /*��ά��ѯ��������Ӧ*/
    CMD_VEDIO_AGENT_OK = 97,/*����ɹ�*/
    CMD_ADD_CARD_TYPE_Q = 98,/*��ӿ���������*/
    CMD_ADD_CARD_TYPE_A = 99,/*��ӿ�������Ӧ*/
    CMD_QUERY_CARD_TYPE_Q = 100,/*��ѯ����������*/
    CMD_QUERY_CARD_TYPE_A = 101,/*��ѯ��������Ӧ*/
    CMD_QUERY_USER_PWD_Q = 102,/*ȡ�õ�ǰ��¼DEV����*/
    #if 0
    CMD_CARD_CHARGE_SYN_Q = 103,/*��ֵ��Ϣͬ������*/
    CMD_CARD_CHARGE_SYN_A = 104,/*��ֵ��Ϣͬ��Ӧ��*/
    CMD_CARD_TYPE_SYN_Q = 105,/*��ֵ������ͬ������*/
    CMD_CARD_TYPE_SYN_A = 106,/*��ֵ������ͬ��Ӧ��*/
    CMD_CARD_STATUS_SYN_Q = 107,/*��ֵ��״̬ͬ������*/
    CMD_CARD_STATUS_SYN_A = 108,/*��ֵ��״̬ͬ��Ӧ��*/
    CMD_DEV_CHARGE_CONFIRM = 109,/*��ֵ�ɹ�ͬ��֪ͨȷ������*/
    #endif
    CMD_YW_DEVSERVER_Q = 110,/*��ά����--�豸��������ѯ*/
    CMD_YW_DEVSERVER_A = 111,/*��ά����--�豸��������ѯ*/
    CMD_BIND = 112, /* �����豸���� */
    CMD_SET_NAME = 113, /* �����豸���� */
    CMD_MS_CTRL = 114,  /* �����豸�������������Ҫ���� */
    CMD_VIDEO_ROLL = 115, /* ������̨ת�� */
    CMD_FM_CONFIG_Q = 116,  /*���ü�ͥ������Ա����*/
    CMD_FM_CONFIG_A = 117,  /*���ü�ͥ������Ա��Ӧ*/
    CMD_FM_Q = 118,  /*��ѯ��ͥ������Ա����*/
    CMD_FM_A = 119,  /*��ѯ��ͥ������Ա��Ӧ*/
    CMD_MESURE_TRANS = 120, /*����������*/
    CMD_MESURE_Q = 121,      /*��ѯ�����������*/
    CMD_MESURE_A = 122,      /*��ѯ���������Ӧ*/
    CMD_MESURE_DEL = 123,   /*ɾ���������*/
    CMD_MESURE_TRANS_CONFIRM = 124, /*�豸��������ͬ����������ȷ��*/
    CMD_PLUG_TIMER_Q = 125, /* ��ʱ���ص����� */
    CMD_PLUG_TIMER_A = 126, /* ��ʱ���صĻ�Ӧ */
    CMD_USER_DEBUG = 127,     /*�ֻ��ϴ�������Ϣ*/
    CMD_IR_LIST = 128,      /*�ֻ�����֧���б�*/
    CMD_IR_CONFIG = 129,    /*�ֻ����úͻ�ȡ�����üҵ�*/
    CMD_IR_DB = 130,        /*�豸�ӷ�������ȡ�����*/
    CMD_IR_URL = 131,       /*�豸��ȡ����ң���б��URL */
    CMD_IR_CTRL= 132,       /* �ֻ����Ժ�������������Ч */
    CMD_REMOTE_CONFIG = 133, /* CMD_REMOTE_CONFIG */
    CMD_REMOTE_KEY_CONFIG = 134, /* �ֻ�Ϊ��ͥ��������һ������ */
    CMD_REMOTE_CODE = 135, /* �ֻ�Ϊ��������һ������ */
    CMD_REMOTE_CTRL= 136, /* �ֻ����ͥ�������Ϳ������� */
    CMD_REMOTE_STATE = 137, /* �ֻ��Ե�������״̬�Ĳ��� */
    CMD_VIDEO_TEMP_QUALITY = 138, /* ������̨����ʱ����ʱ���Ի��ʣ����������� */
    CMD_VIDEO_RECOR_CTRL = 139,/*�豸�˱���¼������------�Ѿ�ȡ���˱���*/
    CMD_ALARM_BIND_PHONE = 140, /*�������Ű��ֻ�*/
    CMD_ALARM_MSG = 141,    /*������Ϣ��������*/
    CMD_FAIL_EX = 142,      /* cmd_fail��չ���� */
    CMD_ALARM_CONFIG_PHONE = 143,
    CMD_SLAVE_HIS = 144, /*���豸ͨ��������Ϣ�����豸*/
    CMD_GET_CMDOK = 145,    /* �豸�����������ѯ�ֻ���֤�ɹ�����Ҫ�������Ϣ */
    CMD_ALARM_LOG = 146,/*������־��ز���*/
    CMD_ALARM_LOG_SYNC = 147,/*������־��ͬ��*/
    CMD_TIME_SYNC = 148,/*�豸���ֻ���ѯʱ��ͬ��*/
    CMD_VTAP_LIST_Q = 149,/*��ѯ�豸¼���б�*/
    CMD_VTAP_LIST_A = 150,/*��Ӧ�豸¼���б�Ĳ�ѯ*/
    CMD_VTAP_Q = 151,/*�����豸¼���ļ�*/
    CMD_VTAP_KEEP = 152,/*¼��ۿ�����*/
    CMD_VTAP_A = 153,/*¼��������*/
    CMD_VTAP_TIMESTAP = 154, /*���󲥷�ָ��ʱ��*/

    CMD_REMOTE_BROADCAST = 155, /* �㲥������������豸 */
    CMD_VIDEO_SONIX = 156, /*��sonix ����*/
    CMD_REC_TIMER_Q = 157,/* ��Ƶ¼�ƶ�ʱ���ص����� */
    CMD_REC_TIMER_A  = 158,/* ��Ƶ¼�ƶ�ʱ���صĻ�Ӧ */
    CMD_VIDEO_CONTROL_ALARM_CFG = 159, /*��ⱨ��������Ϣ*/
    CMD_MS_DEV_INFO = 160, /*�����豸ͨ�Ÿ�֪�໥�İ汾��ʱ����֧�ֵĹ��� */

    CMD_DEV_SAHRD_INFO = 161, /*�豸���������������Ӧ��ͬ*/
    CMD_DEV_SAHRD_COUNT = 162, /*�豸���������������*/
    CMD_OPEN_TELNET = 163, /*�������豸telnet����*/
    CMD_ALARM_SWITCH_CTL = 164, /*���������ܿ��ؿ���*/
    CMD_VTAP_END = 165,/*�豸֪ͨ�ͻ��ˣ�¼�񲥷����*/
    CMD_VIDEO_QUALITY_V2 = 166,/* �Զ���ֱ������� */
    CMD_PLUG_ELECTRIC_STAT = 167,/*����ͳ�ƹ���֧�ֲ�ѯ����*/
    CMD_SCENE_CONFIG = 168,/*�龰ģʽ��������*/
    CMD_OPT_SCAN_Q = 169, /*ɨ��ˢ���豸����*/
    CMD_OPT_SCAN_A = 170, /*ɨ��ˢ���豸��Ӧ*/
    CMD_DEV_REG_Q = 171,  /*ˢ���豸ע������*/
    CMD_DEV_REG_A = 172, /*ˢ���豸ע����Ӧ*/
    CMD_ARIA2C_Q = 173, /*aria2c��������*/
    CMD_ARIA2C_A = 174, /*aria2c������Ӧ*/
    CMD_NETWORK_DETECT_Q = 175, /* ��Ƶ����̽������� */
    CMD_NETWORK_DETECT_A = 176, /* ��Ƶ����̽����Ӧ�� */
    CMD_SSIDPW = 177, /*�ֻ��޸��豸wifi SSID������*/
    CMD_DEVSERVER_LIST_Q = 178, /*��ȡ�豸�������б�����*/
    CMD_DEVSERVER_LIST_A = 179, /*��ȡ�豸�������б���Ӧ*/
    CMD_NET_PROBE_Q = 180,  /*�豸������������̽������*/
    CMD_NET_PROBE_A = 181,  /*�豸������������̽����Ӧ*/
    CMD_LOCATION_Q = 182,  /*�豸λ������*/
    CMD_LOCATION_A = 183,  /*�豸λ����Ӧ*/
    CMD_MACDENY = 184, /*���wifi mac��ֹ�б�*/
    CMD_MACALLOW = 185, /*ɾ��wifi mac��ֹ�б�*/
    CMD_BIND_SLAVE_INFO = 186,/*��ȡ���豸����Ϣ*/
    CMD_MASTER_SLAVE_CTRL = 187,/*����֮��Ŀ��������ͨ��������*/
    CMD_REBOOT = 188,/*Զ����������*/
    CMD_AREA_CONFIG = 189,/*�����������*/    
    CMD_STATIC_PIC_Q = 190,/*�����̬ͼƬ����*/
    CMD_STATIC_PIC_A = 191,/*�����̬ͼƬ��Ӧ*/
    CMD_POSITION_Q = 192, /*��λ��Ϣ����*/
    CMD_POSITION_A = 193, /*��λ��Ϣ��Ӧ*/
    CMD_SPEED_MAX_Q = 194,/*���ٷ�ֵ����*/
    CMD_SPEED_MAX_A = 195,/*���ٷ�ֵ��Ӧ*/
    CMD_SCHLBUS_BIND = 196,/*У����*/
    CMD_VOICE = 197,
    CMD_VOICE_ACK = 198,
    CMD_SPEEK_Q = 199,
    CMD_SPEEK_A = 200,
    CMD_VOICE_REG = 201,
    CMD_REMOTE_CODE_UPLOAD = 202,/*�ϴ������豸ѧϰ���Ŀ��Ʊ��뵽������*/
    CMD_RECORD_QUALITY_V2 = 203,/*¼����������*/
    CMD_NOTIFY_HELLO = 204, /* �豸��С���������������� */
    CMD_NOTIFY_HELLO_ACK = 205, /* �豸��С��������������Ӧ */
    CMD_NOTIFY = 206, /* ��Ϣ��������籨�������������� */
    CMD_NOTIFY_RESULT = 207, /* ��Ϣ����Ӧ�� */
    CMD_NOTIFY_EXPECT = 208, /* ��ϢID ͬ������*/
    CMD_NOTIFY_CENTER_LIST = 209, /* С�����������Լ�ip/port�ϱ��������������������͸��豸*/
    CMD_CMT_OP_DEVICE = 210, /* ��С����������豸���в���������ӡ�����net_cmt_op_device_t*/
    CMD_VOICE_PROMPT  = 211, /* ���ű��������ļ� */
    CMD_REMOTE_BD_BIND = 212,   /* ˫��RFЭ��󶨽�� */
    CMD_REMOTE_CONFIG_SOUNDLIGHT = 213, /* ���ð������������������ⱨ���� */
    CMD_REMOTE_TD_CODE = 214, /* ������ά����� */
    CMD_PHONE_BIND_Q = 215, /*���ֻ������ύ������net_phone_bind_t*/
    CMD_PHONE_REQUESTLIST_Q =216, /*���ֻ������б��ѯ*/
    CMD_PHONE_REQUESTLIST_A =217, /*���ֻ������б���Ӧ������net_phone_bind_list_t*/
    CMD_PHONE_BIND_OPERATION = 218, /*�Ѱ��ֻ��԰��������������net_phone_bind_operation_t*/
    CMD_PHONE_BIND_RESULT = 219, /*�Ѱ��ֻ��԰�����������������net_phone_bind_result_t*/
    CMD_PHONE_BIND_DEL = 220, /*ɾ�����ֻ���net_phone_bind_uuid_t*/
    CMD_PHONE_UNBINDLOGIN_ALLOW = 221, /*����δ���ֻ���¼*/
    CMD_PHONE_UNBINDLOGIN_DENY = 222, /*��ֹδ���ֻ���¼*/
    CMD_PHONE_BINDLIST_Q = 223, /*���ֻ��б��ѯ����*/
    CMD_PHONE_BINDLIST_A = 224, /*���ֻ��б��ѯ��Ӧ������net_phone_bind_list_t*/
    CMD_SCENE_TIMER_Q  =  225,  /*�����龰ģʽ�Ķ�ʱ����������*/
    CMD_SCENE_TIMER_A  =  226,  /*�����龰ģʽ�Ķ�ʱ����Ӧ������*/
    CMD_SCENE_LINKAGE = 227,    /* �����龰�������� */
    CMD_PHONE_APN_OPERATION = 228, /*�����ֻ������������net_phone_push_t*/
    CMD_STATIC_PIC_Q_V2 = 229,/*�ڶ��汾�����̬ͼƬ����*/
    CMD_STATIC_PIC_A_V2 = 230,/*�ڶ��汾�����̬ͼƬ��Ӧ*/
    CMD_REMOTE_VTY = 231,/* ����Զ��telnetd���� */
    CMD_IA = 232,       /* ���ܼҾӵ�Ʒ���ƺͲ�ѯ */
    CMD_NEWUPVER_Q = 233,   /*�ֻ���ȡ�豸���������汾����*/
    CMD_NEWUPVER_A = 234,   /*�ֻ���ȡ�豸���������汾Ӧ��*/
    CMD_SET_NEWUPVER = 235, /*�ֻ������豸�����汾*/
    CMD_NOTICE_DEVUP = 236, /*�ֻ�֪ͨ�豸��������*/
    CMD_V4L2_COLOR = 237, /* v4l2 color������ѯ�Ϳ��� */
    CMD_NOTIFY_QUERY = 238, /* ��ѯ������Ϣ���󣬲���net_notify_query_t����ӦCMD_NOTIFY */
    CMD_MOTO_ATTRIBUTE = 239, /* IPC��̨����������� */
    CMD_MOTO_PRE_POSITION = 240, /* IPC��̨Ԥ��λ���� */
    CMD_MOTO_CRUISE = 241, /* IPC��̨Ѳ������ */
    CMD_UDP_AUTH = 242,
    CMD_UDP_KEEPLIVE = 243,
    CMD_UDP_CTRL = 244,
    CMD_UDP_NOTIFY = 245,
    CMD_UDP_BIND_PHONE = 246,
    CMD_RF2_CTRL = 247,
    CMD_CLOUD_MATCH = 248,  /* �ͻ��˺��豸������ƥ�佻���ı��� */
    CMD_CLOUD_MATCH_RESULT = 249,   /* �ͻ����������������ƥ���� */
    CMD_CLOUD_AC_LIB_INFO = 250,    /* �豸���������ȡ�ƿյ�����ժҪ���� */
    CMD_CLOUD_AC_LIB_DATA = 251,    /* �豸���������ȡ�ƿյ��������ݲ��� */
    CMD_RF_PRODUCTION_TEST = 252,   /* RF������� */
    CMD_807_SYS_LED_CTRL = 253,     /* 807ָʾ�ƿ��� */
    CMD_805_CONFIG = 254,       /*805��ز���*/
    CMD_UDP_MISC = 255, /*udp�����豸��������*/
    CMD_UDP_LUA_REQ=260,      /** @brief   lua�����͵������ض�ת����wuUdpServer����*/ 

    CMD_UDP_ERROR = 300, /*��������*/
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
    // ���ݳ��ȣ�������ͷ��
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
    u_int8_t is_udp;    /*�Ƿ���udp ctrl protocol*/
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

    // ���濪ʼ���ܼ���
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

    
    // ���ܺ󲻺�֪����������һ�����ڵ���
    int cmd;
    u_int8_t action;
    u_int8_t resved2;
    u_int16_t type;

    /** @brief  ��¼һЩlua�ű����͵������Ϣ�������ڽű����ͳɹ������ɶ�Ӧsn��������ʷ��¼ */ 
    int is_lua_update_strategy;     /** @brief   ��ʶ��ǰ�Ƿ���һ��lua�ű����Ͳ��Ե�ĩβ����*/ 
    u_int64_t sn;                    /** @brief   ���Ͷ�Ӧ��sn*/ 
    long strategy_id;             /** @brief   ���͵Ĳ��Ե�id*/ 
    long strategy_version;         /** @brief   ���͵Ĳ��Ե����ݰ汾��*/ 
    /** @brief   *************************************************************************/ 

      /** @brief   �Ƿ���һ��sn�б���Ϣ*/ 
    int is_reset_new_query;   /** @brief   �������µ�һ��Ĳ�ѯʱ������һ�����۱��ĵ������̣߳�ʹ�����߳��ܹ�����仺����Ϣ��ִ��ȫ�µĲ�ѯ����*/ 
    int is_sn_list;
    int sn_count;     /** @brief   sn������sn��buf����8byteһ������������*/ 
    u_int64_t *sn_offset_ptr;     /** @brief   ��д��sn��bufƫ��*/ 

    char buf[1024];
}ucs_in_pkt_t;

/* IA.statforshare�罻�������� */
#define UCSOT_IA_SHARE 26

/** @brief   ���Ʊ��Ķ���--end*/ 


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
    u_int8_t  action;   //��������
    u_int8_t  tmp;  //�¶�
    u_int8_t  wind; //����
    u_int8_t  dir; //����
} tmp_curve_t;

typedef struct {
    // AC_POWER_xxx
    u_int8_t onoff;
    // AC_MODE_XXX
    u_int8_t mode;
    // ʵ���¶� �� temp - AC_TEMP_BASE
    u_int8_t temp;
    // ���٣�AC_WIND_xxx
    u_int8_t wind;
    // ����AC_DIR_xxx
    u_int8_t direct;
    // ��ֵ��AC_KEY_xxx
    u_int8_t key;
    // ��ʽ�յ���ǰ����, AC_CKEY_xxx
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
    // ��һ�ζ�ʱ����ʱ�������ڻ��м�����
    u_int16_t on_minute;

    u_int8_t off_effect;
    u_int8_t reserved2;
    // ��һ�ζ�ʱ�ػ�ʱ�������ڻ��м�����
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
	//��ƥ��ȴ������źų�ʱ
	ERR_CODE_PROCESS_WAIT_IR_TIMEOUT = 1,
	//����ƥ��
	ERR_CODE_PROCESS_MATCHING = 2,
	//���ƶ˻�ȡcode��ʱ
	ERR_CODE_PROCESS_GET_CODE_TIMEOUT = 3,
	//û��ƥ�䵽����
	ERR_CODE_PROCESS_NO_CODE_MATCH = 4,
	// ������û�������ϣ�������ͨ
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
UCT_IA_USERSET_ONOFFSAME      = 1, //����ͬ��
UCT_IA_USERSET_WINDDIRSAME    = 2,//ɨ��ͬ��
UCT_IA_USERSET_WIND_REVERSE   = 3,//��������
UCT_IA_USERSET_ENVTEMP_REPAIR = 4,//���������¶�����
UCT_IA_USERSET_ELE_REPAIR     = 5,//������������
UCT_IA_USERSET_ELE_CLEAR      = 6,//�������
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
    u_int8_t is_online;     //�Ƿ����ߣ�1���ߣ�0������
    u_int8_t onoff;         //��/�� (0=����1=��) 
    u_int8_t temp;          //�¶� (16-30��  0=16 �������� 14=30)
    u_int8_t mode;          //ģʽ (0=�Զ� ��1=���䣬2=��ʪ�� 3=�ͷ磬  4=����)
    u_int8_t direct;        //���� (0=�Զ���1=����1��2=����2��3=����3��4=����4)
    u_int8_t wind;          //���� (0=�Զ���1=����1��2=����2,3=����3)
    u_int8_t key;           //�յ�key(0=���أ�2=��������)	
    u_int8_t curtemp;       //��ǰ�¶�
    u_int32_t curpower;    //��ǰ����
    uc_dev_info_t dev_info;
}get_dev_info_t;


#endif   /* @END  __UDP_PARSE_H__*/ 
