#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>



// NOLINT(namespace-envoy)

#ifdef __cplusplus
extern "C" { // functions
#endif

void startQuicTestServer();

int getServerPort();

void shutdownQuicTestServer();

#ifdef __cplusplus
} // functions
#endif