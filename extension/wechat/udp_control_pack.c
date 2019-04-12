#include "udp_control.h"

/*
函数原型通配 :注意输出都为网络序
int function_pack(u_int8_t* output, ZVAL* input)


@param  output 类型:u_int8_t， 发送udp control报文buffer
		output 说明: 解析关联数组input，并填充报文到output
             
@param  input 类型:PHP关联数组， 输入参数
		input 说明:  输入参数，需要解析相关参数      

@return 填充buffer长度；> 0表示填充成功；<=0 表示解析失败

*/


int uc_pack_u_int64_t(ZVAL* input, u_int64_t* value) {
	switch(Z_TYPE_P(input)) {
    case IS_DOUBLE:
        *value = (u_int64_t)Z_DVAL_P(input);
        break;
    case IS_STRING:
        *value = convert_to_uint64(Z_STRVAL_P(input), Z_STRLEN_P(input));
        //trans_hdr->sn = ntoh_ll(trans_hdr->sn);
        break;
    default:
        return E_INVALID_INPUT;
        break;
    }
	*value = ntoh_ll(*value);
	return E_SUCCESS;
}

//RF网关透传命令头
int uc_pack_rfgw_tt_head(u_int8_t* output, ZVAL* sn, u_int16_t length) {
	IA_RFGW_TT* rfgw_tt = (IA_RFGW_TT*)output;
	uc_pack_u_int64_t(sn, &(rfgw_tt->sn));
	rfgw_tt->length = htons(length);
	return sizeof(IA_RFGW_TT);
}

int uc_pack_rfgw_tlv_summary(u_int8_t* output, ZVAL* input) {
	CMD_GET_SUMMARY* value = (CMD_GET_SUMMARY*)output;
	ZVAL_ARRAY_FOREACH_BEGIN(input)
    UCN_PACK_MEMBER(value, UINT8, getindex);
    ZVAL_ARRAY_FOREACH_END(input)
	value->resv = 0;
	
	return sizeof(CMD_GET_SUMMARY);
}

int uc_pack_rfgw_tlv_onofftime(u_int8_t* output, ZVAL* input) {
	CMD_GET_ONOFFTIME* value = (CMD_GET_ONOFFTIME*)output;
	ZVAL_ARRAY_FOREACH_BEGIN(input)
		
    UCN_PACK_MEMBER(value, UINT32, timestamp);
    ZVAL_ARRAY_FOREACH_END(input)
		
	return sizeof(CMD_GET_ONOFFTIME);
}

int uc_pack_rfgw_tlv_defense(u_int8_t* output, ZVAL* input) {

	CMD_SET_DEFENSE* value = (CMD_SET_DEFENSE*)output;
	ZVAL_ARRAY_FOREACH_BEGIN(input)
    UCN_PACK_MEMBER(value, UINT8, is_defense);
    ZVAL_ARRAY_FOREACH_END(input)
	value->resv = 0;
	return sizeof(CMD_SET_DEFENSE);

}

int uc_pack_rfgw_tt_tlv_value(u_int8_t* output, ZVAL* input, u_int8_t type) {
	u_int8_t len = 0;
	switch(type){
		case SMTDLOCK_CMD_GET_SUMMARY:
			len = uc_pack_rfgw_tlv_summary(output,input);
			break;
		case SMTDLOCK_CMD_GET_ONOFFTIME:
			len = uc_pack_rfgw_tlv_onofftime(output,input);
			break;
		case SMTDLOCK_CMD_SET_DEFENSE:
			len = uc_pack_rfgw_tlv_defense(output,input);
			break;
		case SMTDLOCK_CMD_SET_AUTO_DEFENSE:
			{
				CMD_SET_AUTO_DEFENSE* value = (CMD_SET_AUTO_DEFENSE*)output;
				ZVAL_ARRAY_FOREACH_BEGIN(input)
			    UCN_PACK_MEMBER(value, UINT8, status);
				UCN_PACK_MEMBER(value, UINT8, enable);
				UCN_PACK_MEMBER(value, UINT8, starthour);
				UCN_PACK_MEMBER(value, UINT8, endhour);
			    ZVAL_ARRAY_FOREACH_END(input)
				len = sizeof(CMD_SET_AUTO_DEFENSE);
			}
			break;
		default:
			break;
	}
	return len;
}

