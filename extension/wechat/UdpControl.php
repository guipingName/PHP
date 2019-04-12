<?php

class UdpControl {
	private $_socket = null;
	private $_udpServer = array();
	
	const DEF_DISPATCHER_PORT =  1180;
	const DEF_DISPATCHER_IP   =  "115.29.198.74";
	const DEF_ICE_PORT        =  1196;
	
	const SEND_TIMEOUT_SEC  = 2;
	const SEND_TIMEOUT_USEC = 0;
	const RECV_TIMEOUT_SEC  = 10;
	const RECV_TIMEOUT_USEC = 0;

	protected function getUdpServer($sn) {
		if (isset($this->_udpServer[$sn])) {
			return $this->_udpServer[$sn];
		}

		$sendData = wechat_udp_bind_pack($sn);
		$this->send($sendData, UdpControl::DEF_DISPATCHER_IP, UdpControl::DEF_DISPATCHER_PORT);
		$snData = $this->recv();
		if (!empty($snData)) {
			$snData = wechat_udp_bind_unpack($snData);
		}

		if (empty($snData['devserver_ip'])) {
			$udpIp = self::DEF_DISPATCHER_IP;
			//return false;
		} else {
			$udpIp = long2ip($snData['devserver_ip']);
			$this->_udpServer[$sn] = $udpIp;
		}
		return $udpIp;
	}
	
	public function __construct() {
		if(!extension_loaded('wechat')) {
			dl('wechat.' . PHP_SHLIB_SUFFIX);
		}
		$this->_socket = socket_create( AF_INET, SOCK_DGRAM, SOL_UDP);
		if ( $this->_socket === false ) {
			echo "socket_create() failed:reason:" . socket_strerror( socket_last_error() ) . "\n";
		}
		
		$timeout = array('sec'=> self::SEND_TIMEOUT_SEC,'usec'=> self::SEND_TIMEOUT_USEC);
		socket_set_option($this->_socket,SOL_SOCKET,SO_SNDTIMEO,$timeout);
		$timeout = array('sec'=> self::RECV_TIMEOUT_SEC,'usec'=> self::RECV_TIMEOUT_USEC);
		socket_set_option($this->_socket,SOL_SOCKET,SO_RCVTIMEO,$timeout);
		socket_set_block($this->_socket);
	}
	public function __destruct() {
		$this->close();
	}
	public function send($data, $ip, $port) {
		$result = socket_sendto($this->_socket, $data, strlen($data),
			0, $ip, $port);
		if (empty($result) || $result != strlen($data)) {
			echo 'Error code:'.socket_last_error().'  Reason:'.socket_strerror(socket_last_error());
		}
		return $result;
	}
	
	public function recv() {
		$from = '';
		$port = 0;
		$result = socket_recvfrom($this->_socket, $data, 8096, 0, $from, $port);
		if ($result === false) {
			echo 'Error code:'.socket_last_error().'  Reason:'.socket_strerror(socket_last_error());
		}
		return $data;
	}
	
	public function close() {
		socket_close($this->_socket);
		unset($this->_socket);
	}
	
	public static function generateControl($objct, $subObjct, $attr) {
		return array("objct" => $objct, 
				"sub_objct" => $subObjct, 
				"attr" => $attr);
	}
	
	public function excuteControl($devSn, $cmd, $action) {
		$udpIp = $this->getUdpServer($devSn);
		if (empty($udpIp)) {
			return false;
		}
		$sendData = wechat_udp_control_pack($devSn, $cmd, $action);
		$this->send($sendData, $udpIp, UdpControl::DEF_ICE_PORT);
		$recvData = $this->recv();
		if (empty($recvData)) {
			return false;
		}
		$unpackData = wechat_udp_control_unpack($recvData);	
		return $unpackData;
	}
	
	public static function fetchControl($unpack, $cmd, $keyword = false,$default = false) {
		if (!is_array($unpack) || !is_array($cmd)) {
			return $default;
		}
		
		if (!isset($unpack[UC_DATA]) || !is_array($unpack[UC_DATA])) {
			return $default;
		}
		
		$data = $unpack[UC_DATA];
		foreach ($data as $key => $control) {
			if ($control['objct'] == $cmd['objct'] && $control['sub_objct'] == $cmd['sub_objct']
					&& $control['attr'] == $cmd['attr']) {
		        if (empty($keyword)) {
		        	return $control;
				} else if (isset($control[$keyword])) {
					return $control[$keyword];
				} else if (isset($data[UC_DATA][$keyword])){
					return $data[UC_DATA][$keyword];
				}
				return $default;
			}
		}
		return $default;
	}
}