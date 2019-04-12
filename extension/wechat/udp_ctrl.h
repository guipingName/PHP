#ifndef		__UDP_CTRL_H__
#define	__UDP_CTRL_H__

#include "lib.h"


#define	EMPTY_FUNC \
	log_err(false, "\n\n\nEEEEEEEE please write function %s \n\n\n", __FUNCTION__)

/*****************************************
	UDP����Э�鱨��ͷ(CMD_UDP_XXX)
 *****************************************/

// ���غ�
#define	MAX_UC_PKT_SIZE	1500
// UDP CTRL ͨ�ñ��ĵ�TTL���ݴ�С:object + sub object + attri + param_len
#define UDP_CTRL_TL_SIZE 8
// UDP CTRL ͨ�ñ��ĵ�TTL���ݴ�С:object + sub object + attri
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

	// ���濪ʼ���ܼ���
	u_int16_t command;
	u_int16_t param_len;
} ucph_t;


#define	ucph_hdr_len(ucph)	(((ucph_t *)ucph)->hlen<<2)
#define ucph_hdr_size (sizeof(ucph_t))
#define	get_ucp_payload(pkt, type) (type *)BOFP((pkt->data), ucph_hdr_len((pkt->data)))
#define	get_net_ucp_payload(hdr, type) (type *)BOFP(hdr, ucph_hdr_len(hdr))

// ͷ���������ĵĳ���
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
	�ỰID�ķ���
*/
#define	UC_SID_INVALID	0
// 0 - 0x10000000 �������豸�����������������ֱ�����ֻ�
#define	UC_SID_DEV_RESV_BEGIN	0x00000001
#define	UC_SID_DEV_RESV_END	0x0FFFFFFF

// ������������������豸
#define	UC_SID_SERV_DEV_BEGIN_1	0x80000000
#define	UC_SID_SERV_DEV_END_1	0x8FFFFFFF

#define	UC_SID_SERV_DEV_BEGIN_2	0x90000000
#define	UC_SID_SERV_DEV_END_2	0x9FFFFFFF


// ��������������������������ӵ��ֻ�
#define	UC_SID_SERV_CLIENT_BEGIN	0xA0000000
#define	UC_SID_SERV_CLIENT_END	0xAFFFFFFF


// δ��֤�ɹ���������״̬������ʱ��
#define	DFL_TIME_UCS_HALF_DIE	5
// ɾ�����ӵ���ʱ
#define	DFL_TIME_UCS_DEL_DELAY	3

// �ͻ�����IDLE״̬�ȴ�������󣬽�����һ�γ���
#define	DFL_UC_IDLE_TIME	3
//���Ϊ��¡��ʱ��ʱ���ӳ�
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
	״̬
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
	// ���뱾״̬���õĺ���
	ucs_func_t on_into;
	// ������
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
	// ���뱾״̬���õĺ���
	ucc_func_t on_into;
	// �뿪��״̬���õĺ���
	ucc_func_t on_out;
	// ������
	ucc_func_t proc_pkt;
} ucc_proc_t;


/*
	�������ݵķ��������ؼ��ܺ�ĳ���
*/
typedef int (*ucs_enc_func_t)(void *session, u_int8_t *data, int len);
/*
	�������ݵķ��������ؽ��ܺ�ĳ���
*/
typedef int (*ucs_dec_func_t)(void *session, u_int8_t *data, int len);


/*****************************************
	��֤���ĸ�ʽ (CMD_UDP_AUTH)
 *****************************************/


#define MAX_UUID_BIN_LEN 16

