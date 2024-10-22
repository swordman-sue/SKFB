<?php

require 'include/common.php';

//初始化 工作簿、工作表、生成规则
$excel_list = $sheet_list = $create_config = array();
//脚本名称、处理xml名
$file_name = $_GET['ac'];
//erlang头部、格式、尾部
$erl_head = $erl_format = $erl_end = array();
//cnf头部
$cnf_name = array();

$msg['twoRow'] = array('不可空', '可空#需字段', '可空#不需字段');
$is_need_key = 1;

$excel_list = array($file_name.".xml");

//lock_file($file_name);
//服务器端ERL
isset($create_config['erl']) && $erl_server = $hrl_server = array();

//WIN客户端CNF
isset($create_config['cnf']) && $ui_client = array();

//遍历每一个EXCEL文件
foreach ($excel_list AS $excel_key => $excel_value) {

    //读取该EXCEL文件的数据
    //其中,$excel_data[i][j][k]表示该EXCEL文件第i个工作表第j行第k列的数据
    $excel_data = read_excelxml($excel_value);
    $excel_data_count = count($excel_data) - 1;

    //工作表配置
    for($t=1; $t<=$excel_data_count; $t++)
    {
        $sheet_list[$excel_key][] = array('sheet' => $t, 'name' => $t, 'sheet_name' => $t, 'start_column' => 7);
    }

    //构造文件配置
    get_config($excel_data[0], $create_config, $erl_head, $erl_format, $erl_end, $cnf_name);
    
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
                    $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第2行第".$sub_column."列\"".$sub_value."\"字段配置提示信息(可填值:不可空、可空#需字段、可空#不需字段)不能为空!";
                    output_error($file_name, $error_msg);
                }
                if (!in_array($sheet_data[2][$sub_column], $msg['twoRow'])) {
                    $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第2行第".$sub_column."列\"".$sub_value."\"字段配置编写错误：{$sheet_data[2][$sub_column]}，提示信息(可填值:不可空、可空#需字段、可空#不需字段)!";
                    output_error($file_name, $error_msg);
                }
                if ($sheet_data[3][$sub_column] == '') {
                    $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第3行第".$sub_column."列\"".$sub_value."\"字段配置提示信息(可填值:数字、字符串、一维数组、二维数组、三维数组)不能为空!";
                    output_error($file_name, $error_msg);
                } else {
                    if ($sheet_data[3][$sub_column] == '一维数组' || $sheet_data[3][$sub_column] == '二维数组') {
                        if ($sheet_data[4][$sub_column] == '') {
                            $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第4行第".$sub_column."列\"".$sub_value."\"字段配置提示信息不能为空!";
                            output_error($file_name, $error_msg);
                        } else if (strpos($sheet_data[4][$sub_column], "*") === FALSE && $sheet_data[4][$sub_column] !== "不定长") {
                            $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第4行第".$sub_column."列\"".$sub_value."\"字段配置提示信息格式有误!(缺少*分隔符)";
                            output_error($file_name, $error_msg);
                        }
                    }
                }
            }
        }
        
        
        //遍历该工作表的每一行
        for ($row = $sheet_value['start_column']; $row <= count($sheet_data); $row++) {

            //服务器端、客户端该行数据初始化
            isset($create_config['erl']) && $hrl_server_column_data = array();
            isset($create_config['erl']) && $erl_server_column_data = array();
            isset($create_config['cnf']) && $ui_client_column_data = array();

            //遍历该工作表的每一列
            foreach ($column_title AS $column => $value) {
                //获取该工作表第row行第column列的格子的数据
                $node_value = trim($sheet_data[$row][$column]);
                
                $result = preprocess($row, $column, $node_value, $sheet_data[2][$column], $sheet_data[3][$column], $sheet_data[4][$column], $sheet_data[1][$column]);
                
                if ($sheet_data[1][$column] != '') {
                    if (strpos($sheet_data[1][$column], "|") === FALSE) {
                        $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第1行第".$column."列数据格式有误!(缺少|分隔符)";
                        output_error($file_name, $error_msg);
                    } else {
                        if ($sheet_data[2][$column] == '可空#不需字段' && $node_value == '') {
                            continue;
                        }
                        $sub_assign = explode("|", $sheet_data[1][$column]);
                        if ($sub_assign[0] != '') {
                            $ui_client_column_data[$sub_assign[0]] = ($result !== "[]") ? $result : "{}";
                        }
                        if ($sub_assign[1] != '') {
                            !isset($hrl_server_column_data[$sub_assign[1]]) && $hrl_server_column_data[$sub_assign[1]] = $sub_assign[1];
                            $erl_server_column_data[$sub_assign[1]] = $result;
                        }
                    }
                }
            }
            (isset($create_config['erl']) && $hrl_server_column_data && !isset($hrl_server[$sheet_value['sheet_name']])) && $hrl_server[$sheet_value['sheet_name']] = $hrl_server_column_data;
            (isset($create_config['erl']) && $erl_server_column_data) && $erl_server[$sheet_value['sheet_name']][] = $erl_server_column_data;
            (isset($create_config['cnf']) && $ui_client_column_data) && $ui_client[$sheet_value['sheet_name']][] = $ui_client_column_data;
        }
    }
}

