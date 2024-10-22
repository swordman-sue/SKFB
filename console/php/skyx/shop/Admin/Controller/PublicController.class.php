<?php
/*
**********
后台公共控制器
**********
*/
namespace Admin\Controller;
use Think\Controller;
class PublicController extends Controller {
    public static $model = null;//保存实例化模型
    public $Page=null;//保存分页实例化
    public $field='';
    public $perPage=5;//每页显示条数
    //分页
    public function commonPage(){
        //$User        = M('User'); // 实例化User对象
        $count       = self::$model->count();// 查询满足要求的总记录数
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
    /*
     公共构造函数
     */
    public function __construct(){
        parent::__construct();
        //运行自动实例化
        $this->setModel();
        //从数据库的setup表读取保存网站常量
        //$this->shopVarSetup();
        $list=C('list');
        $title=C('title');

        //读取配置好的栏目名称
        $this->assign('list',$list[CONTROLLER_NAME]);
        $this->assign('title',$title[CONTROLLER_NAME]);
    }

    /*//构造函数
    public function __construct(){
        parent::__construct();
        //运行自动实例化
        $this->setmodel();
    }*/
    /*
     自动实例化方法
    */
    public function setModel(){
        if(self::$model==null){
            self::$model=D(ucfirst(CONTROLLER_NAME));
        }
    }

    /*
     公共列表方法
    */
    public function index(){
        //分页
        $page = $this->commonPage();
        $this->assign('page',$page);

        $result= self::$model->field($this->field,'TABLE_COMMENT')->limit($this->Page->firstRow.','.$this->Page->listRows)->select();
        //var_dump($result);exit;
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
        //var_dump($result);
    }
    /*
    公共添加方法
    */
    public function add(){
        if(IS_POST){
            //var_dump($_POST);exit;
            self::$model->create();
            $result=self::$model->add();
           //var_dump($result);exit;
            if($result===false){
                //var_dump(self::$model->getDbError());
                $this->error('新增失败', U('Admin/'.CONTROLLER_NAME.'/add'));
            }else{
                $this->assign('waitSecond',5);
                $this->success('新增成功', U('Admin/'.CONTROLLER_NAME.'/index'));
            }

        }else{
            $this->display();
        }
    }
    /*
    公共编辑方法
    */
    public function edit(){
        if(IS_POST){
            //更新数据
            self::$model->create();
            $no=self::$model->getPk();
            $result=self::$model->where(self::$model->getPk().'='.$_POST[$no])->save();
            if($result===false){
                //var_dump(self::$model->getDbError());
                $this->error('编辑失败', U('Admin/'.CONTROLLER_NAME.'/edit'));
            }else{
                $this->assign('waitSecond',5);
                $this->success('编辑成功', U('Admin/'.CONTROLLER_NAME.'/edit','id='.$_POST['id']));
            }
        }else{
            //查询原本数据
            $result=self::$model->where(self::$model->getPk().'='.$_GET['id'])->find();
            //var_dump($result);exit;
            //echo self::$model->getLastSql();exit;
            //echo self::$model->getDbError();
            $this->assign('result',$result);
            $this->display(CONTROLLER_NAME.':'.ACTION_NAME);
        }
    }
    /*
         公共删除方法
         */
    public function del(){
        $result=self::$model->where(self::$model->getPk().'='.I('get.id'))->delete();
        //echo self::$model->getLastSql();exit;
        //var_dump($result);exit;
        if($result===false){
            //var_dump(self::$model->getDbError());
            $this->error('删除失败', U('Admin/'.CONTROLLER_NAME.'/index'));
        }else{
            $this->assign('waitSecond',5);
            $this->success('删除成功', U('Admin/'.CONTROLLER_NAME.'/index'));
        }
    }
	 //删除
    public  function shanchu()
    {
        $id=self::$model->where(self::$model->getPk().'='.I('get.id'))->delete();
        if($result===false){
            //var_dump(self::$model->getDbError());
            $this->error('删除失败', U('Admin/'.CONTROLLER_NAME.'/index'));
        }else{
            $this->assign('waitSecond',3);
            $this->success('删除成功', U('Admin/'.CONTROLLER_NAME.'/index'));
        }
    }
}