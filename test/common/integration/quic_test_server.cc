#include "quic_test_server.h"

using bazel::tools::cpp::runfiles::Runfiles;
namespace Envoy {

  // see https://github.com/envoyproxy/envoy/blob/main/test/test_runner.cc
  void QuicTestServer::setup(int argc, char** argv) {
    Envoy::TestEnvironment::initializeTestMain(argv[0]);

    // Create a Runfiles object for runfiles lookup.
    // https://github.com/bazelbuild/bazel/blob/master/tools/cpp/runfiles/runfiles_src.h#L32
    std::string error;
    const std::basic_string<char> argv0;
    std::unique_ptr<Runfiles> runfiles(Runfiles::CreateForTest());
    RELEASE_ASSERT(Envoy::TestEnvironment::getOptionalEnvVar("NORUNFILES").has_value() ||
        runfiles != nullptr,
                   error);

    Envoy::TestEnvironment::setRunfiles(runfiles.get());

    // Select whether to test only for IPv4, IPv6, or both. The default is to
    // test for both. Options are {"v4only", "v6only", "all"}. Set
    // ENVOY_IP_TEST_VERSIONS to "v4only" if the system currently does not support IPv6 network
    // operations. Similarly set ENVOY_IP_TEST_VERSIONS to "v6only" if IPv4 has already been
    // phased out of network operations. Set to "all" (or don't set) if testing both
    // v4 and v6 addresses is desired. This feature is in progress and will be rolled out to all tests
    // in upcoming PRs.
    Envoy::TestEnvironment::setEnvVar("ENVOY_IP_TEST_VERSIONS", "all", 0);

    // We hold on to process_wide to provide RAII cleanup of process-wide
    // state.
    ProcessWide process_wide;

    // Use the recommended, but not default, "threadsafe" style for the Death Tests.
    // See: https://github.com/google/googletest/commit/84ec2e0365d791e4ebc7ec249f09078fb5ab6caa
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";

    testing::Test::RecordProperty("TemporaryDirectory", TestEnvironment::temporaryDirectory());

    TestEnvironment::setEnvVar("TEST_UDSDIR", TestEnvironment::unixDomainSocketDirectory(), 1);

    TestEnvironment::initializeOptions(argc, argv);
    Thread::MutexBasicLockable lock;

    Server::Options& options = TestEnvironment::getOptions();

    Logger::Registry::getSink()->setLock(lock);
    Logger::Registry::getSink()->setShouldEscape(false);
    Logger::Registry::setLogLevel(options.logLevel());
    Logger::Registry::setLogFormat(options.logFormat());
    std::cerr << options.enableFineGrainLogging() << " fine \n";
//    Logger::Context logging_state(options.logLevel(), options.logFormat(), lock, false,
//                                  options.enableFineGrainLogging());

    if (Logger::Registry::getSink()->hasLock()) {std::cerr << "true \n";};
    // Allocate fake log access manager.
    testing::NiceMock<AccessLog::MockAccessLogManager> access_log_manager;
    std::unique_ptr<Logger::FileSinkDelegate> file_logger;

    // Redirect all logs to fake file when --log-path arg is specified in command line.
    if (!TestEnvironment::getOptions().logPath().empty()) {
      file_logger = std::make_unique<Logger::FileSinkDelegate>(
          TestEnvironment::getOptions().logPath(), access_log_manager, Logger::Registry::getSink());
    }


    // Reset all ENVOY_BUG counters.
    Envoy::Assert::resetEnvoyBugCountersForTest();
  }

