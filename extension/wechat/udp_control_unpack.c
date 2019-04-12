#include "udp_control.h"

/*
函数原型通配 :注意输入都为网络序
int function_unpack(ZVAL* output, ucp_obj_t* input)

@param  output 类型: 为PHP数组
		output 说明: 解析报文到关联数组output
             
@param  input 类型: 为ucp_obj_t-udp control命令通用头部
		input 说明:  收到的UDP报文       
             
@return 报文处理结果码；= 0 解析成功；<=0 表示解析失败

*/

//RF网关透传
int uc_unpack_rfgw_tlv_autodefense(ZVAL* output, u_int8_t* input){
	CMD_PUSH_AUTO_DEFENSE* tlv_value1= (CMD_PUSH_AUTO_DEFENSE*)input;
	UCN_ASSOC_MEMBER_UINT8(output, tlv_value1, don_enable);
	UCN_ASSOC_MEMBER_UINT8(output, tlv_value1, doff_enable);
	UCN_ASSOC_MEMBER_UINT8(output, tlv_value1, don_starthour);
	UCN_ASSOC_MEMBER_UINT8(output, tlv_value1, don_endhour);
	UCN_ASSOC_MEMBER_UINT8(output, tlv_value1, doff_starthour);
	UCN_ASSOC_MEMBER_UINT8(output, tlv_value1, doff_endhour);

	return sizeof(CMD_PUSH_AUTO_DEFENSE);
}

int uc_unpack_rfgw_tt_tlv_value(ZVAL* output, u_int8_t* input){
	RF_TT_TLV_HEAD* tlv_head = (RF_TT_TLV_HEAD*)input;
	u_int8_t* tlv_value_p = (u_int8_t*)(tlv_head + 1);
	UCN_ASSOC_MEMBER_UINT8(output, tlv_head, type);
	
	switch(tlv_head->type){
		case SMTDLOCK_CMD_GET_SUMMARY:
			{
				CMD_PUSH_SUMMARY* tlv_value = (CMD_PUSH_SUMMARY*)(tlv_head + 1);
				UCN_ASSOC_MEMBER_UINT8(output, tlv_value, hw_ver);
				UCN_ASSOC_MEMBER_UINT8(output, tlv_value, abc_battery);
				UCN_ASSOC_MEMBER_UINT8(output, tlv_value, abc_battery);
				UC_ADD_ASSOC_LONG(output, "d_onoff", ntohl((tlv_value->flagbits).d_onoff));
				UC_ADD_ASSOC_LONG(output, "b_warn", ntohl((tlv_value->flagbits).b_warn));
				UC_ADD_ASSOC_LONG(output, "pry_d", ntohl((tlv_value->flagbits).pry_d));
				UC_ADD_ASSOC_LONG(output, "arlming", ntohl((tlv_value->flagbits).arlming));
				//UCN_ASSOC_MEMBER_UINT32(output, tlv_value->flagbits), d_onoff);
				//UCN_ASSOC_MEMBER_UINT32(output, &(tlv_value->flagbits), b_warn);
				//UCN_ASSOC_MEMBER_UINT32(output, &(tlv_value->flagbits), pry_d);
				//UCN_ASSOC_MEMBER_UINT32(output, &(tlv_value->flagbits), arlming);
				UC_ALLOC_ARRAY_INIT_ZVAL(soft_ver);
				UC_ADD_ASSOC_LONG(soft_ver, "rev", (tlv_value->soft_ver).rev);
				UC_ADD_ASSOC_LONG(soft_ver, "min_v", (tlv_value->soft_ver).min_v);
				UC_ADD_ASSOC_LONG(soft_ver, "mar", (tlv_value->soft_ver).mar);
				//UCN_ASSOC_MEMBER_UINT8(soft_ver, &(tlv_value->soft_ver), rev);
				//UCN_ASSOC_MEMBER_UINT8(soft_ver, &(tlv_value->soft_ver), min_v);
				//UCN_ASSOC_MEMBER_UINT8(soft_ver, &(tlv_value->soft_ver), mar);
				UC_ADD_ASSOC_ZVAL(output, "soft_ver", soft_ver);
			}
			break;
		case SMTDLOCK_CMD_GET_ONOFFTIME:
			{
				CMD_PUSH_ONOFFTIME* tlv_value= (CMD_PUSH_ONOFFTIME*)(tlv_head + 1);
				u_int32_t flag = 1, index=0;
				CMD_DOOR_ONOFF_STATUS onoff_status;
				u_int32_t timestamp;
				UC_ALLOC_ARRAY_INIT_ZVAL(onoff_time);
				for(flag=1; flag <=4; flag ++){
					switch(flag){
						case 1:
							onoff_status = tlv_value->on_off_1;
							timestamp = tlv_value->timestamp_1;
							break;
						case 2:
							onoff_status = tlv_value->on_off_2;
							timestamp = tlv_value->timestamp_2;
							break;
						case 3:
							onoff_status = tlv_value->on_off_3;
							timestamp = tlv_value->timestamp_3;
							break;
						case 4:
							onoff_status = tlv_value->on_off_4;
							timestamp = tlv_value->timestamp_4;
							break;
						default:
							continue;
					}
						
					if (onoff_status.time_flag == 1) {
						UC_ALLOC_ARRAY_INIT_ZVAL(sub_zval);

						UC_ADD_ASSOC_LONG(sub_zval, "timestamp", ntohl(timestamp));

						UC_ADD_ASSOC_LONG(sub_zval, "value", onoff_status.value);
						UC_ADD_ASSOC_LONG(sub_zval, "type", onoff_status.type);

						//UCN_ASSOC_MEMBER_UINT8(sub_zval, &(tlv_value->STRING_CAT(on_off_,flag)), value);
						//UCN_ASSOC_MEMBER_UINT8(sub_zval, &(tlv_value->STRING_CAT(on_off_,flag)), type);

						UC_ADD_INDEX_ZVAL(onoff_time, index, sub_zval);
						index ++;
					}
				}
				UC_ADD_ASSOC_ZVAL(output, "onoff_time", onoff_time);
			}
			break;
		case SMTDLOCK_CMD_SET_AUTO_DEFENSE:
			uc_unpack_rfgw_tlv_autodefense(output, tlv_value_p);
			break;
		default:
			break;
	}

	return tlv_head->len + sizeof(RF_TT_TLV_HEAD);
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET,UCOT_IA,UCSOT_IA_RFGW,UCAT_IA_RFGW_TT) {
    IA_RFGW_TT* rfgw_tt = (IA_RFGW_TT*)(input + 1);
	u_int16_t length = 0;
	u_int16_t len = 0;
	u_int8_t index = 0;
	u_int8_t* tlvp = (u_int8_t*)(rfgw_tt + 1);
    UCN_ASSOC_MEMBER_UINT64(output, rfgw_tt, sn);
    UC_ALLOC_ARRAY_INIT_ZVAL(tlv);
    while(length < rfgw_tt->length) {
        UC_ALLOC_ARRAY_INIT_ZVAL(sub_zval);
		len = uc_unpack_rfgw_tt_tlv_value(sub_zval, tlvp);
        UC_ADD_INDEX_ZVAL(tlv, index, sub_zval);
		index ++;
		length += len;
		tlvp += len;
    }

    UC_ADD_ASSOC_ZVAL(output, "tlv", tlv);

    return E_SUCCESS;
}


UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_SSID) {
    u_int8_t* ssid = (u_int8_t*)(input + 1);

    UCN_ADD_ASSOC_STRING(output, "ssid", ssid, 1);

    return E_SUCCESS;
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET,  UCOT_SYSTEM,  UCSOT_SYS_SOFTWARE, UCAT_SYS_PASSWD) {
    u_int8_t* passwd = (u_int8_t*)(input + 1);

    UCN_ADD_ASSOC_STRING(output, "passwd", passwd, 1);

    return E_SUCCESS;
}


UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_IA, UCSOT_IA_AC, UCAT_AC_WORK) {
    ucp_ac_work_t* air_work = (ucp_ac_work_t*)(input + 1);

    UCN_ASSOC_MEMBER_UINT8(output, air_work, onoff);
    UCN_ASSOC_MEMBER_UINT8(output, air_work, mode);
    UCN_ASSOC_MEMBER_UINT8(output, air_work, temp);
    UCN_ASSOC_MEMBER_UINT8(output, air_work, wind);
    UCN_ASSOC_MEMBER_UINT8(output, air_work, direct);
    UCN_ASSOC_MEMBER_UINT8(output, air_work, key);

    return E_SUCCESS;
}


UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_IA, UCSOT_IA_PUBLIC, 
        UCAT_IA_PUBLIC_TMP_CURVE) {
    tmp_header_t* tmp_hdr = (tmp_header_t*)(input + 1);

    UCN_ASSOC_MEMBER_UINT8(output, tmp_hdr, id);
    UCN_ASSOC_MEMBER_UINT8(output, tmp_hdr, enable);
    UCN_ASSOC_MEMBER_WEEK(output, tmp_hdr, week);
    UCN_ASSOC_MEMBER_UINT8(output, tmp_hdr, begin_hour);
    UCN_ASSOC_MEMBER_UINT8(output, tmp_hdr, end_hour);
    UCN_ASSOC_MEMBER_UINT8(output, tmp_hdr, time_period);
    UCN_ASSOC_MEMBER_UINT8(output, tmp_hdr, count);
    UCN_ASSOC_MEMBER_UINT8(output, tmp_hdr, pad);
    
    if (tmp_hdr->count <= 0) {
        return E_SUCCESS;
    }

    if ((tmp_hdr->count * sizeof(tmp_curve_t) + sizeof(tmp_header_t)) 
            != ntohs(input->param_len)) {
        UC_LOG("ucat_ia_tmp_curve_unpack size is not correct\n");
        return E_INVALID_LENGTH;
    }

    u_int32_t index = 0;
    UC_ALLOC_ARRAY_INIT_ZVAL(data_zval);
    tmp_curve_t* tmp_cur = (tmp_curve_t*)(tmp_hdr + 1);
    for (index = 0; index < tmp_hdr->count; index++) {
        UC_ALLOC_ARRAY_INIT_ZVAL(sub_zval);
        
        UCN_ASSOC_MEMBER_UINT8(sub_zval, tmp_cur, action);
        UCN_ASSOC_MEMBER_UINT8(sub_zval, tmp_cur, tmp);
        UCN_ASSOC_MEMBER_UINT8(sub_zval, tmp_cur, wind);
        UCN_ASSOC_MEMBER_UINT8(sub_zval, tmp_cur, dir);

        UC_ADD_INDEX_ZVAL(data_zval, index, sub_zval);

        tmp_cur++;
    }

    UC_ADD_ASSOC_ZVAL(output, UC_DATA, data_zval);
    return E_SUCCESS;
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_IA, UCSOT_IA_PUBLIC, UCAT_IA_PUBLIC_TMP_CURVE_UTC) {
    UC_UNPACK_EXEC(UC_ANSWER, UCA_GET, UCOT_IA, UCSOT_IA_PUBLIC, UCAT_IA_PUBLIC_TMP_CURVE);
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_VERSION) {
    u_int8_t* version = (u_int8_t*)(input + 1);
    
    UCN_ADD_ASSOC_VERSION(output, "version", version);
    return E_SUCCESS;
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_UPTIME) {
    u_int32_t* uptime = (u_int32_t*)(input + 1);

    UCN_ADD_ASSOC_UINT32(output, "uptime", *uptime);
    
    return E_SUCCESS;
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_SVN) {
    u_int32_t* svn = (u_int32_t*)(input + 1);
    
    UCN_ADD_ASSOC_UINT32(output, "svn", *svn);
    return E_SUCCESS;
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_IP) {
    u_int32_t* ip = (u_int32_t*)(input + 1);
    
    UC_ADD_ASSOC_IPV4(output, "ip", *ip);
    return E_SUCCESS;
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_SYSTEM, UCSOT_SYS_SERVER, UCAT_SERVER_CONNECT_TIME) {
    u_int32_t* connect_time = (u_int32_t*)(input + 1);

    UCN_ADD_ASSOC_UINT32(output, "connect_time", *connect_time);
    
    return E_SUCCESS;
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_SYSTEM,  UCSOT_SYS_UPGRADE,  UCOT_GET_ALL_ATTRI) {
    u_int8_t* version = (u_int8_t*)(input + 1);

    UCN_ADD_ASSOC_VERSION(output, "version", version);

    return E_SUCCESS;
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_SYSTEM,  UCSOT_SYS_UPGRADE,  UCAT_SYS_VERSION) {
    u_int8_t* version = (u_int8_t*)(input + 1);

    UCN_ADD_ASSOC_VERSION(output, "version", version);

    return E_SUCCESS;
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_SYSTEM,  UCSOT_SYS_HARDWARE, UCAT_SYS_VERSION) {
    u_int8_t* version = (u_int8_t*)(input + 1);

    UCN_ADD_ASSOC_VERSION(output, "version", version);

    return E_SUCCESS;
}


UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_SYSTEM,  UCSOT_SYS_SOFTWARE, UCAT_SYS_DEBUGINFO) {
    u_int16_t param_len = ntohs(input->param_len);
    u_int16_t cur_len = 0;
    /*预留此字段，防止有时候命令解析为空*/
    UCN_ADD_ASSOC_UINT32(output, "average_ad", 0);           
    UCN_ADD_ASSOC_UINT32(output, "max_ad", 0);
    UCN_ADD_ASSOC_UINT32(output, "cold_delay_pn_time", 0);
    UCN_ADD_ASSOC_UINT32(output, "cold_delay_pf_time", 0);
    UCN_ADD_ASSOC_UINT32(output, "hot_delay_pn_time", 0);
    UCN_ADD_ASSOC_UINT32(output, "hot_delay_pf_time", 0);

    uc_tlv_t * tlv = (uc_tlv_t*)(input + 1);
    for (cur_len = 0; cur_len < param_len;) {
        u_int8_t*  val8_p = tlv_val(tlv);
        u_int16_t* val16_p = (u_int16_t*)tlv_val(tlv);
        u_int32_t* val32_p = (u_int32_t*)tlv_val(tlv);
        switch(tlv_n_type(tlv)) {
        case DBG_TYPE_SVN:
            UCN_ADD_ASSOC_UINT32(output, "svn", *val32_p);break;
        case DBG_TYPE_CUR:
            UCN_ADD_ASSOC_UINT32(output, "current", *val32_p);break;
        case DBG_TYPE_CUR_AD:
            UCN_ADD_ASSOC_UINT32(output, "current_ad", *val32_p);break;
        case DBG_TYPE_CUR_K:
            UCN_ADD_ASSOC_UINT32(output, "current_k", *val32_p);break;
        case DBG_TYPE_CUR_B:
            UC_ADD_ASSOC_LONG(output, "current_b", (int32_t)ntohl(*val32_p));break;
        case DBG_TYPE_VOL:
            UCN_ADD_ASSOC_UINT32(output, "voltage", *val32_p);break;
        case DBG_TYPE_VOL_AD:
            UCN_ADD_ASSOC_UINT32(output, "voltage_ad", *val32_p);break;
        case DBG_TYPE_VOL_K:
            UCN_ADD_ASSOC_UINT32(output, "voltage_k", *val32_p);break;
        case DBG_TYPE_VOL_B:
            UCN_ADD_ASSOC_UINT32(output, "voltage_b", *val32_p);break;
        case DBG_TYPE_LIGHT_AD:
            UCN_ADD_ASSOC_UINT32(output, "light_ad", *val32_p);break;  
        case DBG_TYPE_LIGHT_STUDY:
        {
            light_smp_t* smp = (light_smp_t*)val8_p;
            UCN_ADD_ASSOC_UINT32(output, "light_yesterday", smp->yesterday);
            UCN_ADD_ASSOC_UINT32(output, "light_today", smp->today);
        }
            break; 
        case DBG_TYPE_CLIENTS:
            UCN_ADD_ASSOC_UINT32(output, "clients", *val32_p);break;
        case DBG_TYPE_UPTIME:
            UCN_ADD_ASSOC_UINT32(output, "uptime", *val32_p);break;
        case DBG_TYPE_SERVER_CONNTIME:
            UCN_ADD_ASSOC_UINT32(output, "connect_time", *val32_p);break;
        case DBG_TYPE_AVG_AD:
            UCN_ADD_ASSOC_UINT32(output, "average_ad", *val32_p);break;            
        case DBG_TYPE_MAX_AD:
            UCN_ADD_ASSOC_UINT32(output, "max_ad", *val32_p);break;
        case DBG_TYPE_PN_TIME:
            UCN_ADD_ASSOC_UINT32(output, "pn_time", *val32_p);break;
        case DBG_TYPE_NO_LOAD_AD:
            UCN_ADD_ASSOC_UINT32(output, "noload_ad", *val32_p);break;
        case DBG_TYPE_SMT_SOFT_VER:
            UCN_ADD_ASSOC_VERSION(output, "smt_soft_verion", val8_p);break;
        case DBG_TYPE_SMT_HARD_VER:
            UCN_ADD_ASSOC_VERSION(output, "smt_hard_verion", val8_p);break;
        case DBG_TYPE_IR_LIB_ID:
            UCN_ADD_ASSOC_UINT16(output, "irlib_id", *val16_p);break;
        case DBG_TYPE_COLD_DELAY_PN_TIME:
            UCN_ADD_ASSOC_UINT32(output, "cold_delay_pn_time", *val32_p);break;
        case DBG_TYPE_COLD_DELAY_PF_TIME:
            UCN_ADD_ASSOC_UINT32(output, "cold_delay_pf_time", *val32_p);break;
        case DBG_TYPE_HOT_DELAY_PN_TIME:
            UCN_ADD_ASSOC_UINT32(output, "hot_delay_pn_time", *val32_p);break;
        case DBG_TYPE_HOT_DELAY_PF_TIME:
            UCN_ADD_ASSOC_UINT32(output, "hot_delay_pf_time", *val32_p);break;
        case DGB_TYPE_STM32_INFO:
            UCN_ADD_ASSOC_STRING(output, "stm_info", val8_p, 1);break;
        default:
            //uc_control_unpack_unknow(output, NULL, tlv, tlv_n_len(tlv) + sizeof(uc_tlv_t));
            break;
        }
        cur_len += tlv_n_len(tlv) + sizeof(uc_tlv_t);
        tlv = tlv_n_next(tlv);
    }
    return E_SUCCESS;
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_IA, UCSOT_IA_PUBLIC, UCAT_IA_PUBLIC_EXTENDED_TIMER) {
    ucp_ac_extended_timer_t* timer = (ucp_ac_extended_timer_t*)(input + 1);
    u_int16_t param_len = ntohs(input->param_len);
    if (param_len < sizeof (ucp_ac_extended_timer_t)) {
        return E_FALIED;
    }
    
    UCN_ASSOC_MEMBER_UINT8(output,  timer, on_valid);
    UCN_ASSOC_MEMBER_UINT8(output,  timer, on_day);
    UCN_ASSOC_MEMBER_UINT8(output,  timer, on_hour);
    UCN_ASSOC_MEMBER_UINT8(output,  timer, on_min);
    UCN_ASSOC_MEMBER_UINT16(output, timer, on_duration);
    UCN_ASSOC_MEMBER_UINT8(output,  timer, off_day);
    UCN_ASSOC_MEMBER_UINT8(output,  timer, off_hour);
    UCN_ASSOC_MEMBER_UINT8(output,  timer, off_min);
    UCN_ASSOC_MEMBER_UINT16(output, timer, off_duration);

    UC_ALLOC_ARRAY_INIT_ZVAL(data_zval);
    u_int8_t* item_pp = (u_int8_t*)(timer + 1);
    size_t size = 0;
    for (size = sizeof(ucp_ac_extended_timer_t); size < param_len; ) {
        UC_ALLOC_ARRAY_INIT_ZVAL(sub_timer);
        ucp_ac_extended_timer_item_t* item = (ucp_ac_extended_timer_item_t*)item_pp;
 
        UCN_ASSOC_MEMBER_UINT8(sub_timer, item, id);
        UCN_ASSOC_MEMBER_UINT8(sub_timer, item, hour);
        UCN_ASSOC_MEMBER_UINT8(sub_timer, item, minute);
        UCN_ASSOC_MEMBER_WEEK( sub_timer, item, week);
        UCN_ASSOC_MEMBER_UINT8(sub_timer, item, enable);
        UCN_ASSOC_MEMBER_UINT8(sub_timer, item, onoff);
        UCN_ASSOC_MEMBER_UINT16(sub_timer, item, duration);
        
        u_int32_t* extended_len = (u_int32_t*)(item + 1);
        ucp_ac_work_t* work = (ucp_ac_work_t*)(extended_len + 1);
        if (ntohl(*extended_len) >= sizeof(ucp_ac_work_t)) {
            UC_ALLOC_ARRAY_INIT_ZVAL(extended_data);
            UCN_ASSOC_MEMBER_UINT8(extended_data, work, onoff);
            UCN_ASSOC_MEMBER_UINT8(extended_data, work, mode);
            UCN_ASSOC_MEMBER_UINT8(extended_data, work, temp);
            UCN_ASSOC_MEMBER_UINT8(extended_data, work, wind);
            UCN_ASSOC_MEMBER_UINT8(extended_data, work, direct);
            UCN_ASSOC_MEMBER_UINT8(extended_data, work, key);
            
            UC_ADD_ASSOC_ZVAL(sub_timer, "extended_data", extended_data);
        }

        UC_ADD_NEXT_INDEX_ZVAL(data_zval, sub_timer);
        
        /*ucp_ac_timer_item_t + pad + extended_len +extended_data*/
        item_pp += sizeof(ucp_ac_timer_item_t) +sizeof(u_int32_t) + ntohl(*extended_len);
        size += sizeof(ucp_ac_timer_item_t) + sizeof(u_int32_t) + ntohl(*extended_len);
    }

    UC_ADD_ASSOC_ZVAL(output, UC_DATA, data_zval);

    return E_SUCCESS;
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_IA, UCSOT_IA_AC,UCAT_AC_ON_USER_SET) {
    if (ntohs(input->param_len) % (sizeof(uc_tlv_t) + sizeof(userrepair_t)) != 0) {
        return E_FALIED;
    }
    size_t set_count = ntohs(input->param_len) / (sizeof(uc_tlv_t) + sizeof(userrepair_t));
    size_t index = 0;
    u_int8_t* pp = (u_int8_t*)(input + 1);
    for(index = 0; index < set_count; index++) {
        uc_tlv_t* tlv = (uc_tlv_t*)pp;
        userrepair_t* user_pair = (userrepair_t*)(tlv+1);
        switch(ntohs(tlv->type)) {
        case UCT_IA_USERSET_ONOFFSAME ://开关同码
            UC_ADD_ASSOC_LONG(output, "onoff_same", user_pair->vl[0]);
            break;
        case UCT_IA_USERSET_WINDDIRSAME ://扫风同码
            UC_ADD_ASSOC_LONG(output, "winddir_same", user_pair->vl[0]);
            break;
        case UCT_IA_USERSET_WIND_REVERSE ://风力反了
            UC_ADD_ASSOC_LONG(output, "wind_isreverse", user_pair->vl[0]);
            break;
        case UCT_IA_USERSET_ENVTEMP_REPAIR://修正环境温度因子
            UC_ADD_ASSOC_LONG(output, "envtemp_factor", user_pair->vl[0]);
            break;
        case UCT_IA_USERSET_ELE_REPAIR://修正电量因子
            UC_ADD_ASSOC_LONG(output, "ele_factor", (u_int8_t)user_pair->vl[0]);
            break;
        case UCT_IA_USERSET_ELE_CLEAR:
            UC_ADD_ASSOC_LONG(output, "ele_clear", user_pair->vl[0]);
            break;
        };
        pp += sizeof(uc_tlv_t) + sizeof(userrepair_t);
    }

    return E_SUCCESS;
}


UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_IA, UCSOT_IA_AC,UCAT_AC_CUR_TEMP) {
    u_int8_t* cur_tmp = (u_int8_t*)(input + 1);
    if (ntohs(input->param_len) != sizeof(u_int8_t)) {
        return E_FALIED;
    }
    
    UCN_ADD_ASSOC_UINT8(output, "cur_tmp", *cur_tmp);
    return E_SUCCESS;
}


UC_UNPACK_FUNCTION(UC_ANSWER, UCA_SET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_PROCESS) {
    if (ntohs(input->param_len) < sizeof(ucp_code_process_reply_t)) {
        return E_FALIED;
    }

    ucp_code_process_reply_t* match = (ucp_code_process_reply_t*)(input + 1);
    UCN_ASSOC_MEMBER_UINT8(output, match, action);
    UCN_ASSOC_MEMBER_UINT8(output, match, step_type);
    UCN_ASSOC_MEMBER_UINT8(output, match, step_count);
    UCN_ASSOC_MEMBER_UINT8(output, match, step_index);
    UCN_ASSOC_MEMBER_UINT8(output, match, err);
    UCN_ASSOC_MEMBER_UINT8(output, match, flagbits);
    //UCN_ASSOC_MEMBER_STRING(output, match, pad);

    return E_SUCCESS;
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_PUSH,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_PROCESS) {
    UC_UNPACK_EXEC(UC_ANSWER, UCA_SET, UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_PROCESS);
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_PROCESS) {
    UC_UNPACK_EXEC(UC_ANSWER, UCA_SET, UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_PROCESS);
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_IA, UCSOT_IA_AC,UCAT_AC_TMP_SAMPLE_CURVE) {
    size_t param_len = ntohs(input->param_len);
    u_int8_t* pp = (u_int8_t*)(input + 1);

    size_t temp_count = param_len >= 4 ? (param_len - 4) : 0;
    /*if ((temp_count + 4) != ntohs(input->param_len)) {
        return E_INVALID_LENGTH;
    }*/
    
    UCN_ADD_ASSOC_UINT8(output, "count", temp_count);
    size_t index = 0;
    UC_ALLOC_ARRAY_INIT_ZVAL(temp_list);
    u_int8_t* temp_p = (u_int8_t*)(pp + 4);
    for(index = 0; index < temp_count; index++) {
        UC_ADD_NEXT_INDEX_LONG(temp_list, temp_p[index]);
    }
    UC_ADD_ASSOC_ZVAL(output, UC_DATA, temp_list);

    return E_SUCCESS;
}

