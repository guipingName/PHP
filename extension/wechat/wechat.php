<?php
error_reporting(E_ALL );
//wechat_udp_set_global(UC_GLOBAL_DEBUG);


include './UdpControl.php';

class WukongController {
	private $_udpCtrl = NULL;
	public $_devSn = NULL;
	private $_action = array(
		"none"     => UCA_NONE, "request"  => UCA_REQUEST,       
		"reply"    => UCA_REPLY,"question" => UCA_QUESTION, 
		"answer" => UCA_ANSWER,"result" => UCA_RESULT,
		"redirect" => UCA_REDIRECT,"reset" => UCA_RESET,
		"get" => UCA_GET,"set" => UCA_SET,
		"delete" => UCA_DELETE,"push" => UCA_PUSH
	);
	
	public function getAction() {
		$action = $_POST['action'];
		if (isset($this->_action[$action])) {
			return $this->_action[$action];
		}
		return $this->_action["none"];
	}
	
	public function __construct() {
		$this->_udpCtrl = new UdpControl;
	}
	
        /***********************************空调begin>>******************************************************************/
	public function setReboot() {
		$devSn = $this->_devSn;
		$reboot = UdpControl::generateControl(UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_REBOOT);
		$unpackData = $this->_udpCtrl->excuteControl($devSn, $reboot, UCA_SET);
		var_dump($unpackData);
		$retData['sn'] = $devSn;		
	}
	
	public function getTempCurve() {
		$devSn = $this->_devSn;
		$tmpCurve = UdpControl::generateControl(UCOT_IA, UCSOT_IA_PUBLIC, 
				UCAT_IA_PUBLIC_TMP_CURVE);
				
		$unpackData = $this->_udpCtrl->excuteControl($devSn, $tmpCurve, UCA_GET);
		echo json_encode($unpackData);
	}
	
	public function setUserSet() {
		$devSn = $this->_devSn;
		$tmpCurve = UdpControl::generateControl(UCOT_IA, UCSOT_IA_AC, 
				UCAT_AC_ON_USER_SET);

		$tmpCurve['type'] = 5;
		$tmpCurve['vl'] = 50;

		//$sendData = wechat_udp_control_pack($devSn, $tmpCurve, UCA_SET);
		//$unpackData = wechat_udp_control_unpack($sendData);	
		$unpackData = $this->_udpCtrl->excuteControl($devSn, $tmpCurve, UCA_SET);
		var_dump($unpackData);
		//$retData['sn'] = $devSn;
	}
 
	public function getUserSet() {
		$devSn = $this->_devSn;
		$tmpCurve = UdpControl::generateControl(UCOT_IA, UCSOT_IA_AC, 
				UCAT_AC_ON_USER_SET);
		$curTmp = UdpControl::generateControl(UCOT_IA, UCSOT_IA_AC, 
				UCAT_AC_CUR_TEMP);		
		$unpackData = $this->_udpCtrl->excuteControl($devSn, array($tmpCurve,$curTmp), UCA_GET);
		$retData['onoff_same'] = UdpControl::fetchControl($unpackData, $tmpCurve, 'onoff_same');
		$retData[cur_tmp] = UdpControl::fetchControl($unpackData, $curTmp, 'cur_tmp');
		var_dump($unpackData);
		
		echo "\r\n\r\n";
	}
	
	public function getTempSampleCurve() {
		$devSn = $this->_devSn;
		$tmpCurve = UdpControl::generateControl(UCOT_IA, UCSOT_IA_AC, 
				UCAT_AC_TMP_SAMPLE_CURVE);
				
		$unpackData = $this->_udpCtrl->excuteControl($devSn, $tmpCurve, UCA_GET);
		//echo json_encode($unpackData);
		$retData['sn'] = $devSn;
		$retData['count'] = UdpControl::fetchControl($unpackData, $tmpCurve, 'count', 0);
		$retData[UC_DATA] = UdpControl::fetchControl($unpackData, $tmpCurve, UC_DATA, array());
		echo json_encode($retData);
		
		echo "\r\n\r\n";
	}
	
