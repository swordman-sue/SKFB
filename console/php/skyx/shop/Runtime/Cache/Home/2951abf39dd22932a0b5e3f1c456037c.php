<?php if (!defined('THINK_PATH')) exit();?>﻿<!DOCTYPE html>
<html>

	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, target-densitydpi=high-dpi" />
		<title><?php echo C('title')[CONTROLLER_NAME] ?></title>
		<meta name="Keywords" content="英鸿科技，扬声器，音响系统，网络传输，音频处理" />
		<meta name="Description" content="深圳市英鸿科技有限公司是集数字化专业音频扩声系统研发 制造、销售与服务为一体的大型综合性企业，是专业音视频品牌代理商及音响系统解决方案供应商，多年来，坚持以信誉为基础 高新技术为导向、 市场开拓为突破、客户满意为宗旨的理念，深 得广大经销商、工程商的一致认可。" />
		<link rel="stylesheet" type="text/css" href="/Public/Hone/css/bootstrap.min.css" />
		<link rel="stylesheet" type="text/css" href="/Public/Hone/css/animate.css" />
		<link rel="stylesheet" type="text/css" href="/Public/Hone/css/swiper.min.css" />
		<link rel="stylesheet" type="text/css" href="/Public/Hone/css/swiperstyle.css" />
		<link rel="stylesheet" type="text/css" href="/Public/Hone/css/lunbo.css" />
		<link rel="stylesheet" type="text/css" href="/Public/Hone/css/style.css" />
		<script src="/Public/Hone/js/jquery-1.9.1.min.js"></script>
		<script src="/Public/Hone/js/jquery.SuperSlide.2.1.1.js"></script>
		<script src="/Public/Hone/js/bootstrap.min.js" type="text/javascript"></script>
		<script src="/Public/Hone/js/style.js"></script>
		<script type="text/javascript" src="http://api.map.baidu.com/api?key=&v=1.1&services=true"></script>

	</head>

	<body>
		<!-- 在线QQ客服代码 开始 -->

		<div class="main-im" ondragstart="return false" onselectstart="return false">

			<div id="open_im" class="open-im">&nbsp;</div>

			<div class="im_main" id="im_main">

				<div id="close_im" class="close-im">
					<a href="javascript:void(0);" title="点击关闭">&nbsp;</a>
				</div>

				<a href="http://wpa.qq.com/msgrd?v=3&uin=707831735&site=qq&menu=yes" target="_blank" class="im-qq qq-a" title="在线QQ客服">

					<div class="qq-container"></div>

					<div class="qq-hover-c"><img class="img-qq" src="/Public/Hone/images/qq.png"></div>

					<span> QQ在线咨询</span>

				</a>

				<div class="im-tel">

					<div>客服电话1</div>

					<div class="tel-num"><?php echo ($contact["kephone1"]); ?></div>

					<div>客服电话2</div>

					<div class="tel-num"><?php echo ($contact["kephone2"]); ?></div>

				</div>

				<div class="im-footer" style="position:relative">

					<div class="weixing-container">

						<div class="weixing-show">

							<div class="weixing-txt">微信扫一扫<br>关注客服微信号</div>
							<?php if(is_array($kfwem)): $i = 0; $__LIST__ = $kfwem;if( count($__LIST__)==0 ) : echo "" ;else: foreach($__LIST__ as $key=>$vo): $mod = ($i % 2 );++$i;?><img class="weixing-ma" src="/<?php echo ($vo["picture"]); ?>"><?php endforeach; endif; else: echo "" ;endif; ?>
							<div class="weixing-sanjiao"></div>

							<div class="weixing-sanjiao-big"></div>

						</div>

					</div>

					<div class="go-top" onclick="goTop()">
						<a href="javascript:;" title="返回顶部"></a>
					</div>
					<script>
						function goTop() {
							$('html,body').stop();
							$('html,body').animate({
								'scrollTop': 0
							}, 600);
						}
					</script>
					<div style="clear:both"></div>

				</div>

			</div>

		</div>

		<!-- 代码 结束 -->

		<div class="clear"></div>

		<!--头部-->
		<div class="header base">
			<h1 class="fl logo">
				<a href="index.html" title="深圳市英鸿科技有限公司">
					<img src="/Public/Hone/images/logo.png"/>
				</a>
			</h1>
			<div class="phone-num fr">
				英鸿热线：
				<a href="tel:020-02002000">020-02002000</a>
			</div>
			<ul class="header-ul fl">
				<li>
					<a class="other <?php if(CONTROLLER_NAME == 'Index'){echo 'active';} ?>" href="/">首页</a>
				</li>
				<li>
					<a class="other <?php if(CONTROLLER_NAME == 'Company'){echo 'active';} ?>" href="<?php echo U('Company/index');?>">公司简介</a>
				</li>
				<li>
					<a class="other <?php if(CONTROLLER_NAME == 'Brand'){echo 'active';} ?>" href="<?php echo U('Brand/index');?>">产品中心</a>
				</li>
				<li>
					<a class="other <?php if(CONTROLLER_NAME == 'News'){echo 'active';} ?>" href="<?php echo U('News/index');?>">新闻中心</a>
				</li>
				<li>
					<a class="other <?php if(CONTROLLER_NAME == 'Engineering'){echo 'active';} ?>" href="<?php echo U('Engineering/index');?>">工程案例</a>
				</li>
				<li>
					<a class="other <?php if(CONTROLLER_NAME == 'Service'){echo 'active';} ?>" href="<?php echo U('Service/index');?>">技术服务</a>
				</li>
				<li>
					<a class="other <?php if(CONTROLLER_NAME == 'Contact'){echo 'active';} ?>" href="<?php echo U('Contact/index');?>">联系我们</a>
				</li>
			</ul>
		</div>

		<div class="clear"></div>

		<!--轮播图-->
		<div id="slideBox" class="slideBox slideBox1">
			<div class="hd">
				<ul>

				</ul>
			</div>
			<div class="bd">
				<ul>
					<?php if(is_array($lunbo)): $i = 0; $__LIST__ = $lunbo;if( count($__LIST__)==0 ) : echo "" ;else: foreach($__LIST__ as $key=>$vo): $mod = ($i % 2 );++$i;?><li>
						<a href="javascript:void(0)"><img src="/<?php echo ($vo["picture"]); ?>" /></a>
					</li><?php endforeach; endif; else: echo "" ;endif; ?>
				</ul>
			</div>
			<a class="prev" href="javascript:void(0)"></a>
			<a class="next" href="javascript:void(0)"></a>
		</div>
		<script type="text/javascript">
			//轮播图superslide
			//点居中
			$(document).ready(function() {
				var slidewidth = $('.slideBox1 .hd').width() / 2 * -1 + 'px';
				$('.slideBox1 .hd').css('left', '50%');
				$('.slideBox1 .hd').css('marginLeft', slidewidth);
				//				console.log(slidewidth);
			})

			//动态获取点的个数	
			var slidelength = $('.slideBox1 .bd ul li').length;
			for(var i = 0; i < slidelength; i++) {
				$('.slideBox1 .hd ul').append('<li>' + i + '</li>');
			}

			jQuery(".slideBox1").slide({
				mainCell: ".bd ul",
				titCell: ".hd li", //动态获取点的关键
				autoPlay: true,
				interTime: 4000,
				mouseOverStop: false
			});
		</script>
		<div class="clearfix"></div>

		
		
		<div class="product-all">
			<div class="product-container base">

				<div class="product-left fl">
					<ul class="technical-ul">
						<li>
							<h4>公司介绍丨ABOUT US</h4>
						</li>
						<li>
							<a class="<?php if($id ==$vo[id]){echo 'service-active';} ?> " href="<?php echo U('Company/Brief');?>">公司简介</a>
						</li>
						<?php if(is_array($companyclassify)): $i = 0; $__LIST__ = $companyclassify;if( count($__LIST__)==0 ) : echo "" ;else: foreach($__LIST__ as $key=>$vo): $mod = ($i % 2 );++$i;?><li>
							<a class="<?php if($cat_id ==$vo[cat_id]){echo 'service-active';} ?> " href="<?php echo U('Company/index',array('cat_id'=>$vo[cat_id]));?>"><?php echo ($vo["name"]); ?></a>
						</li><?php endforeach; endif; else: echo "" ;endif; ?>
					</ul>

					<!--<table class="table product-tbl">
						<tr>
							<th>
								<h4>联系我们</h4></th>
						</tr>
						<tr>
							<td><img src="/Public/Hone/images/phone.png" /><span><?php echo ($contact["fwphone"]); ?></span></td>
						</tr>
						<tr>
							<td><img src="/Public/Hone/images/address.png" /><span> <?php echo ($contact["site"]); ?></span></td>	
						</tr>
						<tr>
							<td><img src="/Public/Hone/images/email.png" /><span><?php echo ($contact["mailbox"]); ?></span></td>
						</tr>
					</table>-->
				</div>

				<div class="product-right">
					<div class="product-detail service-detail">

						<h3><?php echo ($com["title"]); ?></h3>
						<div style="height: 2px;background: #9f9f9f;"></div>
						 <img class="company-img" src="/<?php echo ($company["fmpicture"]); ?>" />
						<p>
							<?php echo (stripslashes(htmlspecialchars_decode($com["content"]))); ?>
						</p>

					</div>
				</div>
			</div>
		</div>

		<div class="clearfix"></div>
	<!--底部-->
		<div class="footer base">
			<ul>
				<li>
					<h4>首页</h4>
					<table class="table">
						<tr>
							<td style="width: 22%;">
								<a href="<?php echo U('Company/index');?>">公司简介</a>
							</td>
							<td>
								<a href="<?php echo U('Engineering/index');?>">工程案例</a>
							</td>
						</tr>
						<tr>
							<td style="width: 22%;">
								<a href="<?php echo U('Brand/index');?>">产品中心</a>
							</td>
							<td>
								<a href="<?php echo U('Service/index');?>">技术服务</a>
							</td>
						</tr>
						<tr>
							<td style="width: 22%;">
								<a href="<?php echo U('News/index');?>">新闻中心</a>
							</td>
							<td>
								<a href="<?php echo U('Contact/index');?>">联系我们</a>
							</td>
						</tr>
					</table>
				</li>
				<li>
					<h4>联系我们</h4>
					<table class="table">
						<tr>
							<td>电话号码: <?php echo ($contact["fwphone"]); ?></td>
						</tr>
						<tr>
							<td>邮箱地址:<?php echo ($contact["mailbox"]); ?></td>
						</tr>
						<tr>
							<td>公司地址: <?php echo ($contact["site"]); ?></td>
						</tr>
					</table>
				</li>
				<li>
					<img class="fr" src="/<?php echo ($dibewm["picture"]); ?>" />
				</li>
			</ul>
			<p><?php echo ($contact["copyright"]); ?></p>
		</div>