/*通知遥控器进入红外匹配返回*/
UC_UNPACK_FUNCTION(UC_ANSWER, UCA_SET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_MATCH) {
	UC_UNPACK_EXEC(UC_ANSWER, UCA_SET, UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_PROCESS);
	/*
	size_t param_len = ntohs(input->param_len);
    if (param_len != sizeof(ucp_ucat_code_match_r) - 4 )
		return E_FALIED;

	ucp_ucat_code_match_r* match_ret = (ucp_ucat_code_match_r*)(input + 1);
    UCN_ASSOC_MEMBER_UINT8(output, match_ret, action);
    UCN_ASSOC_MEMBER_UINT8(output, match_ret, timeout);
	UCN_ASSOC_MEMBER_UINT16(output, match_ret, id);
    UCN_ASSOC_MEMBER_UINT8(output, match_ret, type);
	UCN_ASSOC_MEMBER_UINT8(output, match_ret, devid);
    //UCN_ASSOC_MEMBER_UINT8(output, match_ret, skey);

    return E_SUCCESS;
    */
}

/*查询红外匹配状态*/
UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_MATCH_STATE) {
	size_t param_len = ntohs(input->param_len);
    if (param_len != sizeof(ucp_ucat_code_match_push))
		return E_FALIED;
	ucp_ucat_code_match_push* match_push = (ucp_ucat_code_match_push*)(input + 1);
	UCN_ASSOC_MEMBER_UINT8(output, match_push, action);
    UCN_ASSOC_MEMBER_UINT8(output, match_push, tep_type);
	UCN_ASSOC_MEMBER_UINT8(output, match_push, tep_count);
    UCN_ASSOC_MEMBER_UINT8(output, match_push, tep_index);
	UCN_ASSOC_MEMBER_UINT8(output, match_push, error_num);
    UCN_ASSOC_MEMBER_UINT8(output, match_push, flagbits);
	UCN_ASSOC_MEMBER_UINT8(output, match_push, skey);

    return E_SUCCESS;
}

