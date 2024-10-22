<?php
namespace Admin\Controller;
use Think\Controller;
class FirstController extends PublicController {
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
     *
     */
    public function index(){
        ini_set('memory_limit', '512M');
        //分页
        $page = $this->commonPage();
        $this->assign('page',$page);
       $result = M('map_first_pass_record as a')->join('left join role as b on b.role_id = a.role_id')->join('left join php_recordconfig as c on c.id = a.map_type')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
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
     * @@角色名字查找
     * @role_name     参数角色名字查找
     * @$where         条件
     *
     */
    public function text(){
        ini_set('memory_limit', '512M');
        $role_name = I('role_name');
        //模糊查询
        $where['role_name'] = array('like',array("%$role_name%"));
        if($role_name){
            $result = M('map_first_pass_record as a')->join('left join role as b on b.role_id = a.role_id')->join('left join php_recordconfig as c on c.id = a.map_type')->where($where)->select();
            $this->assign('result',$result);
            $this->display();
        }else{
            $result = M('map_first_pass_record as a')->join('left join role as b on b.role_id = a.role_id')->join('left join php_recordconfig as c on c.id = a.map_type')->where($where)->select();
            $this->assign('result',$result);
            $this->display();
        }
    }
    /**
     * @@章节之间查询
     * @chapter1    参数1
     * @chapter2    参数2
     * @$map         条件
     *
     */
    public function cahzhangj(){
        $chapter1 = I('post.chapter1');
        $chapter2 = I('post.chapter2');
        $map['a.chapter'] = array('between',"$chapter1,$chapter2");
        $result = M('map_first_pass_record as a')->join('left join role as b on b.role_id = a.role_id')->join('left join php_recordconfig as c on c.id = a.map_type')->where($map)->select();
        $this->assign('result',$result);
        $this->display();
    }
    /**
     * @@等级之间查找
     * @level1   参数1
     * @level2   参数2
     * @$where    条件
     *
     */
    public function dengj(){
        $level1 = I('post.level1');
        $level2 = I('post.level2');
        $where['a.level'] = array('between',"$level1,$level2");
        $result = M('map_first_pass_record as a')->join('left join role as b on b.role_id = a.role_id')->join('left join php_recordconfig as c on c.id = a.map_type')->where($where)->select();
        $this->assign('result',$result);
        $this->display();
    }
}