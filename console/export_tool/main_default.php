<?php

require 'include/common.php';

//初始化 脚本名称、工作簿、工作表、生成规则
$script_name = "";
$excel_list = array();
$sheet_list = array();
$create_config = array();
$file_name = $_GET['ac'];
$is_need_key = 0;
$msg['twoRow'] = array('不可空', '可空#需字段', '可空#不需字段');

require TPL_DIR.$file_name."/{$file_name}.cfg.php";
$is_file_exists = (file_exists(TPL_DIR.$file_name."/{$file_name}.php")) ? true : false;
$is_file_exists && require TPL_DIR.$file_name."/{$file_name}.php";

lock_file($script_name);
//服务器端ERL
isset($create_config['erl']) && $erl_server = array();
//服务器端LUA
isset($create_config['lua']) && $lua_server = array();
//WIN客户端CNF
isset($create_config['cnf']) && $ui_client = array();

//遍历每一个EXCEL文件
foreach ($excel_list AS $excel_key => $excel_value) {

    //读取该EXCEL文件的数据
    //其中,$excel_data[i][j][k]表示该EXCEL文件第i个工作表第j行第k列的数据
    $excel_data = read_excelxml($excel_value);

    //遍历每一个工作表
    foreach($sheet_list[$excel_key] AS $sheet_key => $sheet_value) {
        
        //读取该工作表的数据
        //其中,$sheet_data[i][j]表示该工作表第i行第j列的数据
        $sheet_data = $excel_data[$sheet_value['sheet']];

        //获取该工作表列字段名称
        $column_title = $sheet_data[1];
        //配置提示信息验证
        foreach ($column_title AS $sub_column => $sub_value) {
            if($sub_value){
                $tmp_sub_value = explode("|", $sub_value);
                if(empty($tmp_sub_value[0]) && empty($tmp_sub_value[1])){
                    continue;
                }
                if ($sheet_data[2][$sub_column] == '') {
                    $error_msg = "【出错】：EXCEL文件【{$excel_list[$script_name]}】工作表:{$sheet_value['name']}，第2行第".$sub_column."列\"".$sub_value."\"字段配置提示信息(可填值:不可空、可空#需字段、可空#不需字段)不能为空!";
                    output_error($script_name, $error_msg);
                }
                if (!in_array($sheet_data[2][$sub_column], $msg['twoRow'])) {
                    $error_msg = "【出错】：EXCEL文件【{$excel_list[$script_name]}】工作表:{$sheet_value['name']}，第2行第".$sub_column."列\"".$sub_value."\"字段配置编写错误：{$sheet_data[2][$sub_column]}，提示信息(可填值:不可空、可空#需字段、可空#不需字段)!";
                    output_error($script_name, $error_msg);
                }
                if ($sheet_data[3][$sub_column] == '') {
                    $error_msg = "【出错】：EXCEL文件【{$excel_list[$script_name]}】工作表:{$sheet_value['name']}，第3行第".$sub_column."列\"".$sub_value."\"字段配置提示信息(可填值:数字、字符串、一维数组、二维数组)不能为空!";
                    output_error($script_name, $error_msg);
                } else {
                    if ($sheet_data[3][$sub_column] == '一维数组' || $sheet_data[3][$sub_column] == '二维数组') {
                        if ($sheet_data[4][$sub_column] == '') {
                            $error_msg = "【出错】：EXCEL文件【{$excel_list[$script_name]}】工作表:{$sheet_value['name']}，第4行第".$sub_column."列\"".$sub_value."\"字段配置提示信息不能为空!";
                            output_error($script_name, $error_msg);
                        } else if (strpos($sheet_data[4][$sub_column], "*") === FALSE) {
                            $error_msg = "【出错】：EXCEL文件【{$excel_list[$script_name]}】工作表:{$sheet_value['name']}，第4行第".$sub_column."列\"".$sub_value."\"字段配置提示信息格式有误!(缺少*分隔符)";
                            output_error($script_name, $error_msg);
                        } else {
                            $unit_sheet_data = explode("*", $sheet_data[4][$sub_column]);
                            if (!is_numeric($unit_sheet_data[1])) {
                                $error_msg = "【出错】：EXCEL文件【{$excel_list[$script_name]}】工作表:{$sheet_value['name']}，第4行第".$sub_column."列\"".$sub_value."\"字段配置提示信息子类型必须为数字!";
                                output_error($script_name, $error_msg);
                            }
                        }
                    }
                }
            }
        }
        
        //遍历该工作表的每一行
        for ($row = $sheet_value['start_column']; $row <= count($sheet_data); $row++) {
            if ($sheet_value['sheet_name'] == "Activity")
                continue;
            //服务器端、客户端该行数据初始化
            isset($create_config['erl']) && $erl_server_column_data = array();
            isset($create_config['lua']) && $server_column_data = array();
            isset($create_config['cnf']) && $ui_client_column_data = array();

            //遍历该工作表的每一列
            foreach ($column_title AS $column => $value) {
                //$status 执行状态值(0 : 执行常规处理, 1 : 执行特殊处理, 2 : continue)
                $status = 0;
                //获取该工作表第row行第column列的格子的数据
                $node_value = trim($sheet_data[$row][$column]);
                
                if($is_file_exists){
                    $status = main_extend($server_column_data, $erl_server_column_data, $ui_client_column_data);
                }
                
                if($status > 0)
                {
                    continue;
                }
                elseif($status == 0)
                {
                    if (empty($sheet_data[4][$column])) {
                        $result = preprocess($row, $column, $node_value, $sheet_data[2][$column], $sheet_data[3][$column], '');
                    } else {
                        $result = preprocess($row, $column, $node_value, $sheet_data[2][$column], $sheet_data[3][$column], $sheet_data[4][$column]);
                    }
                    
                    if ($sheet_data[1][$column] != '') {
                        if (strpos($sheet_data[1][$column], "|") === FALSE) {
                            $error_msg = "【出错】：EXCEL文件【{$excel_list[$script_name]}】工作表:{$sheet_value['name']}，第1行第".$column."列数据格式有误!(缺少|分隔符)";
                            output_error($script_name, $error_msg);
                        } else {
                            if ($sheet_data[2][$column] == '可空#不需字段' && $node_value == '') {
                                continue;
                            }
                            $sub_assign = explode("|", $sheet_data[1][$column]);
                            if ($sub_assign[0] != '') {
                                $ui_client_column_data[$sub_assign[0]] = $result;
                            }
                            if ($sub_assign[1] != '') {
                                $server_column_data[$sub_assign[1]] = $result;
                                $erl_server_column_data[$sub_assign[1]] = dispose_erl_array($result, 1);
                            }
                        }
                    }
                }
            }
            
            if($is_need_key){
                (isset($create_config['erl']) && $erl_server_column_data) && $erl_server[$sheet_value['sheet_name']][] = $erl_server_column_data;
                (isset($create_config['lua']) && $server_column_data) && $lua_server[$sheet_value['sheet_name']][] = $server_column_data;
                (isset($create_config['cnf']) && $ui_client_column_data) && $ui_client[$sheet_value['sheet_name']][] = $ui_client_column_data;
            }else{
                (isset($create_config['erl']) && $erl_server_column_data) && $erl_server[] = $erl_server_column_data;
                (isset($create_config['lua']) && $server_column_data) && $lua_server[] = $server_column_data;
                (isset($create_config['cnf']) && $ui_client_column_data) && $ui_client[] = $ui_client_column_data;
            }
        }
    }
}

