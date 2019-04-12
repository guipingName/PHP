<?php

namespace  Web\Lib;

class Session{
    protected $handle       = null;

    public function __destruct() {
        $this->handle = NULL;
    }

    public function __construct($ip = null, $port = 11211) {
        if (null == $ip)
            $ip = check_ip_to_dns(C("MEMCACHE_SERVER_IP"), "127.0.0.1");
        
        $this->handle = new \Memcached();
        if (false === $this->handle->addServer($ip,$port)) {
            DBG_bak('IP:'.$ip.' port:'.$port.' connect failed');
        }
    }

    public function create($sessionID, $sessionData, $lifetime = 3600) {
        return $this->handle->set($sessionID, $sessionData, $lifetime);
    }

    /**
     * 读取Session
     * @access public
     * @param string $sessionID
     */
    public function read($sessionID) {
        return $this->handle->get($sessionID);
    }

    /**
     * 写入Session
     * @access public
     * @param string $sessionID
     * @param String $sessionData  当前session值
     * @param String $updateData   修改的值
     * @param String $lifetime
     */
    public function write($sessionID, $sessionData,$lifetime = 3600) {
        $cas = null;
        if(empty($sessionData)){
            return true;
        }
        if(!is_array($sessionData) && !is_object($sessionData)){
            DBG_bak('[Session] data no array '.json_decode($sessionData));
            return false;
        }
        do {
            /* 获取 $sessionID 的值 和 标记 */
            $values  =  $this->handle->get($sessionID, null, $cas);
            /* 如果列表不存在，创建并进行一个原子添加（如果其他客户端已经添加，这里就返回false）*/
            if ($this->handle->getResultCode() == \Memcached::RES_NOTFOUND) {
                $flag = $this->handle->set($sessionID, $sessionData, $lifetime);
                $flag = ($flag === false) ? false : true;
            } else {/* 其他情况下，添加 session 值到列表中， 并以cas方式去存储， 这样当其他客户端修改过， 则返回false */
                if(empty($values)){
                    $data = json_encode(array_session_filter($sessionData));
                    $flag = $this->handle->cas($cas, $sessionID, $data, $lifetime);
                }else{
                    $values = json_decode($values,true);
                    if($values === false || !is_array($values)){
                        $data = json_encode(array_session_filter($sessionData));
                    }else{
                        $data = array_merge($values, $sessionData);
                        $data = json_encode(array_session_filter($data));
                    }
                    $flag = $this->handle->cas($cas, $sessionID, $data, $lifetime);
                }
            }
        } while ($this->handle->getResultCode() != \Memcached::RES_SUCCESS);
        return $flag;
    }

    /**
     * 删除Session
     * @access public
     * @param string $sessionID
     */
    public function destroy($sessionID) {
        return $this->handle->delete($sessionID);
    }

    /**
     * Session垃圾回收
     * @access public
     * @param string $sessMaxLifeTime
     */
    public function gc($sessMaxLifeTime) {
        return true;
    }

    /**
     * 关闭Session
     * @access public
     */
    public function close() {
        $this->gc(ini_get('session.gc_maxlifetime'));
        $this->handle->close();
        $this->handle       = null;
        return true;
    }
}
