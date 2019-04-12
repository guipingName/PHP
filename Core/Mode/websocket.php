<?php
// +----------------------------------------------------------------------
// | ThinkPHP [ WE CAN DO IT JUST THINK IT ]
// +----------------------------------------------------------------------
// | Copyright (c) 2006-2014 http://thinkphp.cn All rights reserved.
// +----------------------------------------------------------------------
// | Licensed ( http://www.apache.org/licenses/LICENSE-2.0 )
// +----------------------------------------------------------------------
// | Author: liu21st <liu21st@gmail.com>
// +----------------------------------------------------------------------
      
/**
 * ThinkPHP Service模式定义
 */
return array(
    // 配置文件
    'config'    =>  array(
        THINK_PATH.'Conf/convention.php',   // 系统惯例配置
        CONF_PATH.'config'.CONF_EXT,      // 应用公共配置
        MODE_PATH.'Websocket/config'.CONF_EXT,      // 应用公共配置
    ),

    // 别名定义
    'alias'     =>  array(
        'Think\Exception'         => CORE_PATH . 'Exception'.EXT,
        'Think\Model'             => CORE_PATH . 'Model'.EXT,
        'Think\Db'                => CORE_PATH . 'Db'.EXT,
        'Think\Cache'             => CORE_PATH . 'Cache'.EXT,
        'Think\Cache\Driver\File' => CORE_PATH . 'Cache/Driver/File'.EXT,
        'Think\Storage'           => CORE_PATH . 'Storage'.EXT,
    ),

    // 函数和类文件
    'core'      =>  array(
        MODE_PATH.'Websocket/functions.php',
        COMMON_PATH.'Common/function.php',
        MODE_PATH . 'Websocket/App'.EXT,
        MODE_PATH . 'Websocket/Dispatcher'.EXT,
        MODE_PATH . 'Websocket/Controller'.EXT,
    ),
);