// type define for TLV
#define	UCT_VENDOR			1
#define	UCT_LOCAL_TIME		2
#define	UCT_TIME_PARAM		3
#define	UCT_E_TM_PRICE		4
#define UCT_GATHER_SOUND    5
#define UCT_CURRENT_DETECT   6
#define UCT_CHIP_ATTISTR_SEV 	7 //͸��ģʽ�£���ȡ�豸�����ַ�����ͨ���������
#define UCT_DEV_INFO			8
#define UCT_CHIP_TMP		 	9
#define UCT_IA_DELTA        10
#define UCT_IA_CONTROL_BOARD_TYPE	12	// ��ǰ�������
#define UCT_CHIP_ATTISTR_PHONE 	6 //͸��ģʽ�£���ȡ�豸�����ַ�����ͨ����ֻ���������ͳһ��7��Ϊ�˼����ֻ��˴��룬����ʹ��6
#define UCT_PRODUCT_TYPE      11 //͸��ģʽ��,�豸�����������࣬ů�����
#define UCT_SMARTCFG_TIME	  14  //һ�����óɹ���ʱ��
#define UCT_CLONE_PKT_SEND_ENABLE 15 //�������·����Ƿ��ϱ���¡����
#define UCT_UCDS_TIMEZONE_PUT		16//�ظ�ʱ�����ͻ���
#define UCT_DEVFUN_BITS		17//�豸�˹���bitλ
#define UCT_PHONE_INDEX 18//�ֻ�ע��index
#define UCT_PSK		19//ͨ��result���ͻ��˷��ص�psk
#define UCT_IA_IS_AP_MODE  20//ֵΪ1���豸��������APģʽ��ֵΪ0���豸����staģʽ.value����Ϊ1

//UCT_DEVFUN_BITS	17 bitλ���춨��
#define DEVFUN_BITS_SHARE (0) //�豸����bitλ֮ �����û���¼Ȩ��


#define	tlv_next(tlv) (uc_tlv_t *)BOFP((uc_tlv_t *)tlv + 1, tlv->len)
#define	tlv_n_next(tlv) (uc_tlv_t *)BOFP((uc_tlv_t *)tlv + 1, ntohs(tlv->len))
#define	tlv_val(tlv) ((u_int8_t *)((uc_tlv_t *)tlv + 1))

typedef struct {
	// UCT_XX
	u_int16_t type;
	// ���ݳ��ȣ�������ͷ��
	u_int16_t len;
} uc_tlv_t;

/******************debug info********************************/
#define DBG_TLV_HD_LEN		sizeof(uc_tlv_t)
//SVN��
#define DBG_TYPE_SVN		1
//����
#define DBG_TYPE_CUR		2
//������ӦADֵ
#define DBG_TYPE_CUR_AD		3
//����kֵ
#define DBG_TYPE_CUR_K		4
//����bֵ
#define DBG_TYPE_CUR_B		5
//��ѹ
#define DBG_TYPE_VOL		6
//��ѹ��ӦADֵ
#define DBG_TYPE_VOL_AD		7
//��ѹkֵ
#define DBG_TYPE_VOl_K		8
//��ѹbֵ
#define DBG_TYPE_VOL_B		9
//����ADֵ
#define DBG_TYPE_LIGHT_AD	10
//���ӷ���������
#define DBG_TYPE_SERVER_DONAME	11
//���ӷ�����ʱ��
#define DBG_TYPE_SERVER_CONNTIME 12
//��ǰ�������ֻ���������
#define DBG_TYPE_CLIENTS		13
//�豸uptime
#define DBG_TYPE_UPTIME			14
//���ѧϰ
#define DBG_TYPE_LIGHT_STUDY		15
//cpuʹ�ðٷֱ�
#define DGB_TYPE_CPU_PERCENTAGE		16
//memʹ�ðٷֱ�
#define DBG_TYPE_MEM_PERCENTAGE		17

//����ƽ������
#define DBG_TYPE_AVG_AD		18
//����������
#define DBG_TYPE_MAX_AD		19
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
#define DBG_TYPE_IR_LIB_ID	   25

#define DBG_TYPE_COLD_DELAY_PN_TIME 26
#define DBG_TYPE_COLD_DELAY_PF_TIME 27
#define DBG_TYPE_HOT_DELAY_PN_TIME  28
#define DBG_TYPE_HOT_DELAY_PF_TIME  29

//��Ƭ��������Ϣ
#define DGB_TYPE_STM32_INFO		30

