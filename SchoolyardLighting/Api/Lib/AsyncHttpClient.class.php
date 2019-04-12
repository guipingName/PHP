<?php
namespace Api\Lib;

class AsyncHttpClient extends \swoole_http_client{
    private static $_pushMap = array();//当前http请求消息队列
    private static $_connMap = array();//当前http请求连接
    private static $_dnsMap = array();//dns缓存map
    private static $_globalConnId = 0;//当前连接全局ID
    private static $_totalConnId = 0;//当前http实例个数
    private static $callback = array(__CLASS__,'callback');
    
    const MAX_CONNECTION_COUNT = 100000;//最大的请求数量，强制退出，防止内存泄露
    
    const MAX_EXECUTE_TIME = 10;//http请求最大时间，超时就退出
    const MAX_QUEUE_BY_HOST = 1000;//每个域名推送最大队列长度
    const MAX_CONCURRENCE_BY_HOST = 100;//最大并发请求数量
    const MAX_RETRY_TIMES = 2;//最大重传次数
    
    public $_connId = NULL;
    public $_host = '';
    
    public static function jsonPost($ip ,$port,$url, $message = '',$ssl =false) {
        if (!is_string($ip) || !is_numeric($port) || !is_string($url)) {
            return;
        }
        $host = $ip.':'.$port;
        if (isset(self::$_pushMap[$host]) && 
                count(self::$_pushMap[$host]) >=  self::MAX_QUEUE_BY_HOST) {
            //检查队列是否已经满了，直接丢掉
            return;
        }
        $connId = self::$_globalConnId++;
        self::$_pushMap[$host][$connId] = array('ip' => $ip,'port' => $port,'retry' => 0,'url' => $url,
                                            //'message' => is_array($message)?$message:json_decode($message,true),
                                            'message' => is_string($message)?$message:json_encode($message),
                                            'ssl'=>$ssl,'push' => false);
        if (isset(self::$_dnsMap[$host]) && is_string(self::$_dnsMap[$host])) {
            self::doNextPost($host);
        } else if (!isset(self::$_dnsMap[$host])){//异步查询dns
            self::$_dnsMap[$host] = false;
            swoole_async_dns_lookup($ip, function($domain, $ip) use ($host) {
                DBG_bak(json_encode(self::$_pushMap));
                if (empty($ip)) {
                    return;
                }
                self::$_dnsMap[$host] = $ip;
                $count = isset(self::$_connMap[$host])?count(self::$_connMap[$host]):0;
                $min = min(array(self::MAX_CONCURRENCE_BY_HOST,count(self::$_pushMap[$host])));
                for (;$count < $min;$count++) {
                    self::doNextPost($host);
                }
            });
        }
    }
    
    private static function doNextPost($host = false) {
        if (!is_array(self::$_pushMap[$host]) || empty(self::$_pushMap[$host])
                || (isset(self::$_connMap[$host]) 
                && count(self::$_connMap[$host]) >= self::MAX_CONCURRENCE_BY_HOST)) {
            return;
        }
        $keys = array_keys(self::$_pushMap[$host]);
        foreach ($keys as $i => $id) {
            $message = &self::$_pushMap[$host][$id];
            if ($message['retry'] >= self::MAX_RETRY_TIMES && $message['push'] == false){
                unset(self::$_pushMap[$host][$id]);unset($message);
                continue;
            } else if ($message['push'] == true) {
                unset($message);continue;
            }
            $message['retry'] += 1;$message['push'] = true;
            $host = $message['ip'].':'.$message['port'];
            $ip = is_string(self::$_dnsMap[$host])?self::$_dnsMap[$host]:$message['ip'];
            $cli = new AsyncHttpClient($ip,$message['port'], $id, $message['ssl']);
            self::$_totalConnId++;
            self::$_connMap[$host][$id] = array('connection' => $cli,'time' => time());
            if($message['ssl']){
                //https访问
                $http_host = ($message['port'] == 443)?$message['ip']:$host;
            }else{
                $http_host = ($message['port'] == 80)?$message['ip']:$host;
            }
            DBG_bak("is https:" . $message['ssl'] . "-----------http_host=" . $http_host);
            $cli->setHeaders(array('Content-Type' =>'application/json;charset=utf-8',
                    'Host' => $http_host));
            $cli->post($message['url'], $message['message'], self::$callback);
            unset($message);
            break;
        }
    }
    
    public function __construct($ip, $port,$id = 0,$ssl = false) {
        $this->_connId = $id;
        $this->_host = $ip.':'.$port;
        parent::__construct($ip, $port, $ssl);
    }

    public static function checkConnection() {
        foreach (self::$_connMap as $host => &$connections) {
            $keys = array_keys(self::$_connMap[$host]);
            foreach ($keys as $i => $id) {
                $connection = self::$_connMap[$host][$id];
                $handle = $connection['connection'];
                $statusCode = isset($handle->statusCode)?$handle->statusCode:0;
                if ((time() - $connection['time']) > self::MAX_EXECUTE_TIME
                        || ($statusCode != 200 && $statusCode != 0)) {
                    $handle = $connection['connection'];
                    if ($statusCode != 0 && $handle->isConnected()) {
                        $handle->close();
                    }
                    unset(self::$_connMap[$host][$id]);//移除连接队列
                    self::$_pushMap[$host][$id]['push'] = false;//push结束标志
                    self::doNextPost($host);
                } else if ($statusCode == 200){
                    unset(self::$_connMap[$host][$id]);//移除连接队列
                    unset(self::$_pushMap[$host][$id]);//移出推送列队
                    self::doNextPost($host);
                }
                $handle = NULL;$connection= NULL;
            }
            unset($connections);
        }
        if (self::$_totalConnId > self::MAX_CONNECTION_COUNT) {
            DBG_bak('[AsyncHttpClient]normal exit by max connection count');
            exit(0);
        }
    }
    
    public static function callback($cli) {
        $log = 'Host: '.$cli->host.':'.$cli->port.PHP_EOL;
        $log .= 'Result: '.$cli->body.';statusCode:'.$cli->statusCode.PHP_EOL;
        DBG_bak($log);
        self::checkConnection();
    }
    
    public static function init() {
        swoole_timer_tick(4000,function() {
            self::checkConnection();    
        });
    }  

}