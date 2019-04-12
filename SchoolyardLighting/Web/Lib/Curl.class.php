<?php

namespace Web\Lib;
class Curl{  
    public static function callback($url, $type,$params, $header_auth){  
        $SSL = substr($url, 0, 8) == "https://" ? true : false;  
        $ch = curl_init(); 
        if ($SSL) {  
            curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false); // 信任任何证书  
            curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false); // 检查证书中是否设置域名  
        }
        if(is_array($params)){
            $params = json_encode($params);
        }
        $timeout = 5;  
        curl_setopt ($ch, CURLOPT_URL, $url); 
        curl_setopt ($ch, CURLOPT_RETURNTRANSFER, 1);  
        curl_setopt ($ch, CURLOPT_CONNECTTIMEOUT, $timeout);  
        curl_setopt ($ch, CURLOPT_HTTPHEADER, array(
            'Content-Type: application/json;charset=utf-8',
            'Content-Length: ' . strlen($params),
            'Authorization: Bearer ' . $header_auth
            ));
        switch (strtoupper($type)){  
            case "GET" : 
                    curl_setopt($ch, CURLOPT_HTTPGET, true);
                    curl_setopt ($ch, CURLOPT_URL, $url); 
                    break;
            case "POST": 
                    curl_setopt($ch, CURLOPT_POST,true);   
                    curl_setopt($ch, CURLOPT_POSTFIELDS,$params);break;  
            case "PUT" : 
                    curl_setopt ($ch, CURLOPT_CUSTOMREQUEST, "PUT");   
                    curl_setopt($ch, CURLOPT_POSTFIELDS,$params);break;  
            case "DELETE":
                    curl_setopt ($ch, CURLOPT_CUSTOMREQUEST, "DELETE");   
                    curl_setopt($ch, CURLOPT_POSTFIELDS,$params);break;  
        }
        $result = curl_exec($ch);//获得返回值  
        curl_close($ch); 	
        //return urldecode($result);
        DBG_bak("请求地址为：" . $url . ",请求参数为：" . $params . "，接口返回结果：".$result);
        return $result;
    }
    
     public static function get_url($url,$params) {
            $count = 0;
            foreach ($params as $key => $value) {
                    if ($count == 0) {
                            $url .= "?";
                    }
                    $url .= $key."=".$value;
                    if ($count == (count($params) - 1)) {
                            return $url;
                    }
                    $count++;
                    $url .= "&";
            }

            return $url;
    }
    
    
     public static function caculate_sign($appid,$token,$nonce,$time,$post = array()) {
        $sign = $appid.$token.$nonce.$time;
        if (!empty($post)) {
            $sign .= json_encode($post);
        }
        return md5($sign);
    }
    
    
}

?>
