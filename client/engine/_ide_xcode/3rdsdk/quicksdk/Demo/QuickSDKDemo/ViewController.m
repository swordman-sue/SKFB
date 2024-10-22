//
//  ViewController.m
//  QuickPlatforms
//
//

#import "ViewController.h"
#import <CommonCrypto/CommonDigest.h> // CC_MD5
#import <SMPCQuickSDK/SMPCQuickSDK.h>
#import "AppDelegate.h"
#import "MacroHelper.h"

@interface ViewController ()

//平台信息
@property (weak, nonatomic) IBOutlet UILabel *channelLab; //渠道标志


//用户信息
@property (weak, nonatomic) IBOutlet UILabel *userNameLab;
@property (weak, nonatomic) IBOutlet UILabel *userIdLab;


//按钮
@property (weak, nonatomic) IBOutlet UIButton *loginButton;
@property (weak, nonatomic) IBOutlet UIButton *logoutButton;
@property (weak, nonatomic) IBOutlet UIButton *rechargeButton;
@property (weak, nonatomic) IBOutlet UIButton *toolbarButton;
@property (weak, nonatomic) IBOutlet UIButton *settingButton;


@end

@implementation ViewController


#pragma mark -
#pragma mark - life cycle

- (void)viewDidLoad {
    [super viewDidLoad];
#ifdef QUDAO_NAME
    //配合在打包工具上添加的宏以达到控制渠道特殊处理
    NSLog(@"------------------QUDAO_NAME");
#endif
    // Do any additional setup after loading the view from its nib.
    //添加对QuickSDK通知的监听
    //登录
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(smpcQpLoginResult:) name:kSmpcQuickSDKNotiLogin object:nil];
    //注销
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(smpcQpLogoutResult:) name:kSmpcQuickSDKNotiLogout object:nil];
    //充值结果
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(smpcQpRechargeResult:) name:kSmpcQuickSDKNotiRecharge object:nil];
    //暂停结束 这个回调可以不用添加
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(smpcQpPauseOver:) name:kSmpcQuickSDKNotiPauseOver object:nil];
    [self loadMainView];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


#pragma mark -
#pragma mark - Public Method

-(void)loadMainView {
    [self.channelLab setText:[NSString stringWithFormat:@"%d",[[SMPCQuickSDK defaultInstance] channelType]]];
}


#pragma mark -
#pragma mark - Pravire Method



#pragma mark -
#pragma mark - Notifications

- (void)smpcQpLoginResult:(NSNotification *)notify {
    NSLog(@"登录成功通知%@",notify);
    int error = [[[notify userInfo] objectForKey:kSmpcQuickSDKKeyError] intValue];
    NSDictionary *userInfo = [notify userInfo];
    if (error == 0) {
        NSString *uid = [[SMPCQuickSDK defaultInstance] userId];
        NSString *user_name = [[SMPCQuickSDK defaultInstance] userNick];
        //获取user_token，用于从服务器去验证用户信息
        NSString *user_token = userInfo[kSmpcQuickSDKKeyUserToken];
        [self.loginButton setHidden:YES];
        [self.logoutButton setHidden:NO];
        [self.rechargeButton setHidden:NO];
        [self.toolbarButton setHidden:NO];
        [self.settingButton setHidden:NO];
        
        [self.userNameLab setText:user_name];
        [self.userIdLab setText:uid];
        
        // 更新角色信息
        SMPCQuickSDKGameRoleInfo *gameRoleInfo = [SMPCQuickSDKGameRoleInfo new];
        gameRoleInfo.serverName = @"serverName";
        gameRoleInfo.gameRoleName = @"roleName";
        gameRoleInfo.serverId = @"1"; //需要是数字字符串
        gameRoleInfo.gameRoleID = uid;
        gameRoleInfo.gameUserBalance = @"0";
        gameRoleInfo.vipLevel = @"8";
        gameRoleInfo.gameUserLevel = @"50";
        gameRoleInfo.partyName = @"";
        [[SMPCQuickSDK defaultInstance] updateRoleInfoWith:gameRoleInfo isCreate:NO];//如果这个角色是刚刚创建的，这里isCreate可以传YES

    }
}

- (void)smpcQpLogoutResult:(NSNotification *)notify {
    NSLog(@"%s",__func__);
    NSDictionary *userInfo = notify.userInfo;
    int errorCode = [userInfo[kSmpcQuickSDKKeyError] intValue];
    switch (errorCode) {
        case SMPC_QUICK_SDK_ERROR_NONE:
        {
            NSLog(@"注销成功");
            //注销成功
            [self.loginButton setHidden:NO];
            [self.logoutButton setHidden:YES];
            [self.rechargeButton setHidden:YES];
            [self.toolbarButton setHidden:YES];
            [self.settingButton setHidden:YES];
            [self.userNameLab setText:@""];
            [self.userIdLab setText:@""];
        }
            break;
        case SMPC_QUICK_SDK_ERROR_LOGOUT_FAIL:
        default:
        {
            //注销失败
            NSLog(@"注销失败");
        }
            break;
    }
    if (errorCode == SMPC_QUICK_SDK_ERROR_NONE) {
        
    }
    
}
- (void)smpcQpRechargeResult:(NSNotification *)notify{
    NSLog(@"充值结果%@",notify);
    NSDictionary *userInfo = notify.userInfo;
    int error = [[userInfo objectForKey:kSmpcQuickSDKKeyError] intValue];
    switch (error) {
        case SMPC_QUICK_SDK_ERROR_NONE:
        {
            //充值成功
            //QuickSDK订单号,cp下单时传入的订单号，渠道sdk的订单号，cp下单时传入的扩展参数
            NSString *orderID = userInfo[kSmpcQuickSDKKeyOrderId];
            NSString *cpOrderID = userInfo[kSmpcQuickSDKKeyCpOrderId];
            NSLog(@"充值成功数据：%@,%@",orderID,cpOrderID);
        }
            break;
        case SMPC_QUICK_SDK_ERROR_RECHARGE_CANCELLED:
        case SMPC_QUICK_SDK_ERROR_RECHARGE_FAILED:
        {
            //充值失败
            NSString *orderID = userInfo[kSmpcQuickSDKKeyOrderId];
            NSString *cpOrderID = userInfo[kSmpcQuickSDKKeyCpOrderId];
            NSLog(@"充值失败数据%@,%@",orderID,cpOrderID);
        }
            break;
        default:
            break;
    }
}
- (void)smpcQpPauseOver:(NSNotification *)notify{
    NSLog(@"收到QuickSDK暂停结束通知");
}

