//
//  IpaInstallController.h
//  project1
//
//  Created by liwei on 13-9-8.
//  Copyright (c) 2013? shzx. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface IpaInstallController : UIViewController

@property(nonatomic, retain) UIActivityIndicatorView * activityIndicator;
@property(nonatomic, retain) UILabel * titleIndicator;
@property(nonatomic, retain) UIButton * buttonIndicator;
@property(nonatomic, retain) NSString * titleText;
@property(nonatomic, retain) NSString * path;

- (IBAction)install;
- (IBAction)installShowIndicator;
- (id)initwithPath:(NSString *)path WithTitleText:(NSString *)titleText;

@end