	public function getDeviceInfo() {
		$devSn = $this->_devSn;
		$sysVer       = UdpControl::generateControl(UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_VERSION);
		$sysSvn       = UdpControl::generateControl(UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_SVN);			
		$sysSsid      = UdpControl::generateControl(UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_SSID);
		$sysIp        = UdpControl::generateControl(UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_IP);	
		$sysUpgrade   = UdpControl::generateControl(UCOT_SYSTEM, UCSOT_SYS_UPGRADE, UCOT_GET_ALL_ATTRI);	
		$sysDbginfo   = UdpControl::generateControl(UCOT_SYSTEM, UCSOT_SYS_SOFTWARE, UCAT_SYS_DEBUGINFO);
		$hardVer      = UdpControl::generateControl(UCOT_SYSTEM, UCSOT_SYS_HARDWARE, UCAT_SYS_VERSION);


		$cmd = array($sysVer, $sysSvn, $sysSsid, $sysIp,$sysUpgrade, 
			$sysDbginfo, $hardVer);
				
		$unpackData = $this->_udpCtrl->excuteControl($devSn, $cmd, UCA_GET);
		
		$retData['dev_sn'] = $devSn;
		$firewarm_version = UdpControl::fetchControl($unpackData, $sysVer, 'version');
		$firewarm_version .= '(svn:'.UdpControl::fetchControl($unpackData, $sysSvn, 'svn').')';
		$retData['firewarm_version'] = $firewarm_version;
		$retData['hardware_version'] = UdpControl::fetchControl($unpackData, $hardVer, 'version');
		$retData['upgrade_version'] = UdpControl::fetchControl($unpackData, $sysUpgrade, 'version');
		$retData['uptime'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'uptime');
		$retData['connect_time'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'connect_time');
		$retData['ssid'] = UdpControl::fetchControl($unpackData, $sysSsid, 'ssid');
		$retData['ip'] = UdpControl::fetchControl($unpackData, $sysIp, 'ip');

		//解析UCAT_SYS_DEBUGINFO
		$retData['clients'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'clients');
		
		$retData['current'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'current');
		$retData['voltage'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'voltage');
		$retData['light_ad'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'light_ad');
		
		$retData['current_ad'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'current_ad');
		$retData['current_k'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'current_k');
		$retData['current_b'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'current_b');
		$retData['voltage_ad'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'voltage_ad');
		$retData['voltage_k'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'voltage_k');
		$retData['voltage_b'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'voltage_b');
		
		$retData['average_ad'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'average_ad');
		$retData['max_ad'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'max_ad');
		$retData['noload_ad'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'noload_ad');
		$retData['smt_soft_verion'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'smt_soft_verion');
		$retData['smt_hard_verion'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'smt_hard_verion');
		$retData['irlib_id'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'irlib_id');
		$retData['light_yesterday'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'light_yesterday');
		$retData['light_today'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'light_today');
		
		$retData['cold_delay_pn_time'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'cold_delay_pn_time');
		$retData['cold_delay_pf_time'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'cold_delay_pf_time');
		$retData['hot_delay_pn_time'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'hot_delay_pn_time');
		$retData['hot_delay_pf_time'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'hot_delay_pf_time');
		$retData['stm_info'] = UdpControl::fetchControl($unpackData, $sysDbginfo, 'stm_info');
	
		echo json_encode($retData);
		echo "\r\n\r\n";
	}
	
