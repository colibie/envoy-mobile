#include "test/common/jni/quic_test_server_interface.h"

// NOLINT(namespace-envoy)

static std::shared_ptr<Envoy::QuicTestServer> quic_test_server_;

static std::shared_ptr<Envoy::QuicTestServer> quic_test_server() {
  return quic_test_server_;
}

void start_server() {
  if (auto e = quic_test_server()) {
     e->startQuicTestServer();
  }
}

void shutdown_server() {
  if (auto e = quic_test_server()) {
    e->shutdownQuicTestServer();
  }
}

int get_server_port() {
  if (auto e = quic_test_server()) {
    e->getServerPort();
  }
}