#pragma mark -
#pragma mark - Button Action
- (IBAction)onLogin{
    int error = [[SMPCQuickSDK defaultInstance] login];
    if (error != 0) {
        NSLog(@"%d",error);
    }
}
- (IBAction)onRecharge:(id)sender{
    //充值
    CFUUIDRef theUUID = CFUUIDCreate(NULL);
    CFStringRef guid = CFUUIDCreateString(NULL, theUUID);
    CFRelease(theUUID);
    NSString *uuidString = [((__bridge NSString *)guid) stringByReplacingOccurrencesOfString:@"-" withString:@""];
    CFRelease(guid);
    
    SMPCQuickSDKGameRoleInfo *role = [[SMPCQuickSDKGameRoleInfo alloc] init];
    SMPCQuickSDKPayOrderInfo *order = [[SMPCQuickSDKPayOrderInfo alloc] init];
    role.serverName = @"今昔何夕"; //必填
    role.gameRoleName = @"ios tester";//@""
    role.serverId = @"1"; //需要是数字字符串
    role.gameRoleID = @"000001";//
    role.gameUserBalance = @"0";//
    role.vipLevel = @"1";//
    role.gameUserLevel = @"1";
    role.partyName = @"ios";//
    order.goodsID = [NSString stringWithFormat:@"%d",1]; //必填 iap时注意和苹果开发者后台一致，或者渠道映射的
    order.productName = @"元宝";//必填
    order.cpOrderID = [uuidString lowercaseString]; //必填 游戏订单号
    order.count = 10;  //必填 数量
    order.amount = 1; //必填 总价
    order.callbackUrl = @"";
    order.extrasParams = @"This is extrasParams!!!";
    //个别渠道要求单价*数量==总价
    if([SMPCQuickSDK defaultInstance].channelType == 9999){
        //通过判断渠道号处理特定渠道的参数
        order.goodsID = @"productlist.name";
    }
    int error = [[SMPCQuickSDK defaultInstance] payOrderInfo:order
                                                    roleInfo:role];
    if (error!=0)
        NSLog(@"%d", error);
}
- (IBAction)onLogout:(id)sender{
    //代码注销
    [[SMPCQuickSDK defaultInstance] logout];
}
- (IBAction)buttonAction:(id)sender {
    switch (((UIButton*)sender).tag) {
        case 5000: {
            //个人中心
            int error = [[SMPCQuickSDK defaultInstance] enterUserCenter];
            if (error != 0) {
                NSLog(@"%d",error);
            }
            
        }   break;
        case 9998: {
            //显示工具条
            [self.toolbarButton setUserInteractionEnabled:NO];
            [self.toolbarButton setTitle:@"隐藏工具条" forState:UIControlStateNormal];
            [self.toolbarButton setTag:9999];
            [[SMPCQuickSDK defaultInstance] showToolBar:SMPC_QUICK_SDK_TOOLBAR_TOP_LEFT];
            [self.toolbarButton setUserInteractionEnabled:YES];
        }   break;
        case 9999: {
            //隐藏工具条
            [self.toolbarButton setUserInteractionEnabled:NO];
            [self.toolbarButton setTitle:@"显示工具条" forState:UIControlStateNormal];
            [self.toolbarButton setTag:9998];
            [[SMPCQuickSDK defaultInstance] hideToolBar];
            [self.toolbarButton setUserInteractionEnabled:YES];
        }   break;
        default:
            break;
    }
}


#pragma mark -
#pragma mark - StatusBarHidden

- (BOOL)prefersStatusBarHidden {
    return YES;
}


#pragma mark -
#pragma mark - Autorotate

// Override to allow orientations other than the default portrait orientation.
//- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
//    // Return YES for supported orientations
//    if (!IS_DEVICE_IPAD) {
//        return (UIInterfaceOrientationIsLandscape(interfaceOrientation) || UIInterfaceOrientationPortrait == interfaceOrientation);
//    }
//    return UIDeviceOrientationIsValidInterfaceOrientation(interfaceOrientation);
//}
//
//
//- (BOOL)shouldAutorotate {
//    return YES;
//}
//
//- (UIInterfaceOrientationMask)supportedInterfaceOrientations {
//    return UIInterfaceOrientationMaskLandscape;
//}

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation {
    return self.interfaceOrientation;
}


@end
