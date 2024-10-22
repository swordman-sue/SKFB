<?php
namespace Admin\Controller;
use Think\Controller;
class RoleController extends PublicController {
    /**
     * 构造函数
     * @session   判断是否有记录
     *
     */
    public function __construct(){
        parent::__construct();
        //M函数 模型 查询某个数据表的数据
        $this->role=M('role');
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
     * @page                获取分页
     * @limit              条数，每页条数
     * @total_online_time 秒（转换时间格式）
     * @IS_AJAX            加载模板
     * @$model              获取当前控制器
     *
     */
    public function index(){
        //分页
        //$page = $this->commonPage();
        //$this->assign('page',$page);
        //$result= self::$model->field($this->field,'TABLE_COMMENT')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
        //$result = M('role')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
		$result = M('role')->limit(10)->select();
		//$result = M('role')->field("role_id,role_name,account,level,vip_level,vip_level,total_online_time,diamond,total_recharge_amount,
		//gold,create_time,last_update_time,fighting_power,hero_num,equipment_num,treasure_num,item_num,fb_chapter,fb_map,server_id")->limit(10)->select();
        foreach ($result as $key=>$val){
            $result[$key]['total_online_time'] = gmstrftime('%H小时%M分%S秒',$val['total_online_time']);
        }
        $this->assign('result',$result);
        //if(IS_AJAX){
        //   $this->ajaxReturn(array(
        //        'result'=>$result,
        //        'page'=>$this->show,
        //    ));
        //}else{
            $this->display();
        //}
        //$result=self::$model->select();
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
     * @@角色ID查询
     * @role_id            参数角色Id
     * @$where              条件
     * @total_online_time 秒（转换时间格式）
     *
     */
    public function chajiaox(){
        $role_id = I('role_id');
		
		if(empty($role_id)){
			echo "<script>alert('玩家ID为不能空！');history.go(-1);</script>";exit;
		}	
		//$role_id = max($role_id, 1);
       // $where['role_id'] = array('like',"%$role_id%");
        if($role_id){
            $result = M('role')->where('role_id='.$role_id)->limit(1)->select();
            foreach ($result as $key=>$val){
                $result[$key]['total_online_time'] = gmstrftime('%H小时%M分%S秒',$val['total_online_time']);
            }
            $this->assign('result',$result);
            $this->display();
        }else{
            $result = M('role')->where($where)->select();
            foreach ($result as $key=>$val){
                $result[$key]['total_online_time'] = gmstrftime('%H小时%M分%S秒',$val['total_online_time']);
            }
            $this->assign('result',$result);
            $this->display();
        }
    }
    /**
     * @@名字查询
     * @role_name            参数角色名字
     * @$where              条件
     * @total_online_time 秒（转换时间格式）
     *
     */
    public function chaname(){
       $role_name = I('role_name');
		if(empty($role_name)){
			echo "<script>alert('玩家名称为不能空！');history.go(-1);</script>";exit;
		}	
       //$where['role_name'] = array('like',"%$role_name%");
        if($role_name){
            //$result = M('role')->where($where)->select();
			$result = M('role')->where('role_name='.'"'.$role_name.'"')->limit(1)->select();
			
			// 检测是否查找到
			if(empty($result)){
				echo "<script>alert('查找的角色名不存在');history.go(-1);</script>";exit;
			}
			
            foreach ($result as $key=>$val){
                $result[$key]['total_online_time'] = gmstrftime('%H小时%M分%S秒',$val['total_online_time']);
            }
            $this->assign('result',$result);
            $this->display();
        }else{
            $result = M('role')->where($where)->select();
            foreach ($result as $key=>$val){
                $result[$key]['total_online_time'] = gmstrftime('%H小时%M分%S秒',$val['total_online_time']);
            }
            $this->assign('result',$result);
            $this->display();
        }
    }
    /**
     * @@战力之间查询
     * @fighting1            参数1
     * @fighting2           参数2
     * @$map                条件
     * @total_online_time 秒（转换时间格式）
     *
     */
    public function chazhangl(){
        $fighting1 = I('post.fighting1');
        $power = I('post.power');
        $map['fighting_power'] = array('between',"$fighting1,$power");
        $trademarkchax = M('role')->where($map)->select();
        foreach ($trademarkchax as $key=>$val){
            $trademarkchax[$key]['total_online_time'] = gmstrftime('%H小时%M分%S秒',$val['total_online_time']);
        }
        $this->assign('trademarkchax',$trademarkchax);
        $this->display();
    }
    /**
     * @@钻石之间查询
     * @diamond1            参数1
     * @diamond2           参数2
     * @$map                条件
     * @total_online_time 秒（转换时间格式）
     *
     */
    public function zhuans(){
        $diamond1 = I('post.diamond1');
        $diamond2 = I('post.diamond2');
        $map['diamond'] = array('between',"$diamond1,$diamond2");
        $trademarkchax = M('role')->where($map)->select();
        foreach ($trademarkchax as $key=>$val){
            $trademarkchax[$key]['total_online_time'] = gmstrftime('%H小时%M分%S秒',$val['total_online_time']);
        }
        $this->assign('trademarkchax',$trademarkchax);
        $this->display();
    }
    /**
     * 个人奖励
     *
     */
    public function Peraward(){
        if(IS_POST){
            $idx = I('post.idx');
            $cmd_id = I('post.cmd_id');
            $add_time = strtotime(I('post.add_time'));
            $param1 = I('post.param1');
            $param2 = I('post.param2');
            $param3 = I('post.param3');
            $param4 = I('post.param4');
            $param5 = I('post.param5');
            $text = I('post.text');
            //发放奖励时间
            if(empty($add_time)){
                echo "<script>alert('时间不能为空！');history.go(-1);</script>";exit;
            }
            //指定的玩家ID(0表示发放整个服务器玩家)判断
            if(empty($param1)){
                echo "<script>alert('玩家ID为不能空！');history.go(-1);</script>";exit;
            }
            //奖励ID
            if(empty($param5)){
                echo "<script>alert('奖励ID必选择一个！');history.go(-1);</script>";exit;
            }
            //奖励物品ID
            if(empty($param3)){
                echo "<script>alert('奖励物品ID必选择一个！');history.go(-1);</script>";exit;
            }
            //奖励物品数量
            if(empty($param4)){
                echo "<script>alert('奖励物品数量不能为空！');history.go(-1);</script>";exit;
            }
            //添加
            $dataList= array(
                'idx'=>$idx,
                'cmd_id'=>$cmd_id,
                'add_time'=>$add_time,
                'param1'=>$param1,
                'param2'=>$param2,
                'param3'=>$param3,
                'param4'=>$param4,
                'param5'=>$param5,
                'text'=>$text,
            );
            $gm_event = M('gm_event')->add($dataList);
            $gm_event = M('php_gm_event')->add($dataList);
            if($gm_event===false){
                $this->error('新增失败', U('Admin/'.CONTROLLER_NAME.'/index'));
            }else{
                $this->success('发放成功', U('Admin/'.CONTROLLER_NAME.'/index'));
            }

        }
    }
    /**
     * 个人封号操作
     * @$account   配置表2账号
     */
    public function Perban(){
        $is_lock = I('post.is_lock');
        $account = I('post.account');
        $where = array(
            'account'=>$account,
        );
        $data =array(
            'is_lock'=>$is_lock,
        );
        $account = M('account')->where($where)->save($data);
        if($account===false){
            $this->error('操作失败', U('Admin/'.CONTROLLER_NAME.'/index'));
        }else{
            $this->success('操作成功', U('Admin/'.CONTROLLER_NAME.'/index'));
        }

    }
    /**
     *个人禁言操作
     */
    public function Perbanned(){
        if(IS_POST){
            $cmd_id = I('post.cmd_id');
            $param1 = I('post.param1');
            $param2 = I('post.param2');
            $param3 = I('post.param3');
            //禁言时间
            if(empty($param2)){
                echo "<script>alert('禁言时间不能为空！');history.go(-1);</script>";exit;
            }
            //添加
            $data = array(
                'cmd_id'=>$cmd_id,
                'param1'=>$param1,
                'param2'=>$param2,
                'param3'=>$param3,
            );
            $gm_event = M('gm_event')->add($data);
            if($gm_event===false){
                $this->error('新增失败', U('Admin/'.CONTROLLER_NAME.'/index'));
            }else{
                $this->success('禁言成功', U('Admin/'.CONTROLLER_NAME.'/index'));
            }
        }
    }
    /**
     * 个人邮件操作
     */
    public function Permail(){
        if(IS_POST){
            $cmd_id = I('post.cmd_id');
            $param1 = I('post.param1');
            $param2 =I('post.param2');
            //邮件id
            if(empty($param2)){
                echo "<script>alert('邮件id不能为空！');history.go(-1);</script>";exit;
            }
            //添加
            $data = array(
                'cmd_id'=>$cmd_id,
                'param1'=>$param1,
                'param2'=>$param2,
            );
            $gm_event = M('gm_event')->add($data);
            if($gm_event===false){
                $this->error('邮件发送失败', U('Admin/'.CONTROLLER_NAME.'/index'));
            }else{
                $this->success('邮件发送成功', U('Admin/'.CONTROLLER_NAME.'/index'));
            }
        }
    }
}