  Network::TransportSocketFactoryPtr QuicTestServer::createUpstreamTlsContext(testing::NiceMock<Server::Configuration::MockTransportSocketFactoryContext>& factory_context) {
    envoy::extensions::transport_sockets::tls::v3::DownstreamTlsContext tls_context;
    Extensions::TransportSockets::Tls::ContextManagerImpl context_manager_{time_system_};
    std::cerr << "quic_test_serverL50\n";
    //TODO (colibie) fix runfilesPath error std::cerr << TestEnvironment::runfilesPath("") << " testpath\n";
    const std::string yaml = absl::StrFormat(
        R"EOF(
common_tls_context:
  tls_certificates:
  - certificate_chain: { filename: "/usr/local/google/home/colibie/.cache/bazel/_bazel_colibie/bdfb488ae034f37da2dcebdbd6e4d897/execroot/envoy_mobile/bazel-out/k8-fastbuild/bin/test/java/org/chromium/net/quic_test.runfiles/envoy/test/config/integration/certs/upstreamcert.pem" }
    private_key: { filename: "/usr/local/google/home/colibie/.cache/bazel/_bazel_colibie/bdfb488ae034f37da2dcebdbd6e4d897/execroot/envoy_mobile/bazel-out/k8-fastbuild/bin/test/java/org/chromium/net/quic_test.runfiles/envoy/test/config/integration/certs/upstreamkey.pem" }
  validation_context:
    trusted_ca: { filename: "/usr/local/google/home/colibie/.cache/bazel/_bazel_colibie/bdfb488ae034f37da2dcebdbd6e4d897/execroot/envoy_mobile/bazel-out/k8-fastbuild/bin/test/java/org/chromium/net/quic_test.runfiles/envoy/test/config/integration/certs/cacert.pem" }
)EOF");
    std::cerr << "quic_test_serverL63\n";
    TestUtility::loadFromYaml(yaml, tls_context);
    std::cerr << "quic_test_serverL65\n";
    envoy::extensions::transport_sockets::quic::v3::QuicDownstreamTransport quic_config;
    quic_config.mutable_downstream_tls_context()->MergeFrom(tls_context);

    std::cerr << "quic_test_serverL68\n";
    std::vector<std::string> server_names;
    auto& config_factory = Config::Utility::getAndCheckFactoryByName<
        Server::Configuration::DownstreamTransportSocketConfigFactory>(
        "envoy.transport_sockets.quic");
    std::cerr << "quic_test_serverL73\n";
    return config_factory.createTransportSocketFactory(quic_config, factory_context, server_names);
  }

  QuicTestServer::QuicTestServer() : api_(Api::createApiForTest(stats_store_, time_system_)),
                    version_(Network::Address::IpVersion::v6),
                    config_helper_(version_, *api_, ConfigHelper::baseConfig() + R"EOF(
    filter_chains:
      filters:
    )EOF"){
    std::cerr << "[quic_test_server.cc=L49] Initializing quic test server\n";
    ON_CALL(factory_context_, api()).WillByDefault(testing::ReturnRef(*api_));
    ON_CALL(factory_context_, scope()).WillByDefault(testing::ReturnRef(stats_store_));

    char param[] = "/usr/local/google/home/colibie/.cache/bazel/_bazel_colibie/bdfb488ae034f37da2dcebdbd6e4d897/execroot/envoy_mobile/external/envoy/";

    char *argv[]{param, NULL};
    setup(1, argv);

    config_helper_.addConfigModifier(
        [&](envoy::extensions::filters::network::http_connection_manager::v3::HttpConnectionManager&
        hcm) -> void {
          auto* route_config = hcm.mutable_route_config();
          route_config->mutable_max_direct_response_body_size_bytes()->set_value(4096);

          auto* route = route_config->mutable_virtual_hosts(0)->mutable_routes(0);

          route->mutable_match()->set_prefix("/me");

          auto* direct_response = route->mutable_direct_response();
          direct_response->set_status(200);
          direct_response->mutable_body()->set_inline_string("hello you!");
        });
  }

  void QuicTestServer::startQuicTestServer() {
    std::cerr << "Starting quic test server \n";
    FakeUpstreamConfig upstream_config_{time_system_};
    upstream_config_.upstream_protocol_ = Http::CodecType::HTTP3;
    upstream_config_.udp_fake_upstream_ = FakeUpstreamConfig::UdpConfig();

    Network::TransportSocketFactoryPtr factory = createUpstreamTlsContext(factory_context_); // Network::Test::createRawBufferSocketFactory();
    std::cerr << "after factory\n";

    int port = 0;  // let the kernel pick a port that is not in use (avoids test races)
    // upstream = std::make_unique<FakeUpstream>(std::move(factory), port, version_, upstream_config_);
    aupstream = std::make_unique<AutonomousUpstream>(std::move(factory), port, version_, upstream_config_, false);
    std::cerr << "after aupstream \n";

    // see what port was selected.
    std::cerr << "Upstream now listening on " << aupstream->localAddress()->ip()->port();
    Logger::Registry::getSink()->clearLock();
  }

  void QuicTestServer::shutdownQuicTestServer() {
    std::cerr << "shutting down\n";
    aupstream.reset();
    FAIL() << "this way blaze will give you a test log";
  }

  int QuicTestServer::getServerPort() {
    std::cerr << "quic_test_serverL147\n";
    return aupstream->localAddress()->ip()->port();
  }
} // namespace Envoy

