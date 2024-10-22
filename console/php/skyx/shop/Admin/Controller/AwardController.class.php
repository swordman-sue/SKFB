<?php
namespace Admin\Controller;
use Think\Controller;
class AwardController extends PublicController {
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
        $class=session('class');
        if(empty($class)){//判断如果session为空
            $this->error('您不是超级管理员,没有权限操作!!!',U('Admin/index/index_right'));
        }
    }
    /**
     * 列表
     * @page        获取分页
     * @add_time   时间转换
     * @idx         排序
     *
     */
    public function index(){
         $result = M('php_gm_event as a')->join('left join role as b on b.role_id = a.param1')->join('left join server_list as c on c.server_id = a.param2')->order('idx desc')->limit(50)->select();
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
     * 玩家添加
     * @cmd_id    参数命令ID
     * @add_time  时间
     * @param1    参数角色Id
     * @param2    参数服务器id
     * @param3    参数励物品ID
     * @param4    参数物品数量
     * @param5    参数奖励ID
     * @tex       参数描述
     */
    public function player(){
        $server = M('server_list')->select();
        $this->assign('server',$server);
        $jianl = M('php_config')->order('typeid asc')->where('type=2')->select();
        $this->assign('jianl',$jianl);
        $wuping = M('php_config')->order('typeid asc')->where('type=3')->select();
        $this->assign('wuping',$wuping);
        if(IS_POST){
            $cmd_id = I('post.cmd_id');
            $param1 = I('post.param1');
            $param3 = I('post.param3');
            $param4 = I('post.param4');
            $param5 = I('post.param5');
            $text = I('post.text');
            //指定的玩家ID(0表示发放整个服务器玩家)判断
            if(empty($param1)){
                echo "<script>alert('玩家ID为空！');history.go(-1);</script>";exit;
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
                'cmd_id'=>$cmd_id,
                'param1'=>$param1,
                'param3'=>$param3,
                'param4'=>$param4,
                'param5'=>$param5,
                'text'=>$text,
            );
            $gm_event = M('gm_event')->add($dataList);
            $gm_event = M('php_gm_event')->add($dataList);
            if($gm_event===false){
                $this->error('新增失败', U('Admin/'.CONTROLLER_NAME.'/player'));
            }else{
                $this->success('发放成功', U('Admin/'.CONTROLLER_NAME.'/player'));
            }

        }else{
            $this->display();
        }
    }
    /**
     * 五条添加
     * @cmd_id    参数命令ID
     * @add_time  时间
     * @param1    参数角色Id
     * @param11   参数角色Id
     * @param12   参数角色Id
     * @param13   参数角色Id
     * @param14   参数角色Id
     * @param2    参数服务器id
     * @param3    参数励物品ID
     * @param4    参数物品数量
     * @param5    参数奖励ID
     * @tex       参数描述
     */
    public function duoadd(){
        $server = M('server_list')->select();
        $this->assign('server',$server);
        $jianl = M('php_config')->order('typeid asc')->where('type=2')->select();
        $this->assign('jianl',$jianl);
        $wuping = M('php_config')->order('typeid asc')->where('type=3')->select();
        $this->assign('wuping',$wuping);
        if(IS_POST){
            $cmd_id = I('post.cmd_id');
            $param1 = I('post.param1');
            $param11 = I('post.param11');
            $param12 = I('post.param12');
            $param13 = I('post.param13');
            $param14 = I('post.param14');
            $param3 = I('post.param3');
            $param4 = I('post.param4');
            $param5 = I('post.param5');
            $text = I('post.text');
            //指定的玩家ID(0表示发放整个服务器玩家)判断
            if(empty($param1)){
                echo "<script>alert('玩家ID不能为空！');history.go(-1);</script>";exit;
            }
            //指定的玩家ID(0表示发放整个服务器玩家)判断
            if(empty($param11)){
                echo "<script>alert('玩家ID不能为空！');history.go(-1);</script>";exit;
            }
            //指定的玩家ID(0表示发放整个服务器玩家)判断
            if(empty($param12)){
                echo "<script>alert('玩家ID不能为空！');history.go(-1);</script>";exit;
            }
            //指定的玩家ID(0表示发放整个服务器玩家)判断
            if(empty($param13)){
                echo "<script>alert('玩家ID不能为空！');history.go(-1);</script>";exit;
            }
            //指定的玩家ID(0表示发放整个服务器玩家)判断
            if(empty($param14)){
                echo "<script>alert('玩家ID不能为空！');history.go(-1);</script>";exit;
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
            $dataList[] = array(
                'cmd_id'=>$cmd_id,
                'param1'=>$param1,
                'param3'=>$param3,
                'param4'=>$param4,
                'param5'=>$param5,
                'text'=>$text,
            );
            $dataList[] = array(
                'cmd_id'=>$cmd_id,
                'param1'=>$param11,
                'param3'=>$param3,
                'param4'=>$param4,
                'param5'=>$param5,
                'text'=>$text,
            );
            $dataList[] = array(
                'cmd_id'=>$cmd_id,
                'param1'=>$param12,
                'param3'=>$param3,
                'param4'=>$param4,
                'param5'=>$param5,
                'text'=>$text,
            );
            $dataList[] = array(
                'cmd_id'=>$cmd_id,
                'param1'=>$param13,
                'param3'=>$param3,
                'param4'=>$param4,
                'param5'=>$param5,
                'text'=>$text,
            );
            $dataList[] = array(
                'cmd_id'=>$cmd_id,
                'param1'=>$param14,
                'param3'=>$param3,
                'param4'=>$param4,
                'param5'=>$param5,
                'text'=>$text,
            );
            $gm_event = M('gm_event')->addAll($dataList);
            $gm_event = M('php_gm_event')->addAll($dataList);
            if($gm_event===false){
                $this->error('新增失败', U('Admin/'.CONTROLLER_NAME.'/duoadd'));
            }else{
                $this->success('发放成功', U('Admin/'.CONTROLLER_NAME.'/duoadd'));
            }

        }else{
            $this->display();
        }
    }
    /**
     * 服务器添加
     * @cmd_id    参数命令ID
     * @add_time  时间
     * @param1    参数角色Id
     * @param2    参数服务器id
     * @param3    参数励物品ID
     * @param4    参数物品数量
     * @param5    参数奖励ID
     * @tex       参数描述
     */
    public function server(){
        $server = M('server_list')->select();
        $this->assign('server',$server);
        $jianl = M('php_config')->order('typeid asc')->where('type=2')->select();
        $this->assign('jianl',$jianl);
        $wuping = M('php_config')->order('typeid asc')->where('type=3')->select();
        $this->assign('wuping',$wuping);
        if(IS_POST){
            $cmd_id = I('post.cmd_id');
            $param2 = I('post.param2');
            $param3 = I('post.param3');
            $param4 = I('post.param4');
            $param5 = I('post.param5');
            $text = I('post.text');
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
                'cmd_id'=>$cmd_id,
                'param2'=>$param2,
                'param3'=>$param3,
                'param4'=>$param4,
                'param5'=>$param5,
                'text'=>$text,
            );
            $gm_event = M('gm_event')->add($dataList);
            $gm_event = M('php_gm_event')->add($dataList);
            if($gm_event===false){
                $this->error('新增失败', U('Admin/'.CONTROLLER_NAME.'/server'));
            }else{
                $this->success('发放成功', U('Admin/'.CONTROLLER_NAME.'/server'));
            }

        }else{
            $this->display();
        }
    }
}