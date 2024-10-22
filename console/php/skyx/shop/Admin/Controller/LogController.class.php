<?php
namespace Admin\Controller;
use Think\Controller;
class LogController extends PublicController {
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
     * @@列表
     * @page      获取分页
     * @limit     条数，每页条数
     * @IS_AJAX   加载模板
     * @ini_set   加大内存
     * @$model    获取当前控制器
     *
     */
    public function index(){
        ini_set('memory_limit', '512M');
        //分页
        $page = $this->commonPage();
        $this->assign('page',$page);
       $result = M('quick_sdk_recharge_log as a')->join('left join role as b on b.role_id = a.role_id')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
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
     * @@查询
     * @role_name  参数角色名字
     * @$where      条件
     *
     */
    public function text(){
        $role_name = I('role_name');
        //模糊查询
        $where['role_name'] = array('like',"%$role_name%");
        if($role_name){
            $result = M('quick_sdk_recharge_log as a')->join('left join role as b on b.role_id = a.role_id')->where($where)->select();
            $this->assign('result',$result);
            $this->display();
        }else{
            $result = M('quick_sdk_recharge_log as a')->join('left join role as b on b.role_id = a.role_id')->where($where)->select();
            $this->assign('result',$result);
            $this->display();
        }
    }

}