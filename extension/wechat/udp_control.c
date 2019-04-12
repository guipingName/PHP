#include "udp_control.h"

void uc_print_zval(ZVAL *pval, char* key) {

    if (key != NULL) {
        UC_LOG("Key:%s   Type:", key);
    }
    switch (Z_TYPE_P(pval)) {
        case IS_NULL:
            UC_LOG("null  Value:null ");break;
        case IS_BOOL:
            UC_LOG("bool  Value: %s ", Z_LVAL_P(pval) ? "TRUE" : "FALSE");break;
        case IS_LONG:
            UC_LOG("long  Value: %ld ", Z_LVAL_P(pval));break;
        case IS_DOUBLE:
            UC_LOG("double Value: %.0f ", Z_DVAL_P(pval));break;
        case IS_STRING:
            UC_LOG("string  Value: ");UC_WRITE(Z_STRVAL_P(pval), Z_STRLEN_P(pval));
            UC_LOG(" ");break;
        case IS_RESOURCE:
            UC_LOG("resource  Value: resource");break;
        case IS_ARRAY:
            UC_LOG("array  Value:array ");break;
        case IS_OBJECT:
            UC_LOG("object  Value:object ");break;
        default:
            UC_LOG("unknown  Value:unkown");break;
    }
    UC_LOG("\n");
}

u_int64_t convert_to_uint64(char* str, u_int32_t str_len) {
    char buf[64] = {0};
    memmove(buf, str, min(63, str_len));
    return atoll(buf);
}

int uc_control_match(uc_control_t* in_ctrl, uc_control_t contain[UC_MAX_CONTROL]) {

    u_int16_t i = 0;
    for (i = 0; i < UC_MAX_CONTROL; i++) {
        uc_control_t* uc_ctrl = &contain[i];
        if (uc_ctrl->action == 0) {
            break;
        }

        if (uc_ctrl->direct== in_ctrl->direct 
                && uc_ctrl->action == in_ctrl->action && uc_ctrl->objct == in_ctrl->objct
                && uc_ctrl->sub_objct == in_ctrl->sub_objct && uc_ctrl->attr == in_ctrl->attr) {
            return i;
        }
    }
    
    return E_FALIED;;
}

void uc_set_global(int option) {
    if (UC_GLOBAL_DEBUG & option) {
        WECHAT_G(debug_flag) = 1;
    }
}


/*-----------------------uc_control_pack begin------------------------*/
static inline int uc_control_pack_set_trans_header(
        u_int8_t* output, ZVAL* input) {
        
    uc_trans_hdr_t* trans_hdr = (uc_trans_hdr_t*)output;
    uc_print_zval(input, "sn");
    if (input == NULL) {
        return E_INVALID_INPUT;
    }
    
    switch(Z_TYPE_P(input)) {
    case IS_DOUBLE:
        trans_hdr->sn = ntoh_ll((u_int64_t)Z_DVAL_P(input));
        break;
    case IS_STRING:
        trans_hdr->sn = convert_to_uint64(Z_STRVAL_P(input), Z_STRLEN_P(input));
        trans_hdr->sn = ntoh_ll(trans_hdr->sn);
        break;
    default:
        return E_INVALID_INPUT;
        break;
    }
    
    return E_SUCCESS;
}


static inline void uc_control_pack_set_header(u_int8_t* output, 
        u_int16_t cmd, u_int16_t param_len) {

    ucph_t* hdr = (ucph_t*)output;
    hdr->ver = PROTO_VER1;
    hdr->hlen = sizeof(ucph_t)/4;
    hdr->request = 1;
    hdr->encrypt = 0;
    hdr->flags = 4;
    hdr->client_sid = htonl(0);
    hdr->device_sid = htonl(0);
    hdr->request_id = 0;
    
    hdr->command = htons(cmd);
    hdr->param_len = htons(param_len);
}

int uc_control_parse_cmd(uc_control_t* output, ZVAL* input) {
    
    ZVAL_ARRAY_FOREACH_BEGIN(input)
    UC_PACK_MEMBER(output, UINT16, objct);
    UC_PACK_MEMBER(output, UINT16, sub_objct);
    UC_PACK_MEMBER(output, UINT16, attr);
    ZVAL_ARRAY_FOREACH_END(input)

        
    return E_SUCCESS;
}


