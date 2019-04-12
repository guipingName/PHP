<?php
if(version_compare(PHP_VERSION,'5.4.0','<'))  die('require PHP > 5.4.0 !\n');

$fp = popen('uname -a',"r");
$__s=fread($fp,1024);
pclose($fp);

$__PLATFORM="x86";
if(strstr($__s,"x86")){
    $__PLATFORM="x86";
}
elseif(strstr($__s,"arm")){
    $__PLATFORM="arm";
}

if($__PLATFORM == "arm"){
    dl('swoole.so');
    dl('memcache.so');
}

if (!extension_loaded('swoole')) die('require swoole extension\n');

if (!extension_loaded('sockets')) die('require sockets extension\n');

if (!extension_loaded('PDO')) die('require PDO extension\n');

if (!extension_loaded('pdo_mysql')) die('require pdo_mysql extension\n');

if (!extension_loaded('mysql') || !extension_loaded('mysqli')) die('require mysql extension\n');

if (!extension_loaded('memcache')) die('require memcache extension\n');

if (!extension_loaded('curl')) die('require curl extension\n');

// 引入ThinkPHP入口文件
// 开启调试模式 建议开发阶段开启 部署阶段注释或者设为false
define('ROOT_PATH','/home/galaxywind/wechat_server/wechat_aysnc/');
define('APP_DEBUG',true);
define('LOG_PATH','/home/log/School/');
define('APP_MODE','service');
define('APP_PATH',ROOT_PATH.'SchoolyardLighting/');
define('XML_CONFIG','/home/etc/school_service.xml');



require '/home/galaxywind/wechat_server/wechat_aysnc/Core/ThinkPHP.php';

function app_exception_handler($e) {
    $error = array();
    $error['message']   =   $e->getMessage();
    $trace              =   $e->getTrace();
    if('E'==$trace[0]['function']) {
        $error['file']  =   $trace[0]['file'];
        $error['line']  =   $trace[0]['line'];
    }else{
        $error['file']  =   $e->getFile();
        $error['line']  =   $e->getLine();
    }
    $error['trace']     =   $e->getTraceAsString();
    trace_error($error);
}

function app_error_handler($errno, $errstr, $errfile, $errline) {
    switch ($errno) {
    case E_ERROR:
    case E_PARSE:
    case E_CORE_ERROR:
    case E_COMPILE_ERROR:
    case E_USER_ERROR:
        $errorStr = "Description:$errstr;File:".$errfile.";Line number: $errline";
        trace_error("[$errno]".$errorStr);
        break;
    default:
        $errorStr = "Description:$errstr;File:".$errfile.";Line number: $errline";
        trace_error("[$errno]".$errorStr);
        break;
    }
}
function trace_error($error) {
    //date_default_timezone_set('Asia/Shanghai');
    $e = array();
    if (!is_array($error)) {
        $errorLog  = '['.date('Y-m-d H:i:s').']'.PHP_EOL;
        $errorLog .= $error.PHP_EOL;
    } else {
        $e         = $error;
        $errorLog  = '['.date('Y-m-d H:i:s').']'.PHP_EOL;
        $errorLog .= iconv('UTF-8','gbk',$e['message']).PHP_EOL;
        $errorLog .= 'FILE: '.$e['file'].'('.$e['line'].')'.PHP_EOL;
        $errorLog .= $e['trace'].PHP_EOL;
    }
    $errorLog .= PHP_EOL;
    @file_put_contents('/home/log/School/school_'.APP_MODE.'.log', $errorLog, FILE_APPEND);
}
function fatal_error_handler() {
    if ($e = error_get_last()) {
        switch($e['type']){
        case E_ERROR:
        case E_PARSE:
        case E_CORE_ERROR:
        case E_COMPILE_ERROR:
        case E_USER_ERROR:  
            trace_error($e);
            break;
        }
    }
}

register_shutdown_function('fatal_error_handler');
set_error_handler('app_error_handler');
set_exception_handler('app_exception_handler');
if($__PLATFORM == "arm") {
	@file_put_contents('/var/run/school_'.APP_MODE.'.pid', getmypid());
} else {
	@file_put_contents('/docker/pid/school_'.APP_MODE.'.pid', getmypid());
}

use Service\Lib\ProcessControl;

ProcessControl::start();
