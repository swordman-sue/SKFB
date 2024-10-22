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
                    <h5>角色信息</h5>
                    <!--<div class="ibox-tools">-->
                    <!--<a href="<?php echo U('Admin/Usercurrency/add');?>" class="btn btn-primary btn-xs">添加技术服务</a>-->
                    <!--</div>-->
                </div>
                <div class="ibox-content">
                    <div class="row m-b-sm m-t-sm">
                        <div class="col-md-1">
                            <button type="button" id="loading-example-btn" class="btn btn-white btn-sm"><i class="fa fa-refresh"></i> 刷新</button>
                        </div>

                        <div class="col-md-11" style="width: 250px; display: inline;">
                            <form action="<?php echo U('Admin/Role/chajiaox');?>" method="post">
                                <div class="input-group">
                                    <input type="text" style="width:170px;" placeholder="请输入角色ID!角色ID查找" name="role_id" class="input-sm form-control"> <span class="input-group-btn">
                                    <button type="submit" class="btn btn-sm btn-primary" style="margin-right: 1100px;"> 搜索</button> </span>
                                </div>
                            </form>
                        </div>
                        <div class="col-md-11" style="width: 250px; display: inline;">
                            <form action="<?php echo U('Admin/Role/chaname');?>" method="post">
                                <div class="input-group">
                                    <input type="text" style="width:170px;"  placeholder="请输入角色名称!角色名称查找" name="role_name" class="input-sm form-control"> <span class="input-group-btn">
                                    <button type="submit" class="btn btn-sm btn-primary" style="margin-right: 1100px;"> 搜索</button> </span>
                                </div>
                            </form>
                        </div>
                        <div class="col-md-11" style="width: 320px; display: inline;">
                            <form action="<?php echo U('Admin/Role/chazhangl');?>" method="post">
                                <div class="input-group">
                                    <input type="text" style="width:120px;margin-right: 12px;"  placeholder="请输入最小战力!战力之间查找" name="fighting1" class="input-sm form-control"><span class="input-group-btn">
                                          <input type="text" style="width:120px;"  placeholder="请输入最大战力!战力之间查找" name="power" class="input-sm form-control">
                                    <button type="submit" class="btn btn-sm btn-primary" style="margin-right: 1100px;"> 搜索</button> </span>
                                </div>
                            </form>
                        </div>
                        <div class="col-md-11" style="width: 50px; display: inline;">
                            <form action="<?php echo U('Admin/Role/zhuans');?>" method="post">
                                <div class="input-group">
                                    <input type="text" style="width:120px;margin-right: 12px;"  placeholder="请输入最小钻石!钻石之间查找" name="diamond1" class="input-sm form-control"><span class="input-group-btn">
                                          <input type="text" style="width:120px;"  placeholder="请输入最大钻石!钻石之间查找" name="diamond2" class="input-sm form-control">
                                    <button type="submit" class="btn btn-sm btn-primary" style="margin-right: 1100px;"> 搜索</button> </span>
                                </div>
                            </form>
                        </div>
                    </div>

                    <div class="project-list">
                        <table class="table table-hover" style="text-align:center">
                            <tbody>
                            <tr>
                                <th>角色Id</th>
                                <th><i class=""></i>角色名称</th>
                                <th><i class=""></i>帐号</th>
                                <th><i class=""></i>角色等级</th>
                                <th><i class=""></i>钻石</th>
                                <th><i class=""></i>金币</th>
                                <th><i class=""></i>总的充值额度</th>
                                <th><i class=""></i>VIP等级</th>
                                <th><i class=""></i>当前主线副本章节</th>
                                <th><i class=""></i>当前主线副本关卡</th>
                                <th><i class=""></i>在线时间秒(转换时间格式)</th>
                                <th><i class=""></i>物品数量</th>
                                <th><i class=""></i>装备数量</th>
                                <th><i class=""></i>宝物数量</th>
                                <th><i class=""></i>英雄数量</th>
                                <th><i class=""></i>战力</th>
                                <th><i class=""></i>角色创建时间</th>
                                <th><i class=""></i>最后更新数据时间</th>
                            </tr>
                            <style>
                                .no1 td{
                                    font-size:1px;
                                }
                            </style>
                            <?php if(is_array($trademarkchax)): $i = 0; $__LIST__ = $trademarkchax;if( count($__LIST__)==0 ) : echo "" ;else: foreach($__LIST__ as $key=>$vo): $mod = ($i % 2 );++$i;?><tr class="no1">
                                    <td>
                                        <?php echo ($vo["role_id"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["role_name"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["account"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["level"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["diamond"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["gold"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["total_recharge_amount"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["vip_level"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["fb_chapter"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["fb_map"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["total_online_time"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["item_num"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["equipment_num"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["treasure_num"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["hero_num"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["fighting_power"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["create_time"]); ?>
                                    </td>
                                    <td >
                                        <?php echo ($vo["last_update_time"]); ?>
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