int uc_pack_rfgw_tt_tlv(u_int8_t* output, ZVAL* input) {
	RF_TT_TLV_HEAD* rf_tlv_head = (RF_TT_TLV_HEAD*)output;
	u_int8_t len = 0;
	
	ZVAL_ARRAY_FOREACH_BEGIN(input)
    UCN_PACK_MEMBER(rf_tlv_head, UINT8, type);
    ZVAL_ARRAY_FOREACH_END(input)

	len = uc_pack_rfgw_tt_tlv_value(output+sizeof(RF_TT_TLV_HEAD), input, rf_tlv_head->type);

	rf_tlv_head->len = len;

	return len + sizeof(RF_TT_TLV_HEAD);
}

UC_PACK_FUNCTION(UC_REQUEST, UCA_GET,UCOT_IA,UCSOT_IA_RFGW,UCAT_IA_RFGW_TT) {
    ZVAL* sn = NULL;
	ZVAL* tlv = NULL;
	ZVAL* contain[UC_MAX_CONTROL] = {0};
	u_int32_t cmd_count = 0;
	u_int16_t length = 0;
	u_int32_t index  = 0;
	u_int8_t* tlvp = 0;
	u_int16_t tlv_len = 0;
	
    ZVAL_ARRAY_FOREACH_BEGIN(input)
    UC_ZEND_HASH_FIND(sn, "sn");
	UC_ZEND_HASH_FIND(tlv, "tlv");
    ZVAL_ARRAY_FOREACH_END(input)

	cmd_count = uc_control_pack_check_cmd(contain, tlv);
	tlvp = output + sizeof(IA_RFGW_TT);

	for(index=0; index < cmd_count; index ++) {
		tlv_len = uc_pack_rfgw_tt_tlv(tlvp, contain[index]);
		tlvp += tlv_len;
		length += tlv_len;
	}

    return uc_pack_rfgw_tt_head(output, sn, length) + length;
}

UC_PACK_FUNCTION(UC_REQUEST, UCA_SET,UCOT_IA,UCSOT_IA_RFGW,UCAT_IA_RFGW_TT) {
	UC_PACK_EXEC(UC_REQUEST, UCA_GET,UCOT_IA,UCSOT_IA_RFGW,UCAT_IA_RFGW_TT);
}

//重启设备，仅支持set操作，1表示关机
void uc_show_zval(ZVAL *pval, char* key) {

    if (key != NULL) {
        php_printf("Key:%s   Type:", key);
    }
    switch (Z_TYPE_P(pval)) {
        case IS_NULL:
            php_printf("null  Value:null ");break;
        case IS_BOOL:
            php_printf("bool  Value: %s ", Z_LVAL_P(pval) ? "TRUE" : "FALSE");break;
        case IS_LONG:
            php_printf("long  Value: %ld ", Z_LVAL_P(pval));break;
        case IS_DOUBLE:
            php_printf("double Value: %.0f ", Z_DVAL_P(pval));break;
        case IS_STRING:
            php_printf("string  Value: ");UC_WRITE(Z_STRVAL_P(pval), Z_STRLEN_P(pval));
            php_printf(" ");break;
        case IS_RESOURCE:
            php_printf("resource  Value: resource");break;
        case IS_ARRAY:
            php_printf("array  Value:array ");break;
        case IS_OBJECT:
            php_printf("object  Value:object ");break;
        default:
            php_printf("unknown  Value:unkown");break;
    }
    php_printf("\n");
}

