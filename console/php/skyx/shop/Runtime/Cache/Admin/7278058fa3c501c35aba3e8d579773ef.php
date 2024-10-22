<?php if (!defined('THINK_PATH')) exit();?><!DOCTYPE html>
<html>


<!-- Mirrored from www.zi-han.net/theme/hplus/ by HTTrack Website Copier/3.x [XR&CO'2014], Wed, 20 Jan 2016 14:16:41 GMT -->
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="renderer" content="webkit">
    <meta http-equiv="Cache-Control" content="no-siteapp" />
    <title>后台首页</title>
    <meta name="keywords" content="H+后台主题,后台bootstrap框架,会员中心主题,后台HTML,响应式后台">
    <meta name="description" content="H+是一个完全响应式，基于Bootstrap3最新版本开发的扁平化主题，她采用了主流的左右两栏式布局，使用了Html5+CSS3等现代技术">

    <!--[if lt IE 9]>
    <meta http-equiv="refresh" content="0;ie.html" />
    <![endif]-->

    <link rel="shortcut icon" href="favicon.ico">
    <link href="/skyx/Public/Admin/css/bootstrap.min14ed.css?v=3.3.6" rel="stylesheet">
    <link href="/skyx/Public/Admin/css/font-awesome.min93e3.css?v=4.4.0" rel="stylesheet">
    <link href="/skyx/Public/Admin/css/animate.min.css" rel="stylesheet">
    <link href="/skyx/Public/Admin/css/style.min862f.css?v=4.1.0" rel="stylesheet">
</head>

<body class="fixed-sidebar full-height-layout gray-bg" style="overflow:hidden">
    <div id="wrapper">
        <!--左侧导航开始-->
        <nav class="navbar-default navbar-static-side" role="navigation">
            <div class="nav-close"><i class="fa fa-times-circle"></i>
            </div>
            <div class="sidebar-collapse">
                <ul class="nav" id="side-menu">
                    <li class="nav-header">
                        <div class="dropdown profile-element">
                            <span><img alt="image" class="img-circle" src="/skyx/Public/Admin/img/profile_small.jpg" /></span>
                            <a data-toggle="dropdown" class="dropdown-toggle" href="#">
                                <span class="clear">
                               <span class="block m-t-xs"><strong class="font-bold">您好:</strong></span>
                                <span class="text-muted text-xs block"><?php echo session('admin_name') ?><b class="caret"></b></span>
                                </span>
                            </a>
                            <ul class="dropdown-menu animated fadeInRight m-t-xs">
                                <li><a class="J_menuItem" href="">修改头像</a>
                                </li>
                                <li><a class="J_menuItem" href="">个人资料</a>
                                </li>
                                <li class="divider"></li>
                                <li><a href="<?php echo U('Admin/Login/login_out');?>">安全退出</a>
                                </li>
                            </ul>
                        </div>
                        <div class="logo-element">H+
                        </div>
                    </li>
                    <li>
                        <a class="J_menuItem" href="<?php echo U('Admin/Index/index_right');?>"><i class="fa fa-home"></i> <span class="nav-label">首页</span></a>
                    </li>
                    <li>
                        <a href="#"><i class="fa fa-desktop"></i> <span class="nav-label">管理员中心</span><span class="fa arrow"></span></a>
                        <ul class="nav nav-second-level">
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Adminuser/index');?>">
                                    管理员列表
                                </a>
                            </li>
                        </ul>
                    </li>
                    <li>
                        <a href="#"><i class="fa fa-desktop"></i> <span class="nav-label">玩家角色管理</span><span class="fa arrow"></span></a>
                        <ul class="nav nav-second-level">
                            <!--<li>-->
                                <!--<a class="J_menuItem" href="<?php echo U('Usercurrency/index');?>">-->
                                    <!--角色货币日志详情-->
                                <!--</a>-->
                            <!--</li>-->
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Leave/index');?>">
                                    玩家留言列表
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Role/index');?>">
                                    角色信息列表
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Logout/index');?>">
                                    角色下线记录
                                </a>
                            </li>
							<!--
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Object/index');?>">
                                    对象
                                </a>
                            </li>
							-->
                        </ul>
                    </li>
                    <li>
                        <a href="#"><i class="fa fa-desktop"></i> <span class="nav-label">竞技场排行榜</span><span class="fa arrow"></span></a>
                        <ul class="nav nav-second-level">
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Arena/index');?>">
                                    竞技场排行榜
                                </a>
                            </li>
                        </ul>
                    </li>
                    <li>
                        <a href="#"><i class="fa fa-desktop"></i> <span class="nav-label">开服操作</span><span class="fa arrow"></span></a>
                        <ul class="nav nav-second-level">
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Server/index');?>">
                                    开服操作
                                </a>
                            </li>
                        </ul>
                    </li>
                    <li>
                        <a href="#"><i class="fa fa-desktop"></i> <span class="nav-label">GM命令操作</span><span class="fa arrow"></span></a>
                        <ul class="nav nav-second-level">
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Gm/add');?>">
                                    没参数GM命令操作
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Award/index');?>">
                                    已发放奖励列表
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Award/player');?>">
                                    发放奖励(指定玩家)
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Award/server');?>">
                                    发放奖励(指定服务器)
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Notice/add');?>">
                                    发布公告
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Mail/add');?>">
                                    发布邮件
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('RechargeEx/add');?>">
                                    充值操作
                                </a>
                            </li>							
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Kick/add');?>">
                                    强行下线(指定玩家)
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('All/add');?>">
                                    强行下线(整个服务器)
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Banned/add');?>">
                                    禁言设置
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Delete/add');?>">
                                    删除排行榜用户数据
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Anew/add');?>">
                                    排行榜重新排行
                                </a>
                            </li>
                        </ul>
                    </li>
                    <li>
                        <a href="#"><i class="fa fa-desktop"></i> <span class="nav-label">数据统计相关</span><span class="fa arrow"></span></a>
                        <ul class="nav nav-second-level">
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Record/index');?>">
                                    关卡失败记录列表
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('First/index');?>">
                                    首次通关记录
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('ServerInfo/index');?>">
                                    服务器信息
                                </a>
                            </li>							
                        </ul>
                    </li>
                    <li>
                        <a href="#"><i class="fa fa-desktop"></i> <span class="nav-label">充值管理</span><span class="fa arrow"></span></a>
                        <ul class="nav nav-second-level">
                            <li>
                                    <a class="J_menuItem" href="<?php echo U('Recharge/index');?>">
                                    充值记录
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Log/index');?>">
                                    充值日志
                                </a>
                            </li>
                        </ul>
                    </li>
                    <li>
                        <a href="#"><i class="fa fa-desktop"></i> <span class="nav-label">商店购买记录</span><span class="fa arrow"></span></a>
                        <ul class="nav nav-second-level">
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Shop/index');?>">
                                    商店购买记录列表
                                </a>
                            </li>
                        </ul>
                    </li>
                    <li>
                        <a href="#"><i class="fa fa-desktop"></i> <span class="nav-label">工会管理</span><span class="fa arrow"></span></a>
                        <ul class="nav nav-second-level">
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Guild/index');?>">
                                    工会列表
                                </a>
                            </li>
                        </ul>
                    </li>
                    <li>
                        <a href="#"><i class="fa fa-desktop"></i> <span class="nav-label">激动码记录</span><span class="fa arrow"></span></a>
                        <ul class="nav nav-second-level">
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Activation/index');?>">
                                    激活码记录列表
                                </a>
                            </li>
                        </ul>
                    </li>
                    <li>
                        <a href="#"><i class="fa fa-desktop"></i> <span class="nav-label">配置</span><span class="fa arrow"></span></a>
                        <ul class="nav nav-second-level">
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Noticepz/index');?>">
                                    公告下拉配置
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Awardpz/index');?>">
                                    奖励ID
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Awardwuppz/index');?>">
                                    奖励物品ID
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Mailpz/index');?>">
                                    邮件id配置
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Rankingpz/index');?>">
                                    排行榜类型
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('Trtype/index');?>">
                                    踢人下线类型
                                </a>
                            </li>
                            <li>
                                <a class="J_menuItem" href="<?php echo U('RechargeExpz/index');?>">
                                    添加充值档位
                                </a>
                            </li>							
                        </ul>
                    </li>
                </ul>
            </div>
        </nav>
        <!--左侧导航结束-->
        <!--右侧部分开始-->
        <div id="page-wrapper" class="gray-bg dashbard-1">
        <div class="row content-tabs">
            <button class="roll-nav roll-left J_tabLeft"><i class="fa fa-backward"></i>
            </button>
            <nav class="page-tabs J_menuTabs">
                <div class="page-tabs-content">
                    <a href="javascript:void(0);" class="active J_menuTab" data-id="<?php echo U('Admin/Index/index_right');?>">首页</a>
                </div>
            </nav>
            <button class="roll-nav roll-right J_tabRight"><i class="fa fa-forward"></i>
            </button>
            <div class="btn-group roll-nav roll-right">
                <button class="dropdown J_tabClose" data-toggle="dropdown">关闭操作<span class="caret"></span>

                </button>
                <ul role="menu" class="dropdown-menu dropdown-menu-right">
                    <li class="J_tabShowActive"><a>定位当前选项卡</a>
                    </li>
                    <li class="divider"></li>
                    <li class="J_tabCloseAll"><a>关闭全部选项卡</a>
                    </li>
                    <li class="J_tabCloseOther"><a>关闭其他选项卡</a>
                    </li>
                </ul>
            </div>
            <a href="<?php echo U('Admin/Login/login_out');?>" class="roll-nav roll-right J_tabExit"><i class="fa fa fa-sign-out"></i> 退出</a>
        </div>
            <div class="row J_mainContent" id="content-main">
            <!-- 主要框架部分 -->
                <iframe class="J_iframe" name="iframe0" width="100%" height="100%" src="<?php echo U('Admin/Index/index_right');?>" frameborder="0" data-id="index_right.html" seamless></iframe>
            </div>
            <div class="footer">
            </div>
        </div>
    </div>
    <script src="/skyx/Public/Admin/js/jquery.min.js?v=2.1.4"></script>
    <script src="/skyx/Public/Admin/js/bootstrap.min.js?v=3.3.6"></script>
    <script src="/skyx/Public/Admin/js/plugins/metisMenu/jquery.metisMenu.js"></script>
    <script src="/skyx/Public/Admin/js/plugins/slimscroll/jquery.slimscroll.min.js"></script>
    <script src="/skyx/Public/Admin/js/plugins/layer/layer.min.js"></script>
    <script src="/skyx/Public/Admin/js/hplus.min.js?v=4.1.0"></script>
    <script type="text/javascript" src="/skyx/Public/Admin/js/contabs.min.js"></script>
    <script src="_PUBLIC_/Admin/js/plugins/pace/pace.min.js"></script>
</body>
</html>