//��Ƭ���ȴ�У��
#define DBG_TYPE_STM_WAIT_ADJUST	31
//dev server ip
#define DBG_TYPE_DEVSERVERIP		32
//����״̬��ѯ
#define DBG_TYPE_DEV_STATUS			33



//debug2
//��������
#define DBG_TLV_LIGHT			100
//��������
#define DBG_TLV_POWER			101
//�¶�����
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
//������ݽṹ
enum {
	EP_TYPE_HIGH = 1,
	EP_TYPE_LOW = 2,
	EP_TYPE_LEVEL = 3,
};

//���ݵ��:��ֹʱ��-���
typedef struct {
	u_int8_t begin_hour;
	u_int8_t begin_minute;
	u_int8_t end_hour;
	u_int8_t end_minute;
	u_int8_t tm_type;//��1 ��2 ƽ3 EP_TYPE_XXX
	u_int8_t pad;
	u_int16_t price;//��ۣ���λΪ�֣���168��ʾ1Ԫ6ë8
} uc_e_tm_t;

typedef struct {
	u_int8_t type;	//����(ȱʡ):0  ��ҵ:1
	u_int8_t count;	//uc_e_tm_t����
	u_int8_t pad[2];
//	uc_e_tm_t item[0];//uc_e_tm_t
} uc_e_price_t;

#endif

/*
	�ش�ʱ�䡢����ʱ�䡢����ʱ��
*/
typedef struct {
	// �������ش�ʱ�����������κ�����ش�ʱ�䶼�������һ���ˡ���λ100ms
	u_int8_t retry_100ms[3];
	// ���ͱ��������ʱ��������λ��
	u_int8_t keeplive;
	// ������ʱ��������λ��
	u_int16_t die;
} uc_time_param_item_t;

typedef struct {
	// �豸���������
	uc_time_param_item_t dev;
	// �ֻ����豸������ֱ��
	uc_time_param_item_t lan;
	// �ֻ����豸���������ӣ�APP��ǰ������
	uc_time_param_item_t wan_front;
	// �ֻ����豸���������ӣ�APP�ں������
	uc_time_param_item_t wan_background;
} uc_time_param_all_t;


typedef struct {
	// UAA_XXX
	u_int8_t action;
	u_int8_t flag_bits;
	u_int8_t version;
	u_int8_t reserved;
	// �����
	u_int8_t rand1[4];
	u_int64_t sn;
	// �ͻ���Ψһ��ʶ
	u_int8_t my_uuid[MAX_UUID_BIN_LEN];

	// ��ǰ����ʱ�ӣ����ڿͻ��˸��豸ͬ��ʱ�䡣���ɿ���Դ
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
	// �����
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
	// ��ǰ����ʱ�ӣ����ڿͻ��˸��豸ͬ��ʱ�䡣�ɿ���Դ
	uc_time_t time;
	/*�豸��Ҫ�ϱ��豸�����ַ������������TLV,���������չ*/
	//uc_tlv_t *tlv;
} uc_auth_answer_t;

typedef struct {
	// UAA_XXX
	u_int8_t action;
	u_int8_t reserved1;
	u_int16_t select_enc;

	u_int16_t err_num;
	u_int16_t reserved2;

	// Ӧ��Ϊ�豸��ʱ��Ч��Ϊ������ʱ����
	u_int8_t dev_type;
	u_int8_t ext_type;
	u_int8_t login_type; /* LT_NORMAL, LT_BIND, LT_UNBIND */
	u_int8_t net_type; /* NT_SERVER, NT_DEVICE, NT_DEVICE_OFFLINE */

	// UCT_VENDOR, UCT_LOCAL_TIME, UCT_TIME_PARAM
	//�������tlv
	//uc_tlv_t *tlv;
} uc_auth_result_t;

/*****************************************
	����ĸ�ʽ (CMD_UDP_KEEPLIVE)
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
	//�������tlv
	//uc_tlv_t *tlv;
} uc_keeplive_reply_t;

typedef struct {
	u_int8_t action;
	u_int8_t reserved[3];
} uc_keeplive_reset_t;


/* ͸�����Transparent transmission */

