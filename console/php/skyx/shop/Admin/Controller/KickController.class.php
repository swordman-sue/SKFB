<?php
namespace Admin\Controller;
use Think\Controller;
class KickController extends PublicController {
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
        $config = M('php_config')->where('type=6')->select();
        $this->assign('config',$config);
        $class=session('class');
        if(empty($class)){
            $this->error('您不是超级管理员,没有权限操作!!!',U('Admin/index/index_right'));
        }
        if(IS_POST){
            $_POST['cmd_id'] = $_POST['cmd_id'];
            //判断是否为空
            $where =array(
                'param1'=>$param1,
            );
            $gm_event = M('gm_event')->where($where)->select();
            if(empty($_POST['param2'])){
                echo "<script>alert('踢人必须选一个，不能为空！');history.go(-1);</script>";exit;
            }
            self::$model->create();
            $result=self::$model->add();
            if($result===false){
                $this->error('踢人个人下线失败', U('Admin/'.CONTROLLER_NAME.'/add'));
            }else{
                $this->success('踢人个人下线成功', U('Admin/'.CONTROLLER_NAME.'/add'));
            }

        }else{
            $this->display();
        }
    }
    public function cha(){
        echo 11;exit;
    }
}