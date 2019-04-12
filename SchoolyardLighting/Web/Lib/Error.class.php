<?php
namespace Web\Lib;

define("E_SYSTEM_BUSY",         -1);
define("E_SUCCESS",             0);
define("E_SYSTEM_UNKNOWN",      1010);
define("E_INVALID_PARAMETER",   1020);
define("E_PARAMETER_NULL",      1030);
define("E_PARAMETER_CHECK",     1040);
define("E_NO_DATA",             1050);
define("E_TOO_OFTEN",           1060);
define("E_INVALID_AUTH",        2020);

define("E_ADDED_DEVICE",        3001);
define("E_DEVICE_BEEN_ADDED",   3002);
define("E_DEVICE_UNMATCH_SAFECODE",      3003);
define("E_SAFECODE_OVERDUE",    3004);

define("E_INVALID_METHOD",      3050);
define("E_TIMEOUT",             3060);

define("E_PASSWORD_USING",      4001);




class Error {
    public static $GLOBAL_ERROR = array(
                                    E_SYSTEM_BUSY               => "系统繁忙，请稍后再试",
                                    E_SUCCESS                   => "请求成功",
                                    E_SYSTEM_UNKNOWN            => "系统未知错误",
                                    E_INVALID_PARAMETER         => "参数格式错误",
                                    E_PARAMETER_NULL            => "提交参数内容为空",
                                    E_PARAMETER_CHECK           => "参数检查失败，提交参数不正确",
                                    E_NO_DATA                   => "没有查询到相关数据",
                                    E_TOO_OFTEN                 => "操作太频繁，请稍后再试",
                                    E_INVALID_AUTH              => "accessToken无效",
                                    E_ADDED_DEVICE              => "设备添加失败",
                                    E_DEVICE_BEEN_ADDED         => "设备已经被添加",
                                    E_DEVICE_UNMATCH_SAFECODE   => "设备与安全码不匹配",
                                    E_SAFECODE_OVERDUE          => "安全码过期",
                                    E_INVALID_METHOD            => "请求方法错误",
                                    E_TIMEOUT                   => "请求超时",
                                    E_PASSWORD_USING            => "密码正在操作中,不能进行其他操作",
        
                                );
								
    public static function getError($code,$errcode = 'result',$errmsg = 'errmsg') {
        $error = Error::$GLOBAL_ERROR;
        if (isset($code) && isset($error[$code])) {
            if($code == 0){
                return array($errcode => "success", $errmsg => $error[$code]);
            }else{
                return array($errcode => "fail", $errmsg => $error[$code]);
            }
            
        } else {
            return array($errcode => "fail", $errmsg => $error[E_SYSTEM_UNKNOWN]);
        }
    }
}