/*遥控器查询返回*/
UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_IA, UCSOT_IA_CODE, UCAT_CODE_RC_INFO) {
	ucp_ucat_code_g* code_g = (ucp_ucat_code_g*)(input + 1);
    UCN_ASSOC_MEMBER_UINT8(output, code_g, count);
    UCN_ASSOC_MEMBER_UINT8(output, code_g, length);
    u_int8_t count = code_g->count;
	size_t	index = 0;
	UC_ALLOC_ARRAY_INIT_ZVAL(data_zval);
	ucp_ucat_code_info_g* conde_info_p = (ucp_ucat_code_info_g*)(code_g + 1);
	for(index=0; index < count; index++) {
		UC_ALLOC_ARRAY_INIT_ZVAL(sub_data);
		UCN_ASSOC_MEMBER_UINT8(sub_data, conde_info_p, type);
		UCN_ASSOC_MEMBER_UINT8(sub_data, conde_info_p, devid);
		UCN_ASSOC_MEMBER_UINT16(sub_data, conde_info_p, irid);
		UCN_ASSOC_MEMBER_UINT8(sub_data, conde_info_p, flags);
		UCN_ASSOC_MEMBER_STRINGL(sub_data, conde_info_p, name);
		UC_ADD_NEXT_INDEX_ZVAL(data_zval, sub_data);
		conde_info_p ++;
	}
	UC_ADD_ASSOC_ZVAL(output, UC_DATA, data_zval);
    return E_SUCCESS;
}