//服务器端ERL
if(isset($create_config['erl']) && $erl_server)
{
    if(count($create_config['erl']) == 1 && empty($create_config['erl'][0][2])){
        create_server_erl($create_config['erl'][0][0], $file_name ."/". $create_config['erl'][0][1], 'erl_server', $erl_server);
    }else{
        foreach($create_config['erl'] as $erl){
            create_server_erl($erl[0], $file_name ."/". $erl[1], 'erl_server', $erl_server[$erl[2]]);
        }
    }
}
//服务器端LUA
if(isset($create_config['lua']) && $lua_server)
{
    if(count($create_config['lua']) == 1  && empty($create_config['lua'][0][2])){
        if(file_exists(LUA_SERVER_REAL_PATH."{$create_config['lua'][0][0]}")){
            create_server_lua($create_config['lua'][0][0], $file_name ."/". $create_config['lua'][0][1], 'lua_server', $lua_server);
        }else{
            echo $create_config['lua'][0][0]."占位文件没有提交!<br>";
        }
    }else{
        foreach($create_config['lua'] as $lua){
            if(file_exists(LUA_SERVER_REAL_PATH."{$lua[0]}")){
                create_server_lua($lua[0], $file_name ."/". $lua[1], 'lua_server', $lua_server[$lua[2]]);
            }else{
                echo $lua[0]."占位文件没有提交!<br>";
            }
        }
    }
}

//WIN客户端资源CNF
if(isset($create_config['cnf']) && $ui_client)
{
    if(count($create_config['cnf']) == 1 && empty($create_config['cnf'][0][2])){
        if(file_exists(UI_CLIENT_PATH."{$create_config['cnf'][0][0]}")){
            create_client_cnf($create_config['cnf'][0][0], $file_name ."/". $create_config['cnf'][0][1], 'ui_client', $ui_client);
        }else{
            echo $create_config['cnf'][0][0]."占位文件没有提交!<br>";
        }
    }else{
        foreach($create_config['cnf'] as $cnf){
            if(file_exists(UI_CLIENT_PATH."{$cnf[0]}")){
                create_client_cnf($cnf[0], $file_name ."/". $cnf[1], 'ui_client', $ui_client[$cnf[2]]);
            }else{
                echo $cnf[0]."占位文件没有提交!<br>";
            }
            
        }
    }
}

//服务器端global的LUA
if(isset($create_config['lua_global']) && $lua_server)
{
    if(count($create_config['lua_global']) == 1  && empty($create_config['lua_global'][0][2])){
        create_global_lua($create_config['lua'][0][0], $file_name ."/". $create_config['lua_global'][0][1], 'lua_server', $lua_server);
    }else{
        foreach($create_config['lua_global'] as $lua){
            create_global_lua($lua[0], $file_name ."/". $lua[1], 'lua_server', $lua_server[$lua[2]]);
        }
    }
}

del_lock_file($script_name);

output_end();

?>