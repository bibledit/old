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
    NSArray *components = [NSArray arrayWithObjects:path, @"webroot", nil];
    path = [NSString pathWithComponents:components];
    return path;
}


+ (NSString *)documents
{
    NSString * path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSArray *components = [NSArray arrayWithObjects:path, @"webroot", nil];
    path = [NSString pathWithComponents:components];
    return path;
}


@end