int uc_control_pack_check_cmd(ZVAL* ouput[UC_MAX_CONTROL], ZVAL* input) {
    int count = 0;
    int check_type = HASH_KEY_NON_EXISTANT;
    
    ZVAL_ARRAY_FOREACH_BEGIN(input)
    if (check_type ==  HASH_KEY_NON_EXISTANT) {
        check_type = key_type;
    } else if (check_type != key_type) {
        UC_LOG("Command parse falied:cmd type is not avalible\n");
        return E_FALIED;
    }

    if (check_type == HASH_KEY_IS_LONG) {
        if (Z_TYPE_PP(node_val) != IS_ARRAY) {
            UC_LOG("Command parse falied:cmd must be two dimension array\n");
            return E_FALIED;
        }
        ouput[count++] = *node_val;
    }
    ZVAL_ARRAY_FOREACH_END(input)

    if (check_type == HASH_KEY_IS_STRING) {
        ouput[count++] = input;
    }
    
    return count;
}


void uc_control_pack(ZVAL *output, ZVAL *sn, ZVAL* cmd, long option) {
    
    u_int8_t pack_buff[8096] = {0};
    ZVAL* contain[UC_MAX_CONTROL] = {0};
    u_int8_t* pcur = pack_buff;
    u_int8_t action = 0;
    u_int8_t direct = UC_REQUEST;
    
    if (option & UC_ANSWER) {
        action = option ^ UC_ANSWER;
        direct = UC_ANSWER;
    } else if (option & UC_REQUEST) {
        action = option ^ UC_REQUEST;
        direct = UC_REQUEST;
    } else {
        action = option;
        direct = UC_REQUEST;
    }

    if(uc_control_pack_set_trans_header(pack_buff, sn) != E_SUCCESS) {
        UC_LOG("Device sn is not available\n");
        return;
    }
    
    pcur += sizeof(uc_trans_hdr_t) + sizeof(ucph_t);

    u_int32_t param_len =  sizeof(ucp_ctrl_t);
    u_int32_t com_len = sizeof(uc_trans_hdr_t) + sizeof(ucph_t);

    u_int32_t cmd_count = uc_control_pack_check_cmd(contain, cmd);
    if (cmd_count <= 0) {
        return;
    }
    
    ucp_ctrl_t* ctrl = (ucp_ctrl_t*)(pcur);
    ctrl->action     = (u_int8_t)action;
    ctrl->count      = cmd_count;
    ctrl->reserved   = htons(0);
    pcur += sizeof(ucp_ctrl_t);

    u_int32_t i  = 0;
    uc_control_t uc_ctrl;
    for (i = 0; i < cmd_count; i++) {
        ucp_obj_t* sub = (ucp_obj_t *)(pcur);
        
        memset(&uc_ctrl, 0, sizeof(uc_control_t));
        uc_ctrl.action = action;
        uc_ctrl.direct = direct;
        uc_control_parse_cmd(&uc_ctrl, contain[i]);
        
        sub->objct      = htons(uc_ctrl.objct);
        sub->sub_objct  = htons(uc_ctrl.sub_objct);
        sub->attr       = htons(uc_ctrl.attr);
        sub->param_len  = htons(0);
        pcur += sizeof(ucp_obj_t);
        param_len += sizeof(ucp_obj_t);

        int index = uc_control_match(&uc_ctrl, g_uc_pack_control);
        if (index < 0) {
            UC_LOG("Warn: Direct:%s Action:%d Object:%d Sub-object:%d Attribute:%d"\
                " Type: PACK have not been registered\n", \
                direct == UC_ANSWER ? "ANSWER":"REQUEST",
                uc_ctrl.action, uc_ctrl.objct, uc_ctrl.sub_objct, uc_ctrl.attr);
            continue;
        }

        if (g_uc_pack_control[index].callback != NULL) {
            UC_LOG("Entry:%s\n",g_uc_unpack_control[index].description);
            int ret = g_uc_pack_control[index].callback(pcur, contain[i]);
            if (ret > 0) {
                sub->param_len = htons(ret);
                pcur += ret;
                param_len += ret;
            } else {
                UC_LOG("Warn: %s excute falied and return code: %d\n",  
                g_uc_pack_control[index].description, ret);
            }
        }
    } 

    uc_control_pack_set_header(pack_buff + sizeof(uc_trans_hdr_t),
        CMD_UDP_CTRL, param_len);

    UC_ZVAL_STRINGL(output, pack_buff, com_len + param_len, 1);
}
/*-----------------------uc_control_pack end------------------------*/





