#include "test/test_common/utility.h"
#include "test/integration/integration.h"
#include "test/server/utility.h"
#include "test/test_common/environment.h"

namespace Envoy {
namespace {

class QuicTestServer : public BaseIntegrationTest,
                       public testing::TestWithParam<Network::Address::IpVersion> {
 public:
  QuicTestServer() : BaseIntegrationTest(GetParam(), bootstrap_config()) {}

  static std::string bootstrap_config() {
    return absl::StrCat("", R"EOF(
    static_resources:
      listeners:
        - name: cronvoy_quic_listener
          address:
            socket_address:
              address: 127.0.0.1
              port_value: 6121
          filter_chains:
            - filters:
              - name: envoy.filters.network.http_connection_manager
                typed_config:
                  '@type': "type.googleapis.com/envoy.extensions.filters.network.http_connection_manager.v3.HttpConnectionManager"
                  stat_prefix: quic_proxy
                  route_config:
                    name: all
                    virtual_hosts:
                      - name: cronvoy_quic_cluster
                        domains: '*'
                        routes:
                          - match: { prefix: "/simple.txt" }
                            direct_response: { status: 200, body: { inline_string: "Hello you" } }
                  http_filters:
                    - name: envoy.router
                      typed_config:
                        "@type": type.googleapis.com/envoy.extensions.filters.http.router.v3.Router
      )EOF");
  }

  void start() {
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

INSTANTIATE_TEST_SUITE_P(IpVersions, QuicTestServer,
    testing::ValuesIn(TestEnvironment::getIpVersionsForTest()),
    TestUtility::ipTestParamsToString);

TEST_P(QuicTestServer, Basic) {
  std::cout << lookupPort("cronvoy_quic_listener");
  ASSERT_EQ("100", "200");
}

} // namespace
} // namespace Envoy