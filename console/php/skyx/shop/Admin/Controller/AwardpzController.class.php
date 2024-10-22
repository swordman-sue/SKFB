<?php
namespace Admin\Controller;
use Think\Controller;
class AwardpzController extends PublicController {
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
    /**
     * @@列表
     * @page      获取分页
     * @limit     条数，每页条数
     * @IS_AJAX   加载模板
     *
     */
    public function index(){
//        //分页
//        $page = $this->commonPage();
//        $this->assign('page',$page);
        $result = M('php_config')->where('type=2')->order('typeid asc')->select();
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
     * @server_id   参数服务器id
     * @$where       条件
     * @open_time   时间转换
     *
     */
    public function text(){
        $server_id = I('server_id');
        //模糊查询
        $where['server_id'] = array('like',"%$server_id%");
        if($server_id){
            $result = M('server_list')->where($where)->select();
            foreach ($result as $key=>$val){
                $result[$key]['open_time'] = date('Y-m-d H:i:s',$val['open_time']);
            }
            $this->assign('result',$result);
            $this->display();
        }else{
            $result = M('server_list')->where($where)->select();
            foreach ($result as $key=>$val){
                $result[$key]['open_time'] = date('Y-m-d H:i:s',$val['open_time']);
            }
            $this->assign('result',$result);
            $this->display();
        }
    }
    /**
     * @@添加
     * @IS_POST  获取post
     * @$model   获取当前控制器
     *
     */
    public function add(){
        if(IS_POST){
            self::$model->create();
            $result=self::$model->add();
            if($result===false){
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
     * @@编辑
     * @IS_POST   获取post
     * @$model   获取当前控制器
     * @$no      获取id
     *
     */
    public function edit(){
        if(IS_POST){
            //更新数据
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