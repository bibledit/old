//
//  BibleditPaths.m
//  Bibledit
//
//  Created by Mini on 13-09-14.
//  Copyright (c) 2014 Teus Benshop. All rights reserved.
//

#import "BibleditPaths.h"

@implementation BibleditPaths


+ (NSString *)resources
{
    NSString * path = [[NSBundle mainBundle] resourcePath];
    return path;
}


+ (NSString *)library
{
    NSString * path = [NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    return path;
}


+ (NSString *)documents
{
    NSString * path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    return path;
}


@end
