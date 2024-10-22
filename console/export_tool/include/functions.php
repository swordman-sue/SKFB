<?php
/**
 * 读取Excel生成的XML文件，返回数组
 * Excel生成的XML文件数据的我们需要的结构为：
 * Worksheet->Table->Row->Cell->Data
 */
function read_excelxml($data_file) {
    if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') {
        $data_file = iconv("UTF-8","gbk",$data_file);
    }
    
    if (PHP_VERSION >= '5.2') {
        $attr_ssindex = 'ss:Index';
    } else {
        $attr_ssindex = 'Index';
    }
    $doc = new DOMDocument('1.0', 'utf-8');
    $doc->load(EXCEL_PATH.$data_file);

    $worksheets = $doc->getElementsByTagName('Worksheet');
    foreach ($worksheets AS $worksheet_index => $worksheet) {
        $irow = 1;
        $tables = $worksheet->getElementsByTagName('Table');
        foreach ($tables AS $table) {
            $rows = $table->getElementsByTagName('Row');
            foreach ($rows AS $row) {
                $row_ssindex = $row->getAttribute($attr_ssindex);
                while ($irow < $row_ssindex) {
                    $xml_data[$worksheet_index][$irow] = array();
                    $irow++;
                }
                $cells = $row->getElementsByTagName('Cell');
                $icell = 1;
                if ($cells->length > 0) {
                    foreach ($cells AS $cell) {
                        $cell_ssindex = $cell->getAttribute($attr_ssindex);
                        while ($icell < $cell_ssindex) {
                            $xml_data[$worksheet_index][$irow][$icell] = '';
                            $icell++;
                        }
                        $datas = $cell->getElementsByTagName('Data');
                        $data = $datas->item(0)->nodeValue;
                        $xml_data[$worksheet_index][$irow][$icell] = $data; //将数据存储到2维数组里
                        $icell++;
                    }
                }else{
                    $xml_data[$worksheet_index][$irow] = array();
                }
                $irow++;
            }
        }
    }
    return $xml_data;
}

//获取配置信息
function get_config($data, &$create_config, &$erl_head, &$erl_format, &$erl_end, &$cnf_name)
{
    global $file_name, $excel_data_count;

    for($i=1; $i<=MAX_CREATE_FILE; $i++)
    {
        //数据从第二行开始
        $t = $i + 1;
        if(!trim($data[$t][1]) && !trim($data[$t][3]))
        {
            break;
        }

        //erlang配置文件名
        $create_file = trim($data[$t][1]);
        if($create_file)
        {
            $tmp_arr = explode(",", $data[$t][2]);
            foreach($tmp_arr as $v)
            {
                if (!is_numeric($v) || $v > $excel_data_count || $v < 0)
                {
                    $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:配置内容，第{$t}行第2列工作表序号错误!";
                    output_error($file_name, $error_msg);
                }
            }
            $create_config['erl'][] = array($create_file, 'erl_server.html', $tmp_arr);
            
            //获取erlang信息
            $erl_head[$create_file] = $data[$t][5];
            if($data[$t][6])
            {
                $reg = "/get\((.*)\).*#(.*)/";
                preg_match($reg, trim($data[$t][6]), $match);
                if(!$match[1] || !$match[2])
                {
                    $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:配置内容，第{$t}行第6列工作表序号错误!";
                    output_error($file_name, $error_msg);
                }
                $match_arr['get'] = explode(",", $match[1]);
                $match_arr['fun'] = $match[2];
                $erl_format[$create_file] = $match_arr;
            }
            
            $erl_end[$create_file] = $data[$t][7];
        }

        //cnf配置文件名
        $create_file_str = trim($data[$t][3]);
        if($create_file_str)
        {
            $c_arr = explode("|", $create_file_str);
            if (!$c_arr[0] || !$c_arr[1])
            {
                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:配置内容，第{$t}行第3列客户端文件名错误!";
                output_error($file_name, $error_msg);
            }
            $create_file = trim($c_arr[0]);
            $cnf_name[$create_file][] = trim($c_arr[1]);

            $cnf_dir = (isset($c_arr[2]) && trim($c_arr[2]) == 1) ? UI_CLIENT_PATH : UI_CLIENT_PATH_TWO;

            $cnf_special = array();
            if($data[$t][8])
            {
                $l_arr = explode("|", $data[$t][8]);
                $s_arr = explode("=>", $l_arr[0]);
                $cnf_special = array("type"=>$s_arr[0], "data"=>$s_arr[1], "extra"=>$l_arr[1]);
            }

            $tmp_arr = explode(",", $data[$t][4]);
            foreach($tmp_arr as $v)
            {
                if (!is_numeric($v) || $v > $excel_data_count || $v < 0)
                {
                    $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:配置内容，第{$t}行第4列工作表序号错误!";
                    output_error($file_name, $error_msg);
                }
            }
            $create_config['cnf'][$create_file][] = array($create_file, 'client_ui.html', $tmp_arr, $cnf_dir, $cnf_special);
        }
    }
}

