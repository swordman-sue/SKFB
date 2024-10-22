<?php
namespace Admin\Controller;
use Think\Controller;
class ActivationCodeController extends PublicController {
    /**
     * 构造函数
     * @session   判断是否有记录
     *
     */
    public function __construct(){
        parent::__construct();
		  $this->perPage=20;
        //判断不能直接访问后台页面，必须先登录
        $admin_name=session('admin_name');
        if(empty($admin_name)){//判断如果session为空
            $this->error('请先登录!',U('Admin/Login/login'));
            exit();
        }
    }
	
	
	public function randomkeys($length)   
	{   
	   //$pattern = '1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLOMNOPQRSTUVWXYZ';  
	   $pattern = '1234567890ABCDEFGHIJKLOMNOPQRSTUVWXYZ';  
		for($i=0;$i<$length;$i++)   
		{   
			$key .= $pattern{mt_rand(0,35)};    //生成php随机数   
		}   
		return $key;   
	}   
	
    /**
     * @@列表
     * @page      获取分页
     * @limit     条数，每页条数
     * @open_time 时间戳转换
     * @IS_AJAX   加载模板
     * @$model    获取当前控制器
     *
     */
    public function index(){
        //分页
       if(IS_POST){
		   
            $code_num = I('post.code_numb');
            $reward_id = I('post.reward_id');
			$expire_time = I('post.expire_time');
			$type = I('post.type');
            //激活码数量
            if(empty($code_num)){
                echo "<script>alert('请填写激活码数量！');history.go(-1);</script>";exit;
            }
            //奖励ID
            if(empty($reward_id)){
                echo "<script>alert('请填写奖励ID！');history.go(-1);</script>";exit;
            }
			//有效期
		    if(empty($expire_time)){
                echo "<script>alert('请选择有限期！');history.go(-1);</script>";exit;
            }	
						
			// 限制10万
			$code_num = min($code_num, 100000);
			
			$result = M('activation_code_batch')->field("batch")->order('batch desc')->select();
			if($result===false){
				echo "<script>alert('查询批次数量出错');history.go(-1);</script>";exit;
			}
									
			$batch = $result[0]['batch'] + 1;
				
			// 添加激活码
			for ($i = 0; $i < $code_num; $i++) {
				$code = $this->randomkeys(8);
				$data_list[] = array('code'=>$code,'batch'=>$batch);	
			}	
			$result = M('activation_code_record')->addAll($data_list);
			if($result===false){
				$this->error('添加激活码失败', U('Admin/'.CONTROLLER_NAME.'/index'));
			}			
						
			$cur_time = date('Y-m-d H-i-s',time());
			
			$unix_time = strtotime($expire_time); // 时间转换
			
			// 添加批次数据
			$data = array(
				'batch'=>$batch,
				'reward_id'=>$reward_id,
				'create_time'=>$cur_time,
				'expire_time'=>$unix_time,
				'type'=>($type == 'personal' ? 0 : 1),
			);
			$result = M('activation_code_batch')->add($data);
			if($result===false){
				$this->error('添加激活码批次数据失败', U('Admin/'.CONTROLLER_NAME.'/add'));
			}			
			
			$this->success('新增成功', U('Admin/'.CONTROLLER_NAME.'/index'));

        }else{
				
			$result = M('activation_code_batch')->select();
			foreach ($result as $key=>$value){	
				$batch = $value['batch'];
					
				// 总的数量
				$total_num = M('activation_code_record')->where('batch='.$batch)->count();
				// 剩余数量
				$remain_num = M('activation_code_record')->where('status = 0 and batch='.$batch)->count();
				
				// 时间转换
				$result[$key]['expire_time'] = date('Y-m-d',$value['expire_time']);
				
				$result[$key]['type'] = $result[$key]['type'] == 0 ? "个人" : "通用";
				
				$result[$key]['total_num'] = $total_num;
				$result[$key]['remain_num'] = $remain_num;			
			}
			$this->assign('result',$result);		
            $this->display();
        }		
    }
	
