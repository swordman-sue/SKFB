<?php
namespace Admin\Controller;
use Think\Controller; 
class ChatRecordController extends PublicController {
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
     * @IS_AJAX   加载模板
     * @$model    获取当前控制器
     *
     */
    public function index(){
        //分页
		
		// 投递进入
        if(IS_POST){
			$chat_type = I('post.chat_type');
			$role_name = I('post.role_name');
			$begin_time = I('post.begin_time');
			$end_time = I('post.end_time');
		// 刷新进入
        }else{
			$chat_type = I('post.chat_type');
			$role_name = I('post.role_name');
			$begin_time = I('post.begin_time');
			$end_time = I('post.end_time');
        }
		
        $page = $this->commonPage();
		
        $this->assign('page',$page);
		//$result = M('chat_record')->select();
		$result = M('chat_record')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
        $this->assign('result',$result);
				
        if(IS_AJAX){		
            $this->ajaxReturn(array(
                'result'=>$result,
                'page'=>$this->show,
            ));			
        }else{
            $this->display();
        }  
    }
}