UC_PACK_FUNCTION(UC_REQUEST, UCA_SET, UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_REBOOT) {
    typedef struct {
        u_int8_t reboot;
    }__attribute__ ((packed))ucp_sys_reboot_t;
    ucp_sys_reboot_t* reboot = (ucp_sys_reboot_t*)output;
    reboot->reboot = 1;
    
    ZVAL_ARRAY_FOREACH_BEGIN(input)
    UC_PACK_MEMBER(reboot, UINT8, reboot);
    ZVAL_ARRAY_FOREACH_END(input)

    return sizeof(ucp_sys_reboot_t);
}

UC_PACK_FUNCTION(UC_REQUEST, UCA_SET, UCOT_IA, UCSOT_IA_AC, UCAT_AC_ON_USER_SET) {
    uc_tlv_t* tlv = (uc_tlv_t*)output;
    ZVAL* value = NULL;
    ZVAL_ARRAY_FOREACH_BEGIN(input)
        
    UCN_PACK_MEMBER(tlv, UINT16, type);
    UC_ZEND_HASH_FIND(value, "vl");
    
    ZVAL_ARRAY_FOREACH_END(input)
        
    tlv->len = htons((u_int16_t)sizeof(userrepair_t)); 

    userrepair_t* user_pair = (uc_tlv_t*)(tlv + 1);
    if (value != NULL) {
        switch(ntohs(tlv->type)) {
        case UCT_IA_USERSET_ONOFFSAME ://开关同码
        case UCT_IA_USERSET_WINDDIRSAME ://扫风同码
            //user_pair->vl[1]=(int8_t)Z_LVAL_P(value);
            UCN_PACK_INT8(user_pair->vl[1], value);
            break;
        case UCT_IA_USERSET_WIND_REVERSE ://风力反了
        case UCT_IA_USERSET_ENVTEMP_REPAIR://修正环境温度因子
        case UCT_IA_USERSET_ELE_REPAIR://修正电量因子
        case UCT_IA_USERSET_ELE_CLEAR:
            //user_pair->vl[0]=(int8_t)Z_LVAL_P(value);
            UCN_PACK_INT8(user_pair->vl[0], value);
            break;

        }
    } else {
        UC_LOG("User set: falied find value\n");
    }

    return sizeof(uc_tlv_t) + sizeof(userrepair_t);
}

UC_PACK_FUNCTION(UC_REQUEST, UCA_SET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_PROCESS) {
    ucp_code_process_request_t* match = (ucp_code_process_request_t*)output;
    match->action = 1;
    match->timeout = 30;
    ZVAL_ARRAY_FOREACH_BEGIN(input)
    UCN_PACK_MEMBER(match, UINT8, action);
    UCN_PACK_MEMBER(match, UINT8, timeout);
    UCN_PACK_MEMBER(match, UINT16, id);
    ZVAL_ARRAY_FOREACH_END(input)
        
    return sizeof(ucp_ia_matchcode_t);
}

UC_PACK_FUNCTION(UC_REQUEST, UCA_GET, UCOT_IA, UCSOT_IA_CODE, UCAT_CODE_PROCESS) {
    ucp_code_process_request_t* match = (ucp_code_process_request_t*)output;

    ZVAL_ARRAY_FOREACH_BEGIN(input)
    UCN_PACK_MEMBER(match, UINT8, action);
    UCN_PACK_MEMBER(match, UINT8, timeout);
    UCN_PACK_MEMBER(match, UINT16, id);
    ZVAL_ARRAY_FOREACH_END(input)
        
    return sizeof(ucp_code_process_request_t);

}