/*-----------------------uc_control_unpack begin------------------------*/


int uc_control_unpack_callback_default(void* output, void* input) {
    ZVAL* return_value = (ZVAL*)output;
    ucp_obj_t* obj = (ucp_obj_t*)input;

    UCN_ASSOC_MEMBER_UINT16(return_value, obj, objct);
    UCN_ASSOC_MEMBER_UINT16(return_value, obj, sub_objct);
    UCN_ASSOC_MEMBER_UINT16(return_value, obj, attr);
    UCN_ASSOC_MEMBER_UINT16(return_value, obj, param_len);

    return E_SUCCESS;
}
void uc_control_unpack_unknow(ZVAL* output, u_int8_t*key, \
        u_int8_t* input, size_t input_len) {
        
    if (input_len <= 0 || input == NULL) {
        return;
    }
    
    int offset = 5;
    u_int8_t* buff = EMALLOC(input_len * offset + 1);
    memset(buff, 0 , input_len * offset + 1);
    u_int8_t* buff_p = buff;
    size_t i = 0;
    for (i = 0; i < input_len; i++) {
        sprintf(buff_p, "0x%02x ", input[i]);
        buff_p += offset;
    }
    if (key != NULL) {
        UC_ADD_ASSOC_STRINGL(output, key, buff, input_len * offset + 1, 0);
    } else {
        UC_ADD_NEXT_INDEX_STRINGL(output, buff, input_len * offset + 1, 0);
    }
}

