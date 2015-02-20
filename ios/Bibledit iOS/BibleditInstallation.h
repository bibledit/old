//
//  BibleditInstallation.h
//  Bibledit
//
//  Created by Mini on 13-09-14.
//  Copyright (c) 2014 Teus Benshop. All rights reserved.
//


#import <Foundation/Foundation.h>


@interface BibleditInstallation : NSObject


+ (void) run;
+ (NSString *) libraryVersion;
+ (NSString *) installedVersion;
+ (void) installedVersion:(NSString *)version;


@end