	public function setTempCurve() {
		$devSn = $this->_devSn;
		$tmpCurve = UdpControl::generateControl(UCOT_IA, UCSOT_IA_PUBLIC, 
				UCAT_IA_PUBLIC_TMP_CURVE);
				
		$unpackData = $this->_udpCtrl->excuteControl($devSn, $tmpCurve, UCA_SET);
		
		$retData = UdpControl::fetchControl($unpackData, $tmpCurve, false);
		$settmp = array_merge($tmpCurve,$retData);
		$settmp['enable'] = 0;
		var_dump($settmp);
		$unpackData = $this->_udpCtrl->excuteControl($devSn, $tmpCurve, UCA_SET);
		//$sendData = wechat_udp_control_pack($devSn, $settmp, UCA_SET);
		//$unpackData = wechat_udp_control_unpack($sendData);	
		var_dump($unpackData);
		echo "\r\n\r\n";
	}

	public function getExtendedTimer() {
		$devSn = $this->_devSn;
		$timer = UdpControl::generateControl(UCOT_IA, UCSOT_IA_PUBLIC, 
				UCAT_IA_PUBLIC_EXTENDED_TIMER);
		if (1)	{
			$unpackData = $this->_udpCtrl->excuteControl($devSn, $timer, UCA_GET);
		} else {
			$sendData = wechat_udp_control_pack($devSn, $settmp, UCA_SET);
			$unpackData = wechat_udp_control_unpack($sendData);	
		}

		echo json_encode($unpackData);
		echo "\r\n\r\n";
	}

	public function deleteExtendedTimer() {
		$devSn = $this->_devSn;
		$timer = UdpControl::generateControl(UCOT_IA, UCSOT_IA_PUBLIC, 
				UCAT_IA_PUBLIC_EXTENDED_TIMER);
		$timer['id'] = 1;	
		$unpackData = $this->_udpCtrl->excuteControl($devSn, $timer, UCA_DELETE);
		
		var_dump($unpackData);
	}

	public function setExtendedTimer() {
		$devSn = $this->_devSn;
		$timer = UdpControl::generateControl(UCOT_IA, UCSOT_IA_PUBLIC, 
				UCAT_IA_PUBLIC_EXTENDED_TIMER);
		$timer['id'] = 0;
		$timer['hour'] = 12;
		$timer['minute'] = 20;
		$timer['week'] = array(1, 3, 5);
		$timer['enable'] = 1;
		$timer['onoff'] = 0;
		$timer['duration'] = 60;
		$timer['extended_data'] = array('onoff' => 0, 
		"mode" => 255, "temp" => 255, "wind" => 255, 
		"direct" => 255, "key" => 0);
		
		if (1) {
			$unpackData = $this->_udpCtrl->excuteControl($devSn, $timer, UCA_SET);
		} else {
			$sendData = wechat_udp_control_pack($devSn, $timer, UCA_SET);
			$unpackData = wechat_udp_control_unpack($sendData);	
		}
		var_dump($unpackData);
	}

	public function setMatchCode() {
		$devSn = $this->_devSn;
		$match = UdpControl::generateControl(UCOT_IA, UCSOT_IA_CODE,UCAT_CODE_PROCESS);
		if (1) {
			$unpackData = $this->_udpCtrl->excuteControl($devSn, $match, UCA_SET);
		} else {
			$sendData = wechat_udp_control_pack($devSn, $match, UCA_SET);
			$unpackData = wechat_udp_control_unpack($sendData);	
		}
		echo json_encode($unpackData);
		echo "\r\n\r\n";
	}
	
