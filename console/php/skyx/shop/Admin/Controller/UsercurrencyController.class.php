<?php
namespace Admin\Controller;
use Think\Controller;
class UsercurrencyController extends PublicController {
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
    //分页
    public function commonPage(){
        //获取当天开始结束时间
        $start=date("Y-m-d H:i:s",mktime(0,0,0,date('m'),date('d'),date('Y')));
        $end=date("Y-m-d H:i:s",mktime(0,0,0,date('m'),date('d')+1,date('Y'))-1);
        $where['time']=array('between',"$start,$end");
        //$User        = M('User'); // 实例化User对象
        $count       = self::$model->where($where)->count();// 查询满足要求的总记录数
       // var_dump($count);exit;
        $this->Page = new \Think\Page($count,$this->perPage);// 实例化分页类 传入总记录数和每页显示的记录数(25)
        $this->Page->plinkHover='a';
        $this->Page->setConfig('prev','上一页');
        $this->Page->setConfig('next','下一页');
        $this->Page->setConfig('first','首页');
        $this->Page->last='尾页';
        $this->Page->nomClass='class="number"';
        $this->Page->hoverClass='class="number current"';
        $this->show= $this->Page->show();// 分页显示输出// 进行分页数据查询 注意limit方法的参数要使用Page类的属性
        //print_r($this->show);exit;
        return $this->show;
        // $this->assign('page',$this->show);// 赋值分页输出

    }
    /**
     * @@列表
     * @ini_set    加大内存
     * @$start      获取当天开始结束时间
     * @$end        获取当天开始结束时间
     * @$where      条件判断是否过当天时间
     * @limit     条数，每页条数
     * @IS_AJAX   加载模板
     * @$model    获取当前控制器
     *
     */
    public function index(){
        //分页获取
        $page = $this->commonPage();
        $this->assign('page',$page);
        //加大内存
        ini_set('memory_limit', '512M');
        $result = M('currency_logs as a')->join('left join php_currencypez as b on b.id = a.type')->join('left join php_currencydes as c on c.id = a.from_id')->join('left join role as e on e.role_id = a.role_id')->order('time desc')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
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
     * @ini_set    加大内存
     * @role_name  参数角色名字
     * @$where      条件
     *
     */
    public function text(){
        ini_set('memory_limit', '1024M');
        $role_name = I('role_name');
        //模糊查询
        $where['role_name'] = array('like',"%$role_name%");
        if($role_name){
            $result = M('currency_logs as a')->join('left join php_currencypez as b on b.id = a.type')->join('left join php_currencydes as c on c.id = a.from_id')->join('left join role as e on e.role_id = a.role_id')->where($where)->select();
            $this->assign('result',$result);
            $this->display();
        }else{
            $result = M('currency_logs as a')->join('left join php_currencypez as b on b.id = a.type')->join('left join php_currencydes as c on c.id = a.from_id')->join('left join role as e on e.role_id = a.role_id')->where($where)->select();
            $this->assign('result',$result);
            $this->display();
        }
    }
    /**
     * @@查询时间之间
     * @time1    参数1
     * @time2    参数2
     * @$map      条件
     *
     */
    public function cahshij(){
        $time1 = I('post.time1');
        $time2 = I('post.time2');
        $map['time'] = array('between',"$time1,$time2");
        $result = M('currency_logs as a')->join('left join php_currencypez as b on b.id = a.type')->join('left join php_currencydes as c on c.id = a.from_id')->join('left join role as e on e.role_id = a.role_id')->where($map)->select();
        $this->assign('result',$result);
        $this->display();
    }

}