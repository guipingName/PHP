<?php
namespace Service\Lib;

use Service\Lib\ServiceInterface;

use Api\Lib\AsyncTask;

class UdpService extends ServiceInterface {
    const MAX_UDP_BUFF_LEN = 4096;
    
    /*添加异步任务结束回调*/
    public function finish($serv, $task_id, $data) {
        AsyncTask::finishTask($serv, $task_id, $data);
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

    /*如果需要复用对个端口监听不同业务，请重写此方法*/
    public function start() {
        $sock = $this->initSocket();
        if ($sock == false) {
            DBG_bak("create socket falied:".socket_strerror(socket_last_error($sock)));
            exit(1);
        }
        /*允许绑定多个端口*/
        if (is_array($sock)) {
            foreach ($sock as $index => $subSock) {
                swoole_event_add($subSock, array($this,'onPreRecieve'));
            }
        } else {
            $result = swoole_event_add($sock, array($this,'onPreRecieve'));
        }

    }
    
    public function onPreRecieve($fp) {
        $from = "";
        $port = 0;
        socket_recvfrom($fp, $buf, self::MAX_UDP_BUFF_LEN, 0, $from, $port);
        
        $this->onRecieve($fp, $buf, $from, $port);
    }
    
    public function initSocket() {
        return false;
    }
    
    /*此函数用于解析接收报文，并分配给指定的controller作为业务处理*/
    public function onRecieve($fp, $buf, $from, $port) {
        
    }
}