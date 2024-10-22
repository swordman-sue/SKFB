<?php
namespace Admin\Controller;
use Think\Controller;
class GmController extends PublicController {
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
     *
     */
    public function index(){
        ini_set('memory_limit', '512M');
        //分页
        $page = $this->commonPage();
        $this->assign('page',$page);
        $result = M('currency_logs as a')->join('left join currencypez as b on b.id = a.type')->join('left join currencydes as c on c.id = a.from_id')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
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
     * @@添加
     * @$class    session判断是否有权限
     * @$server   服务器列表
     * @IS_POST   获取post
     * @$model    获取当前控制器
     *
     */
    public function add(){
        $class=session('class');
        if(empty($class)){//判断如果session为空
            $this->error('您不是超级管理员,没有权限操作!!!',U('Admin/index/index_right'));
        }
        if(IS_POST){
            $cmd_id = I('post.cmd_id');
            $data = array(
                'cmd_id'=>$cmd_id,
            );
            $result = M('gm_event')->add($data);
            if($result===false){
                $this->error('操作失败', U('Admin/'.CONTROLLER_NAME.'/add'));
            }else{
                $this->success('操作成功', U('Admin/'.CONTROLLER_NAME.'/add'));
            }

        }else{
            $this->display();
        }
    }

}