<?php
namespace Admin\Controller;
use Think\Controller;
class AdminuserController extends PublicController{
    /**
     * 构造函数
     * @session   判断是否有记录
     */
    public function __construct(){
        parent::__construct();
        $this->perPage=8;
        //判断不能直接访问后台页面，必须先登录
        $admin_name=session('admin_name');
        if(empty($admin_name)){//判断如果session为空
            $this->error('请先登录!',U('Admin/Login/login'));
            exit();
        }
    }
    /**
     * 列表
     * @page        获取分页
     * @add_time   时间转换
     *
     */
    public function index(){
        //分页
        $page = $this->commonPage();
        $this->assign('page',$page);
        $result= self::$model->field($this->field,'TABLE_COMMENT')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
        $this->assign('result',$result);
        if(IS_AJAX){
            $this->ajaxReturn(array(
                'result'=>$result,
                'page'=>$this->show,
            ));
        }else{
            $this->display();
        }
        $result=self::$model->select();
    }

    /**
     * @@添加管理员
     * @session         判断是否有权限
     * @admin_id        参数管理员id
     * @admin_name      参数管理员账号
     * @admin_password  参数管理员密码
     * @admin_nickname  参数管理员昵称
     * @class            参数权限
     * @$where            判断账号
     * @$data             添加
     *
     */
    public function add(){
        $class=session('class');
        if(empty($class)){//判断如果session为空
            $this->error('您不是超级管理员,没有权限操作!!!',U('Admin/index/index_right'));
        }
        if(IS_POST){
            $admin_id = I('post.admin_id');
            $admin_name = I('post.admin_name');
            $admin_password = md5(I('post.admin_password'));
            $admin_nickname = I('post.admin_nickname');
            $class = I('post.class');
            //已存在管理员
            $where =array(
                'admin_name'=>$admin_name,
            );
            $adminuser = M('php_adminuser')->where($where)->select();
            if($adminuser){
                $this->error('管理员账号已存在', U('Admin/'.CONTROLLER_NAME.'/add'));
            }
            //管理员添加
            $data =array(
                'admin_id'=>$admin_id,
                'admin_name'=>$admin_name,
                'admin_password'=>$admin_password,
                'admin_nickname'=>$admin_nickname,
                'class'=>$class,
            );
            $adminuser = M('php_adminuser')->add($data);
            if($adminuser===false){
                $this->error('新增失败', U('Admin/'.CONTROLLER_NAME.'/add'));
            }else{
                $this->assign('waitSecond',5);
                $this->success('新增成功', U('Admin/'.CONTROLLER_NAME.'/index'));
            }

        }else{
            $this->display();
        }
    }

    /**
     * @@更改
     * @admin_password   加密
     * @$no               获取id
     *
     */
    public function edit(){
        if(IS_POST){
            //更新数据
            $_POST['admin_password']=md5(I('admin_password'));
            self::$model->create();
            $no=self::$model->getPk();
            $result=self::$model->where(self::$model->getPk().'='.$_POST[$no])->save();
            if($result===false){
                $this->error('编辑失败', U('Admin/'.CONTROLLER_NAME.'/edit'));
            }else{
                $this->assign('waitSecond',5);
                $this->success('编辑成功', U('Admin/'.CONTROLLER_NAME.'/edit','id='.$_POST['id']));
            }
        }else{
            //查询原本数据
            $result=self::$model->where(self::$model->getPk().'='.$_GET['id'])->find();
            $this->assign('result',$result);
            $this->display(CONTROLLER_NAME.':'.ACTION_NAME);
        }
    }
}