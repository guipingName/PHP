<?php 
namespace Web\Controller;

use Web\Lib\Error;
use Think\App;
use Web\Lib\Session;
import("Web.Lib.Error");

class CommonController {
    //const SESSION_NAME = 'school_session_name';

    private  $_hasSend = false;

    protected $_request  = NULL;
    protected $_response = NULL;
    
    protected $_session   = array();
    protected $_session_update = array();
    private   $_sessionId = '';

    protected $_startTime = NULL;
    /*由于有方法restful action 映射关系，因此不再使用udp cotrol协议映射*/
    /*protected static $ACTION = array(
        "none"     => UCA_NONE,     "request"  => UCA_REQUEST,       
        "reply"    => UCA_REPLY,    "question" => UCA_QUESTION, 
        "answer"   => UCA_ANSWER,   "result"   => UCA_RESULT,
        "redirect" => UCA_REDIRECT, "reset"    => UCA_RESET,
        "get"      => UCA_GET,      "set"      => UCA_SET,
        "delete"   => UCA_DELETE,   "push"     => UCA_PUSH
    );*/
	
    protected static $METHOD_ACTION = array(  'get'    =>"get" ,
                                                'post'   => "post", 
                                                'put'    => "put",
                                                'delete' => "delete");
    public function init($request, $response) {
        $this->_request  = $request;
        $this->_response = $response;
        if(C('REQUEST_VARS_FILTER')){
            // 全局安全过滤
            array_walk_recursive($this->_request->get,	 'think_filter');
            array_walk_recursive($this->_request->post,	 'think_filter');
        }
        $this->_method = strtolower($this->_request->server['request_method']);
    }

	
    public function __construct() {
        //parent::__construct();
    }
    
    protected function verify() {
        return true;
    }
	
    protected function session_start() {
        try{
            if (isset($this->_request->cookie[C("PHP_SESSION_NAME")])) {
                $this->_sessionId = $this->_request->cookie[C("PHP_SESSION_NAME")];
            }
            $session = new Session();
            //验证是否为同一个客户端
            if (empty($this->_sessionId)) {
                /*设置随机码*/
                $this->_sessionId = createRandom(26);
                if (!$session->create($this->_sessionId, '')) {
                    return false;
                }
                /*设置cookie*/
                $this->_response->cookie(C("PHP_SESSION_NAME"),$this->_sessionId,0,'/');
                /*if(empty($this->_request->cookie[C("PHP_SESSION_NAME")])){
                    return false;
                }*/
            } else {
                $value = $session->read($this->_sessionId);
                if($value === false){
                    if (!$session->create($this->_sessionId, '')) {
                        return false;
                    }
                }else{
                    $this->_session = empty($value) ? '' : json_decode($value,true);
                }
            }
            return true;
        }catch (\Exception $e){
            DBG_bak("[Session] start {$e->getMessage()}");
            return false;
        }
    }

    protected function session_save() {
        try {
            if (empty($this->_sessionId)) {
                return;
            }
            $session = new Session();
            if (empty($this->_session) && empty($this->_session_update)) {
                $session->destroy($this->_sessionId);
                return;
            } else {
                $session->write($this->_sessionId, $this->_session_update);
            }
        }catch (\Exception $e){
            DBG_bak("[session] save {$e->getMessage()}");
        }
    }

    public function __call($method,$args) {
        if ($this->_hasSend === true) {
            return;
        }
        try {
            $this->_startTime = microtime(true);
            /*
            if (!$this->session_start()) {
                DBG_bak("session start fail------------");
                $this->errorReturn(E_INVALID_PARAMETER);
            }
             */
            $fun  =  $method.'_'.$this->_method;
            if(!$this->verify()) {/*验证openid 和 设备信息*/
                //$this->errorReturn(E_INVALID_PARAMETER);
                return;//继承类自动验证，错误返回
            } else if(method_exists($this,$fun)) { // RESTFul方法支持
            	try {
            	   App::invokeAction($this,$fun);
                }catch (\Exception $e) {
                    DBG_bak('[Controller] __call exception:'.$e->getMessage());
                    $this->errorReturn(E_SYSTEM_UNKNOWN);
                }
            } else {
                $this->errorReturn(E_INVALID_METHOD);
            }
        } catch (\Exception $e) {
            DBG_bak('[Controller] __call exception:'.$e->getMessage());
            $this->errorReturn(E_SYSTEM_UNKNOWN);
        }
    }
	
    public function jsonReturn($data) {
        $success = Error::getError(E_SUCCESS);
        $success['data'] = $data;

        $this->response($success);
    }
	
    public function errorReturn($code, $extra = false) {
        $error = Error::getError($code);

        $this->response($error);

    }
    
