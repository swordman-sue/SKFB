<?php
namespace Admin\Controller;
use Think\Controller;
class GuildController extends PublicController {
    /**
     * 构造函数
     * @session   判断是否有记录
     *
     */
    public function __construct(){
        parent::__construct();
		  $this->perPage=40;
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
     * @ini_set   加大内存
     * @$model    获取当前控制器
     *
     */
    public function index(){
        ini_set('memory_limit', '512M');
        //分页
        $page = $this->commonPage();
        $this->assign('page',$page);
       $result = M('guild_list as a')->join('left join server_list as b on b.server_id = a.server_id')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
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
     * @ini_set    加大内存
     * @guild_id   参数id
     * @$where     条件
     *
     */
    public function text(){
        ini_set('memory_limit', '512M');
        $guild_id = I('guild_id');
        //模糊查询
        $where['guild_id'] = array('like',"%$guild_id%");
        if($guild_id){
            $result = M('guild_list as a')->join('left join server_list as b on b.server_id = a.server_id')->where($where)->select();
            $this->assign('result',$result);
            $this->display();
        }else{
            $result = M('guild_list as a')->join('left join server_list as b on b.server_id = a.server_id')->where($where)->select();
            $this->assign('result',$result);
            $this->display();
        }
    }

}