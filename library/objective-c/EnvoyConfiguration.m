#import "library/objective-c/EnvoyEngine.h"

#import "library/common/main_interface.h"

@implementation EnvoyConfiguration

- (instancetype)initWithStatsDomain:(nullable NSString *)statsDomain
              connectTimeoutSeconds:(UInt32)connectTimeoutSeconds
                  dnsRefreshSeconds:(UInt32)dnsRefreshSeconds
       dnsFailureRefreshSecondsBase:(UInt32)dnsFailureRefreshSecondsBase
        dnsFailureRefreshSecondsMax:(UInt32)dnsFailureRefreshSecondsMax
                  statsFlushSeconds:(UInt32)statsFlushSeconds
           streamIdleTimeoutSeconds:(UInt32)streamIdleTimeoutSeconds
                         appVersion:(NSString *)appVersion
                              appId:(NSString *)appId
                    virtualClusters:(NSString *)virtualClusters
             directResponseMatchers:(NSString *)directResponseMatchers
                    directResponses:(NSString *)directResponses
                  nativeFilterChain:(NSArray<EnvoyNativeFilterConfig *> *)nativeFilterChain
                platformFilterChain:(NSArray<EnvoyHTTPFilterFactory *> *)httpPlatformFilterFactories
                    stringAccessors:
                        (NSDictionary<NSString *, EnvoyStringAccessor *> *)stringAccessors {
  self = [super init];
  if (!self) {
    return nil;
  }

  self.statsDomain = statsDomain;
  self.connectTimeoutSeconds = connectTimeoutSeconds;
  self.dnsRefreshSeconds = dnsRefreshSeconds;
  self.dnsFailureRefreshSecondsBase = dnsFailureRefreshSecondsBase;
  self.dnsFailureRefreshSecondsMax = dnsFailureRefreshSecondsMax;
  self.statsFlushSeconds = statsFlushSeconds;
  self.streamIdleTimeoutSeconds = streamIdleTimeoutSeconds;
  self.appVersion = appVersion;
  self.appId = appId;
  self.virtualClusters = virtualClusters;
  self.directResponseMatchers = directResponseMatchers;
  self.directResponses = directResponses;
  self.nativeFilterChain = nativeFilterChain;
  self.httpPlatformFilterFactories = httpPlatformFilterFactories;
  self.stringAccessors = stringAccessors;
  return self;
}

- (nullable NSString *)resolveTemplate:(NSString *)templateYAML {
  NSMutableString *customClusters = [[NSMutableString alloc] init];
  NSMutableString *customListeners = [[NSMutableString alloc] init];
  NSMutableString *customRoutes = [[NSMutableString alloc] init];
  NSMutableString *customFilters = [[NSMutableString alloc] init];

  NSString *platformFilterTemplate = [[NSString alloc] initWithUTF8String:platform_filter_template];
  for (EnvoyHTTPFilterFactory *filterFactory in self.httpPlatformFilterFactories) {
    NSString *platformFilterConfig =
        [platformFilterTemplate stringByReplacingOccurrencesOfString:@"{{ platform_filter_name }}"
                                                          withString:filterFactory.filterName];
    [customFilters appendString:platformFilterConfig];
  }

  NSString *nativeFilterTemplate = [[NSString alloc] initWithUTF8String:native_filter_template];
  for (EnvoyNativeFilterConfig *filterConfig in self.nativeFilterChain) {
    NSString *nativeFilterConfig =
        [[nativeFilterTemplate stringByReplacingOccurrencesOfString:@"{{ native_filter_name }}"
                                                         withString:filterConfig.name]
            stringByReplacingOccurrencesOfString:@"{{ native_filter_typed_config }}"
                                      withString:filterConfig.typedConfig];
    [customFilters appendString:nativeFilterConfig];
  }

  BOOL hasDirectResponses = self.directResponses.length > 0;
  if (hasDirectResponses) {
    templateYAML = [templateYAML stringByReplacingOccurrencesOfString:@"#{fake_remote_responses}"
                                                           withString:self.directResponses];
    [customClusters appendString:[[NSString alloc] initWithUTF8String:fake_remote_cluster_insert]];
    [customListeners
        appendString:[[NSString alloc] initWithUTF8String:fake_remote_listener_insert]];
    [customRoutes appendString:self.directResponseMatchers];
    [customFilters
        appendString:[[NSString alloc] initWithUTF8String:route_cache_reset_filter_insert]];
  }

  templateYAML = [templateYAML stringByReplacingOccurrencesOfString:@"#{custom_clusters}"
                                                         withString:customClusters];
  templateYAML = [templateYAML stringByReplacingOccurrencesOfString:@"#{custom_listeners}"
                                                         withString:customListeners];
  templateYAML = [templateYAML stringByReplacingOccurrencesOfString:@"#{custom_routes}"
                                                         withString:customRoutes];
  templateYAML = [templateYAML stringByReplacingOccurrencesOfString:@"#{custom_filters}"
                                                         withString:customFilters];

  NSMutableString *definitions =
      [[NSMutableString alloc] initWithString:@"!ignore platform_defs:\n"];

  if (self.statsDomain != nil) {
    [definitions
        appendString:[NSString stringWithFormat:@"- &stats_domain %@\n", self.statsDomain]];
    [definitions appendString:[NSString stringWithFormat:@"- &stats_flush_interval %lus\n",
                                                         (unsigned long)self.statsFlushSeconds]];
  }
  [definitions appendString:[NSString stringWithFormat:@"- &connect_timeout %lus\n",
                                                       (unsigned long)self.connectTimeoutSeconds]];
  [definitions appendString:[NSString stringWithFormat:@"- &dns_refresh_rate %lus\n",
                                                       (unsigned long)self.dnsRefreshSeconds]];
  [definitions
      appendString:[NSString stringWithFormat:@"- &dns_fail_base_interval %lus\n",
                                              (unsigned long)self.dnsFailureRefreshSecondsBase]];
  [definitions
      appendString:[NSString stringWithFormat:@"- &dns_fail_max_interval %lus\n",
                                              (unsigned long)self.dnsFailureRefreshSecondsMax]];
  [definitions
      appendString:[NSString stringWithFormat:@"- &stream_idle_timeout %lus\n",
                                              (unsigned long)self.streamIdleTimeoutSeconds]];
  [definitions
      appendString:[NSString stringWithFormat:
                                 @"- &metadata \{ device_os: %@, app_version: %@, app_id: %@ \}\n",
                                 @"iOS", self.appVersion, self.appId]];
  [definitions
      appendString:[NSString stringWithFormat:@"- &virtual_clusters %@\n", self.virtualClusters]];

  [definitions appendString:templateYAML];

  if ([definitions containsString:@"{{"]) {
    NSLog(@"[Envoy] error: could not resolve all template keys in config:\n%@", definitions);
    return nil;
  }

  return definitions;
}

@end
