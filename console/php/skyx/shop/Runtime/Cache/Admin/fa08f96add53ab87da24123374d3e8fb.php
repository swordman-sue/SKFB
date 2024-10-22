<?php if (!defined('THINK_PATH')) exit();?><!DOCTYPE html>
<html>


<!-- Mirrored from www.zi-han.net/theme/hplus/projects.html by HTTrack Website Copier/3.x [XR&CO'2014], Wed, 20 Jan 2016 14:19:44 GMT -->
<head>

    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">


    <title>文章管理</title>
    <meta name="keywords" content="H+后台主题,后台bootstrap框架,会员中心主题,后台HTML,响应式后台">
    <meta name="description" content="H+是一个完全响应式，基于Bootstrap3最新版本开发的扁平化主题，她采用了主流的左右两栏式布局，使用了Html5+CSS3等现代技术">

    <link rel="shortcut icon" href="favicon.ico"> <link href="/yingx/Public/Admin/css/bootstrap.min14ed.css?v=3.3.6" rel="stylesheet">
    <link href="/yingx/Public/Admin/css/font-awesome.min93e3.css?v=4.4.0" rel="stylesheet">
    <link href="/yingx/Public/Admin/css/plugins/sweetalert/sweetalert.css" rel="stylesheet">
    <link href="/yingx/Public/Admin/css/animate.min.css" rel="stylesheet">
    <link href="/yingx/Public/Admin/css/style.min862f.css?v=4.1.0" rel="stylesheet">
    <style>
        th{
            text-align: center;
        }
    </style>

</head>

<body class="gray-bg">

<div class="wrapper wrapper-content animated fadeInUp">
    <div class="row">
        <div class="col-sm-12">

            <div class="ibox">
                <div class="ibox-title">
                    <h5>产品分类</h5>
                    <div class="ibox-tools">
                        <a href="<?php echo U('Admin/Brandclassify/add');?>" class="btn btn-primary btn-xs">添加产品分类</a>
                    </div>
                </div>
                <div class="ibox-content">
                    <div class="row m-b-sm m-t-sm">
                        <div class="col-md-1">
                            <button type="button" id="loading-example-btn" class="btn btn-white btn-sm"><i class="fa fa-refresh"></i> 刷新</button>
                        </div>

                        <div class="col-md-11">
                            <form action="<?php echo U('Admin/Brandclassify/text');?>" method="get">
                                <div class="input-group">
                                    <input type="text" placeholder="请输名称" name="" class="input-sm form-control"> <span class="input-group-btn">
                                    <button type="submit" class="btn btn-sm btn-primary" disabled="disabled"> 搜索</button> </span>
                                </div>
                            </form>
                        </div>
                    </div>
                    <div class="project-list">
                        <table class="table table-hover" style="text-align:center">
                            <tbody>
                            <div>
                                <samp style="margin: 100px ;">ID</samp>
                                <samp style="margin: 200px ;"><i class="fa fa-user"></i>分类名字</samp>
                                <samp style="margin: 100px ;">操作</samp>
                            </div>
                            <?php if(is_array($list[0])): $i = 0; $__LIST__ = $list[0];if( count($__LIST__)==0 ) : echo "" ;else: foreach($__LIST__ as $key=>$vo): $mod = ($i % 2 );++$i;?><ul>
                                    <li style="list-style-type:none;">
                                        <samp style="width: 140px ;display: inline-block;text-align: center;"><?php echo ($vo["cat_id"]); ?></samp>
                                        <samp style="width: 300px; display: inline-block;padding-left: 170px;">父分类:<?php echo ($vo["name"]); ?></samp>
                                        <samp style="width: 420px; display: inline-block;text-align: center;"> <a  href="<?php echo U('Admin/Brandclassify/edit','id='.$vo['cat_id']);?>" class="btn btn-white btn-sm"><i class="fa fa-pencil"></i> 编辑 </a>
                                        <a  href="<?php echo U('Admin/Brandclassify/del','id='.$vo['cat_id']);?>" class="btn btn-white btn-sm del"><i class="fa fa-trash"></i> 删除 </a></samp>
                                    </li>
                                    <?php if(is_array($list[$vo['cat_id']])): $i = 0; $__LIST__ = $list[$vo['cat_id']];if( count($__LIST__)==0 ) : echo "" ;else: foreach($__LIST__ as $key=>$vo2): $mod = ($i % 2 );++$i;?><li style="list-style-type:none;">
                                            <samp style="margin: 80px ;"><?php echo ($vo2["cat_id"]); ?></samp>
                                            <samp style="width: 420px; display: inline-block;padding-left: 200px;">子分类: <?php echo ($vo2["name"]); ?></samp>
                                            <samp style="width: 250px; display: inline-block;padding: 6px 0 0 56px;"><a   href="<?php echo U('Admin/Brandclassify/edit','id='.$vo2['cat_id']);?>" class="btn btn-white btn-sm"><i class="fa fa-pencil" ></i> 编辑 </a>
                                            <a  href="<?php echo U('Admin/Brandclassify/del','id='.$vo2['cat_id']);?>" class="btn btn-white btn-sm del"><i class="fa fa-trash"></i> 删除 </a></samp>
                                        </li><?php endforeach; endif; else: echo "" ;endif; ?>
                                </ul><?php endforeach; endif; else: echo "" ;endif; ?>
                            </tbody>
                        </table>
                    </div>
                </div>
                <style>
                    .current{
                        background: greenyellow;
                    }
                </style>
                <div  class="dataTables_paginate paging_simple_numbers">
                    <b><?php echo ($page); ?></b>
                    <style>
                        .dataTables_paginate b a,
                        .dataTables_paginate b span
                        {
                            display: inline-block;
                            padding: 3px 5px;
                            margin-right: 5px;
                            border:1px solid;
                        }
                    </style>
                </div>
            </div>
        </div>
    </div>
    <script src="/yingx/Public/Admin/js/jquery.min.js?v=2.1.4"></script>
    <script src="/yingx/Public/Admin/js/bootstrap.min.js?v=3.3.6"></script>
    <script src="/yingx/Public/Admin/js/content.min.js?v=1.0.0"></script>
    <script src="/yingx/Public/Admin/js/plugins/sweetalert/sweetalert.min.js"></script>
    <script>
        $(".del").click(function() {
            swal({
                // title: "您确定要删除吗",
                //text: "删除后将无法恢复，请谨慎操作！",
                type: "warning",
                showCancelButton: true,
                //confirmButtonColor: "#1647a1",
                confirmButtonText: "确定",
                cancelButtonText: "取消",
                closeOnConfirm: false,
                closeOnCancel: false
            }, function(isConfirm) {
                if (isConfirm) {
                    swal("删除成功！", "您已经永久删除了这条信息。", "success")
                } else {
                    swal("已取消", "您取消了删除操作！", "error")
                }
            })
        });

        $(document).ready(function(){$("#loading-example-btn").click(function(){btn=$(this);simpleLoad(btn,true);simpleLoad(btn,false)})});function simpleLoad(btn,state){if(state){btn.children().addClass("fa-spin");btn.contents().last().replaceWith(" Loading")}else{setTimeout(function(){btn.children().removeClass("fa-spin");btn.contents().last().replaceWith(" Refresh")},2000)}};
        $('#loading-example-btn').click(function(){
            location.reload();
        })
    </script>
</body>

<!-- Mirrored from www.zi-han.net/theme/hplus/projects.html by HTTrack Website Copier/3.x [XR&CO'2014], Wed, 20 Jan 2016 14:19:44 GMT -->
</html>