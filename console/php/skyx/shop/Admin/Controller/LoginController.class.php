<?php
namespace Admin\Controller;
use Think\Controller;
class LoginController extends Controller {
	public function __construct(){
		parent::__construct();//继承父类的构造函数
        $this->adminuser=M('php_adminuser');
    }

    /**
     * @@登录
     * @$_POST    获取post
     * @$verify   验证码
     * @$data     获取用户信息
     * @@session  保持到session
     *
     */
    public function login(){
    	if($_POST){
    		
			//验证码判断
    		//$verify = new \Think\Verify();
    		//if(!$verify->check(I('post.verify'))){//用接收到的验证码在check方法里检测
    		//	$this->error('验证码输入有误!');
    		//	//exit();
    		//}
			
    		//验证用户名和密码
    		$data=array(
    			"admin_name"=>I("post.admin_name"),
    			"admin_password"=>md5(I("post.admin_password"))
    		);
    		$res=$this->adminuser->where($data)->find();
    		//判断并跳转
    		if($res){
    			//设置session
    			session("admin_id",$res['admin_id']);//第一个参数是session名，第二个参数是session值
    			session("admin_name",$res['admin_name']);
                session("admin_password",$res['admin_password']);
				session("adminnickname",$res['adminnickname']);
				session("class",$res['class']);
    			$this->success('登录成功!',U('Admin/Index/index'));
				exit();
    		}else{
    			$this->error('用户名或密码错误,请重新输入!');
				exit();
    		}
    	}
		$this->display();//指定视图
    }
    /**
     * @@验证码
     * @
     */
    public function verify(){
    	$Verify = new \Think\Verify();//实例化验证码类
    	$Verify->fontSize=70;//验证码字体大小
    	$Verify->useCurve=false;//不使用混淆曲线
    	$Verify->length=4;//验证码的位数
        ob_clean ();
    	$Verify->entry();//生成验证码
    }
    /**
     * @@退出
     * @session  清楚session
     *
     */
    public function login_out(){
    	session(null);//清除session
    	$this->success('退出成功!',U('Admin/Login/login'));
    	exit();
    }
}