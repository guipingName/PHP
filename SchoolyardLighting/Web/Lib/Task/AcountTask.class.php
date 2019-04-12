<?php 

namespace Web\Lib\Task;

use Api\Lib\AsyncTask;
use Api\Model\UserinfoTblModel;


class AcountTask extends AsyncTask {
    public static  $_GRADE_CONVERSION_NUMBER = array("1" => "一年级","2" => "二年级","3" => "三年级",
        "4" => "四年级","5" => "五年级","6" => "六年级", "7" => "七年级","8" => "八年级","9" => "九年级");
    /*
     * 获取用户信息
     */
    public static function get_user_info_bylogin($data) {
        $model = new UserinfoTblModel();
        $result = $model -> where($data) -> field("id,username,usertype") -> find();
        DBG_bak($model -> getLastSql());
        if(!$result){
            return "用户名或密码错误";
        }
        $update = array("lastlogin_time" => date("Y-m-d H:i:s"));
        $model -> where($data) ->save($update);
        return $result;
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
}