//
//  Utils.m
//  NativeAPI
//
//  Created by Artem Kirichek on 11/2/17.
//

#import "Utils.h"

@implementation Utils

+ (wchar_t *)wcharFromNSString:(NSString *)nsString {
    size_t length = nsString.length;
    
    wchar_t* wcharString = new wchar_t[length + 3];
    
    for (int i = 0; i < length; i++) {
        if (i < length) {
            wcharString[i] = [nsString characterAtIndex:i];
        }
    }
    
    wcharString[length] = 0x00;
    wcharString[length + 1] = 0x00;
    wcharString[length + 2] = 0x00;
    
    return wcharString;
}

@end