/* Transparent transmission ability */
#define TT_ABLILITY BIT(0)

/*͸������ʹ��TLV��ʽ������չ*/

#define UCT_TT_RAW_CHIPDATA 1 //���͸�����оƬ��ԭʼ����
#define UCT_TT_DEVINFO      2 //�豸��Ϣ



/*****************************************
	���Ʊ��ĸ�ʽ (CMD_UDP_CTRL)
 *****************************************/


/*****************************************
	UDP Control Object Type
	UDP Control Sub Object Type
	UDP Control Attribute Type
 ****************************************/

/*
	system. (ϵͳ)
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
#define	UCAT_SYS_VERSION	100 /* ���ܱ�����Ӷ����õģ��Ӵ�һ��Ķ��� */
//system.software.upgrade
#define UCAT_SYS_UPGRADE	101	/*��������*/
//system.softwate.stm_upgrade
#define UCAT_SYS_STM_UPGRADE 102 /*stm��Ƭ������*/
//system.softwate.evm_upgrade
#define UCAT_SYS_EVM_UPGRADE 103 /* ��������������*/

//system.softwate.evm_erase
#define UCAT_SYS_EVM_ERASE 104 /* ��������������*/

//system.softwate.evm_info
#define UCAT_SYS_EVM_INFO 105 /* ������������ѯ*/

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
#define UCSOT_SYS_USER		8 /*һЩ�û�������Ϣ*/
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
	IA(���ܵ���)
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

//IA.stat.push2serv ״̬�ϱ�������
#define UCAT_STAT_PUSH2SERV		20

// IA.stat.ele_daybyday ���һ��ÿ����õ����
#define UCAT_STAT_ELE_DAYBYDAY 21
// IA.stat.cur_power_milliwatt ��ǰ���ʣ���λ����
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
//IA. AIRCONDITION.ON_USER �û����ÿ��ػ�״̬(���Ƿ����⣬ֻ���豸�յ�״̬)
 #define UCAT_IA_ON_USER_SET		13
// IA.AIRCONDITION.CUR_RH
#define	UCAT_AC_CUR_RH	14
//IA.AIRCONDITION.TMP_SAMPLE_CURVE
#define 	UCAT_AC_TMP_SAMPLE_CURVE    15
//IA.AIRCONDITION.RH_SAMPLE_CURVE
#define 	UCAT_AC_RH_SAMPLE_CURVE    16

// IA.AIRCONDITION.LOCK �յ���
#define UCAT_AC_LOCK	17

// IA.AIRCONDITION.POWERCHECK	������⿪�ػ��Ŀ���
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
// �����͵�ƥ�䡢����
#define UCAT_CODE_MATCH_V2  11
#define UCAT_CODE_LIST_V2   12
#define UCAT_CODE_INFO_V2   13
#define UCAT_CODE_DATA_V2   14
#define UCAT_CODE_DATA2_V2  15
// ң������Ϣ
#define UCAT_CODE_RC_INFO   16
// ң����������Ϣ
#define UCAT_CODE_RC_KEY    17
// ң����ѧϰ
#define UCAT_CODE_RC_LEARN  18
// ң��������
#define UCAT_CODE_RC_CTRL   19
// ң����ƥ��
#define UCAT_CODE_RC_MATCH  20

/*
*HEATER_LCYT �͵�֧��
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
*��������¯���
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
���ƿ���������
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

// ��Ѷ�����
#define UCOT_IA_GX_LED		7
#define UCAT_GX_LED_STATUS	1

/*
 * E��
 */
//IA.EB
#define UCOT_IA_EB			8
//IA.EB.WORK
#define UCAT_EB_WORK		1
//IA.EB.TIMER
#define UCAT_EB_TIMER		2



/* ����̺subobject��attributeֵ���� */
#define UCSOT_IA_CH_BLANKET 10
//״̬
#define UCAT_CH_STATUS 1
#define UCAT_CH_QUERY_LEFT 2
#define UCAT_CH_QUERY_RIGHT 3