void uc_control_unpack(ZVAL *output, char* buff, int buff_len, long option) {
    u_int32_t unpack_len = buff_len;
    u_int8_t unpack_buff[8096] = {0};
    memmove(unpack_buff, buff, buff_len);
    UC_ARRAY_INIT(output);

    u_int8_t direct = UC_ANSWER;
    if (option & UC_ANSWER) {
        direct = UC_ANSWER;
    } else if (option & UC_REQUEST) {
        direct = UC_REQUEST;
    } else {
        direct = UC_ANSWER;
    }

    /*解析uc_trans_hdr_t*/
    int count_len = sizeof(uc_trans_hdr_t);
    if (unpack_len < count_len) {
        UC_LOG("Unapck parse falied by length, min length:%d unapck_length:%d\n", 
            count_len,unpack_len);
        return;
    }

    uc_trans_hdr_t* trans_hdr = (uc_trans_hdr_t*)unpack_buff;
    UCN_ASSOC_MEMBER_UINT64(output, trans_hdr, sn);
    UCN_ASSOC_MEMBER_UINT32(output, trans_hdr, errorcode);
    UCN_ASSOC_MEMBER_UINT8(output, trans_hdr, cfg_id);
    UCN_ASSOC_MEMBER_STRINGL(output, trans_hdr, pad);

    /*解析ucph_t*/
    count_len += sizeof(ucph_t);
    if (unpack_len < count_len) {
        return;
    }

    ucph_t* hdr = (ucph_t*)(trans_hdr + 1);
    UCN_ASSOC_MEMBER_UINT8(output, hdr, ver);
    UCN_ASSOC_MEMBER_UINT8(output, hdr, hlen);
    UCN_ASSOC_MEMBER_UINT8(output, hdr, request);
    UCN_ASSOC_MEMBER_UINT8(output, hdr, encrypt);
    UCN_ASSOC_MEMBER_UINT8(output, hdr, ver);
    UCN_ASSOC_MEMBER_UINT8(output, hdr, flags);
    UCN_ASSOC_MEMBER_UINT8(output, hdr, request_id);
    UCN_ASSOC_MEMBER_UINT8(output, hdr, resv);
    UCN_ASSOC_MEMBER_UINT32(output, hdr, client_sid);
    UCN_ASSOC_MEMBER_UINT32(output, hdr, device_sid);
    UCN_ASSOC_MEMBER_UINT16(output, hdr, command);
    UCN_ASSOC_MEMBER_UINT16(output, hdr, param_len);

    /*解析ucp_ctrl_t*/
    count_len += sizeof(ucp_ctrl_t);
    if (unpack_len < count_len) {
        return;
    }
    ucp_ctrl_t* ctrl = (ucp_ctrl_t*)(hdr + 1);
    UCN_ASSOC_MEMBER_UINT8(output, ctrl, action);
    UCN_ASSOC_MEMBER_UINT8(output, ctrl, count);
    UCN_ASSOC_MEMBER_UINT16(output, ctrl, reserved);

    UC_ALLOC_ARRAY_INIT_ZVAL(sub_output);

    uc_control_t uc_ctrl;
    u_int8_t* objct_pp = (u_int8_t*)(ctrl + 1);
    u_int32_t i = 0;
    u_int32_t count = 0;
    for (i = 0; i < ctrl->count; i++) {
        count_len += sizeof(ucp_obj_t);
        if (unpack_len < count_len) {
            break;
        }

        ucp_obj_t* obj = (ucp_obj_t*)objct_pp;
        uc_ctrl.direct = direct;
        uc_ctrl.action = ctrl->action;
        uc_ctrl.objct = ntohs(obj->objct);
        uc_ctrl.sub_objct = ntohs(obj->sub_objct);
        uc_ctrl.attr = ntohs(obj->attr);

        UC_ALLOC_ARRAY_INIT_ZVAL(sub_value);
        uc_control_unpack_callback_default(sub_value, obj);

        objct_pp += sizeof(ucp_obj_t) + ntohs(obj->param_len);
        count_len += ntohs(obj->param_len);
        if (unpack_len >= count_len) {
            int index = uc_control_match(&uc_ctrl, g_uc_unpack_control);
            if (index > 0 && g_uc_unpack_control[index].callback) {
                UC_LOG("Entry:%s\n",g_uc_unpack_control[index].description);
                int ret = g_uc_unpack_control[index].callback(sub_value, obj);
                if (ret < 0) {
                    UC_LOG("Warn: %s excute falied and return code: %d\n",  
                        g_uc_unpack_control[index].description, ret);
                    uc_control_unpack_unknow(sub_value, UC_HEX_CODE, obj + 1, ntohs(obj->param_len));
                } 
            } else if (ntohs(obj->param_len) == 2) {//返回码
                u_int16_t* ret_code = (u_int16_t*)(obj + 1);
                UCN_ADD_ASSOC_UINT16(sub_value,UC_RETURN_CODE,*ret_code);
            } else if (ntohs(obj->param_len) > 0){
                UC_LOG("Warn: Direct:%s Action:%d Object:%d Sub-object:%d Attribute:%d"\
                        " Type: UNPACK have not been registered\n", \
                        direct == UC_ANSWER ? "ANSWER":"REQUEST",
                        uc_ctrl.action,uc_ctrl.objct, uc_ctrl.sub_objct, uc_ctrl.attr);
                uc_control_unpack_unknow(sub_value, UC_HEX_CODE, obj + 1, ntohs(obj->param_len));
            }
        }

        UC_ADD_INDEX_ZVAL(sub_output, count++, sub_value);
    }

    UC_ADD_ASSOC_ZVAL(output, UC_DATA, sub_output);
}
/*-----------------------uc_control_unpack end------------------------*/



/*-----------------------uc_control_devinfo begin------------------------*/