UC_PACK_FUNCTION(UC_REQUEST, UCA_SET, UCOT_IA, UCSOT_IA_PUBLIC, UCAT_IA_PUBLIC_EXTENDED_TIMER) {
    ucp_ac_extended_timer_item_t* timer = (ucp_ac_extended_timer_item_t*)output;
	ZVAL* extended_data = NULL;
    
    /*解析ucp_ac_extended_timer_t*/
    timer->id       = 0;/*默认添加定时器*/
    timer->duration = 0Xffff;/*默认非工作时间段定时器*/
    ZVAL_ARRAY_FOREACH_BEGIN(input)

    UCN_PACK_MEMBER(timer, UINT8,  id);
    UCN_PACK_MEMBER(timer, UINT8,  hour);
    UCN_PACK_MEMBER(timer, UINT8,  minute);
    UCN_PACK_MEMBER(timer, WEEK,  week);
    UCN_PACK_MEMBER(timer, UINT8,  enable);
    UCN_PACK_MEMBER(timer, UINT8,  onoff);
    UCN_PACK_MEMBER(timer, UINT16, duration);
    UC_ZEND_HASH_FIND(extended_data, "extended_data");
    
    ZVAL_ARRAY_FOREACH_END(input)

    u_int32_t* extended_len = (u_int32_t*)(timer + 1);
    /**extended_len = htonl(0);
    if (extended_data == NULL) {
        return sizeof(ucp_ac_extended_timer_item_t) + sizeof(u_int32_t);
    }*/

    /*解析extended data*/
    *extended_len = htonl(8);
    ucp_ac_work_t* work = (ucp_ac_work_t*)(extended_len + 1);
    work->mode   = 0Xff;
    work->temp   = 0Xff;
    work->wind   = 0Xff;
    work->direct = 0Xff;
    ZVAL_ARRAY_FOREACH_BEGIN(extended_data)

    UCN_PACK_MEMBER(work, UINT8, onoff);
    UCN_PACK_MEMBER(work, UINT8, mode);
    UCN_PACK_MEMBER(work, UINT8, temp);
    UCN_PACK_MEMBER(work, UINT8, wind);
    UCN_PACK_MEMBER(work, UINT8, direct);
    UCN_PACK_MEMBER(work, UINT8, key);
    
    ZVAL_ARRAY_FOREACH_END(extended_data)

    return sizeof(ucp_ac_extended_timer_item_t) + sizeof(u_int32_t) + 8;

}

UC_PACK_FUNCTION(UC_REQUEST, UCA_DELETE, UCOT_IA, UCSOT_IA_PUBLIC, UCAT_IA_PUBLIC_EXTENDED_TIMER) {
    typedef struct {
        u_int8_t id;
    }__attribute__ ((packed))ucp_ac_delete_extended_timer_t;
    
    ucp_ac_delete_extended_timer_t* tmp = (ucp_ac_delete_extended_timer_t*)output;
	
    ZVAL_ARRAY_FOREACH_BEGIN(input)
    UCN_PACK_MEMBER(tmp, UINT8, id);
    ZVAL_ARRAY_FOREACH_END(input)

    return sizeof(ucp_ac_delete_extended_timer_t);

}


UC_PACK_FUNCTION(UC_REQUEST, UCA_SET, UCOT_IA, UCSOT_IA_PUBLIC, UCAT_IA_PUBLIC_TMP_CURVE) {
    tmp_header_t* tmp_hdr = (tmp_header_t*)output;
    ZVAL* node_data = NULL;

    /*默认设置*/
    tmp_hdr->id          = 1;
    tmp_hdr->time_period = 60;

	ZVAL_ARRAY_FOREACH_BEGIN(input)
    UCN_PACK_MEMBER(tmp_hdr, UINT8, id);
    UCN_PACK_MEMBER(tmp_hdr, UINT8, enable);
    UCN_PACK_MEMBER(tmp_hdr, WEEK, week);
    UCN_PACK_MEMBER(tmp_hdr, UINT8, begin_hour);
    UCN_PACK_MEMBER(tmp_hdr, UINT8, end_hour);
    UCN_PACK_MEMBER(tmp_hdr, UINT8, time_period);
    UCN_PACK_MEMBER(tmp_hdr, UINT8, count);
    UCN_PACK_MEMBER(tmp_hdr, UINT8, pad);
    UC_ZEND_HASH_FIND(node_data, UC_DATA);

	ZVAL_ARRAY_FOREACH_END(input)
    
    if (tmp_hdr->count <= 0 || node_data == NULL) {
        return sizeof(tmp_header_t);
    }
	tmp_curve_t *tmp_cur = (tmp_curve_t *)(tmp_hdr + 1);
    u_int32_t count = zend_hash_num_elements(HASH_OF(node_data));
        
    size_t index = 0;
    ZVAL **node_array = NULL;
    for (index = 0; index < count; index++) {
        node_array = NULL;
        if (zend_hash_index_find(HASH_OF(node_data), index, (void**)&node_array) != SUCCESS) {
            //php_printf("zend_hash_index_find FALIED\n");
            continue;
        }
        ZVAL_ARRAY_FOREACH_BEGIN(*node_array)
            UC_PACK_MEMBER(tmp_cur, UINT8, action);
            UC_PACK_MEMBER(tmp_cur, UINT8, tmp);
            UC_PACK_MEMBER(tmp_cur, UINT8, wind);
            UC_PACK_MEMBER(tmp_cur, UINT8, dir);
        ZVAL_ARRAY_FOREACH_END(*node_array)
            tmp_cur++;
    }
            
    if (count != tmp_hdr->count) {
        tmp_hdr->count = count;
    }
        
    return sizeof(tmp_header_t) + count * sizeof(tmp_curve_t);
}

