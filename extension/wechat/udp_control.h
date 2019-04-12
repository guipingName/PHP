#ifndef __UDP_CNTRL_H__
#define __UDP_CNTRL_H__

#include "php.h"
#include "php_wechat.h"
#include "ext/standard/php_smart_str.h"

#include "udp_parse.h"

/*
#define define(x,y)    x##y   //连接x,y
#define define(x)      #@x    //加单引号
#define define(x)      #x     //加双引号
*/

#define max(x,y) ((x) > (y) ? (x) : (y))
#define min(x,y) ((x) < (y) ? (x) : (y))

#define UC_ADD_INDEX_LONG         add_index_long
#define UC_ADD_INDEX_DOUBLE       add_index_double
#define UC_ADD_INDEX_STRING       add_index_string
#define UC_ADD_INDEX_STRINGL      add_index_stringl
#define UC_ADD_INDEX_ZVAL         add_index_zval
#define UC_ADD_NEXT_INDEX_LONG    add_next_index_long
#define UC_ADD_NEXT_INDEX_DOUBLE  add_next_index_double
#define UC_ADD_NEXT_INDEX_STRING  add_next_index_string
#define UC_ADD_NEXT_INDEX_STRINGL add_next_index_stringl
#define UC_ADD_NEXT_INDEX_ZVAL    add_next_index_zval
#define UC_ZVAL_STRING(val, s, duplicate)     ZVAL_STRING(val, s, duplicate)
#define UC_ZVAL_STRINGL(val, s, l, duplicate) ZVAL_STRINGL(val, s, l, duplicate)


#define UC_ADD_ASSOC_STRING  add_assoc_string
#define UC_ADD_ASSOC_STRINGL add_assoc_stringl
#define UC_ADD_ASSOC_DOUBLE  add_assoc_double
#define UC_ADD_ASSOC_LONG    add_assoc_long
#define UC_ADD_ASSOC_ZVAL    add_assoc_zval

#define UC_ADD_ASSOC_UINT8(val, keyword, value)  \
    UC_ADD_ASSOC_LONG(val, keyword, value)
#define UC_ADD_ASSOC_UINT16(val, keyword, value) \
    UC_ADD_ASSOC_LONG(val, keyword, ntohs(value))
#define UC_ADD_ASSOC_UINT32(val, keyword, value) \
if (sizeof(long) == sizeof(u_int64_t)) {         \
    UC_ADD_ASSOC_LONG(val, keyword, value);      \
} else if (value > ((u_int32_t)(1<<31))){       \
    UC_ADD_ASSOC_DOUBLE(val, keyword, (double)value); \
} else {                                              \
    UC_ADD_ASSOC_LONG(val, keyword, value);           \
}
#define UC_ADD_ASSOC_UINT64(val, keyword, value) \
    UC_ADD_ASSOC_DOUBLE(val, keyword, (double)ntoh_ll(value));

#if __BYTE_ORDER == __LITTLE_ENDIAN  
#define __UC_PRINTF_IPV4(ip_buff,ip_p) sprintf(ip_buff, "%d.%d.%d.%d", ip_p[3],ip_p[2],ip_p[1],ip_p[0])
#else
#define __UC_PRINTF_IPV4(ip_buff,ip_p) sprintf(ip_buff, "%d.%d.%d.%d", ip_p[0],ip_p[1],ip_p[2],ip_p[3])
#endif
#define UC_ADD_ASSOC_IPV4(val, keyword, ip) {\
    u_int8_t ip_buff[128] = {0};\
    u_int32_t real_ip = ip;\
    u_int8_t* ip_p = (u_int8_t*)&real_ip;\
    size_t len =  __UC_PRINTF_IPV4(ip_buff,ip_p);\
    if (keyword != NULL) {\
        UC_ADD_ASSOC_STRING(val, keyword, ip_buff, 1);\
    } else {\
        UC_ADD_NEXT_INDEX_STRING(val, ip_buff, 1);\
    }\
}
#define UC_ADD_ASSOC_VERSION(val, keyword, version) {\
    u_int8_t ver_buff[128] = {0};\
    u_int8_t* ver_p = version;\
    size_t len = sprintf(ver_buff, "%d.%d.%d", ver_p[0],ver_p[1],ver_p[2]);\
    if (keyword != NULL) {\
        UC_ADD_ASSOC_STRING(val, keyword, ver_buff, 1);\
    } else {\
        UC_ADD_ASSOC_STRING(val, "version", ver_buff, 1);\
    }\
}

