<?php
namespace Admin\Controller;
use Think\Controller;
class ObjectController extends PublicController {
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
        if(empty($admin_name)){
            $this->error('请先登录!',U('Admin/Login/login'));
            exit();
        }
    }
    /**
     * @@列表
     * @page      获取分页
     * @limit     条数，每页条数
     * @IS_AJAX   加载模板
     * @$model    获取当前控制器
     * @is_add    判断（是否添加(1=是0=否)
     *
     */
    public function index(){
        //分页
        $page = $this->commonPage();
        $this->assign('page',$page);
        $result = M('object_record as a')->join('left join role as b on b.role_id = a.role_id')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
        foreach($result as $key=>$val){
            $result[$key]['is_add'] = $val['is_add'] ? '是' : '否';
        }
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
     * @@查询角色名字
     * @role_name    参数角色名字
     * @$where        条件
     * @is_add    判断（是否添加(1=是0=否)
     *
     */
    public function text(){
        $role_name = I('role_name');
        //模糊查询
        $where['role_name'] = array('like',"%$role_name%");
        if($role_name){
            $result = M('object_record as a')->join('left join role as b on b.role_id = a.role_id')->where($where)->select();
            foreach($result as $key=>$val){
                $result[$key]['is_add'] = $val['is_add'] ? '是' : '否';

            }
            $this->assign('result',$result);
            $this->display();
        }else{
            $result = M('object_record as a')->join('left join role as b on b.role_id = a.role_id')->where($where)->select();
            foreach($result as $key=>$val){
                $result[$key]['is_add'] = $val['is_add'] ? '是' : '否';

            }
            $this->assign('result',$result);
            $this->display();
        }
    }

}