<?php if (!defined('THINK_PATH')) exit();?><!DOCTYPE html>
<html>


<!-- Mirrored from www.zi-han.net/theme/hplus/form_basic.html by HTTrack Website Copier/3.x [XR&CO'2014], Wed, 20 Jan 2016 14:19:15 GMT -->
<head>

    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">


    <title>H+ 后台主题UI框架 - 基本表单</title>
    <meta name="keywords" content="">
    <meta name="description" content="">

    <link rel="shortcut icon" href="favicon.ico"> <link href="/Public/Admin/css/bootstrap.min14ed.css?v=3.3.6" rel="stylesheet">
    <link href="/Public/Admin/css/font-awesome.min93e3.css?v=4.4.0" rel="stylesheet">
    <link href="/Public/Admin/css/plugins/iCheck/custom.css" rel="stylesheet">
    <link href="/Public/Admin/css/animate.min.css" rel="stylesheet">
    <link href="/Public/Admin/css/style.min862f.css?v=4.1.0" rel="stylesheet">
    <link rel="stylesheet" href="/Public/Admin/js/kindeditor/themes/default/default.css" />
    
</head>

<body class="gray-bg">
    <div class="wrapper wrapper-content animated fadeInRight">
        <div class="row">
            <div class="col-sm-12">
                <div class="ibox float-e-margins">
                    <div class="ibox-title">
                        <h5>编辑公司介绍</h5>
                        <div class="ibox-tools"><button type="button" id="loading-example-btn" class="btn btn-white btn-sm"><i class="fa fa-refresh"></i> 刷新</button>

                            <a class="collapse-link">
                                <i class="fa fa-chevron-up"></i>
                            </a>
                            <a class="close-link">
                                <i class="fa fa-times"></i>
                            </a>
                        </div>
                    </div>
                    <div class="ibox-content">
                        <form method="post" action="<?php echo U('Admin/Company/edit');?>" class="form-horizontal" enctype="multipart/form-data">
                            <input type="hidden" name="id" value="<?php echo ($result["id"]); ?>" />
                                <div class="form-group">
                                    <label class="col-sm-2 control-label">标题</label>
                                    <div class="col-sm-3">
                                        <input type="text" class="form-control" name="title" value="<?php echo ($result["title"]); ?>" style="width: 520px">
                                    </div>
                                </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">排序</label>
                                <div class="col-sm-3">
                                    <input type="text" class="form-control" name="paix" value="<?php echo ($result["paix"]); ?>">
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">图片</label>
                                <div class="col-sm-10">
                                    <img src="/<?php echo ($result["fmpicture"]); ?>" class="img-circle" alt="" width="50px" height="50px">
                                    <input type="file" name="fmpicture"  id="fmpicture">
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <!--<div class="form-group">
                                <label class="col-sm-2 control-label">内容图片</label>
                                <div class="col-sm-10">
                                    <img src="/<?php echo ($result["nrpicture"]); ?>" class="img-circle" alt="" width="50px" height="50px">
                                    <input type="file" name="nrpicture"  id="nrpicture">
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>-->
                            <div class="form-group">
                                <label class="col-sm-2 control-label">工程案例所属分类</label>
                                <div class="col-sm-3">
                                    <select name="cat_id" id="" class="form-control">
                                        <option value="">..请选择</option>
                                        <?php if(is_array($item)): $i = 0; $__LIST__ = $item;if( count($__LIST__)==0 ) : echo "" ;else: foreach($__LIST__ as $key=>$vo): $mod = ($i % 2 );++$i;?><option value="<?php echo ($vo["cat_id"]); ?>" <?php echo ($vo['cat_id']==$result['cat_id']?'selected':''); ?>>
                                            <?php echo ($vo["name"]); ?>
                                            </option><?php endforeach; endif; else: echo "" ;endif; ?>
                                    </select>
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">描述</label>
                                <div class="col-sm-3">
                                    <textarea style="width:500px;height:150px;" name="describe" value="describe" ><?php echo ($result["describe"]); ?></textarea>
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label >内容</label>
                                <!-- 加载编辑器的容器 -->
                                <script id="aboutus" name="content" type="text/plain" style="width:1000px;height:300px" ><?php echo (stripslashes(htmlspecialchars_decode($result["content"]))); ?></script>
                                <!-- 配置文件 -->
                                <script type="text/javascript" src="/Public/Admin/ueditor/ueditor.config.js"></script>
                                <!-- 编辑器源码文件 -->
                                <script type="text/javascript" src="/Public/Admin/ueditor/ueditor.all.js"></script>
                                <!-- 实例化编辑器 -->
                                <script type="text/javascript">
                                    var ue = UE.getEditor('aboutus');
                                </script>
                            </div>
                            <div class="hr-line-dashed"></div>
                                <div class="form-group">
                                    <div class="col-sm-4 col-sm-offset-2">
                                        <button class="btn btn-primary" type="submit">编辑</button>
                                        <a class="btn btn-white" href="<?php echo U('Admin/Company/index');?>"  >取消</a>
                                    </div>
                                </div>
                        </form>
                    </div>
                </div>
            </div>
        </div>
    </div>
    <script src="/Public/Admin/js/jquery.min.js?v=2.1.4"></script>
    <script src="/Public/Admin/js/bootstrap.min.js?v=3.3.6"></script>
    <script src="/Public/Admin/js/content.min.js?v=1.0.0"></script>
    <script src="/Public/Admin/js/plugins/layer/laydate/laydate.js"></script>
    <script src="/Public/Admin/js/plugins/iCheck/icheck.min.js"></script>
    <script charset="utf-8" src="/Public/Admin/js/kindeditor/kindeditor-min.js"></script>
    <script charset="utf-8" src="/Public/Admin/js/kindeditor/lang/zh_CN.js"></script>
 <script>
        var editor;
             KindEditor.ready(function(K) {
                 editor = K.create('textarea[name="jieshao"]', {
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
</script><script type="text/javascript" src="/Public/Admin/js/datetime.js"></script>
</html>