#define UC_SYSTEM_64 (sizeof(long) == sizeof(u_int64_t))

#define UCN_ADD_ASSOC_UINT8(val, keyword, value) \
    UC_ADD_ASSOC_LONG(val, keyword, value)
#define UCN_ADD_ASSOC_UINT16(val, keyword, value) \
    UC_ADD_ASSOC_LONG(val, keyword, ntohs(value))
#define UCN_ADD_ASSOC_UINT32(val, keyword, value) \
if (UC_SYSTEM_64) {                 \
    UC_ADD_ASSOC_LONG(val, keyword, ntohl(value));       \
} else if (ntohl(value) > ((u_int32_t)(1<<31))){         \
    UC_ADD_ASSOC_DOUBLE(val, keyword, (double)ntohl(value)); \
} else {                                                  \
    UC_ADD_ASSOC_LONG(val, keyword, ntohl(value));        \
}
#define UCN_ADD_ASSOC_UINT64(val, keyword, value) \
    UC_ADD_ASSOC_DOUBLE(val, keyword, (double)ntoh_ll(value)); 

#define UCN_ADD_ASSOC_IPV4(val, keyword, ip) \
    UC_ADD_ASSOC_IPV4(val, keyword, ntol(ip))
    
#define UCN_ADD_ASSOC_VERSION(val, keyword, version) \
    UC_ADD_ASSOC_VERSION(val, keyword, version)
    
#define UCN_ADD_ASSOC_STRING  UC_ADD_ASSOC_STRING
#define UCN_ADD_ASSOC_STRINGL UC_ADD_ASSOC_STRINGL
#define UCN_ADD_ASSOC_ZVAL    UC_ADD_ASSOC_ZVAL
#define UCN_ADD_ASSOC_DOUBLE  UCN_ADD_ASSOC_UINT64
#define UCN_ADD_ASSOC_LONG    UCN_ADD_ASSOC_UINT32

#define UC_ALLOC_INIT_ZVAL   ALLOC_INIT_ZVAL
#define UC_FREE_ZVAL         FREE_ZVAL
#define UC_ARRAY_INIT        array_init

#define STRING_CAT(str1, str2) str1##str2

#define UC_ALLOC_ARRAY_INIT_ZVAL(val)  ZVAL* val = NULL;\
UC_ALLOC_INIT_ZVAL(val);\
UC_ARRAY_INIT(val);

#define ZVAL    zval
#define EMALLOC emalloc
#define EFREE   efree

#define UC_DBG(format, args...)   if(1) {\
    php_printf("%s %d:\n", __FUNCTION__, __LINE__);\
    php_printf(format, ##args);\
    php_printf("\n");\
}


#define UC_LOG(format, args...)   if(0/*WECHAT_G(debug_flag)*/) {\
    php_printf("%s %d:\n", __FUNCTION__, __LINE__);\
    php_printf(format, ##args);\
    php_printf("\n");\
}

#define UC_WRITE(format, len)  if(0/*WECHAT_G(debug_flag)*/) {\
    php_printf("%s %d:\n", __FUNCTION__, __LINE__);\
    PHPWRITE(format, len);\
    php_printf("\n");\
}

#ifndef HASH_KEY_IS_STRING
#define HASH_KEY_IS_STRING    1
#define HASH_KEY_IS_LONG      2
#define HASH_KEY_NON_EXISTANT 3
#endif

