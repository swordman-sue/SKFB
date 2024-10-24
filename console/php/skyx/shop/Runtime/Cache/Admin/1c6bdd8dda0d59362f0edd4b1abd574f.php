<?php if (!defined('THINK_PATH')) exit();?><!DOCTYPE html>
<html>


<!-- Mirrored from www.zi-han.net/theme/hplus/projects.html by HTTrack Website Copier/3.x [XR&CO'2014], Wed, 20 Jan 2016 14:19:44 GMT -->
<head>

    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">


    <title>文章管理</title>
    <meta name="keywords" content="H+后台主题,后台bootstrap框架,会员中心主题,后台HTML,响应式后台">
    <meta name="description" content="H+是一个完全响应式，基于Bootstrap3最新版本开发的扁平化主题，她采用了主流的左右两栏式布局，使用了Html5+CSS3等现代技术">

    <link rel="shortcut icon" href="favicon.ico"> <link href="/skyx/Public/Admin/css/bootstrap.min14ed.css?v=3.3.6" rel="stylesheet">
    <link href="/skyx/Public/Admin/css/font-awesome.min93e3.css?v=4.4.0" rel="stylesheet">
    <link href="/skyx/Public/Admin/css/plugins/sweetalert/sweetalert.css" rel="stylesheet">
    <link href="/skyx/Public/Admin/css/animate.min.css" rel="stylesheet">
    <link href="/skyx/Public/Admin/css/style.min862f.css?v=4.1.0" rel="stylesheet">
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
                        <h5>竞技场排行榜</h5>
                        <!--<div class="ibox-tools">-->
                            <!--<a href="<?php echo U('Admin/Usercurrency/add');?>" class="btn btn-primary btn-xs">添加技术服务</a>-->
                        <!--</div>-->
                    </div>
                    <div class="ibox-content">
                          <div class="row m-b-sm m-t-sm">
                            <div class="col-md-1">
                                <button type="button" id="loading-example-btn" class="btn btn-white btn-sm"><i class="fa fa-refresh"></i> 刷新</button>
                            </div>

                            <div class="col-md-11">                              
                                <form action="<?php echo U('Admin/Arena/text');?>" method="post">
                                <div class="input-group">
                                    <input type="text" style="width:170px;"  placeholder="请输入角色名称" name="role_name" class="input-sm form-control"> <span class="input-group-btn">
                                    <button type="submit" class="btn btn-sm btn-primary" style="margin-right: 1100px;"  > 搜索</button> </span>
                                </div>
                                </form>
                            </div>
                        </div>

                        <div class="project-list">
                            <table class="table table-hover" style="text-align:center">
                                <tbody>
                                  <tr>
                                      <th>角色ID</th>
                                      <th><i class=""></i>角色等级</th>
                                      <th><i class=""></i>角色名称</th>
                                      <th><i class=""></i>当前排名</th>
                                      <th><i class=""></i>历史排名</th>
                                  </tr>
                                  <?php if(is_array($result)): $i = 0; $__LIST__ = $result;if( count($__LIST__)==0 ) : echo "" ;else: foreach($__LIST__ as $key=>$vo): $mod = ($i % 2 );++$i;?><tr>
                                            <td >
                                                <?php echo ($vo["server_id"]); ?>
                                            </td>
                                            <td >
                                                <?php echo ($vo["role_id"]); ?>
                                            </td>
                                            <td >
                                                <?php echo ($vo["role_name"]); ?>
                                            </td>
                                            <td >
                                                <?php echo ($vo["cur_rank"]); ?>
                                            </td>
                                            <td >
                                                <?php echo ($vo["history_rank"]); ?>
                                            </td>
                                        <!--<td>-->
                                            <!--<a href="<?php echo U('Admin/Service/edit','id='.$vo['id']);?>" class="btn btn-white btn-sm"><i class="fa fa-pencil"></i> 编辑 </a>-->
                                            <!--<a href="<?php echo U('Admin/Service/del','id='.$vo['id']);?>" class="btn btn-white btn-sm del"><i class="fa fa-trash"></i> 删除 </a>-->
                                        <!--</td>-->
                                    </tr><?php endforeach; endif; else: echo "" ;endif; ?>
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
                                .dataTables_paginate b a ,
                                .dataTables_paginate b span
                                {
                                    border: 1px solid #ddd;
                                    padding: 6px 12px;
                                    font-weight: normal;
                                }
                            </style>
                        </div>
                </div>
            </div>
        </div>
    <script src="/skyx/Public/Admin/js/jquery.min.js?v=2.1.4"></script>
    <script src="/skyx/Public/Admin/js/bootstrap.min.js?v=3.3.6"></script>
    <script src="/skyx/Public/Admin/js/content.min.js?v=1.0.0"></script>
    <script src="/skyx/Public/Admin/js/plugins/sweetalert/sweetalert.min.js"></script>
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