	public function pushMatchCode() {
		$devSn = $this->_devSn;
		$match = UdpControl::generateControl(UCOT_IA, UCSOT_IA_CODE,UCAT_CODE_PROCESS);
		if (1) {
			$unpackData = $this->_udpCtrl->excuteControl($devSn, $match, UCA_PUSH);
		} else {
			$sendData = wechat_udp_control_pack($devSn, $match, UCA_PUSH);
			$unpackData = wechat_udp_control_unpack($sendData);	
		}
		echo json_encode($unpackData);
		echo "\r\n\r\n";
	}
        /********************************************************遥控器begin>>*****************************************************************/
        
        
        /*
        * 获取遥控器信息
        */
        public function getControllInfo(){
            $devSn = $this->_devSn;
            $codeInfo = UdpControl::generateControl(UCOT_IA, UCSOT_IA_CODE, UCAT_CODE_RC_INFO);
            $unpackData = $this->_udpCtrl->excuteControl($devSn, $codeInfo, UCA_GET);
            var_dump($unpackData);
            
            return;
            
            
            if (!$this->checkReturn($unpackData)) {
                return;
            }
            $retData['count'] = UdpControl::fetchControl($unpackData, $codeInfo, 'count');
            $retData['length'] = UdpControl::fetchControl($unpackData, $codeInfo, 'length');
            $retData['data'] = UdpControl::fetchControl($unpackData, $codeInfo, 'data', array());
            $this->jsonReturn($retData);
        }
        
        /*
        * 修改遥控器名称
        */
        public function setControllinfo(){
            $devSn = $this->_devSn;
            $codeInfo = UdpControl::generateControl(UCOT_IA, UCSOT_IA_CODE, UCAT_CODE_RC_INFO);
            $codeInfo['type'] = 2;
            $codeInfo['devid'] = 2;
            $codeInfo['irid'] = 715;
            $codeInfo['flags'] = 1;
            $codeInfo['name'] = "jidinghe0224";
            $unpackData = $this->_udpCtrl->excuteControl($devSn, $codeInfo, UCA_SET);
            
            var_dump($unpackData);
            return;
                
                
            if (!$this->checkReturn($unpackData)) {
                return;
            }
            $this->errorReturn(E_SUCCESS);
        }
        
        
        /*
        * 云匹配----请求匹配
        */
        public function setMatch(){
            $devSn = $this->_devSn;
            $codeMatch = UdpControl::generateControl(UCOT_IA, UCSOT_IA_CODE, UCAT_CODE_RC_MATCH);
            $codeMatch['action'] = 1;
            $codeMatch['timeout'] = 120;
            $codeMatch['id'] = 0;
            $codeMatch['type'] = 2;
            $codeMatch['devid'] = 2;
            $unpackData = $this->_udpCtrl->excuteControl($devSn, $codeMatch, UCA_SET);
            
            
            var_dump($unpackData);
            return;
            
            if (!$this->checkReturn($unpackData)) {
                return;
            }
            $retData['action'] = UdpControl::fetchControl($unpackData, $codeMatch, 'action');
            $retData['timeout'] = UdpControl::fetchControl($unpackData, $codeMatch, 'timeout');
            $retData['id'] = UdpControl::fetchControl($unpackData, $codeMatch, 'id');
            $retData['type'] = UdpControl::fetchControl($unpackData, $codeMatch, 'type');
            $retData['devid'] = UdpControl::fetchControl($unpackData, $codeMatch, 'devid');
            $retData['skey'] = UdpControl::fetchControl($unpackData, $codeMatch, 'skey');
            $this->jsonReturn($retData);
        }