int unpack_infrared_key_info(ZVAL* infrared_zval, ucp_ucat_key_g* key_get) {
	
    UCN_ASSOC_MEMBER_UINT8(infrared_zval, key_get, type);
    UCN_ASSOC_MEMBER_UINT8(infrared_zval, key_get, flags);
	UCN_ASSOC_MEMBER_UINT16(infrared_zval, key_get, irid);
    UCN_ASSOC_MEMBER_UINT8(infrared_zval, key_get, devid);
	UCN_ASSOC_MEMBER_UINT8(infrared_zval, key_get, fixkey_len);
    UCN_ASSOC_MEMBER_UINT8(infrared_zval, key_get, fixkey_num);
	UCN_ASSOC_MEMBER_UINT8(infrared_zval, key_get, defkey_num);
    UCN_ASSOC_MEMBER_UINT8(infrared_zval, key_get, defkey_len);
	
    u_int8_t fixkey_num = key_get->fixkey_num;
	u_int8_t defkey_num = key_get->defkey_num;
	size_t	index = 0;
	
	//UC_ALLOC_ARRAY_INIT_ZVAL(key_info);
	
	ucp_ucat_fix_key_g* key_info_p = (ucp_ucat_fix_key_g*)(key_get + 1);	
	UC_ALLOC_ARRAY_INIT_ZVAL(data_zval);	
	
	for(index=0; index < fixkey_num; index++) {
		UC_ALLOC_ARRAY_INIT_ZVAL(sub_data);
		UCN_ASSOC_MEMBER_UINT8(sub_data, key_info_p, key);
		UCN_ASSOC_MEMBER_UINT8(sub_data, key_info_p, flags);
		UC_ADD_NEXT_INDEX_ZVAL(data_zval, sub_data);
		key_info_p ++;
	}
	
	UC_ADD_ASSOC_ZVAL(infrared_zval, "fix_key", data_zval);
	//UC_ADD_NEXT_INDEX_ZVAL(key_info, data_zval);

	UC_ALLOC_ARRAY_INIT_ZVAL(ddata_zval);
	ucp_ucat_def_key_g* dkey_info_p = (ucp_ucat_def_key_g*)(key_info_p);
	for(index=0; index < defkey_num; index++) {
		UC_ALLOC_ARRAY_INIT_ZVAL(dsub_data);
		UCN_ASSOC_MEMBER_UINT8(dsub_data, dkey_info_p, key);
		UCN_ASSOC_MEMBER_UINT8(dsub_data, dkey_info_p, flags);
		UCN_ASSOC_MEMBER_STRINGL(dsub_data, dkey_info_p, name);
		UC_ADD_NEXT_INDEX_ZVAL(ddata_zval, dsub_data);
		dkey_info_p ++;
	}
	//UC_ADD_NEXT_INDEX_ZVAL(key_info, ddata_zval);
	
	UC_ADD_ASSOC_ZVAL(infrared_zval, "def_key", ddata_zval);

	return sizeof(ucp_ucat_key_g) + sizeof(ucp_ucat_fix_key_g)*fixkey_num + sizeof(ucp_ucat_def_key_g)*defkey_num;
}

/*查询遥控器按键信息*/
UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_KEY) {   
    int unpack_size = 0;
	int parament_len = ntohs(input->param_len);
	u_int8_t* int_p = (u_int8_t*)(input + 1);
	UC_ALLOC_ARRAY_INIT_ZVAL(out_data_zval);
	while(parament_len > unpack_size){
		UC_ALLOC_ARRAY_INIT_ZVAL(infrared_zval);
		ucp_ucat_key_g* key_get = (ucp_ucat_key_g*)(int_p + unpack_size);
		unpack_size += unpack_infrared_key_info(infrared_zval,  key_get);
		UC_ADD_NEXT_INDEX_ZVAL(out_data_zval, infrared_zval);
	}
    UC_ADD_ASSOC_ZVAL(output, UC_DATA, out_data_zval);
    return E_SUCCESS;
}

/*遥控器按键学习状态取消*/
UC_UNPACK_FUNCTION(UC_ANSWER, UCA_DELETE,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_LEARN) {   
	size_t param_len = ntohs(input->param_len);
    if (param_len != sizeof(ucp_ucat_key_learn))
		return E_FALIED;
	
	ucp_ucat_key_learn* key_learn = (ucp_ucat_key_learn*)(input + 1);
	key_learn->reserved = 0;
	UCN_ASSOC_MEMBER_UINT8(output, key_learn, type);
    UCN_ASSOC_MEMBER_UINT8(output, key_learn, key);
	UCN_ASSOC_MEMBER_UINT8(output, key_learn, devid);
    UCN_ASSOC_MEMBER_UINT8(output, key_learn, action);
	
	return E_SUCCESS;
}

/*遥控器按键学习状态查询*/
UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET,UCOT_IA,UCSOT_IA_CODE,UCAT_CODE_RC_LEARN) {   
	size_t param_len = ntohs(input->param_len);
    if (param_len != sizeof(ucp_ucat_key_learn))
		return E_FALIED;
	
	ucp_ucat_key_learn* key_learn = (ucp_ucat_key_learn*)(input + 1);
	key_learn->reserved = 0;
	UCN_ASSOC_MEMBER_UINT8(output, key_learn, type);
    UCN_ASSOC_MEMBER_UINT8(output, key_learn, key);
	UCN_ASSOC_MEMBER_UINT8(output, key_learn, devid);
    UCN_ASSOC_MEMBER_UINT8(output, key_learn, action);
	
	return E_SUCCESS;
}


UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET, UCOT_IA, UCSOT_IA_AC, UCAT_AC_MATCH_INFO) {
    typedef struct {
        u_int16_t match_id;
        u_int16_t reserved;
    } __attribute__ ((packed))ucp_ac_mtach_info_t;
    typedef struct {
        u_int16_t id;
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
    } __attribute__ ((packed))ucp_ac_mtach_work_t;
    
    size_t param_len = ntohs(input->param_len);
    if (param_len < sizeof(ucp_ac_mtach_info_t)) {
        return E_FALIED;
    }

    ucp_ac_mtach_info_t* match_info = (ucp_ac_mtach_info_t*)(input + 1);
    UCN_ASSOC_MEMBER_UINT16(output, match_info, match_id);
    
    size_t count = (param_len - sizeof(ucp_ac_mtach_info_t)) / sizeof(ucp_ac_mtach_work_t);
    ucp_ac_mtach_work_t* work = (ucp_ac_mtach_work_t*)(match_info + 1);
    size_t index = 0;
    UC_ALLOC_ARRAY_INIT_ZVAL(match_list);
    for(index = 0; index < count; index++) {
        UC_ALLOC_ARRAY_INIT_ZVAL(match);
        UCN_ASSOC_MEMBER_UINT16(match, work, id);
        UCN_ASSOC_MEMBER_UINT8(match, work, onoff);
        UCN_ASSOC_MEMBER_UINT8(match, work, mode);
        UCN_ASSOC_MEMBER_UINT8(match, work, temp);
        UCN_ASSOC_MEMBER_UINT8(match, work, wind);
        UCN_ASSOC_MEMBER_UINT8(match, work, direct);
        UCN_ASSOC_MEMBER_UINT8(match, work, key);

        UC_ADD_NEXT_INDEX_ZVAL(match_list, match);
        work++;
    }
    UC_ADD_ASSOC_ZVAL(output, "data", match_list);

    return E_SUCCESS;
}

