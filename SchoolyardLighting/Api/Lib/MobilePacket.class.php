<?php
namespace Api\Lib;

use Api\Lib\AsyncEvent;


class MobilePacket {
    const MAX_RETRY_COUNT        = 1;
    const MAX_TIMEOUT            = 3000;
    
    const UDP_CTRL_INIT          = 0;
    const UDP_CTRL_SEND_UDP      = 1;
    const UDP_CTRL_RECV_UDP      = 2;
    const UDP_CTRL_END           = 3;
    
    private $_socket = null;
    private $_lastError = "";
    
    private $_step    = null;
    
    private $_callback = null;
    private $_sendData = null;
    private $_rfgwsn = null;
        
    const DEF_ICE_PORT        =  1196;
    
    const SEND_TIMEOUT_SEC  = 2;
    const SEND_TIMEOUT_USEC = 0;
    const RECV_TIMEOUT_SEC  = 10;
    const RECV_TIMEOUT_USEC = 0;

    public function __construct() {        
        $this->_step = self::UDP_CTRL_INIT;
        
        $this->_socket = socket_create( AF_INET, SOCK_DGRAM, SOL_UDP);
        if ( $this->_socket === false ) {
            DBG_bak('[MobilePacket] Socket create code:' .socket_last_error(). '  Reason:'.socket_strerror(socket_last_error()));
        }
        
        $timeout = array('sec'=> self::SEND_TIMEOUT_SEC,'usec'=> self::SEND_TIMEOUT_USEC);
        socket_set_option($this->_socket,SOL_SOCKET,SO_SNDTIMEO,$timeout);
        $timeout = array('sec'=> self::RECV_TIMEOUT_SEC,'usec'=> self::RECV_TIMEOUT_USEC);
        socket_set_option($this->_socket,SOL_SOCKET,SO_RCVTIMEO,$timeout);
        socket_set_block($this->_socket);
    }
	
    public function __destruct() {
    	//DBG_bak("MobilePacket:_destruct");
        $this->close();
    }
    
    public function send($data, $ip, $port) {
        $result = socket_sendto($this->_socket, $data, strlen($data),
            0, $ip, $port);
            
        if (empty($result) || $result != strlen($data)) {
            DBG_bak('[MobilePacket] Send error code:' .socket_last_error(). '  Reason:'.socket_strerror(socket_last_error()));
        }
        return $result;
    }
	
    public function recv() {
        $from = '';
        $port = 0;
        $result = socket_recvfrom($this->_socket, $data, 8096, 0, $from, $port);
        if ($result === false) {
            DBG_bak('[MobilePacket] Recv error code:' .socket_last_error(). '  Reason:'.socket_strerror(socket_last_error()));
        }
        return $data;
    }
    
    public function getLastError() {
        
        return $this->_lastError;
    }
    
    public function checkSocket() {
    
        return !empty($this->_socket);
    }
    
    public function close() {
        $this->_step = self::UDP_CTRL_END;

        if (isset($this->_socket)) {
            AsyncEvent::delete($this->_socket);
            socket_close($this->_socket);
            $this->_socket = null;
        }

        $this->_callback = null;
        $this->_lastError = null;
        $this->_retryCount = 0;
        $this->_ctrlRetryCount = 0;
        $this->_protocal_type = "";
    }
	    
    public function externalExcute($event,$type) {
        
        switch ($type) {
        case AsyncEvent::EVENT_NORMAL:
            $this->excuteStep();
            break;
        case AsyncEvent::EVENT_TIMOUT:
        case AsyncEvent::EVENT_FORCE_TIMOUT:
        default:
            DBG_bak('[UdpContorl] async event force timeout happen:'.$type);
            $this->excuteEnd(E_TIMEOUT);
            break;
        }
    }
    
    /*udp控制协议状态机*/
    protected function excuteStep() {        
        switch($this->_step) {
        case self::UDP_CTRL_INIT:
            $udpIp = "127.0.0.1";

            DBG_bak(sprintf("send udp pkt to icestreamproxy:%s",  $udpIp));
            $this->send($this->_sendData, $udpIp, self::DEF_ICE_PORT);
            $this->_step = self::UDP_CTRL_SEND_UDP;
            break;
        case self::UDP_CTRL_SEND_UDP:
            $unpackData = $this->recv();
            if (!empty($unpackData)) {
                //DBG_bak(sprintf("Recv udp pkt len :%s",strlen($unpackData)));
                //DBG_bak(sprintf("Recv udp pkt bin :%s",bin2hex($unpackData)));
                
                $out = array();
                $unpackData = $this->controlUnpack($unpackData);
                
                //DBG_bak(sprintf("Recv udp pkt:%s",json_encode($unpackData)));
            }
            $this->excuteEnd($unpackData);
            break;
        default:
            $this->excuteEnd(E_UNKNOWN);
            break;
        }
    }
    
