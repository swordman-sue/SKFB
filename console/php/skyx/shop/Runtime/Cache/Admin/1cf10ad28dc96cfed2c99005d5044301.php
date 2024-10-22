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
                        <h5>编辑联系我们</h5>
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
                        <form method="post" action="<?php echo U('Admin/Contact/edit');?>" class="form-horizontal" enctype="multipart/form-data">
                            <input type="hidden" name="id" value="<?php echo ($result["id"]); ?>" />
                                <div class="form-group">
                                    <label class="col-sm-2 control-label">公司地址</label>
                                    <div class="col-sm-3">
                                        <input type="text" style="width: 400px" class="form-control" name="site" value="<?php echo ($result["site"]); ?>">
                                    </div>
                                </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">服务热线电话</label>
                                <div class="col-sm-3">
                                    <input type="text" style="width: 400px" class="form-control" name="fwphone" value="<?php echo ($result["fwphone"]); ?>">
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">客服邮箱</label>
                                <div class="col-sm-3">
                                    <input type="text" style="width: 400px" class="form-control" name="mailbox" value="<?php echo ($result["mailbox"]); ?>">
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">电话TEL</label>
                                <div class="col-sm-3">
                                    <input type="text" style="width: 400px" class="form-control" name="phone" value="<?php echo ($result["phone"]); ?>">
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">传真FAX</label>
                                <div class="col-sm-3">
                                    <input type="text" style="width: 400px" class="form-control" name="fax" value="<?php echo ($result["fax"]); ?>">
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">网子WEB</label>
                                <div class="col-sm-3">
                                    <input type="text" style="width: 400px" class="form-control" name="wangz" value="<?php echo ($result["wangz"]); ?>">
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">QQ</label>
                                <div class="col-sm-3">
                                    <input type="text" style="width: 400px" class="form-control" name="keqq" value="<?php echo ($result["keqq"]); ?>">
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">客服电话1</label>
                                <div class="col-sm-3">
                                    <input type="text" style="width: 400px" class="form-control" name="kephone1" value="<?php echo ($result["kephone1"]); ?>">
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                            <div class="form-group">
                            <label class="col-sm-2 control-label">客服电话2</label>
                            <div class="col-sm-3">
                                <input type="text" style="width: 400px" class="form-control" name="kephone2" value="<?php echo ($result["kephone2"]); ?>">
                            </div>
                        </div>
                        <div class="hr-line-dashed"></div>
                            <div class="form-group">
                                <label class="col-sm-2 control-label">版权</label>
                                <div class="col-sm-3">
                                    <input type="text" style="width: 400px" class="form-control" name="copyright" value="<?php echo ($result["copyright"]); ?>">
                                </div>
                            </div>
                            <div class="hr-line-dashed"></div>
                                <div class="form-group">
                                    <div class="col-sm-4 col-sm-offset-2">
                                        <button class="btn btn-primary" type="submit">编辑</button>
                                        <a class="btn btn-white" href="<?php echo U('Admin/Contact/index');?>"  >取消</a>
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
</html>