#ifndef IS_NULL
#define IS_NULL     0
#define IS_LONG     1
#define IS_DOUBLE   2
#define IS_BOOL     3
#define IS_ARRAY    4
#define IS_OBJECT   5
#define IS_STRING   6
#define IS_RESOURCE 7
#define IS_CONSTANT 8
#define IS_CONSTANT_ARRAY   9
#define IS_CALLABLE 10
#endif

#ifndef Z_TYPE_P
#define Z_TYPE_P   Z_TYPE_P
#define Z_LVAL_P   Z_LVAL_P
#define Z_STRVAL_P Z_STRVAL_P
#define Z_STRLEN_P Z_STRLEN_P
#define Z_DVAL_P   Z_DVAL_P
#define Z_TYPE_PP  Z_TYPE_PP
#endif


#define UC_ASSOC_MEMBER_UINT16(val, pointer, keyword)             \
    UC_ADD_ASSOC_LONG(val, #keyword, (unsigned long)pointer->keyword)

#define UC_ASSOC_MEMBER_UINT32(val, pointer, keyword)             \
if (UC_SYSTEM_64) {                          \
    UC_ADD_ASSOC_LONG(val, #keyword, pointer->keyword);           \
} else if (pointer->keyword > ((u_int32_t)(1<<31))){      \
    UC_ADD_ASSOC_DOUBLE(val, #keyword, (double)pointer->keyword); \
} else {                                                          \
    UC_ADD_ASSOC_LONG(val, #keyword, pointer->keyword);           \
}

#define UC_ASSOC_MEMBER_UINT16(val, pointer, keyword)             \
    UC_ADD_ASSOC_LONG(val, #keyword, (unsigned long)pointer->keyword)

#define UC_ASSOC_MEMBER_UINT8(val, pointer, keyword)              \
    UC_ADD_ASSOC_LONG(val, #keyword, (unsigned long)pointer->keyword)

#define UC_ASSOC_MEMBER_UINT64(val, pointer, keyword)             \
    UC_ADD_ASSOC_DOUBLE(val, #keyword, (double)pointer->keyword)

#define UC_ASSOC_MEMBER_STRING(val, pointer, keyword)         \
    UC_ADD_ASSOC_STRING(val, #keyword, pointer->keyword, 1)

#define UC_ASSOC_MEMBER_STRINGL(val, pointer, keyword)   \
    UC_ADD_ASSOC_STRINGL(val, #keyword, pointer->keyword, min(sizeof(pointer->keyword), strlen(pointer->keyword)), 1)

#define UCN_ASSOC_MEMBER_UINT32(val, pointer, keyword)                  \
if (UC_SYSTEM_64) {                                \
    UC_ADD_ASSOC_LONG(val, #keyword, ntohl(pointer->keyword));          \
} else if (ntohl(pointer->keyword) > ((u_int32_t)(1<<31))){                    \
    UC_ADD_ASSOC_DOUBLE(val, #keyword, (u_int64_t)ntohl(pointer->keyword)); \
} else {                                                                 \
    UC_ADD_ASSOC_LONG(val, #keyword, ntohl(pointer->keyword));           \
}

#define UCN_ASSOC_MEMBER_UINT16(val, pointer, keyword)              \
    UC_ADD_ASSOC_LONG(val, #keyword, (unsigned long)ntohs(pointer->keyword))

#define UCN_ASSOC_MEMBER_UINT8(val, pointer, keyword)               \
    UC_ASSOC_MEMBER_UINT8(val, pointer, keyword) 

#define UCN_ASSOC_MEMBER_UINT64(val, pointer, keyword)              \
    UC_ADD_ASSOC_DOUBLE(val, #keyword, (double)ntoh_ll(pointer->keyword))

#define UCN_ASSOC_MEMBER_STRING(val, pointer, keyword)         \
    UC_ADD_ASSOC_STRING(val, pointer, keyword)

#define UCN_ASSOC_MEMBER_STRINGL(val, pointer, keyword)   \
    UC_ASSOC_MEMBER_STRINGL(val, pointer, keyword)

#define UC_ASSOC_MEMBER_WEEK(val, pointer, keyword) {\
    UC_ALLOC_ARRAY_INIT_ZVAL(__zval_week);\
    u_int8_t __uint_week = (u_int8_t)(pointer->keyword);\
    u_int8_t __bit_week = 1;\
    for (__bit_week = 1; __bit_week <= 7; __bit_week++) {\
        if (__uint_week & (1<<(__bit_week%7))) {\
           UC_ADD_NEXT_INDEX_LONG(__zval_week,__bit_week%7);\
        }\
    }\
    UC_ADD_ASSOC_ZVAL(val, #keyword, __zval_week);\
}

#define UCN_ASSOC_MEMBER_WEEK(val, pointer, keyword) \
    UC_ASSOC_MEMBER_WEEK(val, pointer, keyword)


#define __UC_PACK(pointer, val, func, convert, type) {\
        zval _cpval;\
        _cpval = *val;\
        zval_copy_ctor(&_cpval);\
        func(&_cpval);\
        pointer = convert(type(&_cpval));\
        zval_dtor(&_cpval);\
}

#define UC_PACK_INT64(pointer, val) \
    __UC_PACK(pointer, val, convert_to_double, (int64_t), Z_DVAL_P)
#define UCN_PACK_INT64(pointer, val) \
    __UC_PACK(pointer, val ,convert_to_double, ntoh_ll, (int64_t)Z_DVAL_P)
#define UC_PACK_UINT64(pointer, val) \
    __UC_PACK(pointer, val, convert_to_double, (u_int64_t), Z_DVAL_P)
#define UCN_PACK_UINT64(pointer, val) \
    __UC_PACK(pointer, val ,convert_to_double, ntoh_ll, (u_int64_t)Z_DVAL_P)

#define UC_PACK_INT32(pointer, val) \
    __UC_PACK(pointer, val, convert_to_long, (int32_t), Z_LVAL_P)
#define UCN_PACK_INT32(pointer, val) \
    __UC_PACK(pointer, val ,convert_to_long, ntohl, (int32_t)Z_LVAL_P)
#define UC_PACK_UINT32(pointer, val) \
    __UC_PACK(pointer, val, convert_to_long, (u_int32_t), Z_LVAL_P)
#define UCN_PACK_UINT32(pointer, val) \
    __UC_PACK(pointer, val ,convert_to_long, ntohl, (u_int32_t)Z_LVAL_P)

#define UC_PACK_INT16(pointer, val) \
    __UC_PACK(pointer, val, convert_to_long, (int16_t), Z_LVAL_P)
#define UCN_PACK_INT16(pointer, val) \
    __UC_PACK(pointer, val , convert_to_long, ntohs, (int16_t)Z_LVAL_P)
#define UC_PACK_UINT16(pointer, val) \
    __UC_PACK(pointer, val, convert_to_long, (u_int16_t), Z_LVAL_P)
#define UCN_PACK_UINT16(pointer, val) \
    __UC_PACK(pointer, val , convert_to_long, ntohs, (u_int16_t)Z_LVAL_P)
    
#define UC_PACK_UINT8(pointer, val) \
    __UC_PACK(pointer, val, convert_to_long, (u_int8_t), Z_LVAL_P)
#define UCN_PACK_UINT8(pointer, val) \
    __UC_PACK(pointer, val ,convert_to_long, (u_int8_t), Z_LVAL_P)
#define UC_PACK_INT8(pointer, val) \
    __UC_PACK(pointer, val, convert_to_long, (int8_t), Z_LVAL_P)
#define UCN_PACK_INT8(pointer, val) \
    __UC_PACK(pointer, val ,convert_to_long, (int8_t), Z_LVAL_P)


#define __UC_PACK_MEMBER(pointer, srckw, val, deskw, deslen, func, convert, type) {\
    if (strlen((#srckw)) == (deslen - 1)  && strncmp(((#srckw)), deskw, deslen - 1) == 0){\
        zval _cpval;\
        _cpval = **val;\
        zval_copy_ctor(&_cpval);\
        func(&_cpval);\
        uc_print_zval(&_cpval, #srckw);\
        pointer->srckw = convert(type(&_cpval));\
        zval_dtor(&_cpval);\
        continue;\
    }\
}

#define UC_PACK_MEMBER_UINT64(pointer, srckw, val, deskw, deslen) \
    __UC_PACK_MEMBER(pointer, srckw, val, deskw, deslen,convert_to_double, (u_int64_t), Z_DVAL_P)

#define UCN_PACK_MEMBER_UINT64(pointer, srckw, val, deskw, deslen) \
    __UC_PACK_MEMBER(pointer, srckw, val, deskw, deslen, convert_to_double, ntoh_ll, (u_int64_t)Z_DVAL_P)

#define UC_PACK_MEMBER_UINT32(pointer, srckw, val, deskw, deslen) \
    __UC_PACK_MEMBER(pointer, srckw, val, deskw, deslen,convert_to_long, (u_int32_t), Z_LVAL_P)

#define UCN_PACK_MEMBER_UINT32(pointer, srckw, val, deskw, deslen) \
    __UC_PACK_MEMBER(pointer, srckw, val, deskw, deslen, convert_to_long, ntohl, (u_int32_t)Z_LVAL_P)

#define UC_PACK_MEMBER_UINT16(pointer, srckw, val, deskw, deslen) \
    __UC_PACK_MEMBER(pointer, srckw, val, deskw, deslen,convert_to_long, (u_int16_t), Z_LVAL_P)

#define UCN_PACK_MEMBER_UINT16(pointer, srckw, val, deskw, deslen) \
    __UC_PACK_MEMBER(pointer, srckw, val, deskw, deslen, convert_to_long, ntohs, (u_int16_t)Z_LVAL_P)

#define UC_PACK_MEMBER_UINT8(pointer, srckw, val, deskw, deslen) \
    __UC_PACK_MEMBER(pointer, srckw, val, deskw, deslen,convert_to_long, (u_int8_t), Z_LVAL_P)

#define UCN_PACK_MEMBER_UINT8(pointer, srckw, val, deskw, deslen) \
    UC_PACK_MEMBER_UINT8(pointer, srckw, val, deskw, deslen)

#define UC_PACK_MEMBER_STRING(pointer, srckw, val, deskw, deslen) {\
    if (strlen((#srckw)) == (deslen - 1)  && strncmp(((#srckw)), deskw, deslen) == 0){\
        zval _cpval;\
        _cpval = **val;\
        zval_copy_ctor(&_cpval);\
        convert_to_string(&_cpval);\
        uc_print_zval(&_cpval, #srckw);\
        memmove(pointer->srckw, Z_STRVAL_P(&_cpval), min(Z_STRLEN_P(&_cpval), sizeof(pointer->srckw)));\
        zval_dtor(&_cpval);\
        continue;\
    }\
}

#define UC_PACK_MEMBER_WEEK(pointer, srckw, val, deskw, deslen) {\
    if (strlen((#srckw)) == (deslen - 1)  && strncmp(((#srckw)), deskw, deslen) == 0){\
        if (val == NULL || Z_TYPE_PP(val) != IS_ARRAY) {\
            continue;\
        }\
        ZVAL* __zval = *val;\
        u_int32_t __week_count = zend_hash_num_elements(HASH_OF(__zval));\
        size_t __week_index = 0;\
        ZVAL ** __week_node_zval = NULL;\
        for (__week_index = 0; __week_index < __week_count; __week_index++) {\
            __week_node_zval = NULL;\
            if (zend_hash_index_find(HASH_OF(__zval), __week_index, \
                    (void**)&__week_node_zval) != SUCCESS) {\
                continue;\
            }\
            u_int8_t __day = 7;\
            if (Z_TYPE_PP(__week_node_zval) == IS_LONG) {\
                __day = Z_LVAL_P(*__week_node_zval);\
            } else if (Z_TYPE_PP(__week_node_zval) == IS_STRING) {\
                zval __cpval;\
                __cpval = **__week_node_zval;\
                zval_copy_ctor(&__cpval);\
                convert_to_long(&__cpval);\
                __day = (u_int8_t)Z_LVAL_P(&__cpval);\
                zval_dtor(&__cpval);\
            }\
            if (__day >= 0 && __day < 7) {\
                pointer->srckw |= (1<<(__day%7));\
            }\
        }\
        continue;\
    }\
}

#define UCN_PACK_MEMBER_WEEK(pointer, srckw, val, deskw, deslen) \
    UC_PACK_MEMBER_WEEK(pointer, srckw, val, deskw, deslen)

#define UCN_PACK_MEMBER_STRING(pointer, srckw, val, deskw, deslen) \
    UC_PACK_MEMBER_STRING(pointer, srckw, val, deskw, deslen)

typedef int (*uc_control_callback)(void* output, void* input);

typedef struct uc_control_tag {
    u_int16_t  direct;
    u_int16_t  action;
    u_int16_t objct;
    u_int16_t sub_objct;
    u_int16_t attr;
    uc_control_callback callback;
    u_int8_t* description;
}uc_control_t;

#define UC_MAX_CONTROL 200


#define UC_PACK_MEMBER(val, method, member) \
    UC_PACK_MEMBER_##method(val ,member, node_val, key, key_len)

#define UCN_PACK_MEMBER(val, method, member) \
    UCN_PACK_MEMBER_##method(val,member, node_val, key, key_len)

#define _ZVAL_ARRAY_FOREACH_BEGIN(val) do{\
    if (val == NULL || Z_TYPE_P(val) != IS_ARRAY) {\
        break;\
    }\
    HashTable *ht_table = HASH_OF(val);\
    HashPosition ht_pos;\
    zval **node_val = NULL;\
    char *key = NULL;\
    int key_len = 0;\
    long key_index = 0;\
    int key_type = HASH_KEY_NON_EXISTANT;\
    for(zend_hash_internal_pointer_reset_ex(ht_table, &ht_pos); \
            zend_hash_get_current_data_ex(ht_table, (void**) &node_val, &ht_pos) == SUCCESS;\
            zend_hash_move_forward_ex(ht_table, &ht_pos)) {\
        if (Z_TYPE_PP(node_val) == IS_NULL) {\
            continue;\
        }\
        key_type = zend_hash_get_current_key_ex(ht_table, &key, \
                &key_len, &key_index, 0, &ht_pos);

#define _ZVAL_ARRAY_FOREACH_END(val) }}while(0);

#define ZVAL_ARRAY_FOREACH_BEGIN(val) _ZVAL_ARRAY_FOREACH_BEGIN(val)
#define ZVAL_ARRAY_FOREACH_END(val)   _ZVAL_ARRAY_FOREACH_END(val)


/*#define UC_ZEND_HASH_FIND(val, key, pdata) {\
    HashTable *ht_table = HASH_OF(val);\
    if (zend_hash_find(ht_table, key, strlen(key) + 1, (void**)pdata) != SUCCESS) {\
        php_printf("zend_hash_find find falied\n");\
    }\
}*/

#define UC_ZEND_HASH_FIND(value, srckw)\
if (strlen(srckw )== (key_len - 1)  && strncmp(srckw, key, key_len - 1) == 0){\
    value = *node_val;\
}



#define E_SUCCESS         0
#define E_FALIED         -1
#define E_INVALID_LENGTH -2
#define E_INVALID_INPUT  -3

extern void uc_set_global(int option);
extern void uc_print_zval(ZVAL *pval, char* key);
extern u_int64_t convert_to_uint64(char* str, u_int32_t str_len);
extern void uc_control_unpack_unknow(ZVAL* output, u_int8_t*key, \
    u_int8_t* input, size_t input_len);
extern int uc_pack_u_int64_t(ZVAL* input, u_int64_t* value);
int uc_control_pack_check_cmd(ZVAL* ouput[UC_MAX_CONTROL], ZVAL* input);

/*#define __UC_FNAME(actname, objname, subname, attrname, typename) \
	function_##actname##objname##subname##attrname##typename
	
#define __UC_FUNCTION(action, obj, sub, attr, type) \
	int __UC_FNAME(action, obj, sub, attr, type)(void* output, void* input)


#define UC_PACK_FUNCTION(action, obj, sub, attr)   \
	__UC_FUNCTION(action, obj, sub, attr, PACK)
#define UC_UNPACK_FUNCTION(action, obj, sub, attr) \
	__UC_FUNCTION(action, obj, sub, attr, UNPACK)


#define UC_DEFAULT_FENTRY(action, obj, sub, attr)   \
	{action, obj, sub, attr, NULL}
#define UC_PACK_FENTRY(action, obj, sub, attr)   \
	{action, obj, sub, attr, __UC_FNAME(action, obj, sub, attr, PACK)}
#define UC_UNPACK_FENTRY(action, obj, sub, attr)   \
	{action, obj, sub, attr, __UC_FNAME(action, obj, sub, attr, UNPACK)}
*/

//#define UC_PACK_RFGW_TLV_VALUE_FUCT(tlv_type) \
//	int FUNCTION__RFGW__TLV__##tlv_type##__PACK(u_int8_t* output, ZVAL* input)

#define UC_PACK_FUNCTION(direct, action, obj, sub, attr)   \
    int FUNCTIION##__##direct##__##action##__##obj##__##sub##__##attr##__##PACK(u_int8_t* output, ZVAL* input)
#define UC_UNPACK_FUNCTION(direct, action, obj, sub, attr) \
    int FUNCTIION##__##direct##__##action##__##obj##__##sub##__##attr##__##UNPACK(ZVAL* output, ucp_obj_t* input)
#define UC_PACK_EXEC(direct, action, obj, sub, attr) \
        return FUNCTIION##__##direct##__##action##__##obj##__##sub##__##attr##__##PACK(output, input)
#define UC_UNPACK_EXEC(direct, action, obj, sub, attr) \
        return FUNCTIION##__##direct##__##action##__##obj##__##sub##__##attr##__##UNPACK(output, input)

/*pack default*/
#define UC_PKDF_FENTRY(direct, action, obj, sub, attr)   \
    {direct, action, obj, sub, attr, NULL,\
    "Direct:"#direct"  Action:"#action"  Object:"#obj"  Sub-object:"#sub"  Attribute:"#attr"  Type:PACK"}
#define UC_PACK_FENTRY(direct, action, obj, sub, attr)   \
    {direct, action, obj, sub, attr, \
    FUNCTIION##__##direct##__##action##__##obj##__##sub##__##attr##__##PACK,\
    "Direct:"#direct"  Action:"#action"  Object:"#obj"  Sub-object:"#sub"  Attribute:"#attr"  Type:PACK"}

/*unpack default*/
#define UC_UNPKDF_FENTRY(direct, action, obj, sub, attr)   \
    {direct, action, obj, sub, attr, NULL,\
    "Direct:"#direct"  Action:"#action"  Object:"#obj"  Sub-object:"#sub"  Attribute:"#attr"  Type:UNPACK"}
#define UC_UNPACK_FENTRY(direct, action, obj, sub, attr)   \
    {direct, action, obj, sub, attr, \
    FUNCTIION##__##direct##__##action##__##obj##__##sub##__##attr##__##UNPACK,\
    "Direct:"#direct"  Action:"#action"  Object:"#obj"  Sub-object:"#sub"  Attribute:"#attr"  Type:UNPACK"}

extern uc_control_t g_uc_pack_control[UC_MAX_CONTROL];
extern uc_control_t g_uc_unpack_control[UC_MAX_CONTROL];

#endif   /* @END  __UDP_CNTRL_H__*/ 


