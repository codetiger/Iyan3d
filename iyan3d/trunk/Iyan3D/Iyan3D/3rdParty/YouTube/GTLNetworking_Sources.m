#if defined(__has_feature) && __has_feature(objc_arc)
#error "This file needs to be compiled with ARC disabled."
#endif

#import "Fetcher/GTMGatherInputStream.h"
#import "Fetcher/GTMMIMEDocument.h"
#import "Fetcher/GTMReadMonitorInputStream.h"
#import "Fetcher/GTMHTTPFetcher.h"
#import "Fetcher/GTMHTTPFetcherLogging.h"
#import "Fetcher/GTMHTTPFetcherService.h"
#import "Fetcher/GTMHTTPFetchHistory.h"
#import "Fetcher/GTMHTTPUploadFetcher.h"

#import "OAuth/GTMOAuth2Authentication.h"
#import "OAuth/GTMOAuth2SignIn.h"
#if TARGET_OS_IPHONE
  #import "OAuth/Touch/GTMOAuth2ViewControllerTouch.h"
#elif TARGET_OS_MAC
  #import "OAuth/Mac/GTMOAuth2WindowController.h"
#else
  #error Need Target Conditionals
#endif
