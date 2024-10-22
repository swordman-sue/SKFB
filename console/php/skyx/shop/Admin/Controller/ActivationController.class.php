<?php
namespace Admin\Controller;
use Think\Controller;
class ActivationController extends PublicController {
    /**
     * 构造函数
     * @session   判断是否有记录
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
     * 列表
     * @page   获取分页
     * @limit  条数，每页条数
     * @join   链表
     *
     */
    public function index(){
        ini_set('memory_limit', '512M');
        //分页
        $page = $this->commonPage();
        $this->assign('page',$page);
       $result = M('activation_code_record as a')->join('left join role as b on b.role_id = a.role_id')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
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

}