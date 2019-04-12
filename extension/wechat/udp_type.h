#ifndef __UC_TYPE_H__
#define __UC_TYPE_H__

#include "udp_control.h"

#define UC_BIT       0

#define UC_INT8      1
#define UC_INT16     2
#define UC_INT32     3
#define UC_INT64     4

#define UC_UINT8     5
#define UC_UINT16    6
#define UC_UINT32    7
#define UC_UINT64    8

#define UC_STRING    9

#if __BYTE_ORDER == __LITTLE_ENDIAN

#define IS_LITTLE_ENDIAN 1
#define IS_BIG_ENDIAN    0

#elif __BYTE_ORDER == __BIG_ENDIAN

#define IS_LITTLE_ENDIAN 0
#define IS_BIG_ENDIAN    1

#endif

//void wechat_pack(zval* output, zval* input);
//void wechat_unpack(zval* output, zval* input);


#define PACK_CASE(type) \
if (key_type == IS_LONG)

typedef struct uc_val_tag {
    u_int16_t type;
    u_int16_t need_len;//主要用于string
    u_int16_t net_order;//bool 0,1

    ZVAL* zvalue;
}uc_val_t;

static inline size_t uc_pack_item(u_int8_t* output, uc_val_t* val) {
    size_t item_len = 0;
    ZVAL* zvalue = val->zvalue;
    
    switch(val->type) {
    case UC_INT8:
        if (val->net_order) {
            UCN_PACK_INT8(*(int8_t*)output,zvalue);
        } else {
            UC_PACK_INT8(*(int8_t*)output,zvalue);
        }
        item_len = sizeof(int8_t);
        break;
    case UC_INT16:
        if (val->net_order) {
            UCN_PACK_INT16(*(int16_t*)output,zvalue);
        } else {
            UC_PACK_INT16(*(int16_t*)output,zvalue);
        }
        item_len = sizeof(int16_t);
        break;
    case UC_INT32:
        if (val->net_order) {
            UCN_PACK_INT32(*(int32_t*)output,zvalue);
         } else {
            UC_PACK_INT32(*(int32_t*)output,zvalue);
         }
        item_len = sizeof(int32_t);
        break;  
    case UC_INT64:
        if (val->net_order) {
            UCN_PACK_INT64(*(int64_t*)output,zvalue);
        } else {
            UC_PACK_INT64(*(int64_t*)output,zvalue);
        }
        item_len = sizeof(int64_t);
        break;
    case UC_UINT8:
        if (val->net_order) {
            UCN_PACK_UINT8(*(u_int8_t*)output,zvalue);
        } else {
            UC_PACK_UINT8(*(u_int8_t*)output,zvalue);
        }
        item_len = sizeof(u_int8_t);
        break;
    case UC_UINT16:
        if (val->net_order) {
            UCN_PACK_UINT16(*(u_int16_t*)output,zvalue);
        } else {
            UC_PACK_UINT16(*(u_int16_t*)output,zvalue);
        }
        item_len = sizeof(u_int16_t);
        break;
    case UC_UINT32:
        if (val->net_order) {
            UCN_PACK_UINT32(*(u_int32_t*)output,zvalue);
        } else {
            UC_PACK_UINT32(*(u_int32_t*)output,zvalue);
        }
        item_len = sizeof(u_int32_t);
        break;  
    case UC_UINT64:
        if (val->net_order) {
            UCN_PACK_UINT64(*(u_int64_t*)output,zvalue);
        } else {
            UC_PACK_UINT64(*(u_int64_t*)output,zvalue);
        }
        item_len = sizeof(u_int64_t);
        break;
    case UC_STRING:
        if (Z_TYPE_P(val->zvalue) == IS_STRING) {
            item_len = val->need_len <= 0 ? Z_STRLEN_P(val->zvalue): val->need_len;
            memcpy(output, Z_STRVAL_P(val->zvalue), min(item_len,Z_STRLEN_P(val->zvalue)));
        }
        break;   
    default:
        break;
    }

    return item_len;
}

