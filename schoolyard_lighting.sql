/*
Navicat MySQL Data Transfer

Source Server         : 10.133.17.56
Source Server Version : 50635
Source Host           : 10.133.17.56:3306
Source Database       : schoolyard_lighting

Target Server Type    : MYSQL
Target Server Version : 50635
File Encoding         : 65001

Date: 2019-03-11 14:48:16
*/

SET FOREIGN_KEY_CHECKS=0;


-- ----------------------------
-- Table structure for pre_userinfo_tbl
-- ----------------------------
DROP TABLE IF EXISTS `pre_userinfo_tbl`;
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

