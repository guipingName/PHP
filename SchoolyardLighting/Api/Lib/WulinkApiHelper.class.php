<?php

namespace Api\Lib;

use Api\Lib\Timer;
//use Api\Model\ApiLimitConfigModel;

class WulinkApiHelper {
    private static $_redis = null;
    
    const NOTIFY_TYPE_SYNC_API_LIMIT_CONFIG = 6;
    const SUB_TYPE_SYNC_ALL = 1;
    const SUB_TYPE_SYNC_SINGLE = 2;
    
    
    public static function excute_sql_with_retry($data, $callback) {
        
        try {
            $result = call_user_func($callback, $data);
        } catch (\Exception $e) {
            /*关闭数据库*/
            if (method_exists('\Think\Db', 'closeAll')) {
                \Think\Db::closeAll();
            }
            DBG_bak('DB exception reason:'.$e->getMessage());
            $result = E_SYSTEM;
            /*添加对数据库主动断开异常处理*/
            if (stristr($e->getMessage(), 'MySQL server has gone away')) {
                try {
                    DBG_bak('Begin re-execute after mysql gone away');
                    $result = call_user_func($callback, $data);
                } catch (\Exception $exp) {
                    if (method_exists('\Think\Db', 'closeAll')) {
                        \Think\Db::closeAll();
                    }
                    DBG_bak('Re-execute after mysql gone away reason:'.$exp->getMessage());
                }
            }
        }
        
        return $result;
    }
	
    public static function init() {
        return ;
    }
    
    
    public static function onNotify($data) {
        switch($data['type']) {
        
        case WulinkApiHelper::NOTIFY_TYPE_SYNC_API_LIMIT_CONFIG:
            if (!isset($data['subType'])) {
                break;
            }

            switch ($data['subType']) {
            case WulinkApiHelper::SUB_TYPE_SYNC_ALL:
                break;
            case WulinkApiHelper::SUB_TYPE_SYNC_SINGLE:
                if (!isset($data['vendor'])) {
                    return;
                }
                break;
            }
            break;
            
        default:
            break;
        }
    }
    
}