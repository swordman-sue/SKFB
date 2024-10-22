<?php
session_start();
define('ROOT_DIR', dirname(dirname(__FILE__)).'/');
define('TPL_DIR', ROOT_DIR.'template/');

header("Content-Type: text/html; charset=UTF-8");

require_once(ROOT_DIR.'config.php');
require_once(ROOT_DIR.'include/export.cfg.php');
require_once(ROOT_DIR.'include/functions.php');
require_once(ROOT_DIR.'lib/smarty/Smarty.class.php');

@session_start();
@ini_set('max_execution_time', '1000');//执行超时时间设置为1000秒
@ini_set('memory_limit','600M');
date_default_timezone_set("Asia/Shanghai");//时间区域修改，也可以改php.ini
ini_set('display_errors', 1);
error_reporting(E_ALL ^ E_NOTICE);//暂时屏蔽NOTICE

$tool_url = "http://182.254.186.181/skfb/console_develop/export_tool";
$mysql_server_name='192.168.1.111:3307'; //改成自己的mysql数据库服务器
$mysql_username='root'; //改成自己的mysql数据库用户名
$mysql_password='debug'; //改成自己的mysql数据库密码
$mysql_database='skfb'; //改成自己的mysql数据库名
//$conn = mysqli_connect($mysql_server_name,$mysql_username,$mysql_password,$mysql_database) or die("error connecting") ; //连接数据库

$smarty = new Smarty();
$smarty->compile_dir = ROOT_DIR.'templates_c/';
$smarty->cache_dir = ROOT_DIR.'cache/';
$smarty->template_dir = ROOT_DIR.'template/';
$smarty->left_delimiter = "<{";
$smarty->right_delimiter = "}>";
$smarty->cache_lifetime = 3600;
$smarty->caching = 0;

$error_list = array();
