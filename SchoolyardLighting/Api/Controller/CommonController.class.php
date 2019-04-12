<?php 
namespace Api\Controller;

use Api\Lib\Error;
use Think\App;
import("Api.Lib.Error");

class CommonController {
    private  $_hasSend = false;

    protected $_request  = NULL;
    protected $_response = NULL;

    protected $_startTime = NULL;
    
    protected $_method = NULL;
    
    public function init($request, $response) {
        $this->_request  = $request;
        $this->_response = $response;
        
        if(C('REQUEST_VARS_FILTER')){
            // 全局安全过滤
            array_walk_recursive($this->_request->get,	 'think_filter');
            array_walk_recursive($this->_request->post,	 'think_filter');
        }
        $jsonType = array('application/json','text/x-json','application/jsonrequest','text/json');
        foreach($jsonType as $key=>$val){
            if(stristr($this->_request->header['content-type'], $val)) {
                $this->_request->post = json_decode($this->_request->rawContent(),true);
                break;
            }
        }
        $this->_method = strtolower($this->_request->server['request_method']);
    }

    public function __construct() {
        //parent::__construct();
    }
    
    protected function verify() {
        return true;
    }

    public function __call($method,$args) {
        if ($this->_hasSend === true) {
            return;
        }
        try {
            $this->_startTime = microtime(true);
            if(!$this->verify()) {/*验证openid 和 设备信息*/
                //$this->errorReturn(E_INVALID_PARAMETER);
                return;//继承类自动验证，错误返回
            } else if(method_exists($this,$method.'_'.$this->_method)) { // RESTFul方法支持
            	$fun  =  $method.'_'.$this->_method;
            	$this->invokeAction($fun);
            } else {
                $this->errorReturn(E_INVALID_METHOD);
            }
        } catch (\Exception $e) {
            DBG_bak('[Controller] __call exception:'.$e->getMessage());
            $this->errorReturn(E_SYSTEM_UNKNOWN);
        }
    }
    
    protected function invokeAction($fun) {
        try {
            App::invokeAction($this,$fun);
        }catch (\Exception $e) {
            DBG_bak('[Controller] __call exception:'.$e->getMessage());
            $this->errorReturn(E_SYSTEM_UNKNOWN);
        }
    }
	
    public function jsonReturn($data, $jsonp = false) {
        $success = Error::getError(E_SUCCESS);
        $success['data'] = $data;

        $this->response($success,$jsonp);
    }
	
    public function errorReturn($code, $jsonp = false) {
        $error = Error::getError($code);

        $this->response($error,$jsonp);

    }
    
    /**
     * 输出返回数据
     * @access protected
     * @param mixed $data 要返回的数据
     * @param String $type 返回类型 JSON XML
     * @param integer $code HTTP状态
     * @return void
     */
    protected function response($data,$jsonp = false,$type='json',$code=200) {
        if ($this->_hasSend === true) {
            return;
        }

        /*取出缓冲区buffer保存为日志*/
        $this->sendHttpStatus($code);
        if (is_string($jsonp)) {
            $real_data  = $jsonp.'(';
            $real_data .= $this->encodeData($data,strtolower($type));
            $real_data .= ')';
        } else {
            $real_data = $this->encodeData($data,strtolower($type));
        }
        
        if (APP_DEBUG === true || $data['errcode'] !== E_SUCCESS) {
            $this->debugTrace($real_data);
        }
        
        if (!empty($this->_response)) {
            $this->_response->end($real_data);
        } else {
            exit($real_data);
        }
        /*发送完毕后，清理环境*/
        $this->_hasSend  = true;
        $this->_response = NULL;
        $this->_request  = NULL;
    }
    
    public function _empty($method, $args) {
        $this->errorReturn(E_INVALID_METHOD);
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
        
        DBG_bak($description, $level);
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
                $this->_response->header('Content-Type','application/json');
                //$this->_response->header('Content-Type','application/x-www-form-urlencoded');
                //$this->_response->header('Content-Type','text/javascript; charset=utf-8');
                //跨域设置
                $uri = $this->_request->server['request_uri'];
                if(is_numeric(stripos($uri, "/cmcc/"))){
                    $this->_response->header('Access-Control-Allow-Origin','*');
                    $this->_response->header('Access-Control-Allow-Methods','PUT,POST,GET,DELETE');
                    $this->_response->header('Access-Control-Max-Age','1000');
                }
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
            $data = self::safeJsonEncode($data);
        }elseif('xml' == $type){
            // 返回xml格式数据
            $data = xml_encode($data);
        }elseif('php'==$type){
            $data = serialize($data);
        }// 默认直接输出
        
        return $data;
    }
    public static function safeJsonEncode($data) {
        $jsonData = json_encode($data, JSON_UNESCAPED_UNICODE);
        if ($jsonData == false) {
            $data = self::utf8Json($data);
            $jsonData = json_encode($data, JSON_UNESCAPED_UNICODE);
        }
        return $jsonData;
    }
    public static function utf8Json($array) {
        if (!is_array($array) && !is_object($array)) {
            return $array;
        }
        static $utf8 = 'UTF-8';
        static $jsonSupport   = array('ASCII','UTF-8');
        static $systemSupport = array( 
             'UTF-8', 'ASCII', 'GB18030',
             'ISO-8859-1', 'ISO-8859-2', 'ISO-8859-3', 'ISO-8859-4', 'ISO-8859-5', 
             'ISO-8859-6', 'ISO-8859-7', 'ISO-8859-8', 'ISO-8859-9', 'ISO-8859-10', 
             'ISO-8859-13', 'ISO-8859-14', 'ISO-8859-15',
             'UCS-4','UCS-4BE','UCS-4LE','UCS-2','UCS-2BE','UCS-2LE',
             'Windows-1251', 'Windows-1252', 'Windows-1254'
         );
        $convertArray = array();
        foreach ($array as $key => $value) {
            $keyEncode = mb_detect_encoding($key,$systemSupport);
            if (!in_array($keyEncode,$jsonSupport)) {
                if ($keyEncode) {
                    $key = mb_convert_encoding($key,$utf8,$keyEncode);
                } else {
                    $key = mb_convert_encoding($key,$utf8);
                }
            }
            if (is_array($value) || is_object($value)) {
                $value = self::utf8Json($value);
            } else {
                $valueEncode = mb_detect_encoding($value,$systemSupport);
                if (!in_array($valueEncode,$jsonSupport)) {
                    if ($valueEncode) {
                        $value = mb_convert_encoding($value,$utf8,$valueEncode);
                    } else {
                        $value = mb_convert_encoding($value,$utf8);
                    }
                }
            }
            $convertArray[$key] = $value;
        }
        return $convertArray;
    }
}