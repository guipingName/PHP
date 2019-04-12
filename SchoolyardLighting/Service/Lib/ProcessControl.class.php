<?php
namespace Service\Lib;

use Service\Lib\ServiceInterface;

use Api\Lib\Error;
import("Api.Lib.Error");
import("Service.Lib.ServiceMacro");

class ProcessControl {
    
    /*注册进程服务*/
    protected static $_processMap = array();
    protected static $_processId  = array();
    
    protected static $_taskId     = array();
    protected static $_taskCount  = 0;
    
    protected static function startOne($map) {
        $service = $map['service'];
        $callback = function($process) use ($service) {
            ServiceInterface::$pipePendBuf = NULL;
            
            ProcessControl::$_processId = NULL;
            ProcessControl::$_taskId    = NULL;
            $instance = new $service($process);
            $instance->start();
        };
        
        $process = new \swoole_process($callback);
        $pid = $process->start();
        self::$_processId[$pid] = array('map' => $map, 'process' => $process);
        
        return $pid;
    }
    
    protected static function getTaskProcess() {
        ++self::$_taskCount;
        /*统计现有的任务进程数量*/
        $count = 0;
        foreach (self::$_processId as $pid => $map) {
            $isTask = $map['map']['task'];
            $count += ($isTask === 1)?1:0;
        }
        if ($count == 0) {
            DBG_bak('Task server is not exist, please check config');
            return NULL;
        }
        /*选择投递异步任务的进程*/
        $current =  (self::$_taskCount % $count) + 1;
        $count   = 0;
        foreach (self::$_processId as $pid => $map) {
            $isTask = $map['map']['task'];
            $count += ($isTask === 1)?1:0;
            if ($count == $current) {
                return $map['process'];
            }
        }
        DBG_bak('Find task server falied: this is inpossible');
        return NULL;
    }
    /*分发任务*/
    protected static function dispatch($pid) {
        $process = self::$_processId[$pid]['process'];
        if (empty($process)) {
            DBG_bak('Bind pipe:'.$pid.' falied');
            return false;
        }
    
        $cmdList = ServiceInterface::unserializePipe($process, $pid);
        foreach ($cmdList as $index => $cmd) {
            switch ($cmd[PIPE_COMMAND_TYPE]) {
            case PIPE_COMMAND_TASK:
                self::dispatchTask($pid, $cmd);
                break;
            default:
                break;
            }
        }
    }
    
    /*分发任务*/
    protected static function dispatchTask($pid, $task) {
        $type   = $task[PIPE_PROCESS_TYPE];
        $taskId = $task[PIPE_COMMAND_ID];
        $isTask = self::$_processId[$pid]['map']['task'];

        $process = NULL;
        switch ($type) {
        case PIPE_PROCESS_TASK:
            if ($isTask === 0) {
                DBG_bak('Task could not come from task process');
                break;
            }
            if (!isset(self::$_taskId[$taskId])) {
                DBG_bak('Task result send to worker falied by invalid taskid:'.$taskId);
                break;
            }
            $pid = self::$_taskId[$taskId];
            unset(self::$_taskId[$taskId]);
            $process = self::$_processId[$pid]['process'];
            if (empty($process)) {
                DBG_bak('Task result send to worker falied by invalid process');
                break;
            }
            break;
        case PIPE_RROCESS_WORK:
            if ($isTask === 1) {
                DBG_bak('Task could not send work process');
                break;
            }
            $process = self::getTaskProcess();
            if (!empty($process)) {
                self::$_taskId[$taskId] = $pid;
            }
            break;
        default:
            DBG_bak('Process control dispatch task invalid type:'.$type);
            break;
        }
        
        if (!empty($process)) {
            $process->write(json_encode($task).PIPE_EOF);
        }
        
        return true;
    }
    
    protected static function bindPipe($pid) {
        $process = ProcessControl::$_processId[$pid]['process'];
        if (empty($process)) {
            DBG_bak('Bind pipe failied by invalied process,pid is:'.$pid);
            break;
        }
        
        swoole_event_add($process->pipe, function ($pipe) use($pid){
            ProcessControl::dispatch($pid);
        });
    }
    
    public static function start() {
        self::$_processMap = C('REGISTER_SERVICE_PROCESS');
        
        foreach (self::$_processMap as $index => $map) {
            self::startOne($map);
        }

        foreach (self::$_processId as $pid => $map) {
            self::bindPipe($pid);
        }
        
        \swoole_process::signal(SIGCHLD, function($sig) {
            //必须为false，非阻塞模式
            while($ret =  \swoole_process::wait(false)) {
                DBG_bak('Process exit:'.json_encode($ret));
                $pid = $ret['pid'];
                $handle = ProcessControl::$_processId[$pid];
                if (!empty($handle)) {
                    $map = $handle['map'];
                    $process = $handle['process'];
                    swoole_event_del($process->pipe);
                    $process->close();
                    $process = NULL;
                    ProcessControl::$_processId[$pid] = NULL;
                    unset(ProcessControl::$_processId[$pid]);
                    unset(ServiceInterface::$pipePendBuf[$pid]);
                    
                    DBG_bak('Process name:'.$map['name'].' begin to restart');
                    $pid = ProcessControl::startOne($map);
                    ProcessControl::bindPipe($pid);
                }
                $handle = NULL;
            }
        });

    }
    
    
}