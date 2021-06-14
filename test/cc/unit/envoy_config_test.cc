#include <string>
#include <vector>

#include "absl/synchronization/notification.h"
#include "gtest/gtest.h"
#include "library/cc/engine_builder.h"
#include "library/cc/log_level.h"

namespace Envoy {
namespace {

using namespace Platform;

TEST(TestConfig, ConfigIsApplied) {
  absl::Notification engine_running;
  auto engine_builder = EngineBuilder();
  engine_builder.addLogLevel(LogLevel::debug)
      .addStatsDomain("asdf.fake.website")
      .addConnectTimeoutSeconds(123)
      .addDnsRefreshSeconds(456)
      .addDnsFailureRefreshSeconds(789, 987)
      .addStatsFlushSeconds(654)
      .addVirtualClusters("virtual-clusters")
      .setAppVersion("1.2.3")
      .setAppId("1234-1234-1234")
      .setDeviceOs("probably-ubuntu-on-CI");
  auto config_str = engine_builder.generateConfigStr();

  std::vector<std::string> must_contain = {
      "socket_address: {address: asdf.fake.website, port_value: 443}",
      "connect_timeout: 123s",
      "dns_refresh_rate: 456s",
      "base_interval: 789s",
      "max_interval: 987s",
      "stats_flush_interval: 654s",
      "virtual_clusters: virtual-clusters",
      "app_version : 1.2.3",
      "app_id : 1234-1234-1234",
      "os: probably-ubuntu-on-CI",
  };
  for (const auto& string : must_contain) {
    ASSERT_NE(config_str.find(string), std::string::npos) << "'" << string << "' not found";
  }
}

} // namespace
} // namespace Envoy