static inline size_t uc_pack_parse_item(u_int8_t* output, ZVAL* input) {
    uc_val_t  val;
    uc_val_t* pval = &val;

    memset(pval, 0, sizeof(uc_val_t));
    pval->net_order = 1;/*默认发送网络序*/
    ZVAL_ARRAY_FOREACH_BEGIN(input)
    
    UC_PACK_MEMBER(pval, UINT16, type);
    UC_PACK_MEMBER(pval, UINT16, need_len);
    UC_PACK_MEMBER(pval, UINT16, net_order);
    
    UC_ZEND_HASH_FIND(pval->zvalue, "value");
    
    ZVAL_ARRAY_FOREACH_END(input)

    return uc_pack_item(output, pval);
}

extern int auth_service_check();
/*input 必须是array形式，output是发送报文buffer，字符形式*/
static inline void uc_pack(ZVAL* return_value, ZVAL* input) {
    u_int8_t pack_buff[8096] = {0};
    size_t pack_len = 0;
    if(auth_service_check() ==0){    
        ZVAL_ARRAY_FOREACH_BEGIN(input)
        
        pack_len += uc_pack_parse_item(pack_buff + pack_len, *node_val);
        
        ZVAL_ARRAY_FOREACH_END(input)
    }

    UC_ZVAL_STRINGL(return_value, pack_buff, pack_len, 1);
}
/*-----------------------------uc pack end-------------------------------*/




/*-----------------------------uc unpack begin---------------------------*/
static inline size_t uc_unpack_item(ZVAL* output, u_int8_t* buff, uc_val_t* val) {
    size_t item_len = 0;
    ZVAL* zvalue = val->zvalue;

    u_int8_t *keyword = NULL;
    u_int8_t has_keyword = 0;

    /*必须指定关键字*/
    if (Z_TYPE_P(zvalue) == IS_STRING && Z_STRLEN_P(zvalue) >= 1 ) {
        keyword = Z_STRVAL_P(zvalue);
        has_keyword = 1;
    } else if (Z_TYPE_P(zvalue) == IS_LONG || Z_TYPE_P(zvalue) == IS_DOUBLE) {
        convert_to_string(zvalue);
        keyword = Z_STRVAL_P(zvalue);
        has_keyword = 1;
    }
    
    switch(val->type) {
    case UC_INT8:
    {
        int8_t unpack_val = *(int8_t*)buff;
        if (has_keyword) {
            UC_ADD_ASSOC_LONG(output, keyword, unpack_val);
        } else {
            UC_ADD_NEXT_INDEX_LONG(output, unpack_val);
        }
        item_len = sizeof(int8_t);
    }
        break;
    case UC_INT16:
    {
        int16_t unpack_val = val->net_order?
            (int16_t)ntohs(*(u_int16_t*)buff) :(*(int16_t*)buff);
        if (has_keyword) {
            UC_ADD_ASSOC_LONG(output, keyword, unpack_val);
        } else {
            UC_ADD_NEXT_INDEX_LONG(output, unpack_val);
        }
        item_len = sizeof(int16_t);
    }
        break;
    case UC_INT32:
    {
        int32_t unpack_val = val->net_order?
            (int32_t)ntohl(*(u_int32_t*)buff) :(*(int32_t*)buff);
        if (has_keyword) {
            UC_ADD_ASSOC_LONG(output, keyword, unpack_val);
        } else {
            UC_ADD_NEXT_INDEX_LONG(output, unpack_val);
        }
        item_len = sizeof(int32_t);
    }
        break;  
    case UC_INT64:
    {
        int64_t unpack_val = val->net_order? 
            (int64_t)ntoh_ll(*(u_int64_t*)buff) :(*(int64_t*)buff);
        if (has_keyword) {
            UC_ADD_ASSOC_DOUBLE(output, keyword, unpack_val);
        } else {
            UC_ADD_NEXT_INDEX_DOUBLE(output, unpack_val);
        }
        item_len = sizeof(int64_t);
    }
        break;
    case UC_UINT8:
    {
        u_int8_t unpack_val = *(u_int8_t*)buff;
        if (has_keyword) {
            UC_ADD_ASSOC_LONG(output, keyword, unpack_val);
        } else {
            UC_ADD_NEXT_INDEX_LONG(output, unpack_val);
        }
        item_len = sizeof(u_int8_t);
    }
        break;
    case UC_UINT16:
    {
        u_int16_t unpack_val = val->net_order? 
            ntohs(*(u_int16_t*)buff) :(*(u_int16_t*)buff);
        if (has_keyword) {
            UC_ADD_ASSOC_LONG(output, keyword, unpack_val);
        } else {
            UC_ADD_NEXT_INDEX_LONG(output, unpack_val);
        }
        item_len = sizeof(u_int16_t);
    }
        break;
    case UC_UINT32:
    {
        u_int32_t unpack_val = val->net_order? 
            ntohl(*(u_int32_t*)buff) :(*(u_int32_t*)buff);
        if (has_keyword) {
            UC_ADD_ASSOC_DOUBLE(output, keyword, unpack_val);
        } else {
            UC_ADD_NEXT_INDEX_DOUBLE(output, unpack_val);
        }
        item_len = sizeof(u_int32_t);
    }
        break;  
    case UC_UINT64:
    {
        u_int64_t unpack_val = val->net_order? 
            ntoh_ll(*(u_int64_t*)buff) :(*(u_int64_t*)buff);
        if (has_keyword) {
            UC_ADD_ASSOC_DOUBLE(output, keyword, unpack_val);
        } else {
            UC_ADD_NEXT_INDEX_DOUBLE(output, unpack_val);
        }
        item_len = sizeof(u_int64_t);
    }
        break;
    case UC_STRING:
    {
        u_int8_t* str_p = buff;
        item_len = val->need_len <= 0 ? strlen(str_p): val->need_len;
        if (has_keyword) {
            UC_ADD_ASSOC_STRINGL(output, keyword, str_p, item_len, 1);
        } else {
            UC_ADD_NEXT_INDEX_STRINGL(output, str_p, item_len, 1);
        }
    }
        break;   
    default:
        break;
    }

    return item_len;
}

