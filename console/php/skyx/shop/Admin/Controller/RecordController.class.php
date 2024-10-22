<?php
namespace Admin\Controller;
use Think\Controller;
class RecordController extends PublicController {
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
     *
     */
    public function index(){
        ini_set('memory_limit', '512M');
        //分页
        $page = $this->commonPage();
        $this->assign('page',$page);
        $result = M('map_lose_record as a')->join('left join php_recordconfig as b on b.id = a.map_type')->join('left join role as c on c.role_id = a.role_id')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
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
     *
     */
    public function text(){
        $role_name = I('role_name');
        //模糊查询
        $where['role_name'] = array('like',array("%$role_name%"));
        if($role_name){
            $result = M('map_lose_record as a')->join('left join role as b on b.role_id = a.role_id')->join('left join php_recordconfig as c on c.id = a.map_type')->where($where)->select();
            $this->assign('result',$result);
            $this->display();
        }else{
            $result = M('map_lose_record as a')->join('left join role as b on b.role_id = a.role_id')->join('left join php_recordconfig as c on c.id = a.map_type')->where($where)->select();
            $this->assign('result',$result);
            $this->display();
        }
    }
    /**
     * @@章节章节之间查找
     * @chapter1  参数1
     * @chapter2  参数2
     * @$map       条件
     *
     */
    public function chazhangj(){
        $chapter1 = I('post.chapter1');
        $chapter2 = I('post.chapter2');
        $map['a.chapter'] = array('between',"$chapter1,$chapter2");
        $result = M('map_lose_record as a')->join('left join role as b on b.role_id = a.role_id')->join('left join php_recordconfig as c on c.id = a.map_type')->where($map)->select();
        $this->assign('result',$result);
        $this->display();
    }
    /**
     * @@战力查找之间查找
     * @zhanl1  参数1
     * @zhanl2  参数2
     * @$map     条件
     *
     */
    public function cahzhangli(){
        $zhanl1 = I('post.zhanl1');
        $zhanl2 = I('post.zhanl2');
        $map['a.fighting_power'] = array('between',"$zhanl1,$zhanl2");
        $result = M('map_lose_record as a')->join('left join role as b on b.role_id = a.role_id')->join('left join php_recordconfig as c on c.id = a.map_type')->where($map)->select();
        $this->assign('result',$result);
        $this->display();
    }

}