        /*
        * 云匹配----获取匹配状态
        */
        public function getMatchState(){
            $devSn = $this->_devSn;
            $codeMatch = UdpControl::generateControl(UCOT_IA, UCSOT_IA_CODE, UCAT_CODE_RC_MATCH_STATE);
            $unpackData = $this->_udpCtrl->excuteControl($devSn, $codeMatch, UCA_GET);
            
            var_dump($unpackData);
            return;
            
            if (!$this->checkReturn($unpackData)) {
                return;
            }
            $retData['action'] = UdpControl::fetchControl($unpackData, $codeMatch, 'action');
            $retData['step_type'] = UdpControl::fetchControl($unpackData, $codeMatch, 'step_type');
            $retData['step_count'] = UdpControl::fetchControl($unpackData, $codeMatch, 'step_count');
            $retData['step_index'] = UdpControl::fetchControl($unpackData, $codeMatch, 'step_index');
            $retData['err'] = UdpControl::fetchControl($unpackData, $codeMatch, 'err');
            $retData['flagbits'] = UdpControl::fetchControl($unpackData, $codeMatch, 'flagbits');
            $retData['skey'] = UdpControl::fetchControl($unpackData, $codeMatch, 'skey');
            $this->jsonReturn($retData);
        }
        

	
	public function getAttri() {
		$devSn = $this->_devSn;
		$match = UdpControl::generateControl(UCOT_IA, UCSOT_IA_AC,UCAT_AC_ATTRI);
		if (1) {
			$unpackData = $this->_udpCtrl->excuteControl($devSn, $match, UCA_GET);
		} else {
			$sendData = wechat_udp_control_pack($devSn, $match, UCA_GET);
			$unpackData = wechat_udp_control_unpack($sendData);	
		}
		echo json_encode($unpackData);
		echo "\r\n\r\n";
	}	
        
        /*
        * 遥控器按键查询
        */
        public function getKey(){
            $devSn = $this->_devSn;
            $codeKey = UdpControl::generateControl(UCOT_IA, UCSOT_IA_CODE, UCAT_CODE_RC_KEY);
            $unpackData = $this->_udpCtrl->excuteControl($devSn, $codeKey, UCA_GET);
            
            
            //var_dump($unpackData);
            //exit;
            /*
            if (!$this->checkReturn($unpackData)) {
                return;
            }
             * 
             */
            $retData['type'] = UdpControl::fetchControl($unpackData, $codeKey, 'type');
            $retData['flags'] = UdpControl::fetchControl($unpackData, $codeKey, 'flags');
            $retData['irid'] = UdpControl::fetchControl($unpackData, $codeKey, 'irid');
            $retData['devid'] = UdpControl::fetchControl($unpackData, $codeKey, 'devid');
            $retData['fixkey_len'] = UdpControl::fetchControl($unpackData, $codeKey, 'fixkey_len');
            $retData['fixkey_num'] = UdpControl::fetchControl($unpackData, $codeKey, 'fixkey_num');
            $retData['defkey_len'] = UdpControl::fetchControl($unpackData, $codeKey, 'defkey_len');
            $retData['defkey_num'] = UdpControl::fetchControl($unpackData, $codeKey, 'defkey_num');
            /*
            $returnData = UdpControl::fetchControl($unpackData, $codeKey, 'data', array());
            $retData['data'] = json_decode($returnData['data'], true);
            */
            $retData['data'] = UdpControl::fetchControl($unpackData, $codeKey, 'data', array());

            var_dump($retData);
            return;
        
            //$retData['data'] = UdpControl::fetchControl($unpackData, $codeKey, 'data', array());
            $this->jsonReturn($retData);
        }
    
        
        /*
        * 新增自定义遥控器按键
        */
        public function postKey(){
            /*
            if(isset($_POST['key']) && ($_POST['key'] < 128 || $_POST['key'] > 255)){
                return $this->errorReturn(E_INVALID_PARAMETER);
            }
             */
            $devSn = $this->_devSn;
            $codeKey = UdpControl::generateControl(UCOT_IA, UCSOT_IA_CODE, UCAT_CODE_RC_KEY);
            $codeKey['type'] = 2;
            $codeKey['key'] = 0;//0：新增  自定义按键id：修改
            $codeKey['irid'] = 715;
            $codeKey['devid'] = 2;
            $codeKey['flags'] = 0;
            $codeKey['name'] = '2';
            $unpackData = $this->_udpCtrl->excuteControl($devSn, $codeKey, UCA_SET);
            
            
            var_dump($unpackData);
            return;
            
            
            if (!$this->checkReturn($unpackData)) {
                return;
            }
            $this->errorReturn(E_SUCCESS);
        }

