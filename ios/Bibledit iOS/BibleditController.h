//
//  BibleditPaths.h
//  Bibledit
//
//  Created by Mini on 13-09-14.
//  Copyright (c) 2014 Teus Benshop. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BibleditController : NSObject

+ (void) bibleditLaunched;
+ (void) bibleditLoaded:(UIView *)uiview;
+ (void) bibleditInstall;
+ (void) bibleditForeground;
+ (void) bibleditView;
+ (void) bibleditBrowse:(NSString*)urlString;
+ (void) bibleditMemory;
+ (void) bibleditBackground;
+ (void) bibleditTerminates;

@end
