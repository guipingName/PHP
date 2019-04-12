<?php

namespace Api\Lib;

/*此文件是用于管理全局定时器*/
class Timer {
    const TIMER_TYPE_ONCE    = 1;//一次性定时器
    const TIMER_TYPE_ADD     = 2;//有限次定时器
    const TIMER_TYPE_PERSIST = 3;//无限次定时器
    
    //定时器最大生命时间，目前只针对TIMER_TYPE_ADD有效,单位s
    const TIMER_MAX_TIME     = 20;
    
    private static $_timer = array();
    private static $_timerCallback = array('Api\Lib\Timer', 'executeTimeout');
    
    public static function executeTimeout($timerId) {
        if (!isset(self::$_timer[$timerId])) {
            DBG_bak('[Timer]Timerid: '.$timerId.' is not exist when callback');
            return;
        }
        
        $callback = self::$_timer[$timerId]['callback'];
        if (!is_callable($callback)) {
            DBG_bak('[Timer]Timerid: '.$timerId.' callback is not callable');
            self::clear($timerId);
            return;
        }
        
        try {
            $result = $callback();
        } catch (\Exception $e) {
            DBG_bak('[Timer]Timerid: '.$timerId.' excute callback exception, reason:'.$e->getMessage());
        }
        
        /*定时器在回调函数中可能已经被删除，此处检查*/
        if (!isset(self::$_timer[$timerId])) {
            return;
        }
        
        $type = self::$_timer[$timerId]['type'];
        $time = self::$_timer[$timerId]['time'];
        switch($type) {
        case self::TIMER_TYPE_ONCE:
            self::clear($timerId);
            break;
        case self::TIMER_TYPE_ADD:
            $time = time() - $time;
            if ($time > self::TIMER_MAX_TIME) {
                self::clear($timerId);
            }
            break;
        case self::TIMER_TYPE_PERSIST:
            break;
        default:
            /*无效的定时器类型*/
            self::clear($timerId);
            break;
        }
    }
    
    public static function add($ms, $callback, $persist = false) {
        $timerId = swoole_timer_tick($ms, self::$_timerCallback);
        
        $type = $persist === true?self::TIMER_TYPE_PERSIST:self::TIMER_TYPE_ADD;
        self::$_timer[$timerId]= array('callback' => $callback, 'type' => $type, 'time' => time());
        
        return $timerId;
    }
    
    public static function clear($timerId) {
        if (!isset(self::$_timer[$timerId])) {
            DBG_bak('[Timer]Timerid: '.$timerId.' is not exist when clear');
            return;
        }
        
        if (!swoole_timer_clear($timerId)) {
            DBG_bak('[Timer]Timerid: '.$timerId.' clear falied');
        }
        
        //self::$_timer[$timerId]['callback'] = NULL;
        self::$_timer[$timerId] = NULL;
        unset(self::$_timer[$timerId]);
    }
    
    public static function after($ms, $callback) {
        $timerId = swoole_timer_tick($ms, self::$_timerCallback);
        self::$_timer[$timerId]= array('callback' => $callback, 'type' => self::TIMER_TYPE_ONCE, 'time' => time());
        
        return $timerId;
    }
    

}