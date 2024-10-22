<?php
require 'include/common.php';

//$v = !empty($_GET['v']) ? trim($_GET['v']) : '';
$time_start = time();
$SERVER_PATH = 'http://'.$_SERVER['SERVER_NAME'].substr($_SERVER['REQUEST_URI'], 0, strrpos($_SERVER['REQUEST_URI'], '/'));

foreach($_cfg as $key => $val)
{
    output_msg( "<br>--开始生成 {$val}配置<br />".curl_get_content($SERVER_PATH."/main_default_new.php?ac={$val}&type=all"));
}

$time_end = time();
output_end();
output_msg("<br>总共用时：".($time_end - $time_start)."秒");