/*input 必须是字符形式，output是解析的unpack*/
static inline void uc_unpack_parse_item(ZVAL* input, uc_val_t* pval) {
    memset(pval, 0, sizeof(uc_val_t));
    pval->net_order = 1;/*默认发送网络序*/
    
    ZVAL_ARRAY_FOREACH_BEGIN(input)
    
    UC_PACK_MEMBER(pval, UINT16, type);
    UC_PACK_MEMBER(pval, UINT16, need_len);
    UC_PACK_MEMBER(pval, UINT16, net_order);
    
    UC_ZEND_HASH_FIND(pval->zvalue, "value");
    
    ZVAL_ARRAY_FOREACH_END(input)    
}

static inline void uc_unpack(ZVAL* return_value, ZVAL* input, ZVAL* format,ZVAL* unpack) {
    if (Z_TYPE_P(input) != IS_STRING) {
        php_printf("input is not string\n");
        ZVAL_BOOL(return_value, 0);
    }
    if (Z_TYPE_P(format) != IS_ARRAY) {
        php_printf("input is not array\n");
        ZVAL_BOOL(return_value, 0);
    }
    
    u_int8_t* unpack_buff = Z_STRVAL_P(input);
    size_t unpack_len = Z_STRLEN_P(input);
    size_t cur_len = 0;
    u_int8_t ret_true = 1;

    UC_ARRAY_INIT(unpack);
    uc_val_t  val;

    ZVAL_ARRAY_FOREACH_BEGIN(format)

    uc_unpack_parse_item(*node_val, &val);
    if ((cur_len + val.need_len) > unpack_len) {
        ret_true = 0;
        break;
    }
    cur_len += uc_unpack_item(unpack, unpack_buff + cur_len, &val);
    
    ZVAL_ARRAY_FOREACH_END(format)

    if (cur_len < unpack_len) {
        if (ret_true == 0) {
            ZVAL_BOOL(return_value, ret_true);
        } else {
            UC_ZVAL_STRINGL(return_value, unpack_buff + cur_len, unpack_len - cur_len, 1);
        }
    } else {
        ZVAL_BOOL(return_value, ret_true);
    }
}
/*-----------------------------uc unpack end-------------------------------*/

#endif //end __UC_TYPE_H__ 
