//
//  BibleditHttp.m
//  Bibledit
//
//  Created by Mini on 13-09-14.
//  Copyright (c) 2014 Teus Benshop. All rights reserved.
//

#import "BibleditHttp.h"

@implementation BibleditHttp


+ (NSString *)getHeaderGET:(NSString *)request
{
    NSString * get = @"";

    // Get the value for the GET header.
    NSArray * headers = [request componentsSeparatedByString:@"\n"];
    for (NSString * header in headers) {
        NSString * item = [header stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
        if (item.length >= 1) {
            NSArray * bits = [item componentsSeparatedByCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
            if ([bits count] == 3) {
                NSString * bit0 = [bits objectAtIndex:0];
                if ([bit0 isEqualToString:@"GET"]) {
                    get = [bits objectAtIndex:1];
                }
            }
        }
    }

    // Complete the value if necessary.
    if (([get isEqualToString:@""]) || ([get isEqualToString:@"/"])) {
        get = @"/index.php";
    }
    NSString *lastCharacter = [get substringFromIndex:[get length] - 1];
    if ([lastCharacter isEqualToString:@"/"]) {
        get = [get stringByAppendingString:@"index.php"];
    }
    // The result.
    return get;
}




@end