//���ܵ���֮͸������: Transparent transmission
//IA.TT
#define UCSOT_IA_TT  11

//IA.TT. ALLSTATE ��ѯ��push���е�״̬�������Է����ֻ�ͳһ��������WIFIģ�鶨ʱ��ѯά����ֵ
#define UCAT_IA_TT_ALLSTATE 1
//IA.TT.command ctrl �������ѯ����
#define UCAT_IA_TT_CMD 2
//IA.TT.command result ���һ������Ľ��
#define UCAT_IA_TT_CMDRET 3
//IA.TT.uart_cmd �豸͸���Ŀͻ��˴�������
#define UCAT_IA_TT_UARTCMD 4
//IA.TT.ir_notify �豸�ϱ��ĺ������
#define UCAT_IA_TT_IR_NOTIFY 5
//IA.TT.reset �豸����ͻ�������reset����
#define UCAT_IA_TT_RESET 6
//IA.TT.test_flag	�����־
#define UCAT_IA_TT_TESTFLAG 7
//IA.TT.ir_sound_notify �豸�ϱ������ɼ�����
#define UCAT_IA_TT_IR_SOUND_NOTIFY 8
//͸��ģʽ�£���ʱ���Ŀ��Ĵ�������
#define UCAT_TT_ON_CMD			9
//͸��ģʽ�£���ʱ���Ĺ�����
#define UCAT_TT_OFF_CMD         10


/*----------------------UCOT_IA	2  UCSOT_IA_PUBLIC 12----------------------*/
//���ܵ�����������
//IA.PUBLIC
#define UCSOT_IA_PUBLIC 12
//IA.PUBLIC. PERIOD_TIMER ʱ��ζ�ʱ��
#define UCAT_IA_PUBLIC_PERIOD_TIMER  1
// IA.PUBLIC.EXCEPTION_QUERY �쳣����
#define UCAT_IA_PUBLIC_EXCEPTION 2
//ͨ���ֻ���������·������ssid��password
#define UCAT_IA_PUBLIC_WIFI_SET            3

//IA.PUBLIC. EXTENDED_TIMER ��չ��ʱ��ζ�ʱ��
#define UCAT_IA_PUBLIC_EXTENDED_TIMER  4
#define UCAT_IA_TMP_CURVE			5
#define UCAT_IA_TMP_CURVE_UTC			6
// �������������˽������
// IA.JCX
#define UCSOT_IA_JCX 16

// ��������
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


// ͨ�õ��õ���ͳ��
#define UCSOT_PUBLIC_ELE 17
// ����ͬIA STAT��ȫһ��

//�����¿�������ռ��
//IA.YL �����Ӷ���
#define UCSOT_IA_THERMOSTAT_YL			18
//�����¿�������
#define UCAT_IA_THERMOSTAT_YL_WORK		1
//�����¿���ģʽ
#define UCAT_IA_THERMOSTAT_YL_MODE		2
//�����¿����¶�����
#define UCAT_IA_THERMOSTAT_YL_SET_TMP	3
//�����¿�������
#define UCAT_IA_THERMOSTAT_YL_ROOM_TMP	4
//�����¿������ܲ˵�
#define UCAT_IA_THERMOSTAT_YL_FUNC_MENU	5
//�����¿��������¶�
#define UCAT_IA_THERMOSTAT_YL_OFFSET_TMP 6


#define UCSOT_IA_JNB_THERMOSTAT 20	/* ���ܱ��¿��� */
#define UCAT_IA_JNB_STAT 1	/* ���ܱ��¿������� */

/* �����¿��� */
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


