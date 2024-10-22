<?php
namespace Admin\Controller;
use Think\Controller;
class ServerInfoController extends PublicController {
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
		
		$today_start_time =  date('Y-m-d 00:00:00',time());
				
        //分页
        $page = $this->commonPage();
		
        $this->assign('page',$page);
        $result = M('server_list')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
		$total_role_num = 0;
		$total_payment_amount = 0;
		$total_payment_num = 0;
        foreach ($result as $key=>$value){
            $result[$key]['open_time'] = date('Y-m-d H-i-s',$value['open_time']);
			
			// 总角色人数
			//$total_role_num += $result[$key]['role_num'];
			
			// 总付费额度			
			//$total_payment_amount += $result[$key]['amount'];
			
			// 总付费人数			
			//$total_payment_num += $result[$key]['recharge_role_num'];
        }
        $this->assign('result',$result);
		//$today_create_role_num = M('role')->where('create_time >='.'"'.$today_start_time.'"')->count();
		//$today_payment_amount = M('recharge_record')->where('record_time >='.'"'.$today_start_time.'"')->sum('amount');
		//$today_active_num = M('role')->where('last_update_time >='.'"'.$today_start_time.'"')->count();
		
		$total_role_num = M('server_list')->sum('role_num');
		$total_payment_amount = M('server_list')->sum('amount');
		$total_payment_num = M('server_list')->sum('recharge_role_num');

		// 总数据统计
		$this->assign('role_total_num',$total_role_num);
		$this->assign('total_payment_amount',$total_payment_amount);
		$this->assign('total_payment_num',$total_payment_num);

		// 今日数据统计
		$this->assign('today_create_role_num',$today_create_role_num);
		$this->assign('today_payment_amount',$today_payment_amount);	
		$this->assign('today_active_num',$today_active_num);		
        if(IS_AJAX){
            $this->ajaxReturn(array(
                'result'=>$result,
                'page'=>$this->show,
            ));
        }else{			
            $this->display();
        }
        //$result=self::$model->select();
    }
	
    /**
     * 个人操作
     *
     */
    public function edit()
    {	
		// $_GET['role_id']
        //分页 ->where('server_id='.$_POST['server_id']) $result = M('statistics_server_detail')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
        $page = $this->commonPage();
        $this->assign('page',$page);
		//echo 'server_id='.$_GET['server_id'];
        $result = M('statistics_server_detail')->where('server_id='.$_GET['server_id'])->select();
	    foreach ($result as $key=>$value){
            $result[$key]['payment_rate'] = round(100*$value['recharge_role_num'] / $value['active_num'],2); 
			$result[$key]['ARPU'] = round($value['amount'] / $value['active_num'],2); 
			$result[$key]['ARPPU'] = round($value['amount'] / $value['recharge_role_num'],2); 
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
     * 日志
     *
     */
    public function dayreport()
    {
        $page = $this->commonPage();
        $this->assign('page',$page);
        $result = M('server_day_report')->select();
	    foreach ($result as $key=>$value){
            $result[$key]['payment_rate'] = round(100*$value['payment_role_num'] / $value['active_role_num'],2); 
			$result[$key]['ARPU'] = round($value['payment_amount'] / $value['active_role_num'],2); 
			$result[$key]['ARPPU'] = round($value['payment_amount'] / $value['payment_role_num'],2); 
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