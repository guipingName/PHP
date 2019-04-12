<?php

namespace Api\Model;

use Think\Model;
/*
CREATE TABLE `pre_userinfo_tbl` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `username` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  `school_id` int(11) NOT NULL,
  `manage_grade` varchar(64) NOT NULL DEFAULT '',
  `usertype` smallint(3) NOT NULL DEFAULT '2',
  `lastlogin_time` datetime NOT NULL,
  `create_time` datetime NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `index_name_schoolid` (`username`,`school_id`) USING BTREE
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;
 */
class UserinfoTblModel extends Model {
	protected $trueTableName = 'pre_userinfo_tbl'; 
	
	public function __construct($name='',$tablePrefix='',$connection='') {
	    parent::__construct($name, $tablePrefix, C('DB_SCHOOLYARD_LIGHTING'));
	}
}