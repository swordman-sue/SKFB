<?php
header("Content-type: text/html; charset=utf-8");
/*
 * Created on 2015-9-1
 *
 * To change the template for this generated file go to
 * Window - Preferences - PHPeclipse - PHP - Code Templates
 */

function test() {
	
	//echo "充值成功";
	$arr = array('a' => 1, 'b' => 2, 'c' => 3, 'd' => 4, 'e' => 5);
	echo json_encode($arr);
}
test();


?>
