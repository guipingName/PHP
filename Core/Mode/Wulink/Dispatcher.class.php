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
namespace Think;
/**
 * ThinkPHP内置的Dispatcher类
 * 完成URL解析、路由和调度
 */
class Dispatcher {
    
    private static $_loadModule = array();
    
    /*加载指定模块的配置文件*/
    public static function loadModule($module) {
        if (empty($module) || in_array_case($module, self::$_loadModule)) {
            return;
        }
        
        /* Wulink下面沒有wulink的文件夾，所以需要去掉$module的拼接检查
         * 需要导入后面的xml配置 */
        $module = ucfirst($module);
        if (!is_dir(APP_PATH)) {
            return;
        }
        
        $module_path= APP_PATH.'/';
        
        // 加载模块配置文件
        if(is_file($module_path.'Conf/config'.CONF_EXT)) {
            C(load_config($module_path.'Conf/config'.CONF_EXT));
        }
        // 加载应用模式对应的配置文件
        if('common' != APP_MODE && is_file($module_path.'Conf/config_'.APP_MODE.CONF_EXT)) {
            C(load_config($module_path.'Conf/config_'.APP_MODE.CONF_EXT));
        }
            // 当前应用状态对应的配置文件
        if (APP_STATUS && is_file($module_path.'Conf/'.APP_STATUS.CONF_EXT)) {
            C(load_config($module_path.'Conf/'.APP_STATUS.CONF_EXT));
        }
        
        // 加载模块别名定义
        if(is_file($module_path.'Conf/alias.php')) {
            Think::addMap(include $module_path.'Conf/alias.php');
        }
                    // 加载模块tags文件定义
        if (is_file($module_path.'Conf/tags.php')) {
            Hook::import(include $module_path.'Conf/tags.php');
        }
        // 加载模块函数文件
        if(is_file($module_path.'Common/function.php'))
            include $module_path.'Common/function.php';


        
        //加载XML定义配置文件
        if(file_exists(XML_CONFIG)){
            C(load_config(XML_CONFIG));
        }
        // 加载模块的扩展配置文件
        load_ext_file($module_path);
        self::$_loadModule[] = $module;
    }

    /**
     * URL映射到控制器
     * @access public
     * @return void
     */
    static public function dispatch($path_info,$dispatch) {
        /*默认加载Api配置*/
        self::loadModule($dispatch['module']);
        
        /*正则去掉头部php，如index.php*/
        if (strpos($path_info, '/index.php') === 0) {
            $path_info = preg_replace("/^\/index\.php/", '',$path_info);
        }
        
        $controller = '';
        $action = '';
        $module = '';
        $depr = C('URL_PATHINFO_DEPR');
        $path_info_url = trim($path_info ,'/');
        $path_info_ext = strtolower(pathinfo($path_info, PATHINFO_EXTENSION));
        
        if ($path_info_url && C('MULTI_MODULE')){ // 获取模块名
            $paths  =  explode($depr,$path_info_url,2);
            $path_info_url = isset($paths[1]) ? $paths[1]: '';
            $module = ucfirst(preg_replace('/\.' . $path_info_ext . '$/i', '',$paths[0])); 
        } else {
            return $dispatch;
        }

        $allowList =   C('MODULE_ALLOW_LIST'); // 允许的模块列表
        $denyList  =   C('MODULE_DENY_LIST'); // 允许的模块列表
        /*检查模块是否存在以及访问权限*/
        if (empty($module) || !is_dir(APP_PATH.$module)) {
            return $dispatch;
        }
        if(is_array($allowList) && !in_array_case($module, $allowList)){
            return $dispatch;
        }
        if(is_array($denyList) && in_array_case($module, $denyList)){
            return $dispatch;
        }
        
        self::loadModule($module);
        $path_info_url = preg_replace('/\.'.$path_info_ext.'$/i','', $path_info_url);
        $paths  =   explode($depr,trim($path_info_url,$depr));
        if (empty($paths)) {
            return $dispatch;
        }
        
        /*安全检查*/
        $action     = array_pop($paths);
        $controller = implode('/',$paths);
        if (empty($controller)){
            $controller = 'index';
        }
        if (empty($controller) || !preg_match('/^[A-Za-z](\/|\w|-)*$/', $controller)) {
            return $dispatch;
        }
        
        if (empty($action) || ($action != '_empty' && !preg_match('/^[A-Za-z](\/|\w)*$/', $action))) {
            return $dispatch;
        }
        $dispatch['module'] = $module;
        $dispatch['action'] = $action;
        $dispatch['controller'] = $controller;
     
        return $dispatch;
    }
}