//��Ѹ�Ʊڻ�
#define UCSOT_IA_BREAK_MACHINE                 22
enum{
	//���ú�Ѹ�Ʊڻ��Ĺ���ģʽ
	UCAT_IA_BREAK_MACHINE_MODE = 1,
	//��ȡ�豸�ѹ���ʱ��
	UCAT_IA_BREAK_MACHINE_ON_TIME,
	//��ȡ�¶�
	UCAT_IA_BREAK_MACHINE_TEMP,
	//��ȡת��
	UCAT_IA_BREAK_MACHINE_SPEED,
	//�������
	UCAT_IA_BREAK_MACHINE_FINISH,
	//�����쳣��ֹ
	UCAT_IA_BREAK_MACHINE_STOP,
	//diy ����
	UCAT_IA_BREAK_MACHINE_DIYNAME,
	//������ͣ
	UCAT_IA_BREAK_MACHINE_PAUSE,
	//��������
	UCAT_IA_BREAK_MACHINE_IDLE,
	//�����¶�
	UCAT_IA_BREAK_MACHINE_KEEP_WARM,
	UCAT_IA_BREAK_MACHINE_MAX
};


//ǧ�����ܲ���
//IA.TEA_TRAY_QP ǧ�����ܲ����Ӷ���
#define UCSOT_IA_TEA_TRAY_QP			24
//ǧ�����ܲ��̿���
#define UCAT_IA_TEA_TRAY_QP_WORK		1
//ǧ�����ܲ����ֶ���ˮ
#define UCAT_IA_TEA_TRAY_QP_WATER		2
//ǧ�����ܲ����ֶ�����
#define UCAT_IA_TEA_TRAY_QP_CTRL	3
//ǧ�����ܲ��̷���
#define UCAT_IA_TEA_TRAY_QP_PLAN	4
//ǧ�����ܲ��̷���ID��ѯ
#define UCAT_IA_TEA_TRAY_QP_PLAN_ID	5
//ǧ�����ܲ��̷���ִ��
#define UCAT_IA_TEA_TRAY_QP_PLAN_EXECUTE	6
//ǧ�����ܲ��̹��ϸ�λ
#define UCAT_IA_TEA_TRAY_QP_RESET	7

/*ǧ����*/
#define   UCSOT_IA_QPG   	   25
//����ǧ�������������
#define UCAT_IA_QPG_SET_PROC    1
//�����Զ����������
 #define UCAT_IA_QPG_PLAN       2
//��ѯ����״̬��Ϣ
 #define UCAT_IA_QPG_QUERY       3
//��ѯ�豸��������з���id
 #define UCAT_IA_QPG_PLAN_ID       4
//�޸ķ����󵥶�push����
 #define UCAT_IA_QPG_MODIFYED       5


/* IA.statforshare�罻�������� */
#define UCSOT_IA_SHARE 26
/* IA.statforshare.tempsetcount�����¶��ֶ����ô���ͳ�ƣ�����ϰ��ͳ�� */
#define UCAT_IA_SHARE_TEMP_SET_COUNT 1
/* IA.statforshare.acontime�ۼƿյ�����ʱ�� */
#define UCAT_IA_SHARE_AC_ON_HOUR 2
/* IA.statforshare.coldfast�����������¶��½��仯�����ֵ�������������� */
#define UCAT_IA_SHARE_COLD_TOP 3
/* IA.statforshare.warmfast�����������¶������仯�����ֵ�������������� */
#define UCAT_IA_SHARE_WARM_TOP 4
/* IA.statforshare.powertoday����Ŀյ����ʣ����ڹ������� */
#define UCAT_IA_SHARE_POWER_TODAY 5
/* IA.statforshare.powertoday�����¶ȱ仯�������ڣ������������� */
#define UCAT_IA_SHARE_CONFIG_SAMPLE_PERIOD 6

/*
 *[> IA.TBHEATER.RTC_SYNC <]
 *#define UCAT_TBHEATER_RTC_SYNC       3
 *
 */

/*���֡�ɳ�ز���*/
#define UCSOT_IA_EPLUG_OEM      27
//����״̬
#define UCAT_IA_EP_OEM_ONOFF    0x1
//����
#define UCAT_IA_EP_OEM_ROOM_TEMP   0x2
//����
#define UCAT_IA_EP_OEM_TEMP_RANGE    0x3
//����¶ȷ�Χ
#define UCAT_IA_EP_OEM_MAX_TEMP    0x4
//���߱���
#define UCAT_IA_EP_OEM_OFFLINE_PROTECT    0x5
//����̽��
#define UCAT_IA_EP_OEM_PERSION_DETECT   0x6


