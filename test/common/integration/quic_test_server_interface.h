#pragma once

#include "test/common/integration/quic_test_server.h"

#ifdef __cplusplus
extern "C" { // functions
#endif

void start_server();

void shutdown_server();

int get_server_port();

#ifdef __cplusplus
} // functions
#endif
