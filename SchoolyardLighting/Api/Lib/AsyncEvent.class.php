<?php

namespace Api\Lib;

use Api\Lib\Timer;

/*此文件是用于管理全局异步事件*/
class AsyncEvent {
    const EVENT_NORMAL           = 1;//事件正常回调
    const EVENT_TIMOUT           = 2;//事件超时事件触发
    const EVENT_FORCE_TIMOUT     = 3;//事件必须回调退出
    
    private static $_checkTimerId = NULL;
    
    const TIMER_MAX_TIME     = 20;

    private static $_event = array();
    private static $_eventCallback = array('Api\Lib\AsyncEvent', 'executeEvent');
    
    public static function executeEvent($event,$type = AsyncEvent::EVENT_NORMAL) {
        if (!isset(self::$_event[$event])) {
            DBG_bak('[AsyncEvent]Event id: '.$event.' is not exist when callback');
            return;
        }
        
        $callback = self::$_event[$event]['callback'];
        if (!is_callable($callback)) {
            DBG_bak('[AsyncEvent]Event id: '.$event.' callback is not callable');
            self::clear($event);
            return;
        }
        
        try {
            $result = $callback($event,$type);
        } catch (\Exception $e) {
            DBG_bak('[AsyncEvent]Event id: '.$event.' excute callback exception, reason:'.$e->getMessage());
        }
    }
    /*检查全局事件是否有超过*/
    public static function checkMaxTimeout() {
        $time = time();
        $tmpEvent = self::$_event;
        foreach ($tmpEvent as $event => $value) {
            if (($time - $value['time']) >= self::TIMER_MAX_TIME) {
                DBG_bak('[AsyncEvent]Event id: '.$event.' is over the max event time');
                AsyncEvent::excuteEvent($event,AsyncEvent::EVENT_FORCE_TIMOUT);
                AsyncEvent::delete($event);
            }
        }
        $tmpEvent = NULL;
    }
    
    /*callback 原型：function  fun($event,$isTimeout)，
     * 注意;暂时不支持带参数；而且采用use更方便
     * 由于考虑udp控制协议流程，此处不控制delete操作，外部需要主动调用删除
     * */
    public static function add($event, $callback,$timeout = 3000,$isOnce = false) {
        if (is_null(self::$_checkTimerId)){
            self::$_checkTimerId = Timer::add(self::TIMER_MAX_TIME*1000/2, array('Api\Lib\AsyncEvent', 'checkMaxTimeout'),true);
        }
        if (isset(self::$_event[$event])) {
            DBG_bak('[AsyncEvent]Event id: '.$event.' has been add');
            return;
        }
        if (!swoole_event_add($event, self::$_eventCallback)) {
            DBG_bak('[AsyncEvent]Event id: '.$event.' add failed');
            return;
        }
        self::$_event[$event] = array('callback' => $callback);
        
        if (is_numeric($timeout) && $timeout > 0) {
            if ($isOnce === true) {//只执行一次timeout
                $timerId = Timer::after($timeout, function() use ($event) {
                    self::$_event[$event]['timerid'] = NULL;
                    AsyncEvent::executeEvent($event,AsyncEvent::EVENT_FORCE_TIMOUT);
                    AsyncEvent::delete($event);
                });
            } else {//执行多次timeout,交给顶层去delete
                $timerId = Timer::add($timeout, function() use ($event) {
                    AsyncEvent::executeEvent($event,AsyncEvent::EVENT_TIMOUT);
                },true);
            }
            self::$_event[$event]['timerid'] = $timerId;
        }
        self::$_event[$event]['time'] = time();
    }
    
    public static function delete($event) {
        if (!isset(self::$_event[$event])) {
            return;
        }

        if (!swoole_event_del($event)) {
            DBG_bak('[AsyncEvent]Event id: '.$event.' delete falied');
        }
        
        /*在非超时场景下，需要主动清除定时器*/
        if (isset(self::$_event[$event]['timerid'])) {
            Timer::clear(self::$_event[$event]['timerid']);
        }
        
        //self::$_event[$event]['callback'] = NULL;
        self::$_event[$event] = NULL;
        unset(self::$_event[$event]);
    }
}