UC_UNPACK_FUNCTION(UC_ANSWER, UCA_GET,  UCOT_IA, UCSOT_IA_AC, UCAT_AC_ATTRI) {
    size_t param_len = ntohs(input->param_len);
    u_int8_t* pp = (u_int8_t*)(input + 1);
    if (param_len != 4) {
        return E_FALIED;
    }
    
    UCN_ADD_ASSOC_UINT8(output, "match_action", *pp);

    return E_SUCCESS;
}


/*
UC_UNPKDF_FENTRY:  Udp control unpack default fuction entry
UC_UNPACK_FENTRY:  Udp control pack fuction entry
*/

uc_control_t g_uc_unpack_control[UC_MAX_CONTROL] = {
/*-----------------UC_UNPKDF_FENTRY:  Udp control unpack default fuction entry--------------*/
UC_UNPKDF_FENTRY(UC_ANSWER, UCA_GET, UCOT_IA,      UCOT_GET_ALL_ATTRI, UCOT_GET_ALL_ATTRI),
UC_UNPKDF_FENTRY(UC_ANSWER, UCA_GET, UCOT_SYSTEM,  UCSOT_SYS_SOFTWARE, UCOT_GET_ALL_ATTRI),

/*--------------------UC_UNPACK_FENTRY:  Udp control pack fuction entry---------------------*/
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_SYSTEM,  UCSOT_SYS_SOFTWARE, UCAT_SYS_SSID),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_SYSTEM,  UCSOT_SYS_SOFTWARE, UCAT_SYS_VERSION),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_SYSTEM,  UCSOT_SYS_SOFTWARE, UCAT_SYS_UPTIME),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_SYSTEM,  UCSOT_SYS_SOFTWARE, UCAT_SYS_PASSWD),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_SYSTEM,  UCSOT_SYS_SOFTWARE, UCAT_SYS_IP),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_SYSTEM,  UCSOT_SYS_SOFTWARE, UCAT_SYS_SVN),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_SYSTEM,  UCSOT_SYS_SOFTWARE, UCAT_SYS_DEBUGINFO),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_SYSTEM,  UCSOT_SYS_HARDWARE, UCAT_SYS_VERSION),

UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_SYSTEM,  UCSOT_SYS_SERVER,   UCAT_SERVER_CONNECT_TIME),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_SYSTEM,  UCSOT_SYS_UPGRADE,  UCOT_GET_ALL_ATTRI),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_SYSTEM,  UCSOT_SYS_UPGRADE,  UCAT_SYS_VERSION),
    
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_IA,      UCSOT_IA_AC,        UCAT_AC_WORK),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_IA,      UCSOT_IA_AC,        UCAT_AC_TMP_SAMPLE_CURVE),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_IA,      UCSOT_IA_AC,        UCAT_AC_ON_USER_SET),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_IA,      UCSOT_IA_AC,        UCAT_AC_CUR_TEMP), 
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_IA,      UCSOT_IA_AC,        UCAT_AC_MATCH_INFO),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_IA,      UCSOT_IA_AC,        UCAT_AC_ATTRI),
    

UC_UNPACK_FENTRY(UC_ANSWER, UCA_SET,  UCOT_IA,      UCSOT_IA_CODE,      UCAT_CODE_PROCESS),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_PUSH, UCOT_IA,      UCSOT_IA_CODE,      UCAT_CODE_PROCESS),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_IA,      UCSOT_IA_CODE,      UCAT_CODE_PROCESS),

UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_IA,      UCSOT_IA_PUBLIC,    UCAT_IA_PUBLIC_EXTENDED_TIMER),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_IA,      UCSOT_IA_PUBLIC,    UCAT_IA_PUBLIC_TMP_CURVE),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_IA,      UCSOT_IA_PUBLIC,    UCAT_IA_PUBLIC_TMP_CURVE_UTC),

UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_IA,      UCSOT_IA_CODE,		UCAT_CODE_RC_INFO),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_IA,		UCSOT_IA_CODE,		UCAT_CODE_RC_KEY),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_IA,		UCSOT_IA_CODE,		UCAT_CODE_RC_LEARN),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_DELETE,UCOT_IA,		UCSOT_IA_CODE,		UCAT_CODE_RC_LEARN),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_SET,  UCOT_IA,		UCSOT_IA_CODE,		UCAT_CODE_RC_MATCH),
UC_UNPACK_FENTRY(UC_ANSWER, UCA_GET,  UCOT_IA,		UCSOT_IA_CODE,		UCAT_CODE_RC_MATCH_STATE),

{0, 0, 0, 0, NULL},
};