        /*
        * 修改自定义遥控器按键
        */
        public function setKey(){
            /*
            if(isset($_POST['key']) && ($_POST['key'] < 128 || $_POST['key'] > 255)){
                return $this->errorReturn(E_INVALID_PARAMETER);
            }
             */
            $devSn = $this->_devSn;
            $codeKey = UdpControl::generateControl(UCOT_IA, UCSOT_IA_CODE, UCAT_CODE_RC_KEY);
            $codeKey['type'] = 2;
            $codeKey['key'] = 129;//0：新增  自定义按键id：修改
            $codeKey['irid'] = 715;
            $codeKey['devid'] = 2;
            $codeKey['flags'] = 1;
            $codeKey['name'] = '上一页';
            $unpackData = $this->_udpCtrl->excuteControl($devSn, $codeKey, UCA_SET);
            
            
            var_dump($unpackData);
            return;
            
            
            if (!$this->checkReturn($unpackData)) {
                return;
            }
            $this->errorReturn(E_SUCCESS);

        }
        
        
        /*
        * 删除自定义遥控器按钮
        */
        public function deleteKey(){
            /*
            if(!isset($_POST['key']) || $_POST['key'] < 128 || $_POST['key'] > 255){
                return $this->errorReturn(E_INVALID_PARAMETER);
            }
             */
            $devSn = $this->_devSn;
            $codeKey = UdpControl::generateControl(UCOT_IA, UCSOT_IA_CODE, UCAT_CODE_RC_KEY);
            $codeKey['type'] = 2;
            $codeKey['key'] = 129;
            $codeKey['irid'] = $_POST['irid'];
            $codeKey['devid'] = $_POST['devid'];
            $codeKey['flags'] = $_POST['flags'];
            $codeKey['reserved'] = 0;
            $codeKey['name'] = $_POST['name'];
            $unpackData = $this->_udpCtrl->excuteControl($devSn, $codeKey, UCA_DELETE);
            
            
            var_dump($unpackData);
            return;
            
            
            if (!$this->checkReturn($unpackData)) {
                return;
            }
            $this->errorReturn(E_SUCCESS);
        }
        
        /*
        * 遥控器按键学习请求（UCA_SET）
        */
        public function putLearn(){
            /*
            if(!isset($_POST['key']) || $_POST['key'] > 255){
                return $this->errorReturn(E_INVALID_PARAMETER);
            }
             */
            $devSn = $this->_devSn;
            $codeLearn = UdpControl::generateControl(UCOT_IA, UCSOT_IA_CODE, UCAT_CODE_RC_LEARN);
            $codeLearn['type'] = 2;
            $codeLearn['key'] = 129;
            $codeLearn['devid'] = 2;
            $codeLearn['action'] = 0;
            $unpackData = $this->_udpCtrl->excuteControl($devSn, $codeLearn, UCA_SET);
            
            var_dump($unpackData);
            return;
            
            if (!$this->checkReturn($unpackData)) {
                return;
            }
            $this->errorReturn(E_SUCCESS);
        }
        
        /*
        * 取消遥控器按键学习请求
        */
        public function deleteLearn(){
            /*
            if(!isset($_POST['key']) || $_POST['key'] > 255){
                return $this->errorReturn(E_INVALID_PARAMETER);
            }
             */
            $devSn = $this->_devSn;
            $codeLearn = UdpControl::generateControl(UCOT_IA, UCSOT_IA_CODE, UCAT_CODE_RC_LEARN);
            $codeLearn['type'] = 2;
            $codeLearn['key'] = 128;
            $codeLearn['devid'] = 2;
            $codeLearn['action'] = 1;
            $unpackData = $this->_udpCtrl->excuteControl($devSn, $codeLearn, UCA_DELETE);
            
            var_dump($unpackData);
            return;
            
            if (!$this->checkReturn($unpackData)) {
                return;
            }
            $this->errorReturn(E_SUCCESS);
        }
    