//服务器端ERL
if(isset($create_config['erl']) && $erl_server)
{
    foreach($create_config['erl'] as $erl){
        foreach($erl[2] as $v)
        {
            $erl_server_data[$v] = $erl_server[$v];
            $hrl_server_data[$v] = $hrl_server[$v];
        }

        $assign_erl_arr = array(
            'erl_server' => $erl_server_data,
            'erl_server_num' => $erl[2],
            'erl_head' => $erl_head[$erl[0]],
            'erl_format' => $erl_format[$erl[0]],
            'erl_end' => $erl_end[$erl[0]],
            'hrl_server' => $hrl_server_data,
        );
        if(file_exists(ERL_SERVER_PATH."{$erl[0]}")){
            create_server_erl($erl[0], "main_default/". $erl[1], $assign_erl_arr);
        }else{
	    echo "<br>";
            echo $erl[0]."占位文件不存在, 正在创建...!<br>";
	    system("mkdir -p ".ERL_SERVER_PATH);
	    system("touch ".ERL_SERVER_PATH."{$erl[0]}");
	    system("svn add ".ERL_SERVER_PATH."{$erl[0]}");
	    create_server_erl($erl[0], "main_default/". $erl[1], $assign_erl_arr);
	   
        }
    }
}

//WIN客户端资源CNF
if(isset($create_config['cnf']) && $ui_client)
{
    
    foreach($create_config['cnf'] as $cnf_data)
    {
        $cnf = array();

        foreach($cnf_data as $cnf_key => $cnf_val)
        {
            $cnf[0] = $cnf_val[0];
            foreach($cnf_val[2] as $v)
            {
                $ui_client_data[$v] = $ui_client[$v];
            }

            $assign_cnf_arr[$cnf_key] = array(
                'ui_client' => $ui_client_data,
                'ui_client_num' => $cnf_val[2],
                'cnf_name' => $cnf_name[$cnf_val[0]][$cnf_key],
                'cnf_special' => $cnf_val[4],
            );
        }

        if(file_exists(UI_CLIENT_PATH."{$cnf[0]}")){
            create_client_cnf($cnf[0], "main_default/client_ui.html", $assign_cnf_arr);
        }else{
            echo "<br>"; 
	    echo $cnf[0]."占位文件不存在, 正在创建...!<br>";
            system("mkdir -p ".UI_CLIENT_PATH);
	    system("touch ".UI_CLIENT_PATH."{$cnf[0]}");
	    system("svn add ".UI_CLIENT_PATH."{$cnf[0]}");
	    create_client_cnf($cnf[0], "main_default/client_ui.html", $assign_cnf_arr);
        }
    }
}

del_lock_file($file_name);

output_end();

?>
