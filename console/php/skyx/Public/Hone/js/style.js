$(function() {
	$(document).ready(function(){
		var prheight = $('.product-right').height();
		var plheight = $('.product-left').height();
		if (plheight>=prheight) {
			prheight = $('.product-right').height(plheight)
		}
		if (plheight<prheight) {
			plheight = $('.product-left').height(prheight)
		}
	})
	
})

//客服
$(function(){
	$(document).ready(function(){
		// $('#im_main').fadeOut();
		// $('.main-im').css("width","40px");
		// $('.main-im .open-im').css('margin-left','0');
		// $('#open_im').css('display','block');
		// $('.weixing-show').fadeIn();
		$('.weixing-show').css('display','none');
		$('.main-im').css("width","110px");
		$('#open_im').css('display','none');
		$('.main-im .open-im').css('margin-left','68px');
		$('#main-im').css("height","272px");
		$('#im_main').fadeIn();
		$(this).css('display','none');
	})
	$('#close_im').bind('click',function(){
		$('.main-im').css("width","40px");
		$('.main-im .open-im').css('margin-left','0');
		$('#main-im').css("height","0");
		$('#im_main').css('display','none');
		$('#open_im').fadeIn();
	});
	$('#open_im').bind('click',function(e){
		$('.main-im').css("width","110px");
		$('.main-im .open-im').css('margin-left','68px');
		$('#main-im').css("height","272px");
		$('#im_main').fadeIn();
		$(this).css('display','none');
	});
	/*$('.go-top').bind('click',function(){
		$(window).scrollTop(0);
	});*/
	
	$(".weixing-container").bind('mouseenter',function(){
		$('.weixing-show').show();
	})
	$(".weixing-container").bind('mouseleave',function(){        
		$('.weixing-show').hide();
	});
});

$(function(){
		$('#submit-yh').click(function(){
			//去掉所有空格后再判断是否为空
			if($('#exampleInputName2').val().replace(/(^\s*)|(\s*$)/g, "") == "") {
				alert("姓名不能为空，请重填");
				$('#exampleInputName2').focus();
				return false;
			}
		    if(!(/^1[34578]\d{9}$/.test($('#exampleInputphone2').val()))){ 
		        alert("手机号码有误，请重填");
		        $('#exampleInputphone2').focus();  
		        return false; 
		    } 
			if(!(/^([a-zA-Z0-9_-])+@([a-zA-Z0-9_-])+(.[a-zA-Z0-9_-])+/).test($('#exampleInputEmail2').val())) {
				alert("邮箱输入有误，请重填");
				$('#exampleInputEmail2').focus();
				return false;
			}
			if($('#exampleInputdetail2').val().replace(/(^\s*)|(\s*$)/g, "") == "") {
				alert("详情不能为空，请重填");
				$('#exampleInputdetail2').focus();
				return false;
			}
			
		})
})