        /*
        * 遥控器按键学习状态（UCA_PUSH）
        */
        public function getLearn(){
            $devSn = $this->_devSn;
            $codeLearn = UdpControl::generateControl(UCOT_IA, UCSOT_IA_CODE, UCAT_CODE_RC_LEARN);
            /*
            $codeLearn['type'] = $_POST['type'];
            $codeLearn['key'] = $_POST['key'];
            $codeLearn['devid'] = $_POST['devid'];
            $codeLearn['action'] = $_POST['action'];
            * 
            */
            $unpackData = $this->_udpCtrl->excuteControl($devSn, $codeLearn, UCA_GET);
            
            
            var_dump($unpackData);
            return;
            
            if (!$this->checkReturn($unpackData)) {
                return;
            }
            $retData['type'] = UdpControl::fetchControl($unpackData, $codeLearn, 'type');
            $retData['key'] = UdpControl::fetchControl($unpackData, $codeLearn, 'key');
            $retData['devid'] = UdpControl::fetchControl($unpackData, $codeLearn, 'devid');
            $retData['action'] = UdpControl::fetchControl($unpackData, $codeLearn, 'action');
            $this->jsonReturn($retData);
        }
        
        
        /*
        * 遥控器按键控制
        */
        public function putControl(){
            /*
            if(!isset($_POST['key']) || $_POST['key'] > 255){
                return $this->errorReturn(E_INVALID_PARAMETER);
            }
             * 
             */
            $devSn = $this->_devSn;
            $codeControl = UdpControl::generateControl(UCOT_IA, UCSOT_IA_CODE, UCAT_CODE_RC_CTRL);
            $codeControl['type'] = 1;
            $codeControl['key'] = 0;
            $codeControl['irid'] = 881;
            $codeControl['devid'] = 1;
            $codeControl['flags'] = 1;
            $unpackData = $this->_udpCtrl->excuteControl($devSn, $codeControl, UCA_SET);
            
            
            var_dump($unpackData);
            return;
            
            if (!$this->checkReturn($unpackData)) {
                return;
            }
            $this->errorReturn(E_SUCCESS);
        }
        
        
        
        
        
        
        
        
        
}
//808100011211  808000022657 808100000082
//"115.29.198.74" "10.133.34.112"
$wkController = new WukongController;
$wkController->_devSn = '808600000224';
//$wkController->_devSn = '808600001469';


/*
//$wkController->getDeviceInfo();
//$wkController->getTempCurve();
//$wkController->setReboot();
//$wkController->getTempSampleCurve();
//$wkController->setMatchCode();
//$wkController->setUserSet();
//$wkController->getUserSet();
//$wkController->setTempCurve();
//$wkController->setMatchCode();
//$wkController->setExtendedTimer();
//$wkController->getExtendedTimer();
//$wkController->setMatchCode();
 */
$wkController->getControllInfo();//获取遥控器信息----自测通过
//$wkController->setControllinfo();//修改遥控器名称----自测通过
//$wkController->setMatch();//云匹配请求匹配----自测通过
//sleep(3);

//$wkController->getMatchState();//云匹配获取匹配状态----自测通过
//$wkController->getKey();//遥控器按键查询----自测通过
//$wkController->postKey();//新增自定义遥控器按键----自测通过
//$wkController->setKey();//修改自定义遥控器按键----自测通过
//$wkController->deleteKey();//删除自定义遥控器按钮----自测通过
//$wkController->putLearn();//遥控器按键学习请求（UCA_SET）----自测通过
//$wkController->deleteLearn();//取消遥控器按键学习请求----自测通过(//返回结果报文不符合标准报文格式，此处做返回成功处理)
//$wkController->getLearn();//遥控器按键学习状态（UCA_PUSH）----自测通过
//$wkController->putControl();//遥控器按键控制----自测通过






exit;
/*
sleep(2);
$wkController->getAttri();

sleep(2);
$wkController->getAttri();

sleep(2);
$wkController->getAttri();
*/
