<?php
namespace Admin\Controller;
use Think\Controller;
class ServerController extends PublicController {
    /**
     * 构造函数
     * @session   判断是否有记录
     *
     */
    public function __construct(){
        parent::__construct();
		  $this->perPage=10;
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
     * @open_time 时间戳转换
     * @IS_AJAX   加载模板
     * @$model    获取当前控制器
     *
     */
    public function index(){
        //分页
        $page = $this->commonPage();
        $this->assign('page',$page);
        $result = M('server_list')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
        foreach ($result as $key=>$value){
            $result[$key]['open_time'] = date('Y-m-d H-i-s',$value['open_time']);
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
     * 个人操作
     *
     */
    public function edit()
    {
        $server = M('server_list')->select();
        $this->assign('server',$server);
        $jianl = M('php_config')->order('typeid asc')->where('type=2')->select();
        $this->assign('jianl',$jianl);
        $wuping = M('php_config')->order('typeid asc')->where('type=3')->select();
        $this->assign('wuping',$wuping);
        if(IS_POST){
            //更新数据
            self::$model->create();
            $no=self::$model->getPk();
            $result=self::$model->where(self::$model->getPk().'='.$_POST[$no])->save();
            if($result===false){
                $this->error('编辑失败', U('Admin/'.CONTROLLER_NAME.'/edit'));
            }else{
                $this->assign('waitSecond',5);
                $this->success('编辑成功', U('Admin/'.CONTROLLER_NAME.'/edit','id='.$_POST['role_id']));
            }
        }else{
            //查询原本数据
            $result=self::$model->where(self::$model->getPk().'='.$_GET['role_id'])->find();
            $dan = M('role as a')->join('left join account as b on b.account = a.account')->where(self::$model->getPk().'='.$_GET['role_id'])->find();
            $this->assign('dan',$dan);
            $this->assign('result',$result);
            $this->display(CONTROLLER_NAME.':'.ACTION_NAME);
        }
    }	
	
    /**
     * @@查询服务区
     * @server_id   参数服务区id
     * @$where       条件
     * @open_time   时间戳转换
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
     * @@添加服务区
     * @IS_POST     获取poat
     * @server_id   参数服务区id
     * @open_time   参数时间
     * @name        参数名字
     * @$where       条件（是否有开过）
     * @$data        添加条件
     *
     */
    public function add(){
        if(IS_POST){
            $server_id = I('post.server_id');
            $open_time = I('post.open_time');
            $server_name = I('post.name');
            //服务器id
            if(empty($server_id)){
                echo "<script>alert('服务器id不能为空！');history.go(-1);</script>";exit;
            }
            //开服
            if(empty($open_time)){
                echo "<script>alert('开服时间不能为空！');history.go(-1);</script>";exit;
            }
            //开服名字
            if(empty($server_name)){
                echo "<script>alert('开服名字不能为空！');history.go(-1);</script>";exit;
            }
            //查询Id语句
            $select_id =array(
                'server_id'=>$server_id,
            );
			// 检测ID是否重复
            $result = M('server_list')->where($select_id)->select();
            if($result){
                $this->error('重复的服务器ID', U('Admin/'.CONTROLLER_NAME.'/add'));
            }
			
		    //查询名称语句
            $select_name =array(
                'name'=>$server_name,
            );
			// 检测名称是否重复
            $result = M('server_list')->where($select_name)->select();
            if($result){
                $this->error('重复的服务器名称', U('Admin/'.CONTROLLER_NAME.'/add'));
            }			
			
		   //查询时间语句
           // $select_time =array(
           //     'open_time'=>$open_time,
           // );			
		   // 检测时间是否重复
           // $result = M('server_list')->where($select_time)->select();
           // if($result){
           //     $this->error('重复的开服时间', U('Admin/'.CONTROLLER_NAME.'/add'));
           // }			
			
            //开服
			$unix_time = strtotime($open_time); // 时间转换
            $data = array(
                'server_id'=>$server_id,
                'open_time'=>$unix_time,
				'open_str_time'=>$open_time,
                'name'=>$server_name,
            );
            $server_list = M('server_list')->add($data);
			if($server_list===false){
                $this->error('新增失败', U('Admin/'.CONTROLLER_NAME.'/add'));
            }else{
                $this->success('新增成功', U('Admin/'.CONTROLLER_NAME.'/index'));
            }

        }else{
			
			$result = M('server_list')->field("server_id")->order('server_id desc')->select();
			if($result===false){
				echo "<script>alert('查询服务器ID出错');history.go(-1);</script>";exit;
			}
									
			$server_id = $result[0]['server_id'] + 1;	

			$this->assign('server_id',$server_id);	
			
			//$server_list = M('server_list')->add($data);
			
            $this->display();
        }
    }

}