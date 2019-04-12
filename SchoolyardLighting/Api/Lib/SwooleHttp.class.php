<?php 
namespace Api\Lib;

use Api\Lib\WulinkApiHelper;

class SwooleHttp {
    private static $_server = NULL;
    const MAX_UDP_BUFF_LEN = 10920;
    

    public static function onPipeMessage($server, $src_worker_id, $data) {
        WulinkApiHelper::onNotify(json_decode($data,true));
    }
    
    public static function onWorkerStop(swoole_server $server, $worker_id) {
        
    }
    
    public static function onWorkerStart(swoole_server $server, $worker_id) {
        /*worker_id 为0时候才会处理*/
        if (!$server->taskworker) {
            WulinkApiHelper::init();
            //WulinkApiHelper::initMqServer();
        }
        self::$_server = $server;
    }
    
    public static function onWulinkWebWorkerStart(swoole_server $server, $worker_id) {
        
        /* 不需要绑定端口和接收同步消息 */
        if (!$server->taskworker) {
            //self::bindNotifyPort();
            WulinkApiHelper::init();
        }
        self::$_server = $server;
    }
    
    
    public static function onShutdown(swoole_server $server, $worker_id) {
    
    }
    
    public static function bindNotifyPort() {
        $bindIp = check_ip_to_dns(C("HTTP_V3_NOFIFY_IP"), "127.0.0.1");
        $bindPort = C("HTTP_V3_NOFIFY_PORT") ? : 12000;
        
        $sock = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
        if ($sock == false || !socket_bind($sock, $bindIp, $bindPort)) {
            DBG_bak('[Websocket]bind notify port falied by:'.socket_last_error($sock));
            return;
        }
        
        swoole_event_add($sock, array(__CLASS__,'onNotify'));
    }
    
    /*wulink service进程推送的消息*/
    public static function onNotify($fd) {
        $from = "";
        $port = 0;
        socket_recvfrom($fd, $buf, self::MAX_UDP_BUFF_LEN, 0, $from, $port);
        $workNum = self::$_server->setting['worker_num'];
        for($x = 1; $x < $workNum; $x++) {
            $result = self::$_server->sendMessage($buf, $x);
            if (!$result) {
                DBG_bak('[SwooleHttp]send message to the other worker falied,message:'.$buf);
            }
        }
        WulinkApiHelper::onNotify(json_decode($buf,true));
    }
    
    public static function syncData($data) {
        $workNum = self::$_server->setting['worker_num'];
        for($x = 0; $x < $workNum; $x++) {
            $result = self::$_server->sendMessage($data, $x);
            if (!$result) {
                DBG_bak('[SwooleHttp]send message to the other worker falied,message:'.$buf);
            }
        }
        
        WulinkApiHelper::onNotify(json_decode($data, true));
    }

}

?>