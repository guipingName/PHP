<?php
namespace Service\Lib;

use Service\Lib\App\DispatchService;
class DispatchHelper {
	private static $_server = false;
	
	public static function onPacket($data, $address) {
		try {
			DispatchService::onPacket($data, $address);
		} catch (\Exception $e) {
			DBG_bak('[ServiceHelper]onPacket exception:'.$e->getMessage());
		}

	}
	public static function onPipeMessage($server, $src_worker_id, $data) {

	}
	
	public static function onWorkerStop(swoole_server $server, $worker_id) {
	
	}
	
	public static function sendto($data,$address) {
	    self::$_server->sendto($address['address'], $address['port'], $data, $address['server_socket']);
	}
	
	public static function onWorkerStart(swoole_server $server, $worker_id) {
		self::$_server = $server;
		if (!$server->taskworker) {
		  DispatchService::init();
		}
	}
	
	public static function onShutdown(swoole_server $server, $worker_id) {
	
	}
	public  static function getServer() {
		return self::$_server;
	}
}