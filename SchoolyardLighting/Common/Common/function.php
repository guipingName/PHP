<?php  

/**
 * 日志调试
 * @param $description 日志字符串
 * @param $level 日志级别
 * @return void
 */

/*
 * 旧的日志模块
 */
function DBG_bak($description, $level = 'Log',$destination = '') {
//function DBG($description, $level = 'Log',$destination = '') {
	if (empty($description)) {
		return;
	}

    if (!empty($destination) && is_string($destination) 
            && is_dir(APP_PATH.ucfirst($destination))) {
        $description = realpath(LOG_PATH).'/'.ucfirst($destination).'/';
    }
    
	\Think\Log::write($description, $level,'',$destination);
}


//通过堆栈获取文件名
function get_caller_info_ilog(&$file, &$func, &$line, &$modelname) {
	$file = '';
	$func = '';
	$line = '';
	$modelname = '';
	$trace = debug_backtrace();
	if (isset($trace[1])) {
		$file = $trace[1]['file'];
		$line = $line = $trace[1]['line'];
	}
	if (isset($trace[2]['class']) && isset($trace[2]['function'])) {
		$modelname = $trace[2]['class'];
		$modelname = preg_replace('/.*\\\/','', $modelname);
		$func = $trace[2]['function']; $func = str_replace('\\','/', $func);
		
	}

}

//去掉日志中的特殊字符
function handle_msg(&$msg){
	$msg = str_replace('%','%%', $msg);
	$msg = str_replace("\"","'", $msg);
	
}

//新的日志接口
function DBG($msg, $level = 'log', $destination = ''){
	$stime = microtime(TRUE);
	$file = 'UNKOWN FILE';   $func = 'UNKONW FUNCTION'; $line = 'UNKOWN LINE';$modelname = 'UNKOWN MODEL';
	handle_msg($msg);
	get_caller_info_ilog($file,$func,$line, $modelname);
	$etime = microtime(TRUE); $timedelta = (string)($etime - $stime);
	//DBG_bak("time:$timedelta,modelname: $modelname");
	return ilogapi_php::ilog_output($modelname,ilogapi_php::LEVEL_DEBUG,
			$file,$func,$line,$msg);
}


function com_uuid_create() {
    if (function_exists('com_create_guid') === true) {
        return trim(com_create_guid(), '{}');
    }
    
    return sprintf('%04X%04X-%04X-%04X-%04X-%04X%04X%04X', mt_rand(0, 65535),
            mt_rand(0, 65535), mt_rand(0, 65535), mt_rand(16384, 20479),
            mt_rand(32768, 49151), mt_rand(0, 65535),
            mt_rand(0, 65535), mt_rand(0, 65535));
}


function udp_send($ip,$port,$data) {
    $socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP); //第三个参数为0
    if (!$socket) {
        DBG("[udp_send]socket_create failed; reason: " . socket_strerror($socket));
        return;
    }
    
    //$send_buff = json_encode($post);
    if (is_array($data)) {
        $data = json_encode($data);
    }
    $result = socket_sendto($socket, $data, strlen($data), 0, $ip, $port);
    socket_close($socket);
    $socket = null;
}


/**
 * 移除数组空值
 */
function array_session_filter($arr) {
    if (is_array($arr)) {
        $arr = array_filter($arr);
        foreach ( $arr as $k => $v ) {
            if(is_numeric($k)){
                unset($arr[$k]);
            }elseif(is_array($v)) {
                $arr[$k] = array_session_filter($v);
            }
        }
    }
    return $arr;
}

/**
 * 自动生成随机数，然后将随机数和生成时间写入session当中。
 */
function createRandom($length=16) {
    $pattern='1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLOMNOPQRSTUVWXYZ';
    $str = '';
    for ($i = 0; $i < $length; $i++) {
        $str = $str.$pattern{mt_rand(0,61)};
    }
    return $str;
}



/*
 * 域名解析判断
 */
function check_ip_to_dns($domain, $default){
    $returnIP = $default;
    if($domain){
        $returnIP = genp_resolv_dns_php($domain);
    }
    if($returnIP == "invalid dns"){
        $returnIP = $default;
    }
    return $returnIP;
}




/*
 * 构造GET请求地址
 */
 function get_url($url,$params) {
	$count = 0;
        if(is_array($params)){
                foreach($params as $key => $value) {
                        if ($count == 0) {
                                $url .= "?";
                        }
                        if(is_array($value)){
                            foreach($value as $k => $v){
                                if($k<(count($value)-1)){
                                    $url .= $key."[]=".rawurlencode($v)."&";
                                }else{
                                    $url .= $key."[]=".rawurlencode($v);
                                }
                                
                            }
                        }else{
                            $url .= $key."=".rawurlencode($value);
                        }
                        if ($count == (count($params) - 1)) {
                                return $url;
                        }
                        $count++;
                        $url .= "&";
                }
        }

	return $url;
 }
 
/*
 * 给定数组的某个键的键值进行排序
 * param $b 要排序的数组
 * param $array_key 指定需要按照排序的键值
 */ 
function array_ksort($b, $array_key){
    $a = array();
    foreach($b as $key=>$val){
        $a[] = $val[$array_key];//这里要注意$val[$array_key]不能为空，不然后面会出问题
    }
    sort($a);
    $a = array_flip($a);
    $result = array();
    foreach($b as $k=>$v){
        $temp1 = $v[$array_key];
        $temp2 = $a[$temp1];
        $result[$temp2] = $v;
    }
    //这里还要把$result进行排序，健的位置不对
    ksort($result);
    return $result;
 }
 