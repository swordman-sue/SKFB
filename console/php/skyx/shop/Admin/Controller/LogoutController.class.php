<?php
namespace Admin\Controller;
use Think\Controller;
class LogoutController extends PublicController {
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
     * @page         获取分页
     * @limit        条数，每页条数
     * @online_time  秒(转换时间格式)
     * @IS_AJAX      加载模板
     * @$model        加载当前控制器
     *
     */
    public function index(){
        //分页
        $page = $this->commonPage();
        $this->assign('page',$page);
        $result = M('role_logout_record as a')->join('left join role as b on b.role_id = a.role_id')->limit($this->Page->firstRow.','.$this->Page->listRows)->order('idx desc')->select();
        foreach ($result as $key=>$val){
            $result[$key]['online_time'] = gmstrftime('%H小时-%M分-%S秒',$val['online_time']);
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
     * @@查询idx
     * @idx          参数idx
     * @$where       条件
     * @online_time 秒(转换时间格式)
     */
    public function text(){
        $idx = I('idx');
        //模糊查询
        $where['idx'] = array('like',"%$idx%");
        if($idx){
            $result = M('role_logout_record as a')->join('left join role as b on b.role_id = a.role_id')->where($where)->select();
            foreach ($result as $key=>$val){
                $result[$key]['online_time'] = gmstrftime('%H小时-%M分-%S秒',$val['online_time']);
            }
            $this->assign('result',$result);
            $this->display();
        }else{
            $result = M('role_logout_record as a')->join('left join role as b on b.role_id = a.role_id')->where($where)->select();
            foreach ($result as $key=>$val){
                $result[$key]['online_time'] = gmstrftime('%H小时-%M分-%S秒',$val['online_time']);
            }
            $this->assign('result',$result);
            $this->display();
        }
    }

    /**
     * @@角色名字
     * @role_name    参数角色名字
     * @$where        条件
     * @online_time 秒(转换时间格式)
     *
     */
    public function chaname(){
        $role_name = I('role_name');
        //模糊查询
        $where['role_name'] = array('like',"%$role_name%");
        if($role_name){
            $result = M('role_logout_record as a')->join('left join role as b on b.role_id = a.role_id')->where($where)->select();
            foreach ($result as $key=>$val){
                $result[$key]['online_time'] = gmstrftime('%H小时-%M分-%S秒',$val['online_time']);
            }
            $this->assign('result',$result);
            $this->display();
        }else{
            $result = M('role_logout_record as a')->join('left join role as b on b.role_id = a.role_id')->where($where)->select();
            foreach ($result as $key=>$val){
                $result[$key]['online_time'] = gmstrftime('%H小时-%M分-%S秒',$val['online_time']);
            }
            $this->assign('result',$result);
            $this->display();
        }
    }
    /**
     * @@钻石之间查询
     * @diamond1     参数1
     * @diamond2     参数2
     * @$where       条件
     * @online_time 秒(转换时间格式)
     *
     */
    public function cahzhuans(){
        $diamond1 = I('post.diamond1');
        $diamond2 = I('post.diamond2');
        $where['b.diamond'] = array('between',"$diamond1,$diamond2");
        $result = M('role_logout_record as a')->join('left join role as b on b.role_id = a.role_id')->where($where)->select();
        foreach ($result as $key=>$val){
            $result[$key]['online_time'] = gmstrftime('%H小时-%M分-%S秒',$val['online_time']);
        }
        $this->assign('result',$result);
        $this->display();
    }
    /**
     * @@等级之间查询
     * @level1   参数1
     * @level2   参数2
     * @$where    条件
     * @online_time 秒(转换时间格式)
     *
     */
    public function cahdengj(){
        $level1 = I('post.level1');
        $level2 = I('post.level2');
        $where['b.level'] = array('between',"$level1,$level2");
        $result = M('role_logout_record as a')->join('left join role as b on b.role_id = a.role_id')->where($where)->select();
        foreach ($result as $key=>$val){
            $result[$key]['online_time'] = gmstrftime('%H小时-%M分-%S秒',$val['online_time']);
        }
        $this->assign('result',$result);
        $this->display();
    }
}