<?php

//定义项目根目录
define('ROOT_PATH',			dirname(dirname(__FILE__)));

//定义服务器端ERL目录
define('ERL_SERVER_PATH',	ROOT_PATH.'/server_config/');

//定义服务器端HRL目录
define('HRL_SERVER_PATH',   ROOT_PATH.'/server_config_record/');

//定义WIN客户端资源配置CNF目录
define('UI_CLIENT_PATH',	ROOT_PATH.'/client_config/');

//定义WIN客户端资源配置CNF目录2
define('UI_CLIENT_PATH_TWO',   ROOT_PATH.'/client_config2/');

//定义EXCEL目录
define('EXCEL_PATH',		ROOT_PATH.'/planner_xml/');

//生成时文件锁定时间
define('CREATE_LOCK_TIME',	30);

//最多生成配置文件数
define('MAX_CREATE_FILE',  50);

?>