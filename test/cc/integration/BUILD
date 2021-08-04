load("@envoy//bazel:envoy_build_system.bzl", "envoy_cc_test", "envoy_package")

licenses(["notice"])  # Apache 2

envoy_package()

envoy_cc_test(
    name = "send_headers_test",
    srcs = ["send_headers_test.cc"],
    repository = "@envoy",
    deps = [
        "//library/cc:envoy_engine_cc_lib_no_stamp",
    ],
)

envoy_cc_test(
    name = "lifetimes_test",
    srcs = ["lifetimes_test.cc"],
    repository = "@envoy",
    deps = [
        "//library/cc:envoy_engine_cc_lib_no_stamp",
    ],
)

envoy_cc_test(
    name = "quic_test",
    srcs = ["quic_test.cc"],
    repository = "@envoy",
    deps = [
        "//library/cc:envoy_engine_cc_lib_no_stamp",
        "@envoy//test/integration:http_integration_lib",
        "@envoy//test/server:utility_lib",
    ],
)

envoy_cc_test(
    name = "quic_test_0",
    srcs = ["quic_test_0.cc"],
    repository = "@envoy",
    deps = [
        "//library/common/extensions/filters/http/local_error:config",
        "//library/common/extensions/filters/http/local_error:filter_cc_proto",
        "//library/common/http:client_lib",
        "//library/common/http:header_utility_lib",
        "//library/common/types:c_types_lib",
        "@envoy//test/common/http:common_lib",
        "@envoy//test/integration:http_integration_lib",
        "@envoy//test/server:utility_lib",
    ],
)

envoy_cc_test(
    name = "quic_test_s",
    srcs = ["quic_test_1.cc"],
    repository = "@envoy",
    deps = [
        "//library/common/extensions/filters/http/local_error:config",
        "//library/common/extensions/filters/http/local_error:filter_cc_proto",
        "//library/common/http:client_lib",
        "//library/common/http:header_utility_lib",
        "//library/common/types:c_types_lib",
        "@envoy//test/common/http:common_lib",
        "@envoy//test/integration:http_integration_lib",
        "@envoy//test/server:utility_lib",
    ],
)