    /**
     * 修改
     *
     */
    public function edit()
    {	
		// 投递进入
        if(IS_POST){
			
			echo '投递进入';
			
			$batch = I('post.batch');
			$expire_time = I('post.expire_time');
			$reward_id = I('post.reward_id');
			$create_num = I('post.create_num');
			$btn1 = I('post.btn1');
			$btn2 = I('post.btn2');
			$btn3 = I('post.btn3');	
			
			// 限制10万
			$create_num = min($create_num, 100000);
							
			
			// 激活码数量添加
			if(!empty($btn1)){
				//echo '激活码数量添加';
				// 添加激活码
				for ($i = 0; $i < $create_num; $i++) {
					$code = $this->randomkeys(8);
					$data_list[] = array('code'=>$code,'batch'=>$batch);	
				}	
				$result = M('activation_code_record')->addAll($data_list);
				if($result===false){
					$this->error('添加激活码失败', U('Admin/'.CONTROLLER_NAME.'/index'));
				}		
			}
			// 激活码奖励ID修改
			else if(!empty($btn2)){
				//echo '激活码奖励ID修改';		
				$result = M('activation_code_batch')->where('batch='.$batch)->setField('reward_id',$reward_id);
				if($result===false){
					$this->error('激活码有效期修改失败', U('Admin/'.CONTROLLER_NAME.'/edit'));
				}				
			}
			// 激活码有效期修改
			else if(!empty($btn3)){
				//echo '激活码有效期修改';			
				$unix_time = strtotime($expire_time); // 时间转换			
				//where('id',1)->update(['name' => 'thinkphp']);
				$result = M('activation_code_batch')->where('batch='.$batch)->setField('expire_time',$unix_time);
				if($result===false){
					$this->error('激活码有效期修改失败', U('Admin/'.CONTROLLER_NAME.'/edit'));
				}	
			}	

			$this->success('操作成功', U('Admin/'.CONTROLLER_NAME.'/edit?batch='.$batch));
			
		// 刷新进入
        }else{		
			$batch = $_GET['batch'];							
			$result = M('activation_code_batch')->where('batch='.$batch)->select();
			foreach ($result as $key=>$value){	
				$batch = $value['batch'];
					
				// 总的数量
				$total_num = M('activation_code_record')->where('batch='.$batch)->count();
				// 剩余数量
				$remain_num = M('activation_code_record')->where('status = 0 and batch='.$batch)->count();
				
				// 时间转换
				$result[$key]['expire_time'] = date('Y-m-d',$value['expire_time']);
					
				$result[$key]['total_num'] = $total_num;
				$result[$key]['remain_num'] = $remain_num;			
			}
			$this->assign('result',$result);
			$this->display();			
        }	
    }	
	
    /**
     * 删除
     *
     */
    public function del()
    {				
		// 投递进入
        if(IS_POST){
			$batch = I('post.batch');
		// 刷新进入
        }else{
			$batch = $_GET['batch'];
        }

		// 删除批次数据
		$result = M('activation_code_batch')->where('batch='.$batch)->delete();
		if($result===false){
			$this->error('删除激活码批次数据失败!', U('Admin/'.CONTROLLER_NAME.'/edit'));
		}	
		
		// 删除激活码
		$result = M('activation_code_record')->where('batch='.$batch)->delete();
		if($result===false){
			$this->error('删除激活码数据失败!', U('Admin/'.CONTROLLER_NAME.'/edit'));
		}	
		
		$this->success('删除成功', U('Admin/'.CONTROLLER_NAME.'/index'));
    }		
	
