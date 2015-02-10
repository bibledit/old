//
//  WebServer.m
//  Bibledit
//
//  Created by Mini on 22-09-14.
//  Copyright (c) 2014 Teus Benshop. All rights reserved.
//

#import "WebServer.h"
#import <arpa/inet.h>
#import <signal.h>
#import <dirent.h>
#import <errno.h>
#import <fcntl.h>
#import <time.h>
#import <netinet/in.h>
#import <netinet/tcp.h>
#import <stdio.h>
#import <stdlib.h>
#import <string.h>
#import <sys/socket.h>
#import <sys/stat.h>
#import <sys/types.h>
#import <unistd.h>
#include <stdio.h>
#include <sapi/embed/php_embed.h>
#import "BibleditHttp.h"
#import "BibleditPaths.h"


@implementation WebServer


NSString * ubwrite;


static int bibledit_ub_write (const char *str, unsigned int str_length TSRMLS_DC)
{
    //printf ("%s", str);
    NSString * string = [NSString stringWithCString:str encoding:NSUTF8StringEncoding];
    ubwrite = [ubwrite stringByAppendingString:string];
    return str_length;
}


+ (void) run
{
    // Create a socket descriptor.
    int listenfd = socket (AF_INET, SOCK_STREAM, 0);
    int optval = 1;

    // Eliminate "Address already in use" error from bind.
    setsockopt (listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof (int));
    
    // 6 is TCP's protocol number
    // Enable this, much faster : 4000 req/s -> 17000 req/s
    setsockopt (listenfd, 6, TCP_NOPUSH, (const void *) &optval , sizeof (int));
    
    // Listenfd will be an endpoint for all requests to port on any IP address for this host.
    typedef struct sockaddr SA;
    struct sockaddr_in serveraddr;
    memset (&serveraddr, 0, sizeof (serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)8080);
    bind (listenfd, (SA *) &serveraddr, sizeof (serveraddr));
        
    // Make it a listening socket ready to accept many connection requests.
    listen (listenfd, 100);
    
    // Ignore SIGPIPE signal: When the browser cancels the request, it won't kill the app.
    signal(SIGPIPE, SIG_IGN);

    // Keep waiting for, accepting, and processing connections.
    while (YES) {
        
        // Socket and file descriptor for the client connection.
        struct sockaddr_in clientaddr;
        socklen_t clientlen = sizeof (clientaddr);
        int connfd = accept (listenfd, (SA *) &clientaddr, &clientlen);
        // &clientaddr contains client info.

        // Read the client's request.
        char buffer[65535];
        size_t bytes_read;
        bytes_read = read (connfd, buffer, sizeof (buffer));
        NSString * request = [NSString stringWithCString:buffer encoding:NSUTF8StringEncoding];

        // The GET url.
        NSString * get = [BibleditHttp getHeaderGET:request];
        NSLog(get, @"");

        
        NSArray *components = [NSArray arrayWithObjects:[BibleditPaths documents], @"web", get, nil];
        NSString *path = [NSString pathWithComponents:components];

        
        //NSString *content = [NSString stringWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
        //NSLog(content, @"");

        
        // File handle structure.
        zend_file_handle script;
        script.type = ZEND_HANDLE_FP;
        script.filename = [path UTF8String];
        script.opened_path = NULL;
        script.free_filename = 0;
        if ((script.handle.fp = fopen(script.filename, "rb"))) {
            
            // PHP output handlers.
            ubwrite = [NSMutableString string];
            php_embed_module.name = "bibledit";
            php_embed_module.pretty_name = "Embedded PHP library for iOS (https://github.com/teusbenschop/php-ios)";
            php_embed_module.ub_write = bibledit_ub_write;
            php_embed_module.flush = NULL;
            php_embed_module.log_message = NULL;
            php_embed_module.header_handler = NULL;
            php_embed_module.send_headers = NULL;
            php_embed_module.send_header = NULL;
            
            // After the PHP_EMBED_START_BLOCK() will have been called,
            // the application is positioned at the start of a PHP request cycle,
            // just after the completion of RINIT callbacks.
            int argc = 0;
            char * argv[] = { NULL };
            PHP_EMBED_START_BLOCK (argc, argv)
            
            // Make settings like in php.ini.
            zend_alter_ini_entry ("opcache.enable", sizeof ("opcache.enable"), "0", sizeof ("0") - 1, PHP_INI_ALL, PHP_INI_STAGE_RUNTIME);

            // Run the PHP script.
            php_execute_script (&script TSRMLS_CC);
            
            // Shutdown the PHP library.
            PHP_EMBED_END_BLOCK ()
            
        }
        
        // Determine the length of the content, in bytes.
        NSUInteger bytes = [ubwrite lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
        NSString *content_length = [NSString stringWithFormat:@"Content-Length: %d", bytes];
        
        NSMutableArray * response = [[NSMutableArray alloc] init];
        [response addObject: @"HTTP/1.1 200 OK"];
        [response addObject: content_length];
        [response addObject: @"Content-Type: text/html;charset=UTF-8"];
        [response addObject: @""];
        if (ubwrite != nil) {
            [response addObject: ubwrite];
        }
        
        NSString * reply = [[response valueForKey:@"description"] componentsJoinedByString:@"\n"];
        
        const char * output = [reply UTF8String];
        size_t length = strlen (output);
        write (connfd, output, length);
       
        
        
        
        
        
        close (connfd);
    }
}





@end
