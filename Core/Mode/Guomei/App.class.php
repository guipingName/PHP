<?php
/*******************************************************************************
* [Galaxywind] 银河风云
* @Copyright (C) 2014-2015 All rights reserved.
* @Team  应用云项目组
* @Author: wzh
*******************************************************************************/

namespace Think;
/**
 * ThinkPHP API模式 应用程序类
 */
class App {

    /**
     * 应用程序初始化
     * @access public
     * @return void
     */
    static public function init() {
        // 定义当前请求的系统常量
        load_ext_file(COMMON_PATH);
        // 日志目录转换为绝对路径
        C('LOG_PATH',realpath(LOG_PATH).'/'.ucfirst(APP_MODE).'/');
        // TMPL_EXCEPTION_FILE 改为绝对地址
        C('TMPL_EXCEPTION_FILE',realpath(C('TMPL_EXCEPTION_FILE')));
        return ;
    }
    /**
     * 执行应用程序
     * @access public
     * @return void
     */
    static public function exec($request, $response) {
        /*确保此必须存在*/
        static $default_disptach =  array('module' => 'Guomei', 
                    'controller' => 'Empty', 'action' => '_empty');
        $path_info = $request->server['path_info'];
        //$path_info = $request;
        $dispatch = Dispatcher::dispatch($path_info);

        $request->server['disptach'] = $dispatch;
        /*Dispatch 保证参数*/
        $module = A($dispatch['module']."/".$dispatch['controller']);
        if (!$module) {
            $module = A($default_disptach['module']."/".$default_disptach['controller']);
            $request->server['disptach'] = $default_disptach;
        }

        $module->init($request, $response);
        // 获取当前操作名 支持动态路由
        $action     =   $dispatch['action'];

        try{
            // 方法调用发生异常后 引导到__call方法处理
            $method = new \ReflectionMethod($module,'__call');
            $method->invokeArgs($module,array($action,''));
        } catch (\Exception $e) { 
            // 方法调用发生异常后 引导到__call方法处理
            $method = new \ReflectionMethod($module,'__call');
            $method->invokeArgs($module,array($action,''));
        }
        return ;
    }
    
    public static function invokeAction($module,$action){
        //执行当前操作
        $method =   new \ReflectionMethod($module, $action);
        if($method->isPublic() && !$method->isStatic()) {
            $class  =   new \ReflectionClass($module);
            // 前置操作
            if($class->hasMethod('_before_'.$action)) {
                $before =   $class->getMethod('_before_'.$action);
                if($before->isPublic()) {
                    $before->invoke($module);
                }
            }
            $method->invoke($module);
            // 后置操作
            if($class->hasMethod('_after_'.$action)) {
                $after =   $class->getMethod('_after_'.$action);
                if($after->isPublic()) {
                    $after->invoke($module);
                }
            }
        }else{
            // 操作方法不是Public 抛出异常
            throw new \ReflectionException();
        }
    }
    /**
     * 运行应用实例 入口文件使用的快捷方法
     * @access public
     * @return void
     */
    static public function run() {
        App::init();
        Dispatcher::loadModule(APP_MODE);
    }

}