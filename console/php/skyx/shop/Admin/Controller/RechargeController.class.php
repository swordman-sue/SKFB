<?php
namespace Admin\Controller;
use Think\Controller;
class RechargeController extends PublicController {
		
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
     * @$model    获取当前控制器
     *
     */
    public function index(){	
	
		$recharge_name[1]="月卡";
		$recharge_name[2]="普通充值";
		$recharge_name[3]="超极折扣";
		$recharge_name[4]="直购特惠";
		$recharge_name[5]="今日推荐";
		$recharge_name[6]="精准推荐";
		$recharge_name[7]="热卖商品";	
		$recharge_name[8]="新超值折扣";	

        ini_set('memory_limit', '512M');
        //分页
        $page = $this->commonPage();
        $this->assign('page',$page);
        $result = M('recharge_record')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();		
		
       // $page = $this->commonPage();
        //$this->assign('page',$page); // $this->Page->firstRow.','.$this->Page->listRows
        //$result = M('recharge_record as a')->join('left join server_list as b on b.server_id = a.server_id')->
		//join('left join role as c on c.role_id = a.role_id')->limit(10)->select();
		//where('a.role_id='.$role_id)->select();
	    //$result = M('recharge_record')->limit(1000)->select();	
        	
		foreach ($result as $key=>$val){
			$result[$key]['recharge_type'] = $recharge_name[$val['recharge_type']];
			$role_list = M('role')->where('role_id='.$val['role_id'])->limit(1)->select();
			$result[$key]['role_name'] = $role_list[0]['role_name'];
		}
		
		$this->assign('result',$result);
		
        //if(IS_AJAX){
        //    $this->ajaxReturn(array(
        //        'result'=>$result,
        //        'page'=>$this->show,
        //    ));
        //}else{
            $this->display();
        //}
        //$result=self::$model->select();
    }
    /**
     * @@查询角色ID
     * @role_name    参数角色ID
     * @$where        条件
     *
     */
    public function text(){
        $role_id = I('role_id');
		
		if(empty($role_id)){
			echo "<script>alert('玩家ID为不能空！');history.go(-1);</script>";exit;
		}		
		
		$recharge_name[1]="月卡";
		$recharge_name[2]="普通充值";
		$recharge_name[3]="超极折扣";
		$recharge_name[4]="直购特惠";
		$recharge_name[5]="今日推荐";
		$recharge_name[6]="精准推荐";
		$recharge_name[7]="热卖商品";	
		$recharge_name[8]="新超值折扣";	
		
        //模糊查询
        //$where['role_name'] = array('like',"%$role_name%");
        if($role_id){
			$result = M('recharge_record')->where('role_id='.$role_id)->limit(200)->select();
			$role_name = null;
            //$result = M('recharge_record as a')->join('left join server_list as b on b.server_id = a.server_id')->
			//join('left join role as c on c.role_id = a.role_id')->where('a.role_id='.$role_id)->select();
			foreach ($result as $key=>$val){
				$result[$key]['recharge_type'] = $recharge_name[$val['recharge_type']];
				
				// if name is null
				if($role_name == null){
					$role_list = M('role')->where('role_id='.$val['role_id'])->limit(1)->select();
					$role_name = $role_list[0]['role_name'];
					//$result[$key]['role_name'] = $role_list[0]['role_name'];						
				}
				$result[$key]['role_name'] = $role_name;
			}
			
            $this->assign('result',$result);
            $this->display();
        }else{
            $result = M('recharge_record as a')->join('left join server_list as b on b.server_id = a.server_id')->join('left join role as c on c.role_id = a.role_id')->where($where)->select();
            $this->assign('result',$result);
            $this->display();
        }
    }
}