#include "source/common/stats/isolated_store_impl.h"
#include "source/extensions/transport_sockets/tls/ssl_socket.h"

#include "gtest/gtest.h"

#include "absl/strings/str_format.h"
#include "envoy/extensions/transport_sockets/quic/v3/quic_transport.pb.h"
#include "test/mocks/server/transport_socket_factory_context.h"
#include "test/test_common/environment.h"
#include "test/integration/fake_upstream.h"
#include "test/integration/autonomous_upstream.h"
#include "test/config/utility.h"
#include "test/test_common/network_utility.h"

namespace Envoy {
namespace {
class QuicTestServer {
 private:
  testing::NiceMock<Server::Configuration::MockTransportSocketFactoryContext>
      factory_context_;
  Stats::IsolatedStoreImpl stats_store_;
  Event::GlobalTimeSystem time_system_;
  Api::ApiPtr api_;
  Network::Address::IpVersion version_;
  ConfigHelper config_helper_;
  std::unique_ptr<FakeUpstream> upstream;
  std::unique_ptr<AutonomousUpstream> aupstream;
  Network::TransportSocketFactoryPtr createUpstreamTlsContext(testing::NiceMock<Server::Configuration::MockTransportSocketFactoryContext>& factory_context_) {
    envoy::extensions::transport_sockets::tls::v3::DownstreamTlsContext
        tls_context;
    Extensions::TransportSockets::Tls::ContextManagerImpl
        context_manager_{time_system_};

    const std::string yaml = absl::StrFormat(
        R"EOF(
common_tls_context:
  tls_certificates:
  - certificate_chain: { filename: "%s" }
    private_key: { filename: "%s" }
  validation_context:
    trusted_ca: { filename: "%s" }
)EOF",
        TestEnvironment::runfilesPath(
            "test/config/integration/certs/upstreamcert.pem"),
        TestEnvironment::runfilesPath(
            "test/config/integration/certs/upstreamkey.pem"),
        TestEnvironment::runfilesPath("test/config/integration/certs/cacert.pem"));
    TestUtility::loadFromYaml(yaml, tls_context);

    /** HTTP1 CONFIG
    tls_context.mutable_common_tls_context()->add_alpn_protocols("http/2");
    auto cfg = std::make_unique<Extensions::TransportSockets::Tls::ServerContextConfigImpl>(
        tls_context, factory_context_);
    static Stats::Scope* upstream_stats_store = new Stats::IsolatedStoreImpl();
    return std::make_unique<Extensions::TransportSockets::Tls::ServerSslSocketFactory>(
        std::move(cfg), context_manager_, *upstream_stats_store, std::vector<std::string>{});
    */

    // quic
    envoy::extensions::transport_sockets::quic::v3::QuicDownstreamTransport
        quic_config;
    quic_config.mutable_downstream_tls_context()->MergeFrom(tls_context);

    std::vector<std::string> server_names;
    auto& config_factory = Config::Utility::getAndCheckFactoryByName<
        Server::Configuration::DownstreamTransportSocketConfigFactory>(
        "envoy.transport_sockets.quic");
    return config_factory.createTransportSocketFactory(quic_config,
                                                       factory_context_,
                                                       server_names);
  }

 public:
  QuicTestServer() : api_(Api::createApiForTest(stats_store_, time_system_)),
                     version_(Network::Address::IpVersion::v6),
                     config_helper_(version_,
                                    *api_,
                                    ConfigHelper::baseConfig() + R"EOF(
    filter_chains:
      filters:
    )EOF") {
    ON_CALL(factory_context_, api()).WillByDefault(testing::ReturnRef(*api_));
    ON_CALL(factory_context_, scope()).WillByDefault(testing::ReturnRef(
        stats_store_));

    config_helper_.addConfigModifier(
        [&](envoy::extensions::filters::network::http_connection_manager::v3::HttpConnectionManager&
        hcm) -> void {
          auto* route_config = hcm.mutable_route_config();
          route_config->mutable_max_direct_response_body_size_bytes()->set_value(
              4096);

          auto* route =
              route_config->mutable_virtual_hosts(0)->mutable_routes(0);

          route->mutable_match()->set_prefix("/me");

          auto* direct_response = route->mutable_direct_response();
          direct_response->set_status(200);
          direct_response->mutable_body()->set_inline_string("hello you!");
        });
  }

  void startQuicTestServer() {
    FakeUpstreamConfig upstream_config_{time_system_};
    upstream_config_.upstream_protocol_ = Http::CodecType::HTTP3;
    upstream_config_.udp_fake_upstream_ = FakeUpstreamConfig::UdpConfig();

    Network::TransportSocketFactoryPtr factory =
        createUpstreamTlsContext(factory_context_); // Network::Test::createRawBufferSocketFactory();

    int port =
        0;  // let the kernel pick a port that is not in use (avoids test races)
    // upstream = std::make_unique<FakeUpstream>(std::move(factory), port, version_, upstream_config_);
    aupstream = std::make_unique<AutonomousUpstream>(std::move(factory),
                                                     port,
                                                     version_,
                                                     upstream_config_,
                                                     false);

    // see what port was selected.
    std::cerr << "Upstream now listening on "
              << aupstream->localAddress()->ip()->port();
  }

  void shutdownQuicTestServer() {
    std::cerr << "shutting down";
    aupstream.reset();
    FAIL() << "this way blaze will give you a test log";
  }

  int getServerPort() {
    return aupstream->localAddress()->ip()->port();
  }
};

TEST(CreateServer, BasicServer) {
  QuicTestServer server{};
  server.startQuicTestServer();
  std::cerr << "server port is" << server.getServerPort();
  // sleep(10000);
  server.shutdownQuicTestServer();
}

} // namespace Http
} // namespace Envoy