function preprocess($row, $column, $node_value, $allow_empty, $type, $sub_type, $field_value) {
    global $file_name, $excel_list, $sheet_value;
    
    if(!$field_value)
    {
        return;
    }

    if ($allow_empty == '不可空' && $node_value == '') {
        $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据不允许为空!";
        output_error($file_name, $error_msg);
    } else if ($allow_empty == '可空#需字段' && $node_value == '') {
        switch ($type) 
        {
            case '数字':
                $node_value = "0";
            break;
            case '字符串':
                $node_value = "PHP_EMPTY";
            break;
            case '一维数组':
            case '二维数组':
            case '二维数组2':
            case '三维数组':
                $node_value = "[]";
            break;
        }
        return $node_value;
    } else if (($allow_empty == '不可空' && $node_value != '') || ($allow_empty == '可空#需字段' && $node_value != '') || ($allow_empty == '可空#不需字段' && $node_value != '')) {

        switch ($type) {
            case '数字':
                if (!is_numeric($node_value)) {
                    $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据类型必须为数字!";
                    output_error($file_name, $error_msg);
                } else {
                    return $node_value;
                }
                break;
            case '字符串':
                return $node_value;
                break;
            case '一维数组':
                $unit = explode("*", $sub_type);
                $unit_type = $unit[0];
                $unit_num = intval($unit[1]);
                switch ($unit_type) {
                    case '数字':
                        if (strpos($node_value, "|") === FALSE) {
                            $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据格式有误!(缺少|分隔符)";
                            output_error($file_name, $error_msg);
                        } else {
                            $unit_node_value = explode("|", $node_value);
                            if ($unit_num < count($unit_node_value)) {
                                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据所填元素个数比实际要多!";
                                output_error($file_name, $error_msg);
                            } else if ($unit_num > count($unit_node_value)) {
                                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据所填元素个数比实际要少!";
                                output_error($file_name, $error_msg);
                            } else {
                                foreach ($unit_node_value AS $unit_key => $unit_value) {
                                    if (!is_numeric($unit_value)) {
                                        $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据子类型必须为数字!";
                                        output_error($file_name, $error_msg);
                                    }
                                }
                                return $unit_node_value;
                            }
                        }
                        break;
                    case '不定长':
                        $unit_node_value = dispose_str_to_array($node_value, 1);
                        return $unit_node_value;
                        break;
                    default:
                        break;
                }
                break;
            case '二维数组':
                $unit = explode("*", $sub_type);
                $unit_type = $unit[0];
                $unit_num = intval($unit[1]);
                switch ($unit_type) {
                    case '数字':
                        if (strpos($node_value, "#") === FALSE) {
                            $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据格式有误!(缺少#分隔符)";
                            output_error($file_name, $error_msg);
                        } else {
                            $unit_node_value = explode("#", $node_value);
                            if ($unit_num < count($unit_node_value)) {
                                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据所填元素个数比实际要多!";
                                output_error($file_name, $error_msg);
                            } else if ($unit_num > count($unit_node_value)) {
                                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据所填元素个数比实际要少!";
                                output_error($file_name, $error_msg);
                            } else {
                                $res = array();
                                foreach ($unit_node_value AS $unit_key => $unit_value) {
                                    if (strpos($unit_value, "|") === FALSE) {
                                        $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据格式有误!(缺少|分隔符)";
                                        output_error($file_name, $error_msg);
                                    } else {
                                        $sub_node_value = explode("|", $unit_value);
                                        foreach($sub_node_value as $v)
                                        {
                                            if(!is_numeric($v))
                                            {
                                                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据子类型必须为数字!";
                                                output_error($file_name, $error_msg);
                                            }
                                        }
                                        $res[] = $sub_node_value;
                                    }
                                }
                                return $res;
                            }
                        }
                        break;
                    case '不定长':
                        $unit_node_value = dispose_str_to_array($node_value, 2);
                        return $unit_node_value;
                        break;
                    default:
                        break;
                }
                break;
            case '二维数组2':
                $unit = explode("*", $sub_type);
                $unit_type = $unit[0];
                $unit_num = intval($unit[1]);
                switch ($unit_type) {
                    case '数字':
                        if (strpos($node_value, "\n") === FALSE) {
                            $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据格式有误!(缺少#分隔符)";
                            output_error($file_name, $error_msg);
                        } else {
                            $unit_node_value = explode("\n", $node_value);
                            if ($unit_num < count($unit_node_value)) {
                                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据所填元素个数比实际要多!";
                                output_error($file_name, $error_msg);
                            } else if ($unit_num > count($unit_node_value)) {
                                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据所填元素个数比实际要少!";
                                output_error($file_name, $error_msg);
                            } else {
                                $res = array();
                                foreach ($unit_node_value AS $unit_key => $unit_value) {
                                    if (strpos($unit_value, "|") === FALSE) {
                                        $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据格式有误!(缺少|分隔符)";
                                        output_error($file_name, $error_msg);
                                    } else {
                                        $sub_node_value = explode("|", $unit_value);
                                        foreach($sub_node_value as $v)
                                        {
                                            if(!is_numeric($v))
                                            {
                                                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据子类型必须为数字!";
                                                output_error($file_name, $error_msg);
                                            }
                                        }
                                        $res[] = $sub_node_value;
                                    }
                                }
                                return $res;
                            }
                        }
                        break;
                    case '不定长':
                        $unit_node_value = dispose_str_to_array($node_value, 3);
                        return $unit_node_value;
                        break;
                    default:
                        break;
                }
                break;
            case '三维数组':
                $unit_node_value = dispose_str_to_array($node_value, 4);
                return $unit_node_value;
                break;
            default:
                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第3行第".$column."列数据类型有误!";
                output_error($file_name, $error_msg);
        }
    }
}

//转化erlang数组
function dispose_erl_array($value, $type = 0){
    if(!is_array($value))
    {
        return $value;
    }
    
    $str = "";
    $status = 1;
    foreach($value as $k => $v)
    {
        if(is_array($v))
        {
            $str .= dispose_erl_array($v);
        }
        else
        {
            $status = 0;
            $str .= $v . ",";
        }
    }
	
	// 去掉一个字符；
	//if($type || $status)
	//{
	//	$str = rtrim($str, ";");
	//}
	
    $str = rtrim($str, ",");
    $str = ($type || $status) ? "[".$str."]" : "{".$str."}";
	!$type && $str .= ",";
	
	// modify by Kevin 2016/09/28
	//if(!$type && !$status)
	//{
	//	$str .= ";";
	//}
	//else if(!$type)
	//{
	//	$str .= ",";
	//}
	// modify by Kevin 2016/09/28
	
   
    return $str;
}

//获取cnf类型
function dispose_cnf_type($value){
    if(is_numeric($value) || $value == "{}")
    {
        return 1;
    }
    elseif(!is_array($value))
    {
        return 2;
    }
    elseif(is_array($value))
    {
        $return = arrayLevel($value);
        return $return + 2;
    }
}

//按数组元素个数
function init_array(&$data, $sdata, &$return)
{
    $is_list = 0;
    
    foreach($sdata as $sk => $sv)
    {
        $tmp_sv = $data[$sv];
        $tmp_sk = $sdata[$sk];
        unset($sdata[$sk]);

        if($sdata)
        {
            !isset($return[$tmp_sv]) && $return[$tmp_sv] = array();

            foreach($data as $k => $v)
            {
                if(strpos($k, "#".$tmp_sk))
                {
                    $k_arr = explode("#", $k);
                    $return[$tmp_sv][$k_arr[0]] = $v;
                    unset($data[$k]);
                    !$is_list && $is_list = 1;
                }
                if($k == $sv)
                {
                    $return[$tmp_sv][$k] = $v;
                    unset($data[$sv]);
                }
            }
            init_array($data, $sdata, $return[$tmp_sv]);
        }
        else
        {
            !isset($return['LIST'][$tmp_sv]) && $return['LIST'][$tmp_sv] = array();

            foreach($data as $k => $v)
            {
                $return['LIST'][$tmp_sv][$k] = $v;
            }
        }
        break;
    }
}

//特殊处理cnf数据
function dispose_cnf_data(&$cnf_data, $cnf_special)
{
    if(!isset($cnf_special['type']) || !$cnf_special['type'])
    {
        return 0;
    }

    $extra_data = array();
    $dispose_data = array();
    switch ($cnf_special['type']) {
        //erl联合key处理为二维数组
        case '1':
            $s_data = explode(",", $cnf_special['data']);
            $s_count = count($s_data);

            foreach($cnf_data as $key => $val)
            {
                init_array($val, $s_data, $dispose_data);
            }

            //字段排序
            // foreach($dispose_data as $k => $v)
            // {
            //     krsort($dispose_data[$k]);
            // }
            $cnf_data = $dispose_data;
            //var_dump($cnf_data);die();
            $extra_data['s_count'] = $s_count;
            break;
        //自定义key
        case '2':
            foreach($cnf_data as $k => $v)
            {
                $cnf_data[$k]['id'] = $v[$cnf_special['data']];
            }
            break;
        default:
            break;
    }
    return array("type"=>$cnf_special['type'],"extra"=>$extra_data);
}

/** 
 * 返回数组的维度 
 */
function arrayLevel($arr)
{
    $al = array(0);
    aL($arr,$al);
    return max($al);
}

function aL($arr, &$al, $level =0)
{
    if(is_array($arr)){
        $level++;
        $al[] = $level;
        foreach($arr as $v){
            aL($v, $al, $level);
        }
    }
}

//服务器端ERL
function create_server_erl($save_url, $temp_url, $assign_arr) {
    global $smarty;

    $file_url = ERL_SERVER_PATH.$save_url;

    $hrl_file = str_replace("erl", "hrl", $save_url);
    $hrl_url = HRL_SERVER_PATH.$hrl_file;

    foreach($assign_arr as $assign => $value)
    {
        $smarty->assign($assign, $value);
    }

    $smarty->register_function("dispose_erl_array", "dispose_erl_array");
    $content = $smarty->fetch($temp_url);
    file_put_contents($file_url, $content);

    $content_hrl = $smarty->fetch("main_default/hrl_server.html");
    
    if(!file_exists($hrl_url))
    {
        system("mkdir -p ".HRL_SERVER_PATH);
        system("touch ".HRL_SERVER_PATH."{$hrl_file}");
        system("svn add ".HRL_SERVER_PATH."{$hrl_file}");
    }
    file_put_contents($hrl_url, $content_hrl);
    
    $sub_path = substr(ERL_SERVER_PATH, strlen(ROOT_PATH));
    output_msg('..'.$sub_path.$save_url, 1);
    
    $sub_path = substr(HRL_SERVER_PATH, strlen(ROOT_PATH));
    output_msg('..'.$sub_path.$hrl_file, 1);
}

//WIN客户端资源CNF
function create_client_cnf($save_url, $temp_url, $assign_arr) {
    global $smarty;
    $file_url = UI_CLIENT_PATH.$save_url;

    $content = "";
    $smarty->register_function("dispose_cnf_type", "dispose_cnf_type");
    
    foreach($assign_arr as $assign_data)
    {
        foreach($assign_data as $assign => $value)
        {
            $smarty->assign($assign, $value);
        }
        $content .= $smarty->fetch($temp_url);
        $content .= "\n";
    }
    
    file_put_contents($file_url, $content);
    $sub_path = substr(UI_CLIENT_PATH, strlen(ROOT_PATH));
    output_msg('..'.$sub_path.$save_url, 1);
}

function output_msg($msg, $is_url = 0) {
    if ($is_url) {
        $sub_str = substr($msg, 3);
        $rand_arg = rand(1, 1000);
        $msg = "<a target='_blank' href='http://develop.xzfb.com/tools/$sub_str?rng=$rand_arg'>$msg</a><br />";
    }
    $msg .= '<script>window.scroll(0,document.body.scrollHeight);</script>';
    ob_flush();
    flush();
    echo str_pad($msg, 256);
}

function output_end() {
    if ($_GET['type'] != 'all') {
        output_msg("<br><font color='blue'><strong>生成完毕</strong></font><br><br><input type='button' value='点击返回' onclick='ReturnIndex()' />
        <script>function ReturnIndex() {location.href= 'index.php';}</script><br>");
    }
}

function lock_file($file_name){
    $file_name = iconv("UTF-8", "GBK", $file_name); 
    $lock_file = ROOT_DIR.'cache/'.$file_name.'.lock';
    $lock_time = CREATE_LOCK_TIME;//生成配置时文件锁定的有效时间（秒）
    if( file_exists($lock_file) && (time()- file_get_contents($lock_file)) < $lock_time ) {
        die('此文件正在使用中...');
    }

    $f = fopen($lock_file, w);
    fwrite($f, time());
    fclose($f);
}

function del_lock_file($file_name) {
    $lock_file = ROOT_DIR.'cache/'.$file_name.'.lock';
    if (file_exists($lock_file)) {
        unlink($lock_file);
    }
}

function output_error($file_name, $error_msg) {
    del_lock_file($file_name);
    die($error_msg);
}

/*
 * 判断数据信息
 * @param max $data 判断数据
 * @param int $type 限制数据类型 
 *     0 => 不能为空, 1 => 数字, 2 => 需要 "|", 3 => 需要 "#", 4 => 需要 "\n", 5 => 判断值数量
 */

function error_judge($data, $type, $lenght = 0){
    global $excel_list, $file_name, $sheet_value, $row, $column;
    switch($type){
        case 0 :
            if($data == ''){
                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据不能为空!";
                output_error($file_name, $error_msg);
            }
            break;
        case 1 :
            if (!is_numeric($data)) {
                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据子类型必须为数字!";
                output_error($file_name, $error_msg);
            }
            break;
        case 2 :
            if (strpos($data, "|") === 'FALSE') {
                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据格式有误!(缺少|分隔符)";
                output_error($file_name, $error_msg);
            }
            break;
        case 3 : 
            if (strpos($data, "#") === 'FALSE') {
                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据格式有误!(缺少#分隔符)";
                output_error($file_name, $error_msg);
            }
        break;
        case 4 :
            if (strpos($data, "\n") === 'FALSE') {
                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据格式有误!(缺少回车分隔符)";
                output_error($file_name, $error_msg);
            }
        break;
        case 5 :
            if (count($data) != $lenght) {
                $error_msg = "【出错】：EXCEL文件【{$file_name}.xml】工作表:{$sheet_value['name']}，第".$row."行第".$column."列数据所填元素个数不正确";
                output_error($file_name, $error_msg);
            }
        break;
            
        default:
            break;
    }
}
/*
 * 解析配置文件特殊处理
 * @param string $node_value 配置内容(XML解释获取)
 * @param int $type 特殊情况类型
 *     1 => 以 "|" 分割的一维数组(不定长)
 *     2 => 以"#", "|" 分割的二维数组(不定长)
 *     3 => 以"\n", "|" 分割的二维数组(不定长)
 *     4 => 以"\n", "#", "|" 分割的三维数组(不定长)
 *     5 => 以"\n" 分割的一维数组(不定长)
 * @param string $restrict 限制数据类型 
 *     0 => 空, 1 => 数字, 2 => 需要 "|", 3 => 需要 "#", 4 => 需要 "\n"
 * @param int $length 限制数据类型数量 
 * return array $res 处理后获得的数组
 */
function dispose_str_to_array($node_value, $type ,$restrict = "", $length = 0){
    $res = array();

    strstr($restrict ,'0') && $is_empty = 1;
    strstr($restrict ,'1') && $is_num = 1;
    strstr($restrict ,'2') && $is_vertical = 1;
    strstr($restrict ,'3') && $is_pound = 1;
    strstr($restrict ,'4') && $is_enter = 1;
    
    switch($type){
        case 1 : 
            (isset($is_vertical) && $is_vertical) && error_judge($node_value, 2);
            $unit_node_value = explode("|", $node_value);
            ($length > 0) && error_judge($unit_node_value, 5, $length);
            foreach ($unit_node_value AS $unit_key => $sub_node_value) {
                (isset($is_empty) && $is_empty) && error_judge($sub_node_value, 0);
                if($sub_node_value != ''){
                    (isset($is_num) && $is_num) && error_judge($sub_node_value, 1);
                    $res[] = $sub_node_value;
                }
            }
            break;
        case 2 :
            (isset($is_pound) && $is_pound) && error_judge($node_value, 3);
            $unit_node_value = explode("#", $node_value);
            foreach ($unit_node_value AS $unit_key => $unit_value) {
                (isset($is_vertical) && $is_vertical) && error_judge($node_value, 2);
                $sub_node_value = explode("|", $unit_value);
                ($length > 0) && error_judge($sub_node_value, 5, $length);
                $tmp = array();
                foreach($sub_node_value as $n_k => $n_val){
                    (isset($is_empty) && $is_empty) && error_judge($n_val, 0);
                    if ($n_val != '') {
                        (isset($is_num) && $is_num) && error_judge($n_val, 1);
                        $tmp[] = $n_val;
                    }
                }
                $res[] = $tmp;
            }
            break;
        case 3 :
            (isset($is_enter) && $is_enter) && error_judge($node_value, 4);
            $unit_node_value = explode("\n", $node_value);
            foreach ($unit_node_value AS $unit_key => $unit_value) {
                (isset($is_vertical) && $is_vertical) && error_judge($node_value, 2);
                $sub_node_value = explode("|", $unit_value);
                ($length > 0) && error_judge($sub_node_value, 5, $length);
                $tmp = array();
                foreach($sub_node_value as $n_k => $n_val){
                    (isset($is_empty) && $is_empty) && error_judge($n_val, 0);
                    if ($n_val != '') {
                        (isset($is_num) && $is_num) && error_judge($n_val, 1);
                        $tmp[] = $n_val;
                    }
                }
                $res[] = $tmp;
            }
            break;
        case 4 :
            (isset($is_enter) && $is_enter) && error_judge($node_value, 4);
            $tmp_node_value = explode("\n", $node_value);
            foreach($tmp_node_value as $key => $nval){
                (isset($is_pound) && $is_pound) && error_judge($node_value, 3);
                $unit_node_value = explode("#", $nval);
                foreach ($unit_node_value AS $unit_key => $unit_value) {
                    (isset($is_vertical) && $is_vertical) && error_judge($node_value, 2);
                    $sub_node_value = explode("|", $unit_value);
                    ($length > 0) && error_judge($sub_node_value, 5, $length);
                    $tmp = array();
                    foreach($sub_node_value as $n_k => $n_val){
                        (isset($is_empty) && $is_empty) && error_judge($n_val, 0);
                        if ($n_val != '') {
                            (isset($is_num) && $is_num) && error_judge($n_val, 1);
                            $tmp[] = $n_val;
                        }
                    }
                    $res[$key][] = $tmp;
                }
            }
            break;
        case 5 :
            (isset($is_enter) && $is_enter) && error_judge($node_value, 4);
            $unit_node_value = explode("\n", $node_value);
            ($length > 0) && error_judge($unit_node_value, 5, $length);
            foreach ($unit_node_value AS $unit_key => $sub_node_value) {
                (isset($is_empty) && $is_empty) && error_judge($sub_node_value, 0);
                if($sub_node_value != ''){
                    (isset($is_num) && $is_num) && error_judge($sub_node_value, 1);
                    $res[] = $sub_node_value;
                }
            }
            break;
        default:
            break;
    }

    return $res;
}

function curl_get_content($url) {
    $ch = curl_init();
    $timeout = 1000;
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
    curl_setopt($ch, CURLOPT_CONNECTTIMEOUT, $timeout);
    $file_contents = curl_exec($ch);
    curl_close($ch);
    return $file_contents;
}


?>
