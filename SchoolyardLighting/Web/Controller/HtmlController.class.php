<?php

// html异步调用接口
namespace Web\Controller;
use Web\Controller\CommonController;
use Web\Lib\Session;
use Api\Lib\AsyncTask;
class HtmlController extends CommonController {
    
    
    //public $_session  = NULL;
    
    
    public function __construct() {
        //parent::__construct();
        //$this->_session = new Session;
    }
    
    
    public function __destruct() {
        /*
        DBG_bak(gettype($this->_session));
        if (isset($this->_session) && is_object($this->_session)) {
            $this->_session->close();
        }
        $this->_session = null;
         * 
         */
    }
    
    
    /*
     * 返回成功数据
     */
    public function jsonReturPos($data=array()) {
        $returnData['result'] = 'success';
        $returnData['data'] = $data ? $data : array();
        $this->response($returnData);
    }
    /*
     * 返回成功数据
     */
    public function successReturnPos() {
        $returnData['result'] = 'success';
        $this->response($returnData);
    }
    /*
     * 返回失败数据
     */
    public function errorReturnPos($errmsg){
        $returnData['result'] = 'fail';
        $returnData['errmsg'] = $errmsg ? $errmsg : 'error!';
        $this->response($returnData);
    }
    /*
     * 返回失败数据
     */
    public function errorReturnCode($code){
        $returnData['result'] = 'fail';
        $returnData['code'] = $code;
        $this->response($returnData);
    }
    /*
     * 返回未登录
     */
    public function noauthReturn(){
        $returnData['result'] = 'noauth';
        $this->response($returnData);
    }
    
    /*
     * 检查账号是否登录
     */
    public static function checkIsLogin($session_name){
        $lifetime = C("LOGIN_ALIVE_TIME")?:1800;
        $_session = new Session;
        $login_info = json_decode($_session->read($session_name), true);
        DBG_bak("session user_id=".$login_info['user_id'].",login_time=".$login_info['login_time'].",LOGIN_ALIVE_TIME=" .C("LOGIN_ALIVE_TIME"));
        if(!isset($login_info['user_id']) || $login_info['login_time'] < time() - C("LOGIN_ALIVE_TIME")){
            unset($_session);
            return "noauth";
        }else{
            $session_data = array("user_id" => intval($login_info['user_id']), "login_time" => time());
            $_session ->create($session_name, json_encode($session_data), $lifetime);
            unset($_session);
            return $login_info;
        }
    }
    
    
    public static function checkSession($_cookie){
        if(!isset($_cookie[C("PHP_SESSION_NAME")]) || !$_cookie[C("PHP_SESSION_NAME")]){
            return false;
        }
        $check_info = self::checkIsLogin($_cookie[C("PHP_SESSION_NAME")]);
        if($check_info == "noauth"){
            return false;
        }
        return $check_info;
    }

}