/*通知设备进入红外匹配状态*/
UC_PACK_FUNCTION(UC_REQUEST, UCA_SET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_MATCH) {
	ucp_ucat_code_match_r* match_set = (ucp_ucat_code_match_r*)output;

	match_set->reserved = 0;
	match_set->reverved1 = 0;
	match_set->reverved2 = 0;
	match_set->skey = 0;
	
	ZVAL_ARRAY_FOREACH_BEGIN(input)
	
	UCN_PACK_MEMBER(match_set, UINT8, action);
	UCN_PACK_MEMBER(match_set, UINT8, timeout);
	UCN_PACK_MEMBER(match_set, UINT8, id);
	UCN_PACK_MEMBER(match_set, UINT8, type);
	UCN_PACK_MEMBER(match_set, UINT8, devid);
	
	ZVAL_ARRAY_FOREACH_END(input)
	return sizeof(ucp_ucat_code_match_r);
}

/*获取红外匹配状态*/
UC_PACK_FUNCTION(UC_REQUEST, UCA_GET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_MATCH_STATE) {
	
	return 0;
}

/*查询遥控器信息*/
UC_PACK_FUNCTION(UC_REQUEST, UCA_GET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_INFO) {
    return 0;
}

/*设置遥控器信息*/
UC_PACK_FUNCTION(UC_REQUEST, UCA_SET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_INFO) {

	ucp_ucat_code_info_g *conde_info = (ucp_ucat_code_info_g *)output;
	conde_info->reverted0 = 0;
	conde_info->reverted1 = 0;
	ZVAL_ARRAY_FOREACH_BEGIN(input)
	UCN_PACK_MEMBER(conde_info, UINT8, type);
    UCN_PACK_MEMBER(conde_info, UINT8, devid);
    UCN_PACK_MEMBER(conde_info, UINT16, irid);
    UCN_PACK_MEMBER(conde_info, UINT8, flags);
	UCN_PACK_MEMBER(conde_info, STRING, name);
	ZVAL_ARRAY_FOREACH_END(input)   
    return sizeof(ucp_ucat_code_info_g);
}

int pack_infrared_key_info(u_int8_t* output, ZVAL* input)
{
	ucp_ucat_key_c* infrared_key = (ucp_ucat_key_c*)output;
	infrared_key->reserved = 0;
	
	ZVAL_ARRAY_FOREACH_BEGIN(input)
		
	UCN_PACK_MEMBER(infrared_key, UINT8, type);
	UCN_PACK_MEMBER(infrared_key, UINT8, key);
	UCN_PACK_MEMBER(infrared_key, UINT16, irid);
    UCN_PACK_MEMBER(infrared_key, UINT8, devid);
    UCN_PACK_MEMBER(infrared_key, UINT8, flags);
	UCN_PACK_MEMBER(infrared_key, STRING, name);

	ZVAL_ARRAY_FOREACH_END(input)
		
    return sizeof(ucp_ucat_key_c);
}

