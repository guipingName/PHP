<?php
namespace Service\Lib;

use Service\Lib\ServiceInterface;
use Api\Lib\AsyncTask;

/*！！！异步任务进程必须是同步执行
 * 
 * */
class TaskService extends ServiceInterface {
    private static $_curTaskId = NULL;

    public function start() {

    }
    
    /*添加异步任务结束回调
     * TRICK:为了兼容接口，异步任务$serv为回调data
     * */
    public function finish($serv, $task_id = NULL, $data = NULL) {
        $data = $serv;
        $taskId = self::$_curTaskId;
        $realData = self::serializePipe(PIPE_COMMAND_TASK, PIPE_PROCESS_TASK, $taskId, $data);
        //DBG_bak('[Task server]task id:'.$taskId.' end excute');
        $this->_process->write($realData);
    }
    
    public function pipe($process) {
        $cmdList = self::unserializePipe($process);
        if (empty($cmdList) || !is_array($cmdList)) {
            DBG_bak('[Task server]read pipe data is not valid array');
            return;
        }
        
        foreach ($cmdList as $index => $cmd) {
            switch ($cmd[PIPE_COMMAND_TYPE]) {
            case PIPE_COMMAND_TASK:
                $taskId = $cmd[PIPE_COMMAND_ID];
                $taskData = $cmd[PIPE_COMMAND_DATA];
                self::$_curTaskId = $taskId;

                AsyncTask::excuteTask($this, $taskId, $taskId, $taskData);
                break;
            default:
                break;
            }
        }
    }
}