int uc_control_devinfo_unpack(ZVAL *output, char *str, int str_len) {
    if (str_len < sizeof(uc_dev_info_t)) {
        UC_LOG("Input length is %d,and required: %d\n", str_len, sizeof(uc_dev_info_t));
        return 0;
    }
    uc_dev_info_t* ucInfo = (uc_dev_info_t*)str;

    //ALLOC_INIT_ZVAL(array);
    UC_ARRAY_INIT(output);

    dev_info_t* devInfo = &ucInfo->old;
    UC_ASSOC_MEMBER_UINT32(output, devInfo, ver);

    UC_ASSOC_MEMBER_UINT64(output, devInfo, sn);
    UC_ASSOC_MEMBER_UINT32(output, devInfo, type);
    UC_ASSOC_MEMBER_UINT32(output, devInfo, subtype);
    UC_ASSOC_MEMBER_STRINGL(output, devInfo, passwd);
    UC_ASSOC_MEMBER_UINT32(output, devInfo, time);
    UC_ASSOC_MEMBER_STRINGL(output, devInfo, nickname);
    UC_ASSOC_MEMBER_STRINGL(output, devInfo, email);
    UC_ASSOC_MEMBER_STRINGL(output, devInfo, phone);
    UC_ASSOC_MEMBER_STRINGL(output, devInfo, service_date);
    UC_ASSOC_MEMBER_UINT8(output, devInfo, service_type);
    UC_ASSOC_MEMBER_UINT8(output, devInfo, ext_type);
    UC_ASSOC_MEMBER_UINT32(output, devInfo, devserver_id);
    UC_ASSOC_MEMBER_UINT32(output, devInfo, devserver_id_b);
    UC_ASSOC_MEMBER_UINT8(output, devInfo, async_passwd);
    
    UC_ASSOC_MEMBER_STRINGL(output, ucInfo, vendor);
    UC_ASSOC_MEMBER_STRINGL(output, ucInfo, preshare_key);
    UC_ASSOC_MEMBER_STRINGL(output, ucInfo, uuid);
}


/*	
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
    u_int32_t devserver_id;
    u_int32_t devserver_id_b;
    u_int8_t async_passwd;
}dev_info_t;

typedef struct{
    dev_info_t old;
    u_int8_t vendor[UCKEY_VENDOR_LEN];
    u_int8_t preshare_key[UCKEY_PRESHAREKEY_LEN];
    u_int8_t uuid[MAX_UUID_BIN_LEN];
}uc_dev_info_t;
*/

int wechat_udp_devinfo_pack(zval *output, zval *input) {
    uc_dev_info_t ucInfo;
    memset(&ucInfo, 0, sizeof(uc_dev_info_t));	

    dev_info_t* devInfo = &ucInfo.old;
    
    ZVAL_ARRAY_FOREACH_BEGIN(input)
    if (key_type != HASH_KEY_IS_STRING) {
        UC_LOG("Warn:assoc key must be string\n");
        continue;
    }

    UC_PACK_MEMBER(devInfo, UINT32, ver);

    UC_PACK_MEMBER(devInfo, UINT64, sn);
    UC_PACK_MEMBER(devInfo, UINT32, type);
    UC_PACK_MEMBER(devInfo, UINT32, subtype);
    UC_PACK_MEMBER(devInfo, STRING, passwd);
    UC_PACK_MEMBER(devInfo, UINT32, time);
    UC_PACK_MEMBER(devInfo, STRING, nickname);
    UC_PACK_MEMBER(devInfo, STRING, email);
    UC_PACK_MEMBER(devInfo, STRING, phone);
    UC_PACK_MEMBER(devInfo, STRING, service_date);
    UC_PACK_MEMBER(devInfo, UINT8,  service_type);
    UC_PACK_MEMBER(devInfo, UINT8,  ext_type);
    UC_PACK_MEMBER(devInfo, UINT32, devserver_id);
    UC_PACK_MEMBER(devInfo, UINT32, devserver_id_b);
    UC_PACK_MEMBER(devInfo, UINT8,  async_passwd);

    UC_PACK_MEMBER((&ucInfo), STRING, vendor);
    UC_PACK_MEMBER((&ucInfo), STRING, preshare_key);
    UC_PACK_MEMBER((&ucInfo), STRING, uuid);

    ZVAL_ARRAY_FOREACH_END(input)

    UC_ZVAL_STRINGL(output, (u_int8_t*)&ucInfo, sizeof(uc_dev_info_t), 1);
    
    return sizeof(uc_dev_info_t);
}

