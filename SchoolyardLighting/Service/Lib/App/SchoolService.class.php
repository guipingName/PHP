<?php
namespace Service\Lib\App;

use Service\Lib\ServiceInterface;
use Service\Lib\TaskService;
use Api\Lib\Timer;
use Api\Lib\AsyncTask;

class SchoolService extends TaskService {
    public static $_maxCount = 0;
    public static $_currentDate = '';
    
    public function start() {
        parent::start();
        self::$_currentDate = date("Y-m-d");
        
        self::init_when_start();
        /*2分钟更新统计数据：*/
        Timer::add(2*60*1000, function() {
            /**/
            self::update_minute_statistic();
        },true);
        /* 两小时更新一下基于天的统计数据 */
        Timer::add(5*60*1000, function() {
           /* 更新API访问统计 */
            self::update_day_statistic();
        },true);
        //AsyncHttpClient::init();
    }

    
    /*添加异步任务结束回调*/
    public function finish($serv, $task_id, $data) {
        try {
        AsyncTask::finishTask($serv, $task_id, $data);
        }catch(\Exception $e ) {
            DBG_bak("finish error message = ".$e->getMessage());
        }
    }
    
    public function pipe($process) {
        $cmdList = self::unserializePipe($process);
        if (empty($cmdList) || !is_array($cmdList)) {
            DBG_bak('[UdpService]read pipe data is not valid array');
            return;
        }
        
        foreach ($cmdList as $index => $cmd) {
            switch ($cmd[PIPE_COMMAND_TYPE]) {
            case PIPE_COMMAND_TASK:
                $taskId = $cmd[PIPE_COMMAND_ID];
                $taskData = $cmd[PIPE_COMMAND_DATA];
                $this->finish($process, $taskId, $taskData);
                break;
            default:
                break;
            }
        }
    }
     
    public static function init_when_start() {
        self::update_minute_statistic();
        self::update_day_statistic();
    }
     
     
    public static function update_minute_statistic() {
        DBG_bak("update_minute_statistic now");
        $today = date("Y-m-d");
        DBG_bak(" cur max count = ".self::$_maxCount);
        if (self::$_currentDate != $today) {
            self::$_currentDate = $today;
            self::$_maxCount = 0;
        }
        
        try {
        /* 更新统计 */
        AsyncTask::addTask(array("max_count" => self::$_maxCount), 
            array('Service\Lib\Task\ServiceTask', 'update_miniute_satistics'),
            function ($data) {
                if (is_array($data)) {
                    self::$_maxCount = $data['max_count'];
                    DBG_bak("new max count = ".$data['max_count']);
                }
            });
        }catch(\Exception $e ) {
            DBG_bak("finish error message = ".$e->getMessage());
        }
    }
    
    public static function update_day_statistic() {
        
        DBG_bak("update_day_statistic now");
        try{
        AsyncTask::addTask(array(), 
            array('Service\Lib\Task\ServiceTask', 'update_day_statistics'),
            function ($data) {
                
            });
        } catch(\Exception $e) {
            DBG_bak("add task error , e = ".$e->getMessage());
        }
    }
     
}