    /**
     * 输出返回数据
     * @access protected
     * @param mixed $data 要返回的数据
     * @param String $type 返回类型 JSON XML
     * @param integer $code HTTP状态
     * @return void
     */
    protected function response($data,$type='json',$code=200) {
        if ($this->_hasSend === true) {
            return;
        }
        
        /*取出缓冲区buffer保存为日志*/
        $this->sendHttpStatus($code);
        if (C('USE_DATA_BACK')) {
            $real_data  = 'dataBack(';
            $real_data .= $this->encodeData($data,strtolower($type));
            $real_data .= ')';
        } else {
            $real_data = $this->encodeData($data,strtolower($type));
        }
        
        if (APP_DEBUG === true ||
                (is_numeric($data['code']) && $data['code'] !== E_SUCCESS)) {
            $this->debugTrace($real_data);
        }
        
        if (!empty($this->_response)) {
            $this->_response->end($real_data);
        } else {
            exit($real_data);
        }
        /*发送完毕后，清理环境*/
        $this->session_save();
        $this->_hasSend  = true;
        $this->_response = NULL;
        $this->_request  = NULL;
    }
    
    public function _empty($method, $args) {
        $this->errorReturn(E_INVALID_PARAMETER);
    }
    
    protected function debugTrace($trace, $level = 'LOG') {
        $nextLine = "\r\n";
        
        $description  = 'ExcuteTime:' . (microtime(true) - $this->_startTime).'s';
        $description .= $nextLine;
        $description .= 'Request Uri:' . $this->_request->server['request_uri']; 
        $description .= $nextLine;
        $description .= 'Route:' .json_encode($this->_request->server['disptach']);
        $description .= $nextLine;
        $description .= 'Get params: '.json_encode($this->_request->get);
        $description .= $nextLine;
        $description .= 'Post params: '.json_encode($this->_request->post);
        $description .= $nextLine;
        $description .= 'Description: '.$trace;
        
        DBG_bak('[' . __FILE__ . ':' . __LINE__ . ']' . $description, $level);
    }
    
    // 发送Http状态信息
    protected function sendHttpStatus($code) {
        static $_status = array(
                // Informational 1xx
                100 => 'Continue',
                101 => 'Switching Protocols',
                // Success 2xx
                200 => 'OK',
                201 => 'Created',
                202 => 'Accepted',
                203 => 'Non-Authoritative Information',
                204 => 'No Content',
                205 => 'Reset Content',
                206 => 'Partial Content',
                // Redirection 3xx
                300 => 'Multiple Choices',
                301 => 'Moved Permanently',
                302 => 'Moved Temporarily ',  // 1.1
                303 => 'See Other',
                304 => 'Not Modified',
                305 => 'Use Proxy',
                // 306 is deprecated but reserved
                307 => 'Temporary Redirect',
                // Client Error 4xx
                400 => 'Bad Request',
                401 => 'Unauthorized',
                402 => 'Payment Required',
                403 => 'Forbidden',
                404 => 'Not Found',
                405 => 'Method Not Allowed',
                406 => 'Not Acceptable',
                407 => 'Proxy Authentication Required',
                408 => 'Request Timeout',
                409 => 'Conflict',
                410 => 'Gone',
                411 => 'Length Required',
                412 => 'Precondition Failed',
                413 => 'Request Entity Too Large',
                414 => 'Request-URI Too Long',
                415 => 'Unsupported Media Type',
                416 => 'Requested Range Not Satisfiable',
                417 => 'Expectation Failed',
                // Server Error 5xx
                500 => 'Internal Server Error',
                501 => 'Not Implemented',
                502 => 'Bad Gateway',
                503 => 'Service Unavailable',
                504 => 'Gateway Timeout',
                505 => 'HTTP Version Not Supported',
                509 => 'Bandwidth Limit Exceeded'
        );
        if(isset($_status[$code])) {
            if (!empty($this->_response)) {
                $this->_response->status($code);
                //$this->_response->header('Content-Type','application/json');
                $this->_response->header('Content-Type','text/html; charset=UTF-8');
                $_cookie = $this->_request->cookie;
                if(!isset($_cookie[C("PHP_SESSION_NAME")]) || !$_cookie[C("PHP_SESSION_NAME")]){
                    $this->_response->header('Set-Cookie',C("PHP_SESSION_NAME").'='.md5(com_uuid_create()) . ";path=/");
                }
                $this->_response->header('Pragma','no-cache');
                //$this->_response->header('Cache-Control','max-age=2592000');
                $this->_response->header('Cache-Control','no-store');
                $this->_response->header('Access-Control-Allow-Origin','*');
                $this->_response->header('Access-Control-Allow-Methods','PUT,POST,GET,DELETE');
                $this->_response->header('Access-Control-Max-Age','1000');
            } else {
                header('HTTP/1.1 '.$code.' '.$_status[$code]);
                // 确保FastCGI模式下正常
                header('Status:'.$code.' '.$_status[$code]);
            }
        }
    }
    /**
     * 编码数据
     * @access protected
     * @param mixed $data 要返回的数据
     * @param String $type 返回类型 JSON XML
     * @return string
     */
    protected function encodeData($data,$type='') {
        if(empty($data))  return '';
        if('json' == $type) {
            // 返回JSON数据格式到客户端 包含状态信息
            //$data = json_encode($data);
            /*！！！解决json_encode解码中文乱码问题,PHP5.4后支持*/
            $data = json_encode($data, JSON_UNESCAPED_UNICODE);
        }elseif('xml' == $type){
            // 返回xml格式数据
            $data = xml_encode($data);
        }elseif('php'==$type){
            $data = serialize($data);
        }// 默认直接输出
        
        return $data;
    }
}