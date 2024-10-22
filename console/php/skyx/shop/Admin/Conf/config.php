<?php
return array(

    'DEFAULT_MODULE'     => 'Index', //默认模块
    'URL_CASE_INSENSITIVE'  =>  false,   // 默认false
   // 'SESSION_AUTO_START' => true, //是否开启session
    'URL_MODEL'          => '0', //URL模式
    'ERROR_MESSAGE'         =>  '页面错误！请稍后再试～',
    'TMPL_CACHE_ON'         =>  false,    // 是否开启模板编译缓存,设为false则每次都会重新编译
    //错误显示信息,非调试模式有效
    'SHOW_ERROR_MSG'        =>  true,    // 显示错误信息
    // 加载扩展配置文件
    'LOAD_EXT_CONFIG' => 'db',

    'TMPL_ACTION_ERROR'     =>  'Public/error', // 默认错误跳转对应的模板文件
    'TMPL_ACTION_SUCCESS'   =>  'Public/success', // 默认成功跳转对应的模板文件
   // 'TMPL_ACTION_SUCCESS'   =>  'Public/qux', // 默认成功跳转对应的模板文件
);