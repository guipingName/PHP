<?php

namespace Api\Lib;

use Api\Lib\Error;
import("Api.Lib.Error");

class AsyncTask {
    
    const TASK_TYPE_MODEL = 1;
    
    private static $_server       = NULL;
    private static $_taskCallback = array();
    
    public static function setServer($serv) {
        self::$_server = $serv;
    }
    
    /*
     * 得到当前Server的活动TCP连接数，启动时间，accpet/close的总次数等信息
     */
    public static function serverStats(){
        if (empty(self::$_server)) {
            throw new \Exception('Invaild server instance');
        }
        $serverStats = self::$_server->stats();
        if(!is_array($serverStats)){
            throw new \Exception('Invaild type of Server stats');
        }
        if(is_numeric($serverStats["start_time"])){
            $serverStats["start_time"] = date("Y-m-d H:i:s", $serverStats["start_time"]);
        }
        return $serverStats;
    }
    
    public static function addTask($data, $remoteCb, $finishCb) {
        if (empty(self::$_server)) {
            throw new \Exception('Invaild server instance');
        }
        if (!is_array($remoteCb) || !is_callable($remoteCb)) {
            throw new \Exception('Invaild remote callback function');
        }
        if (!is_callable($finishCb)) {
            throw new \Exception('Invaild finish callback function');
        }
        $task_id = self::$_server->task(array('data' => $data, 'callback' => $remoteCb));
        self::$_taskCallback[$task_id] = $finishCb;
    }
    
    public static function excuteTask($serv, $task_id, $from_id, $data) {
        if (!isset($data['data']) || !is_array($data['callback']) 
                || !is_callable($data['callback'])) {
            self::taskReturn($serv, E_INVALID_PARAMETER);
            return;
        }
        
        $callback = $data['callback'];
        /*加载模块的配置文件，保证配置正确加载*/
        $class = explode("\\", $callback[0]);
        if (is_array($class) && count($class) >= 2) {
            \Think\Dispatcher::loadModule(empty($class[0])?$class[1]:$class[0]);
        }
        try {
            $result =  call_user_func($callback, $data['data']);
        } catch (\Exception $e) {
            /*关闭数据库*/
            if (method_exists('\Think\Db', 'closeAll')) {
                \Think\Db::closeAll();
            }
            DBG_bak('DB exception reason:'.$e->getMessage());
            $result = E_SYSTEM_UNKNOWN;
            /*添加对数据库主动断开异常处理*/
            if (stristr($e->getMessage(), 'MySQL server has gone away')) {
                try {
                    DBG_bak('Task begin re-execute after mysql gone away');
                    $result =  call_user_func($callback, $data['data']);
                } catch (\Exception $exp) {
                    if (method_exists('\Think\Db', 'closeAll')) {
                        \Think\Db::closeAll();
                    }
                    DBG_bak('Task re-execute after mysql gone away reason:'.$exp->getMessage());
                }
            }
        }
        self::taskReturn($serv,$result);
    }
    
    public static function taskReturn($serv, $data = E_SUCCESS) {
        $serv->finish($data);
    }
    
    public static function finishTask($serv, $task_id, $data) {
        if (!isset(self::$_taskCallback[$task_id])) {
            return;
        }
        $callback = self::$_taskCallback[$task_id];
        if (is_callable($callback)) {
            //提供异步读取序列化文件，防止文件过大引起异步调用失败
            if (is_array($data) && isset($data['task_file_uuid']) 
                    && count($data) == 1 && is_file($data['task_file_uuid'])) {
                $value = file_get_contents($data['task_file_uuid']);
                unlink($data['task_file_uuid']);
                $data = unserialize($value);
                if ($data == false) {
                    $data = E_SYSTEM_UNKNOWN;
                }
            }
            $callback($data);
            $callback = null;
            self::$_taskCallback[$task_id] = null;
        }
        unset(self::$_taskCallback[$task_id]);
    }
    
    public static function serializeResult($data,$max = 7*1024) {
        $value = serialize($data);
        if (strlen($value) < $max) {
            return $data;
        }
        $uuid_file = '/tmp/'.com_uuid_create();
        file_put_contents($uuid_file, $value);
        return array('task_file_uuid' => $uuid_file);
    }
}