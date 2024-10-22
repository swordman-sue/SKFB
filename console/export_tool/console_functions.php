<?php
require 'include/common.php';


/** 
 * 取角色名称
 */
function get_role_name($conn,$role_id){	
	$select_sql = "select role_name,level from role where role_id = ".$role_id;
	$result=mysqli_query($conn,$select_sql);
	$row=mysqli_fetch_array($result);
	$role_name = $row["role_name"];		
    return $role_name;
}


/** 
 * 取角色等级
 */
function get_role_level($conn,$role_id){
	$select_sql = "select role_name,level from role where role_id = ".$role_id;
	$result=mysqli_query($conn,$select_sql);
	$row=mysqli_fetch_array($result);
	$role_level = $row["level"];
    return $role_level;
}

/** 
 * 取角色信息
 */
function get_role_info($conn,$role_id){
	$select_sql = "select role_name,level,server_id from role where role_id = ".$role_id;
	$result=mysqli_query($conn,$select_sql);
	$row=mysqli_fetch_array($result);
	$role_info[0] = $row["level"];
	$role_info[1] = $row["role_name"];
	$role_info[2] = $row["server_id"];
    return $role_info;
}



?>
