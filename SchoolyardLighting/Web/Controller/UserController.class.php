<?php
namespace Web\Controller;
use Web\Controller\HtmlController;
use Web\Lib\Session;
use Api\Lib\AsyncTask;
use Web\Lib\Error;
class UserController extends HtmlController{
    
    
    /*
     * 得到当前Server的活动TCP连接数，启动时间，accpet/close的总次数等信息
     *
    "start_time": 1509076009,
    "connection_num": 1,
    "accept_count": 16,
    "close_count": 15,
    "tasking_num": 0,
    "request_count": 15,
    "worker_request_count": 15
     */
    public function statd_get(){
        $serverStats = AsyncTask::serverStats();
        $returnMsg = $serverStats;
        $this->jsonReturPos($returnMsg);
        return;
    }
    
    /*
     * service通知消息推送测试地址
     */
    public function wulinks_post(){
        DBG_bak(json_encode($this->_request));
        $post = &$this->_request->post;
        $get = &$this->_request->get;
        DBG_bak(json_encode($post));
        DBG_bak(json_encode($get));
        $returnMsg["post"] = $post;
        $returnMsg["get"] = $get;
        $this->jsonReturPos($returnMsg);
        return;
    }
    
    /*
     * 用户登陆 /web/user/login----POST
     * @param   username    用户名
     * @param   password    密码的md5
     */
    public function login_post(){
        $param = $this->_request->post;
        //$param = $this->_request->get;
        $_cookie = $this->_request->cookie;
        if(!isset($param["username"]) || !isset($param["password"])){
            return $this -> errorReturnPos("参数错误");
        }
        if(!isset($_cookie[C("PHP_SESSION_NAME")]) || !$_cookie[C("PHP_SESSION_NAME")]){
            return $this -> errorReturnPos("请求需携带cookie");
        }
        $_session = new Session();
        AsyncTask::addTask(array('username' => $param["username"], 'password' => $param["password"]),
            array('Web\Lib\Task\AcountTask','get_user_info_bylogin'),
            function($dataBack) use ($_cookie, $param, $_session) {
                if(!is_array($dataBack)){
                    return $this -> errorReturnPos("用户名或密码错误");
                }
                $lifetime = C("LOGIN_ALIVE_TIME")?:1800;
                $sessionData = array("user_id" => intval($dataBack["id"]), "login_time" => time());
                $memBack = $_session ->create($_cookie[C("PHP_SESSION_NAME")], json_encode($sessionData), $lifetime);
                return $this->jsonReturPos($dataBack);
        });
    }
    
    
    
}

?>
