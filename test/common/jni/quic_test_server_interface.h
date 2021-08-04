#pragma once

#include "test/common/integration/quic_test_server.h"
// NOLINT(namespace-envoy)

#ifdef __cplusplus
extern "C" { // functions
#endif

static std::shared_ptr<Envoy::QuicTestServer> quic_test_server();

void start_server();

void shutdown_server();

int get_server_port();

#ifdef __cplusplus
} // functions
#endif
