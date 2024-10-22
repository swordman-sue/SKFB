
Macros = Macros or {}

Macros.SDK =
{
    --SDK相关常量
    Constants = {
        --成功的回调码
        SUCCESS_CODE = 0,
        --失败的回调码
        FAILURE_CODE = 1,
        --取消的回调码
        CANCEL_CODE = 2,
        --登出回调
        LOGOUT_CODE = 3,
        --充值成功回调
        RECHARGE_CODE = 4,
        --切换账号回调
        SWITCH_ACCOUNT_CODE = 5,
        --调用QK调用操作函数成功
        COMMON_CALLFUNC_SUCCESS = 6,
        --调用QK调用操作函数失败
        COMMON_CALLFUNC_FAILED = 7,
    },

    --CP渠道号
    CPChannelType = 
    {
        --996BT
        NNS_BT = 10000,
    },

    --QuickSDK渠道号
    QKChannelType = 
    {
        --小米
        MI = 15,

        --游戏FAN(新)
        YOUXI_FAN = 926,
    },

    --是否走CP实现的苹果内购流程
    CPIAPToggleMap = 
    {
    },
   
    --Adjust事件
    AdjustEvent = 
    {
        LOGIN_SUCCESS = 1,
        LOGIN_FACEBOOK = 2,
        LOGIN_VISITOR = 3,
        --月卡充值
        PAY_MONTH_CARD = 4,
        --普通充值
        PAY_COMMON = 5,
    },

    --自定义事件
    CustomEvent = 
    {
        --进入加载画面
        RES_UPDATE_ENTER = 1,
        --通过加载画面
        RES_UPDATE_FINISHED = 2,

        --进入创建角色
        ROLE_CREATED_ENTER = 3,        
        --通过创建角色
        ROLE_CREATED = 4,

        --完成新手引导
        LEAD_FINISHED = 5,
     }
}