//��Դ�¿���
//IA_THERMOSTAT.XY
#define UCSOT_IA_THERMOSTAT_XY			28
//͸����������
#define UCAT_IA_THERMOSTAT_CTRL			1
//push����
#define UCAT_IA_THERMOSTAT_PUSH			2
//����ģʽʱ�������
#define UCAT_IA_THERMOSTAT_TIME			3
//�ⲿ�¶�����
#define UCAT_IA_THERMOSTAT_EX_TMP		4
//���ܻؼҿ���
#define UCAT_IA_THERMOSTAT_SMART_HOME	5


/*�������*/
//IA.CARWUKONG ��������Ӷ���
#define UCSOT_IA_CAR_WUKONG				29
//����
#define UCAT_IA_CAR_WUKONG_ON			1
//Ѱ������
#define UCAT_IA_CAR_WUKONG_SERCH		2
//�����¶Ȳ�ѯ
#define UCAT_IA_CAR_WUKONG_TEMP			3
//����ƿ��ȫ��ѹ��⿪��
#define UCAT_IA_CAR_WUKONG_VALCHECK		4
//�豸���ܿ���
#define UCAT_IA_CAR_WUKONG_POWERSAVE	5
//����push����
#define UCAT_IA_CAR_WUKONG_ALARM		6

//��Ѹ������
#define UCSOT_IA_HX_YS_POT     31
//״̬
#define UCAT_IA_HX_YS_POT_WORK_STAT 0x1
//�龰ִ��
#define UCAT_IA_HX_YS_POT_EXEC_SCENE   0x2
//�Զ����龰
#define UCAT_IA_HX_YS_POT_EDIT_SCENE   0x3
//�龰ID�б�
#define UCAT_IA_HX_YS_POT_SCENE_LIST   0x4


//���ÿյ�
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


//�ĵ�����ˮ��
#define UCSOT_IA_ADS	33

//��������
#define UCAT_IA_ADS_CTRL		1
//��������
#define UCAT_IA_ADS_CONF		2


//��ʯ΢��¯
#define UCSOT_IA_JS_WEIBOLU     34
enum{
//״̬
UCAT_IA_JS_WEIBOLU_WORK_STATE  = 0x01,
// �������ԣ�û�в�ѯ�����������״̬��������
UCAT_IA_JS_WEIBOLU_PARAM_SET = 0x02,
//��ʼ����ͣ��ȡ��
//UCAT_IA_JS_WEIBOLU_START_PAUSE_CANCEL,
//ͯ��
UCAT_IA_JS_WEIBOLU_TONGSUO = 0x04
};


//��ɽ���¯
#define  UCSOT_IA_ELEC_HEATER  36
//WIFI��������
#define  UCAT_IA_ELEC_HEATER_WIFI_SET      0x1
//�����������
#define  UCAT_IA_ELEC_HEATER_PARAM_PRO   0x2

/*--------------------------------UCOT_STORAGE 3-----------------------------------*/

/*
	STORAGE(�洢)
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
*�������
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
*�������
*/
//dev_product
#define UCOT_DEVPRO 			5
//devproduct.ir ���������Ӷ���������
#define UCSOT_DEVPRO_IR			1
//devpro.ir.val_kb ��ѹ getʱad/ad2,setʱk/ad/ad2ֵ
#define UCAT_IR_VALKADKB		2
//devpro.ir.cur_kb ���� getʱad,setʱb/k
#define UCAT_IR_CURKB			3
//devpro.ir.adjust
#define UCAT_IR_ADJUST			4
//���2.0����оƬHLW8012
#define UCAT_IR_HLW8012			5

/*--------------------------------UCOT_PLUG_DEVPRO 7-----------------------------------*/
/*
*�������
*/
//plug_product
#define UCOT_PLUG_DEVPRO 			7
//plug_product.av
#define UCSOT_PLUG_DEVPRO_AV        1
//plug_product.av.adjust
#define UCAT_AV_ADJUST	1


