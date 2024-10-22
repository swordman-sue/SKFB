<?php
namespace Admin\Controller;
use Think\Controller;
class MailExController extends PublicController {
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
    /**
     * @@添加
     * @$class   判断session
     * @IS_POST  获取post
     * @$model   当前控制器
     *
     */
    public function add(){
        $class=session('class');
        if(empty($class)){
            $this->error('您不是超级管理员,没有权限操作!!!',U('Admin/index/index_right'));
        }
        $config = M('php_config')->where('type=4')->select();
        $this->assign('config',$config);
        if(IS_POST){
            //$_POST['cmd_id'] = $_POST['cmd_id'];
			
			$cmd_id = $_POST['cmd_id'];
			$target_id = $_POST['param1'];
			$type = $_POST['param3'];
			$text = $_POST['text']; 
			$title = $_POST['title'];			
			
            //判断是否为空
            if(empty($_POST['param1']) && $type != 2){
                echo "<script>alert('目标Id不能为空！');history.go(-1);</script>";exit;
            }
			

			
			$gm_event_sql = M('gm_event');	
			$data = array('cmd_id' =>$cmd_id, 'param1' =>$target_id, 'param3' =>$type,'text' =>$text, 'text1' =>$title);
			$result=$gm_event_sql->data($data)->add();				
			
            //self::$model->create();
            //$result=self::$model->add();	
			
            if($result===false){
                $this->error('发送失败', U('Admin/'.CONTROLLER_NAME.'/add'));
            }else{
                $this->success('发送成功', U('Admin/'.CONTROLLER_NAME.'/add'));
            }

        }else{
            $this->display();
        }
    }

}