/*设置遥控器按键信息*/
UC_PACK_FUNCTION(UC_REQUEST, UCA_SET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_KEY) {
    return pack_infrared_key_info(output, input);
}

/*删除遥控器按键信息*/
UC_PACK_FUNCTION(UC_REQUEST, UCA_DELETE,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_KEY) {
    return pack_infrared_key_info(output, input);
}

/*查询遥控器按键信息*/
UC_PACK_FUNCTION(UC_REQUEST, UCA_GET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_KEY) {   
    return 0;
}

/*遥控器按键学习*/
UC_PACK_FUNCTION(UC_REQUEST, UCA_SET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_LEARN) {   
	ucp_ucat_key_learn* key_learn = (ucp_ucat_key_learn*)output;
	key_learn->reserved = 0;
	
	ZVAL_ARRAY_FOREACH_BEGIN(input)
		
	UCN_PACK_MEMBER(key_learn, UINT8, type);
	UCN_PACK_MEMBER(key_learn, UINT8, key);
    UCN_PACK_MEMBER(key_learn, UINT8, devid);
	UCN_PACK_MEMBER(key_learn, UINT8, action);

	ZVAL_ARRAY_FOREACH_END(input)
		
    return sizeof(ucp_ucat_key_learn);
}

/*取消遥控器按键学习*/
UC_PACK_FUNCTION(UC_REQUEST, UCA_DELETE,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_LEARN) {   
    return 0;
}

/*遥控器按键学习状态查询*/
UC_PACK_FUNCTION(UC_REQUEST, UCA_PUSH,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_LEARN) {   
    return 0;
}

/*遥控器按键学习状态查询*/
UC_PACK_FUNCTION(UC_REQUEST, UCA_GET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_LEARN) {   
    return 0;
}


/*遥控器按键控制*/
UC_PACK_FUNCTION(UC_REQUEST, UCA_SET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_CTRL) {   
	ucp_ucat_key_ctrl* key_ctrl = (ucp_ucat_key_ctrl*)output;
	key_ctrl->reserved = 0;
	key_ctrl->reserved1 = 0;
	
	ZVAL_ARRAY_FOREACH_BEGIN(input)
		
	UCN_PACK_MEMBER(key_ctrl, UINT8, type);
	UCN_PACK_MEMBER(key_ctrl, UINT8, key);
	UCN_PACK_MEMBER(key_ctrl, UINT16, irid);
    UCN_PACK_MEMBER(key_ctrl, UINT8, devid);
	UCN_PACK_MEMBER(key_ctrl, UINT8, flags);

	ZVAL_ARRAY_FOREACH_END(input)
		
    return sizeof(ucp_ucat_key_ctrl);
}

/*
UC_PKDF_FENTRY:  Udp control pack default fuction entry
UC_PACK_FENTRY:  Udp control pack fuction entry
*/

