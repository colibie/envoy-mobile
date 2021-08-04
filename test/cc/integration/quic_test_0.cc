#include "test/integration/integration.h"
#include "test/server/utility.h"
#include "test/test_common/environment.h"
#include "test/test_common/utility.h"

namespace Envoy {
namespace {

// TODO(junr03): move this to derive from the ApiListenerIntegrationTest after moving that class
// into a test lib.
class QuicTestServer : public BaseIntegrationTest,
                        public testing::TestWithParam<Network::Address::IpVersion> {
 public:
  QuicTestServer() : BaseIntegrationTest(GetParam(), bootstrap_config()) {}

  static std::string bootstrap_config() {
    // At least one empty filter chain needs to be specified.
    return ConfigHelper::baseConfig() + R"EOF(
    filter_chains:
      filters:
    )EOF";
  }

  void start() {
    config_helper_.addConfigModifier(
        [&](envoy::extensions::filters::network::http_connection_manager::v3::HttpConnectionManager&
        hcm) -> void {
          auto* route_config = hcm.mutable_route_config();

          auto* route = route_config->mutable_virtual_hosts(0)->mutable_routes(0);

          route->mutable_match()->set_prefix("/simple");

          auto* direct_response = route->mutable_direct_response();
          direct_response->set_status(200);
          direct_response->mutable_body()->set_inline_string("hello you");
        });
    // set up quic protocol: do these have to set once?
    setUpstreamProtocol(Http::CodecType::HTTP3);
    setUpstreamProtocol(Http::CodecType::HTTP3);
    initialize();
  }

  // shuts down the quicTestServer
  void shutdown() {
    test_server_.reset();
    fake_upstreams_.clear();
  }

  // returns the server port
  int getServerPort() {
    // I'll have to use listeners to get ports
    return static_cast<int>(lookupPort("cronvoy_quic_listener"));
  }
};

// GTEST_ALLOW_UNINSTANTIATED_PARAMETERIZED_TEST(QuicTestServer);
INSTANTIATE_TEST_SUITE_P(IpVersions, QuicTestServer,
    testing::ValuesIn(TestEnvironment::getIpVersionsForTest()),
    TestUtility::ipTestParamsToString);

TEST_P(QuicTestServer, Basic) {
  std::cout << "hello";
  ASSERT_EQ("100", "200");
}

} // namespace
} // namespace Envoy
