<?php if (!defined('THINK_PATH')) exit();?><!DOCTYPE html>
<html>


<!-- Mirrored from www.zi-han.net/theme/hplus/form_basic.html by HTTrack Website Copier/3.x [XR&CO'2014], Wed, 20 Jan 2016 14:19:15 GMT -->
<head>

    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">


    <title>H+ 后台主题UI框架 - 基本表单</title>
    <meta name="keywords" content="H+后台主题,后台bootstrap框架,会员中心主题,后台HTML,响应式后台">
    <meta name="description" content="H+是一个完全响应式，基于Bootstrap3最新版本开发的扁平化主题，她采用了主流的左右两栏式布局，使用了Html5+CSS3等现代技术">

    <link rel="shortcut icon" href="favicon.ico"> <link href="/skyx/Public/Admin/css/bootstrap.min14ed.css?v=3.3.6" rel="stylesheet">
    <link href="/skyx/Public/Admin/css/font-awesome.min93e3.css?v=4.4.0" rel="stylesheet">
    <link href="/skyx/Public/Admin/css/plugins/iCheck/custom.css" rel="stylesheet">
    <link href="/skyx/Public/Admin/css/animate.min.css" rel="stylesheet">
    <link href="/skyx/Public/Admin/css/style.min862f.css?v=4.1.0" rel="stylesheet">
    <link rel="stylesheet" href="js/kindeditor/themes/default/default.css" />
    <link href="/skyx/Public/Admin/css/plugins/sweetalert/sweetalert.css" rel="stylesheet">

</head>

<body class="gray-bg">
    <div class="wrapper wrapper-content animated fadeInRight">
        <div class="row">
            <div class="col-sm-12">
                <div class="ibox float-e-margins">
                    <div class="ibox-title">
                        <h5>公告添加</h5>
                        <div class="ibox-tools"><button type="button" id="loading-example-btn" class="btn btn-white btn-sm"><i class="fa fa-refresh"></i> 刷新</button>

                        </div>
                    <div class="ibox-content">
                        <form method="post" action="<?php echo U('Admin/Notice/add');?>" class="form-horizontal" enctype='multipart/form-data'>
                            <div class="form-group" style="display:none">
                                <label class="col-sm-2 control-label">cmd_id:</label>
                                <div class="col-sm-3">
                                    <input  type="text" class="form-control" name="cmd_id" value="5">
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">发布公告时间</label>
                                <div class="col-sm-3">
                                    <input type="text" style="width:230px;height:35px" class="sang_Calender" name="add_time">
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">指定的服务器(0表示所有的服务器)：(0不用选)</label>
                                <div class="col-sm-3">
                                    <select name="param1" class="form-control">
                                        <option value="">..请选择</option>
                                        <?php if(is_array($server)): $i = 0; $__LIST__ = $server;if( count($__LIST__)==0 ) : echo "" ;else: foreach($__LIST__ as $key=>$vo): $mod = ($i % 2 );++$i;?><option  value="<?php echo ($vo['server_id']); ?>" <?php if($vo['server_id'] == $resut['param1']): ?>selected<?php endif; ?> >
                                            <?php echo ($vo['server_id']); ?> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; <?php echo ($vo['name']); ?>
                                            </option><?php endforeach; endif; else: echo "" ;endif; ?>
                                    </select>
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">公告ID(ID优先)：</label>
                                <div class="col-sm-3">
                                    <select name="param2" class="form-control">
                                        <option value="">..请选择</option>
                                        <?php if(is_array($ptnotice)): $i = 0; $__LIST__ = $ptnotice;if( count($__LIST__)==0 ) : echo "" ;else: foreach($__LIST__ as $key=>$vo): $mod = ($i % 2 );++$i;?><option  value="<?php echo ($vo['typeid']); ?>" <?php if($vo['typeid'] == $resut['param2']): ?>selected<?php endif; ?> >
                                            <?php echo ($vo['typeid']); ?> &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; <?php echo ($vo['name']); ?>
                                            </option><?php endforeach; endif; else: echo "" ;endif; ?>
                                    </select>
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">公告内容</label>
                                <textarea name="text" rows="8" cols="80"></textarea>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <div class="col-sm-4 col-sm-offset-2">
                                    <button class="btn btn-primary" type="submit">添加</button>
                                    <a class="btn btn-white" href="<?php echo U('Admin/Notice/add');?>"  >重置</a>
                                </div>
                            </div>
                        </form>
                    </div>
                </div>
            </div>
        </div>
    </div>
    <script src="/skyx/Public/Admin/js/jquery.min.js?v=2.1.4"></script>
    <script src="/skyx/Public/Admin/js/bootstrap.min.js?v=3.3.6"></script>
    <script src="/skyx/Public/Admin/js/content.min.js?v=1.0.0"></script>
    <script src="/skyx/Public/Admin/js/plugins/layer/laydate/laydate.js"></script>
    <script src="/skyx/Public/Admin/js/plugins/iCheck/icheck.min.js"></script>
 <script charset="utf-8" src="/skyx/Public/Admin/js/kindeditor/kindeditor-min.js"></script>
 <script charset="utf-8" src="/skyx/Public/Admin/js/kindeditor/lang/zh_CN.js"></script>
 <script src="/skyx/Public/Admin/js/plugins/sweetalert/sweetalert.min.js"></script>
 <script>
       $(".btn-primary").click(function() {
           swal({
               text: "提示信息",
           })
       });
             var editor;
             KindEditor.ready(function(K) {
                 editor = K.create('textarea[name="article_desc"]', {
                     allowFileManager : true
                 });
             });
             var editor1;
             KindEditor.ready(function(K) {
                 editor1 = K.create('textarea[name="article_content"]', {
                     allowFileManager : true
                 });
             });
        $(document).ready(function(){$(".i-checks").iCheck({checkboxClass:"icheckbox_square-green",radioClass:"iradio_square-green",})});
      $('#loading-example-btn').click(function(){
        location.reload();
       })
      laydate({elem:"#date",event:"focus"});
    </script>
</body>
</script><script type="text/javascript" src="/skyx/Public/Admin/js/datetime.js"></script>
</html>