uc_control_t g_uc_pack_control[UC_MAX_CONTROL] = {
UC_PKDF_FENTRY(UC_REQUEST, UCA_GET, UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCOT_GET_ALL_ATTRI),
UC_PKDF_FENTRY(UC_REQUEST, UCA_GET, UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_SSID),
UC_PKDF_FENTRY(UC_REQUEST, UCA_GET, UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_VERSION),
UC_PKDF_FENTRY(UC_REQUEST, UCA_GET, UCOT_IA,     UCOT_GET_ALL_ATTRI, UCOT_GET_ALL_ATTRI),
UC_PKDF_FENTRY(UC_REQUEST, UCA_GET, UCOT_IA,     UCSOT_IA_STAT,      UCAT_STAT_PERIOD_TIME),
UC_PKDF_FENTRY(UC_REQUEST, UCA_GET, UCOT_IA,     UCSOT_IA_AC,        UCAT_AC_WORK),

UC_PKDF_FENTRY(UC_REQUEST, UCA_PUSH,UCOT_IA,	 UCSOT_IA_CODE,		 UCAT_CODE_RC_LEARN),
UC_PKDF_FENTRY(UC_REQUEST, UCA_GET,	UCOT_IA,	 UCSOT_IA_CODE,		 UCAT_CODE_RC_LEARN),
UC_PKDF_FENTRY(UC_REQUEST, UCA_DELETE,UCOT_IA,	 UCSOT_IA_CODE,		 UCAT_CODE_RC_LEARN),

UC_PKDF_FENTRY(UC_REQUEST, UCA_GET,	   UCOT_IA,	 	UCSOT_IA_CODE,		UCAT_CODE_RC_MATCH_STATE),
UC_PKDF_FENTRY(UC_REQUEST, UCA_GET,	   UCOT_IA,     UCSOT_IA_CODE,		UCAT_CODE_RC_INFO),
UC_PKDF_FENTRY(UC_REQUEST, UCA_GET,	   UCOT_IA,		UCSOT_IA_CODE,		UCAT_CODE_RC_KEY),

UC_PACK_FENTRY(UC_REQUEST, UCA_SET,    UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_REBOOT),
UC_PACK_FENTRY(UC_REQUEST, UCA_SET,    UCOT_IA,     UCSOT_IA_AC,        UCAT_AC_ON_USER_SET),
UC_PACK_FENTRY(UC_REQUEST, UCA_SET,    UCOT_IA,     UCSOT_IA_PUBLIC,    UCAT_IA_PUBLIC_TMP_CURVE),
UC_PACK_FENTRY(UC_REQUEST, UCA_SET,    UCOT_IA,     UCSOT_IA_PUBLIC,    UCAT_IA_PUBLIC_EXTENDED_TIMER),
UC_PACK_FENTRY(UC_REQUEST, UCA_DELETE, UCOT_IA, 	UCSOT_IA_PUBLIC, 	UCAT_IA_PUBLIC_EXTENDED_TIMER),

UC_PACK_FENTRY(UC_REQUEST, UCA_SET,    UCOT_IA,     UCSOT_IA_CODE,      UCAT_CODE_PROCESS),
UC_PACK_FENTRY(UC_REQUEST, UCA_GET,    UCOT_IA,     UCSOT_IA_CODE,      UCAT_CODE_PROCESS),

UC_PACK_FENTRY(UC_REQUEST, UCA_SET,	   UCOT_IA,		UCSOT_IA_CODE,		UCAT_CODE_RC_MATCH),
UC_PACK_FENTRY(UC_REQUEST, UCA_SET,	   UCOT_IA,		UCSOT_IA_CODE,		UCAT_CODE_RC_INFO),
UC_PACK_FENTRY(UC_REQUEST, UCA_DELETE, UCOT_IA,		UCSOT_IA_CODE,		UCAT_CODE_RC_KEY),
UC_PACK_FENTRY(UC_REQUEST, UCA_SET,	   UCOT_IA,		UCSOT_IA_CODE,		UCAT_CODE_RC_KEY),
UC_PACK_FENTRY(UC_REQUEST, UCA_SET,	   UCOT_IA,		UCSOT_IA_CODE,		UCAT_CODE_RC_LEARN),
UC_PACK_FENTRY(UC_REQUEST, UCA_SET,	   UCOT_IA,		UCSOT_IA_CODE,		UCAT_CODE_RC_CTRL),
UC_PACK_FENTRY(UC_REQUEST, UCA_GET,	   UCOT_IA,		UCSOT_IA_RFGW,		UCAT_IA_RFGW_TT),
UC_PACK_FENTRY(UC_REQUEST, UCA_SET,	   UCOT_IA,		UCSOT_IA_RFGW,		UCAT_IA_RFGW_TT),

{0, 0, 0, 0, NULL},
};