/*--------------------------------UCOT_MISC 6-----------------------------------*/
/*
*misc����
*/
//misc
#define UCOT_MISC				6
//misc.debug
#define UCSOT_MISC_DEBUG		1
//misc.debug.monthstat
#define UCAT_DEBUG_MONTH		1


/*
 * �ذ���ˮ��
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

//�����ذ�
//״̬���ݻ�ȡ����
#define UCAT_IA_TB_STATUS				1
//��������
//�¶�����
#define UCAT_IA_TB_TEMP					2
//ģʽ����
#define UCAT_IA_TB_MODE					3
//��������
#define UCAT_IA_TB_ONOFF				4
//��˪����
#define UCAT_IA_TB_DEFROST				5
//���޵��籣������������
#define UCAT_IA_TB_MISC					6
//һЩ��������,�������
#define UCAT_IA_TB_PROTECT				7
//���̵���������¶�����
#define UCAT_IA_TB_BERT					8
//���»�ȡ
#define UCAT_IA_TB_ROOTTMP				9
//�������ͷ�����
#define UCAT_IA_TB_EEV					10


//IA.BREAK_MACHAIN ǧ���Ʊڻ�
#define UCSOT_IA_BREAK_MACHAIN		30
//��������
#define UCAT_IA_QP_BREAK_DO_WORK		(1)
//ִ��ʳ��
#define UCAT_IA_QP_BREAK_DO_EXECEUTE		(2)
//��ɾ��ʳ��
#define UCAT_IA_QP_BREAK_DO_RECIPE		(3)
//��ȡ��ǰʳ��list
#define UCAT_IA_QP_BREAK_GET_RECIPE_LIST	(4)
//�������״̬��Ϣ
#define UCAT_IA_QP_BREAK_RESET_FAULT		(5)

//ͨ���0xFFFF
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
	// ���汾��
	u_int8_t major;
	// �ΰ汾��
	u_int8_t minor;
	// �޶��汾��
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


// ����
#define	AC_POWER_ON	0
#define	AC_POWER_OFF	1

// ģʽ
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



// ����
#define	AC_WIND_AUTO	0
#define	AC_WIND_1	1
#define	AC_WIND_2	2
#define	AC_WIND_3	3

// ����
#define	AC_DIR_AUTO	0
#define	AC_DIR_1	1
#define	AC_DIR_2	2
#define	AC_DIR_3	3

// ��ֵ
#define	AC_KEY_POWER	0
#define	AC_KEY_MODE	1
#define	AC_KEY_TEMP	2
#define	AC_KEY_WIND	3
#define	AC_KEY_DIR		4
#define AC_KEY 5

// ��ʽ�յ���ʵ�ʰ���
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


typedef struct {
	// ��ʼʱ�䣬��λ�����ӣ�0:0����
	u_int16_t begin_minute;
	// �����೤����λ����
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


/* ����ƥ������ݽṹ */
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

//���ܻؼң���λ��Ϣ
#define LAC_GET 1	//�ֻ���ȡ�豸�ϱ���Ķ�Ϊ��Ϣ��get�����豸��дis_east�� is_north�� latitude�� longitude
#define LAC_PUT 2 //�ֻ�֪ͨ�豸�����λ��Ϣ��set�����豸����is_east�� is_north�� latitude�� longitude
#define LAC_SMART_ON 3 //�ֻ����㵽��Ҫ�����ˣ�֪ͨ�豸�������ܻؼң�set��
#define LAC_SMART_OFF 4 //�ֻ����ָղż���ִ��LAC_SMART_ON����֪ͨ�豸ȡ�����ܻؼң�set��
#define LAC_BACKHOME 5 //�ֻ���֪����(set)


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


//��Դ�¿��� cmd_ctrl
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

// ��Ѹ��������ʼ
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

// ��Ѹ����������

/*ͨ��Э�����ݽṹ1�ֽڶ��룬�����ݽṹ����ڱ���֮ǰ*/
#pragma pack(pop)


/*****************************************
	���ú���
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

