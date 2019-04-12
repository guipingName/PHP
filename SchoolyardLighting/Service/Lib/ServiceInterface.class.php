<?php
namespace Service\Lib;

use Api\Lib\AsyncTask;

abstract class ServiceInterface {
    protected $_process = NULL;
    
    public static $pipePendBuf = NULL;
    
    public function __construct($process) {
        $this->_process = $process;
        AsyncTask::setServer($this);
        swoole_event_add($process->pipe, function ($pipe) use ($process){
            $this->pipe($process);
        });
    }
    
    public static function serializePipe($command, $type, $id, $data) {
        $realData =  array(
                PIPE_COMMAND_TYPE => $command,
                PIPE_PROCESS_TYPE => $type,
                PIPE_COMMAND_ID   => $id,
                PIPE_COMMAND_DATA => $data);
        return json_encode($realData).PIPE_EOF;
    }
    
    public static function unserializePipe($process, $pid = NULL) {
        $pipePend = &self::$pipePendBuf;
        
        $data = $process->read();
        
        $pidKey = $pid."";

        if (is_array($pipePend) && !empty($pid) && isset($pipePend[$pidKey]) && is_string($pipePend[$pidKey])) {
            $data = $pipePend[$pidKey].$data;
            unset($pipePend[$pidKey]);
        } else if (is_string($pipePend)){
            $data = $pipePend.$data;
            $pipePend = NULL;
        } else {
            $pipePend = NULL;
        }
        
        $tasks = explode(PIPE_EOF, $data);
        $taskList = array();
        foreach ($tasks as $index => $value) {
            if (empty($value)) {
                continue;
            }
            $task = json_decode($value, true);
            //检查边界问题
            if (empty($task) && !empty($value)) {
                if (empty($pid)) {
                    $pipePend = $value;
                } else {
                    $pipePend[$pidKey] = $value;
                }
            } else if (!empty($task)) {
                $taskList[] = $task;
            }
        }
        return $taskList;
    }
    
    /*添加异步任务:采用数组方式传递*/
    public function task($data) {
        $taskId = self::getGUID();
        
        $realData = self::serializePipe(PIPE_COMMAND_TASK, PIPE_RROCESS_WORK, $taskId, $data);
        $this->_process->write($realData);
        
        return $taskId;
    }
    
    public static function getGUID() {
        if (function_exists('com_create_guid') === true) {
            return trim(com_create_guid(), '{}');
        }
        
        return sprintf('%04X%04X-%04X-%04X-%04X-%04X%04X%04X', mt_rand(0, 65535), mt_rand(0, 65535), 
                mt_rand(0, 65535), mt_rand(16384, 20479), 
                mt_rand(32768, 49151), mt_rand(0, 65535), 
                mt_rand(0, 65535), mt_rand(0, 65535));
    }

    /*添加进程启动*/
    public abstract function start();
    
    /*添加异步任务结束回调*/
    public abstract function finish($serv, $task_id, $data);

    /*父进程传递数据*/
    public abstract function pipe($process);
}