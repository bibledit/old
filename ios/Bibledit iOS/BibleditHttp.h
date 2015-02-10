//
//  BibleditHttp.h
//  Bibledit
//
//  Created by Mini on 13-09-14.
//  Copyright (c) 2014 Teus Benshop. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface BibleditHttp : NSObject

+ (NSString *)getHeaderGET:(NSString *)request;

@end