    /**
     * 导出文件
     *
     */
    public function export()
    {				
		// 当前目录: D:\xampp\htdocs\skyx\shop\Admin\Controller
	
		// 投递进入
        if(IS_POST){
			$batch = I('post.batch');
		// 刷新进入
        }else{
			$batch = $_GET['batch'];
        }
		
		$cur_time = date('Y-m-d H-i-s',time());
		$file_name = 'activation_code_'.$cur_time.'.txt';
		$dir_name = './temp_file/';
		
		// 如果您用 fopen() 打开并不存在的文件，此函数会创建文件，假定文件被打开为写入（w）或增加（a）
		$export_file = fopen($dir_name.$file_name,'w');
		
		fwrite($export_file,$txt);
		
		// 检索指定数据
		$result = M('activation_code_record')->field("code")->where('status = 0 and batch='.$batch)->select();
		foreach ($result as $key=>$val){
			
			$code = $result[$key]['code']."\r\n";
					
			// 给文件写入内容
			fwrite($export_file,$code);	
		}	
		// 关闭文件
		fclose($fp);

		/*
		$path = $dir_name.$file_name;
		$filename = "activation_code.zip";
		$zip = new ZipArchive();
		$zip->open($filename,ZipArchive::CREATE);   //打开压缩包
		$zip->addFile($path,basename($path));   //向压缩包中添加文件
		$zip->close();*/  
				
		$this->assign('file_name',$file_name);	
		$this->display();
    }		
	
	
    /**
     * @@查询服务区
     * @server_id   参数服务区id
     * @$where       条件
     * @open_time   时间戳转换
     *
     */
    public function text(){
        $server_id = I('server_id');
        //模糊查询
        $where['server_id'] = array('like',"%$server_id%");
        if($server_id){
            $result = M('server_list')->where($where)->select();
            foreach ($result as $key=>$val){
                $result[$key]['open_time'] = date('Y-m-d H:i:s',$val['open_time']);
            }
            $this->assign('result',$result);
            $this->display();
        }else{
            $result = M('server_list')->where($where)->select();
            foreach ($result as $key=>$val){
                $result[$key]['open_time'] = date('Y-m-d H:i:s',$val['open_time']);
            }
            $this->assign('result',$result);
            $this->display();
        }
    }
    /**
     * @@添加服务区
     * @IS_POST     获取poat
     * @server_id   参数服务区id
     * @open_time   参数时间
     * @name        参数名字
     * @$where       条件（是否有开过）
     * @$data        添加条件
     *
     */
    public function add(){
        if(IS_POST){
            $server_id = I('post.server_id');
            $open_time = I('post.open_time');
            $server_name = I('post.name');
            //服务器id
            if(empty($server_id)){
                echo "<script>alert('服务器id不能为空！');history.go(-1);</script>";exit;
            }
            //开服
            if(empty($open_time)){
                echo "<script>alert('开服时间不能为空！');history.go(-1);</script>";exit;
            }
            //开服名字
            if(empty($server_name)){
                echo "<script>alert('开服名字不能为空！');history.go(-1);</script>";exit;
            }
            //查询Id语句
            $select_id =array(
                'server_id'=>$server_id,
            );
			// 检测ID是否重复
            $result = M('server_list')->where($select_id)->select();
            if($result){
                $this->error('重复的服务器ID', U('Admin/'.CONTROLLER_NAME.'/add'));
            }
			
		    //查询名称语句
            $select_name =array(
                'name'=>$server_name,
            );
			// 检测名称是否重复
            $result = M('server_list')->where($select_name)->select();
            if($result){
                $this->error('重复的服务器名称', U('Admin/'.CONTROLLER_NAME.'/add'));
            }			
			
		   //查询时间语句
           // $select_time =array(
           //     'open_time'=>$open_time,
           // );			
		   // 检测时间是否重复
           // $result = M('server_list')->where($select_time)->select();
           // if($result){
           //     $this->error('重复的开服时间', U('Admin/'.CONTROLLER_NAME.'/add'));
           // }			
			
            //开服
			$unix_time = strtotime($open_time); // 时间转换
            $data = array(
                'server_id'=>$server_id,
                'open_time'=>$unix_time,
				'open_str_time'=>$open_time,
                'name'=>$server_name,
            );
            $server_list = M('server_list')->add($data);
			if($server_list===false){
                $this->error('新增失败', U('Admin/'.CONTROLLER_NAME.'/add'));
            }else{
                $this->success('新增成功', U('Admin/'.CONTROLLER_NAME.'/index'));
            }

        }else{
            $this->display();
        }
    }

}