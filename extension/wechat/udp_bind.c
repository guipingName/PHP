#include "udp_control.h"

/*
typedef struct{
    u_int64_t sn;
    u_int32_t devserver_id;
    u_int32_t devserver_ip;
    u_int16_t devserver_port;
    u_int8_t major_ver;
    u_int8_t minor_ver;
    u_int8_t is_udp;
    u_int8_t reserverd2;
    tcp_tlv_t tlv[0];
}net_device_location_t;
*/
void uc_bind_unpack(ZVAL *return_value, char *str, int str_len) {
    int needLen = sizeof(net_header_t) + sizeof(net_device_location_t);
    if (str_len < needLen) {
        UC_LOG("Input length is %d,and required: %d\n", str_len, needLen);
        return;
    }
    u_int8_t* buff = (u_int8_t*)EMALLOC(needLen);
    memmove(buff, str, needLen);

    net_header_t* hd = (net_header_t*)buff;
    hd->command = ntohs(hd->command);
    hd->param_len = ntohl(hd->param_len);

    if (hd->command != CMD_USER_BIND_A) {
        UC_LOG("Command error! required command: %d\n", CMD_USER_BIND_A);
        return;
    }

    net_device_location_t *loc = (net_device_location_t*)(hd + 1);

    UC_ARRAY_INIT(return_value);

    UCN_ASSOC_MEMBER_UINT64(return_value, loc, sn);
    UCN_ASSOC_MEMBER_UINT32(return_value, loc, devserver_id);
    UCN_ASSOC_MEMBER_UINT32(return_value, loc, devserver_ip);
    UCN_ASSOC_MEMBER_UINT16(return_value, loc, devserver_port);
    UCN_ASSOC_MEMBER_UINT8(return_value, loc, major_ver);
    UCN_ASSOC_MEMBER_UINT8(return_value, loc, minor_ver);
    UCN_ASSOC_MEMBER_UINT8(return_value, loc, is_udp);
    UCN_ASSOC_MEMBER_UINT8(return_value, loc, reserverd2);

    EFREE(buff);
}

void uc_bind_pack(ZVAL *output,char* str, int str_len) {
    u_int32_t needLen = sizeof(net_header_t) + sizeof(net_para_user_bind_q);
    u_int8_t* buff = (u_int8_t*)EMALLOC(needLen);
    memset(buff, 0, needLen);

    net_para_user_bind_q *para;
    net_header_t *hd = (net_header_t*)buff;
    set_net_proto_hd(0, 0, TP_USER, CMD_USER_BIND_Q, sizeof(net_para_user_bind_q),hd);
    para = (net_para_user_bind_q *)(hd + 1);

    //sprintf((char*)para->username,"%llu",str);
    memmove(para->username, str, str_len);
    para->major_ver = 5;
    para->minor_ver = 0;
    para->client_id = 0;
    para->isp = 0;

    UC_ZVAL_STRINGL(output, buff, needLen, 0);
}