    protected function excuteEnd($unpackData = false) {
    	$this->_step = self::UDP_CTRL_END;
    	
        if (isset($this->_timerId)) {
            swoole_timer_clear($this->_timerId);
            $this->_timerId = null;
        }
        
        $this->_retryCount = 0;
        //DBG_bak("Exctute end 11111111111111 sn = ");
        $this->_ctrlRetryCount = 0;
        
        /*！！！必须在回调之前删除定时器，否则在二次回调的时候已经被删掉*/
        AsyncEvent::delete($this->_socket);
        
        $callback = $this->_callback; 
        if (is_callable($callback)) {
            $this->_callback = null;
            $continue = $callback($unpackData);
            $callback = null;
        }
    }
       
    
    public static function generatePack($type, $value, $need_len = 0, $net_order = 1) {
        if (is_null($value)) {
            $value = ($type == UC_STRING)?"":0;
        }
        switch($type) {
            case UC_INT8:
            case UC_UINT8:
                $need_len = 1;
                break;
            case UC_INT16:
            case UC_UINT16:
                $need_len = 2;
                break;
            case UC_INT32:
            case UC_UINT32:
                $need_len = 4;
                break;
            case UC_INT64:
            case UC_UINT64:
                $need_len = 8;
                break;
            case UC_STRING:
                if ($need_len <=0 ) {
                    throw new \Exception("String must set need_len");
                }
                break;
            default:
                return false;
                break;
        }
    
        return array('type'    => $type,
                'value'     => $value,
                'need_len'  => $need_len,
                'net_order' => $net_order != 1? 0:$net_order,
        );
    }
    public static function generateUnpack($type, $value, $need_len = 0, $net_order = 1) {
        switch($type) {
            case UC_INT8:
            case UC_UINT8:
                $need_len = 1;
                break;
            case UC_INT16:
            case UC_UINT16:
                $need_len = 2;
                break;
            case UC_INT32:
            case UC_UINT32:
                $need_len = 4;
                break;
            case UC_INT64:
            case UC_UINT64:
                $need_len = 8;
                break;
            case UC_STRING:
                break;
            default:
                return false;
                break;
        }
    
        return array('type'    => $type,
                'value'     => $value,
                'need_len'  => $need_len,
                'net_order' => $net_order != 1? 0:$net_order,
        );
    }
    
    
    protected  function controlPack($data, $datalen) {
        $format = array();
    	if ($this->_protocal_type == 'MACBEE_V4.0') { //跟ice通信采用MACBEE4.0协议
    		DBG_bak('MACBEE_V4.0');
    		/*uc_trans_hdr*/
    		$randnum = rand();
    		$format[] = self::generatePack(UC_UINT64, $this->_rfgwsn);
    		$format[] = self::generatePack(UC_UINT32, $randnum);  //填充为随机数，用于判断ice回包
    		$format[] = self::generatePack(UC_UINT8,  0);
    		$format[] = self::generatePack(UC_STRING, "\0\0\6", 3);  //0-透传，2-macbee4.0
    	}
    	else{ //跟ice通行的第一版协议
    		/*uc_trans_hdr*/
    		$format[] = self::generatePack(UC_UINT64, $this->_rfgwsn);
    		$format[] = self::generatePack(UC_UINT32, 0);
    		$format[] = self::generatePack(UC_UINT8,  0);
    		$format[] = self::generatePack(UC_STRING, "\0\0\0", 3);  //后两个字节为0-透传
    	}
    		
    	$hdr = wechat_pack($format);
    	$data = $hdr . $data;
    	
    	DBG_bak("send packet:" . bin2hex($data));
    	return $data;
    }
    
    public  function controlUnpack($buff, &$return) {
        static $format = null;
        if (empty($format)) {
            /*uc_trans_hdr*/
            $format[] = self::generateUnpack(UC_UINT64, 'sn');
            $format[] = self::generateUnpack(UC_UINT32, 'errorcode');
            $format[] = self::generateUnpack(UC_UINT8, 'cfg_id');
            $format[] = self::generateUnpack(UC_STRING, 'pad', 3);
            /*ucph_t*/
            $format[] = self::generateUnpack(UC_UINT8, 'ucph_hdr');;
            $format[] = self::generateUnpack(UC_UINT8, 'flags');
            $format[] = self::generateUnpack(UC_UINT8, 'request_id');
            $format[] = self::generateUnpack(UC_UINT8, 'resv');
            $format[] = self::generateUnpack(UC_UINT32, 'client_sid');
            $format[] = self::generateUnpack(UC_UINT32, 'device_sid');
            $format[] = self::generateUnpack(UC_UINT16, 'command');
            $format[] = self::generateUnpack(UC_UINT16, 'param_len');
        }
        $r = wechat_unpack($buff, $format,$return);
        //var_dump($return);
        
        return $return;
    }

    
    
    /*
     * 执行报文整个操作流程，兼容以前版本
     * swoole_event_add swoole_timer_tick可能会引起callback内存泄露
     * 根据源码显示是因为引用引起
     */
    public function excuteControl($rfgwsn,$data,$callback,$outtime=false, $protocal_type = '') {
        if (is_callable($callback)) {
            if (!function_exists('swoole_event_add')) {
                throw new \Eexception("swoole_event_add not exist");
            }
            
            $this->_callback   = $callback;
            $this->_rfgwsn   = $rfgwsn;
            $this->_retryCount = 0;
            $this->_ctrlRetryCount = 0;
            $this->_protocal_type = $protocal_type;
            $this->_step       = self::UDP_CTRL_INIT;
            /* 根据配置决定用多少秒的timeout */
            $updTimeout = C("UDP_TIMEOUT_MS") ? : 3000;
            if($outtime){
                    $updTimeout = $outtime;
            }
            
            $this->_sendData = $this->controlPack($data);
            //DBG_bak("  controlPack data:" . bin2hex($this->_sendData));
            
            AsyncEvent::add($this->_socket, array($this, 'externalExcute'), $updTimeout);
            
            $this->excuteStep();
            return true;
        }
        
        $this->_protocal_type = $protocal_type;
        $this->_sendData = $this->controlPack($data);
        //DBG_bak("controlPack data:" . bin2hex($this->_sendData));        
        $this->send($this->_sendData, "127.0.0.1", self::DEF_ICE_PORT);
        $recvData = $this->recv();
        if (empty($recvData)) {
            return false;
        }
        $unpackData = $this->controlUnpack($recvData);
        return $unpackData;
    }
        
}

?>
