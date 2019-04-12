<?php 

namespace   Service\Lib\Task;

use Api\Lib\AsyncTask;

use Api\Model\UserinfoTblModel;


class ServiceTask extends AsyncTask {
    public static  $_GRADE_CONVERSION_WORD = array("一" => "1","二" => "2","三" => "3","四" => "4","五" => "5","六" => "6",
        "七" => "7","八" => "8","九" => "9");
    public static  $_DEVICE_EXTYPE = array("125" => "sensor", "239" => "light_desktop", "241" => "light_blackboard");
    
    /*
     * 更新每分钟数据
     */
    public static function update_miniute_satistics($data) {
        DBG_bak("ServiceTask---update_miniute_satistics start");
        DBG_bak("ServiceTask---update_miniute_satistics end");
        return 1;
    }
    
    
    /*
     *更新每天数据
     */
    public static function update_day_statistics($data) {
        DBG_bak("ServiceTask---update_day_statistics start");
        DBG_bak("ServiceTask---update_day_statistics end");
        return 2;
    }
    
    
}