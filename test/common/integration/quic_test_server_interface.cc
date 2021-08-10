#include "test/common/integration/quic_test_server_interface.h"

// NOLINT(namespace-envoy)

static std::shared_ptr<Envoy::QuicTestServer> strong_quic_test_server_;
static std::weak_ptr<Envoy::QuicTestServer> quic_test_server_;

static std::shared_ptr<Envoy::QuicTestServer> quic_test_server() {
  return quic_test_server_.lock();
}

void start_server() {
  // test runner setup; required to access envoy test server
  
  // start server
  strong_quic_test_server_ = std::make_shared<Envoy::QuicTestServer>();
  quic_test_server_ = strong_quic_test_server_;

//  quic_test_server()->startQuicTestServer();
  std::cerr << "server starting\n";
  if (auto e = quic_test_server()) {
     e->startQuicTestServer();
    std::cerr << "server started\n";
  }
}

void shutdown_server() {
  auto e = strong_quic_test_server_;
  strong_quic_test_server_.reset();
  e->shutdownQuicTestServer();
  std::cerr << "server shutting down";

//  if (auto e = quic_test_server()) {
//    e->shutdownQuicTestServer();
//  }
}

int get_server_port() {
  std::cerr << "quic_test_server_interfaceL39\n";
  if (auto e = quic_test_server()) {
    std::cerr << "quic_test_server_interfaceL41\n";
    return e->getServerPort();
  }
  return 1; // failure
}