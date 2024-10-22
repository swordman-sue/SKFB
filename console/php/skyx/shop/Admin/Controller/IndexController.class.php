<?php
namespace Admin\Controller;
use Think\Controller;
class IndexController extends Controller {
    /**
     * 构造函数
     * @session   判断是否有记录
     *
     */
    public function __construct(){
        parent::__construct();
        //判断不能直接访问后台页面，必须先登录
        $admin_name=session('admin_name');
        if(empty($admin_name)){//判断如果session为空
            $this->error('请先登录!',U('Admin/Login/login'));
            exit();
        }
    }
    public function index(){
        $this->display(); // 输出模板
    }
}