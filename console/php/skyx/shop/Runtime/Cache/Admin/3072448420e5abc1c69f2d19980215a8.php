<?php if (!defined('THINK_PATH')) exit();?><!DOCTYPE html>
<html>
<head>

    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">


    <title>后台主页</title>
    <meta name="keywords" content="响应式后台">
    <meta name="description" content="响应式后台">

    <link rel="shortcut icon" href="favicon.ico"> <link href="/skyx/Public/Admin/css/bootstrap.min14ed.css?v=3.3.6" rel="stylesheet">
    <link href="/skyx/Public/Admin/css/font-awesome.min93e3.css?v=4.4.0" rel="stylesheet">

    <!-- Morris -->
    <link href="/skyx/Public/Admin/css/plugins/morris/morris-0.4.3.min.css" rel="stylesheet">

    <!-- Gritter -->
    <link href="/skyx/Public/Admin/js/plugins/gritter/jquery.gritter.css" rel="stylesheet">

    <link href="/skyx/Public/Admin/css/animate.min.css" rel="stylesheet">
    <link href="/skyx/Public/Admin/css/style.min862f.css?v=4.1.0" rel="stylesheet">

</head>

<body class="gray-bg">
    <div class="wrapper wrapper-content">
        <div class="row">
            <div class="col-sm-3">
                <div class="ibox float-e-margins">
                    <div class="ibox-title">
                        <span class="label label-success pull-right">localhost</span>
                        <h5>操作系统</h5>
                    </div>
                    <div class="ibox-content">
                        <h1 class="no-margins">linux</h1>
                    </div>
                </div>
            </div>
            <div class="col-sm-3">
                <div class="ibox float-e-margins">
                    <div class="ibox-title">
                        <span class="label label-info pull-right">apache</span>
                        <h5>服务器版本</h5>
                    </div>
                    <div class="ibox-content">
                        <h1 class="no-margins">apache 2.4</h1>
                    </div>
                </div>
            </div>
            <div class="col-sm-3">
                <div class="ibox float-e-margins">
                    <div class="ibox-title">
                        <span class="label label-primary pull-right">mysql</span>
                        <h5>数据库版本</h5>
                    </div>
                    <div class="ibox-content">
                        <h1 class="no-margins">mysql 5.6</h1>
                    </div>
                </div>
            </div>
            <div class="col-sm-3">
                <div class="ibox float-e-margins">
                    <div class="ibox-title">
                        <span class="label label-danger pull-right">PHP</span>
                        <h5>php版本</h5>
                    </div>
                    <div class="ibox-content">
                        <h1 class="no-margins">5.5</h1>
                    </div>
                </div>
            </div>
        </div>
		
		<!--
        <div class="row">
            <div class="col-sm-4">
                <div class="ibox float-e-margins">
                    <div class="ibox-title">
                        <h5>留言列表</h5>
                        <div class="ibox-tools">
                            <a class="collapse-link">
                                <i class="fa fa-chevron-up"></i>
                            </a>
                            <a class="close-link">
                                <i class="fa fa-times"></i>
                            </a>
                        </div>
                    </div>
                    <div class="ibox-content ibox-heading">
                        <h3><i class="fa fa-envelope-o"></i> 新留言</h3>
                        <small><i class="fa fa-tim"></i> 您有22条未审核留言</small>
                    </div>
                    <div class="ibox-content">
                        <div class="feed-activity-list">
							
							
                            <div class="feed-element">
                                <div>
                                    <small class="pull-right text-navy">1月前</small>
                                    <strong>井幽幽</strong>
                                    <div>有人说：“一辈子很长，要跟一个有趣的人在一起”。我想关注我的人，应该是那种喜欢找乐子也乐意分享乐趣的人，你们一定挺优秀的。所以单身的应该在这条留言，互相勾搭一下。特别有钱人又帅可以直接私信我！</div>
                                    <small class="text-muted">4月11日 00:00</small>
                                </div>
                            </div>

                            <div class="feed-element">
                                <div>
                                    <small class="pull-right">2月前</small>
                                    <strong>马伯庸 </strong>
                                    <div>又方便，又防水，手感又好，还可以用手机遥控。简直是拍戏利器，由其是跟老师们搭戏的时候…想想还有点小激动啊，嘿嘿。</div>
                                    <small class="text-muted">11月8日 20:08 </small>
                                </div>
                            </div>

                            <div class="feed-element">
                                <div>
                                    <small class="pull-right">5月前</small>
                                    <strong>芒果宓 </strong>
                                    <div>一个完整的梦。</div>
                                    <small class="text-muted">11月8日 20:08 </small>
                                </div>
                            </div>

                            <div class="feed-element">
                                <div>
                                    <small class="pull-right">5月前</small>
                                    <strong>刺猬尼克索</strong>
                                    <div>哈哈哈哈 你卖什么萌啊! 蠢死了</div>
                                    <small class="text-muted">11月8日 20:08 </small>
                                </div>
                            </div>


                            <div class="feed-element">
                                <div>
                                    <small class="pull-right">5月前</small>
                                    <strong>老刀99</strong>
                                    <div>昨天评论里你见过最“温暖和感人”的诗句，整理其中经典100首，值得你收下学习。</div>
                                    <small class="text-muted">11月8日 20:08 </small>
                                </div>
                            </div>
                            <div class="feed-element">
                                <div>
                                    <small class="pull-right">5月前</small>
                                    <strong>娱乐小主 </strong>
                                    <div>你是否想过记录自己的梦？你是否想过有自己的一个记梦本？小时候写日记，没得写了就写昨晚的梦，后来变成了习惯………翻了一晚上自己做过的梦，想哭，想笑…</div>
                                    <small class="text-muted">11月8日 20:08 </small>
                                </div>
                            </div>
                            <div class="feed-element">
                                <div>
                                    <small class="pull-right">5月前</small>
                                    <strong>DMG电影 </strong>
                                    <div>《和外国男票乘地铁，被中国大妈骂不要脸》妹子实在委屈到不行，中国妹子找外国男友很令人不能接受吗？大家都来说说自己的看法</div>
                                    <small class="text-muted">11月8日 20:08 </small>
                                </div>
                            </div>
							

                        </div>
                    </div>
                </div>
            </div>-->

			<!--
            <div class="col-sm-8">

                <div class="row">
                    <div class="col-sm-6">
                        <div class="ibox float-e-margins">
                            <div class="ibox-title">
                                <h5>会员列表</h5>
                                <div class="ibox-tools">
                                    <a class="collapse-link">
                                        <i class="fa fa-chevron-up"></i>
                                    </a>
                                    <a class="close-link">
                                        <i class="fa fa-times"></i>
                                    </a>
                                </div>
                            </div>
                            <div class="ibox-content">
                                <table class="table table-hover no-margins">
                                    <thead>
                                        <tr>
                                            <th><i class="fa fa-user"></i>昵称</th>
                                            <th><i class="fa fa-sitemap"></i>等级</th>
                                            <th><i class="fa fa-mobile"></i>手机号码</th>
                                            <th>性别</th>
                                        </tr>
                                    </thead>
                                    <tbody>

                                        <tr>
                                            <td>donate
                                            </td>
                                            <td><span class="label label-primary">白金会员</span></td>
                                            <td>15989196759</td>
                                            <td class="text-navy"> <i class="fa fa-male"></i>男</td>
                                        </tr>
                                        <tr>
                                            <td>青衣5858
                                            </td>
                                            <td><span class="label label-warning">黄金会员</span></td>
                                            <td>15989196759</td>
                                            <td class="text-navy"><i class="fa fa-female"></i>女</td>
                                        </tr>
                                        <tr>
                                            <td>next
                                            </td>
                                            <td><span class="label label-default">普通会员</span></td>
                                            <td>15989196759</td>
                                            <td class="text-navy"><i class="fa fa-male"></i>女</td>
                                        </tr>

                                    </tbody>
                                </table>
                            </div>
                        </div>
                    </div>
                    <div class="col-sm-6">
                        <div class="ibox float-e-margins">
                            <div class="ibox-title">
                                <h5>各种总数</h5>
                                <div class="ibox-tools">
                                    <a class="collapse-link">
                                        <i class="fa fa-chevron-up"></i>
                                    </a>
                                    <a class="close-link">
                                        <i class="fa fa-times"></i>
                                    </a>
                                </div>
                            </div>
                            <div class="ibox-content">
                                <ul class="todo-list m-t small-list ui-sortable">
                                    <li>
                                        <a href="widgets.html#" class="check-link"><i class="fa fa-star"></i> </a>
                                        <span class="m-l-xs ">留言总数</span>
                                        <small class="label label-primary pull-right">100</small>
                                    </li>
                                    <li>
                                        <a href="widgets.html#" class="check-link"><i class="fa fa-star"></i> </a>
                                        <span class="m-l-xs  ">会员总数</span>
                                        <small class="label label-primary pull-right">100</small>
                                    </li>
                                    <li>
                                        <a href="widgets.html#" class="check-link"><i class="fa fa-star"></i> </a>
                                        <span class="m-l-xs">产品总数</span>
                                        <small class="label label-primary pull-right">100</small>
                                    </li>
                                    <li>
                                        <a href="widgets.html#" class="check-link"><i class="fa fa-star"></i> </a>
                                        <span class="m-l-xs">文章总数</span>
                                        <small class="label label-primary pull-right">100</small>
                                    </li>
                                </ul>
                            </div>
                        </div>
                    </div>
                </div> -->
				
				<!--
                <div class="row">
                    <div class="col-sm-12">
                        <div class="ibox float-e-margins">
                            <div class="ibox-title">
                                <h5>管理员管理</h5>
                                <div class="ibox-tools">
                                    <a class="collapse-link">
                                        <i class="fa fa-chevron-up"></i>
                                    </a>
                                    <a class="close-link">
                                        <i class="fa fa-times"></i>
                                    </a>
                                </div>
                            </div>
                            <div class="ibox-content">

                                <div class="row">
                                    <div class="col-sm-10">
                                        <table class="table table-hover margin bottom">
                                            <thead>
                                                <tr>
                                                    <th style="width:15%" class="text-center">序号</th>
                                                    <th>名称</th>
                                                    <th class="text-center">创建时间</th>
                                                    <th class="text-center">头像</th>
                                                </tr>
                                            </thead>
                                            <tbody>
                                                <tr>
                                                    <td class="text-center">1</td>
                                                    <td>donate
                                                        </small>
                                                    </td>
                                                    <td class="text-center small"><i class="fa fa-clock-o"></i> 2014.9.15</td>
                                                    <td class="text-center"><img src="/skyx/Public/Admin/img/profile_small.jpg" class="img-circle" alt="">
                                                    </td>

                                                </tr>
                                            </tbody>
                                        </table>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div> -->
				
            </div>
        </div>
    </div>

    <script src="/skyx/Public/Admin/js/jquery.min.js?v=2.1.4"></script>
    <script src="/skyx/Public/Admin/js/bootstrap.min.js?v=3.3.6"></script>
    <script src="/skyx/Public/Admin/js/plugins/flot/jquery.flot.js"></script>
    <script src="/skyx/Public/Admin/js/plugins/flot/jquery.flot.tooltip.min.js"></script>
    <script src="/skyx/Public/Admin/js/plugins/flot/jquery.flot.spline.js"></script>
    <script src="/skyx/Public/Admin/js/plugins/flot/jquery.flot.resize.js"></script>
    <script src="/skyx/Public/Admin/js/plugins/flot/jquery.flot.pie.js"></script>
    <script src="/skyx/Public/Admin/js/plugins/flot/jquery.flot.symbol.js"></script>
    <script src="/skyx/Public/Admin/js/plugins/peity/jquery.peity.min.js"></script>
    <script src="/skyx/Public/Admin/js/demo/peity-demo.min.js"></script>
    <script src="/skyx/Public/Admin/js/content.min.js?v=1.0.0"></script>
    <script src="/skyx/Public/Admin/js/plugins/jquery-ui/jquery-ui.min.js"></script>
    <script src="/skyx/Public/Admin/js/plugins/sparkline/jquery.sparkline.min.js"></script>
    <script src="/skyx/Public/Admin/js/demo/sparkline-demo.min.js"></script>
</body>


<!-- Mirrored from www.zi-han.net/theme/hplus/index_v3.html by HTTrack Website Copier/3.x [XR&CO'2014], Wed, 20 Jan 2016 14:18:49 GMT -->
</html>