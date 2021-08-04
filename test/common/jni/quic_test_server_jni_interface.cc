#include <jni.h>

#include "library/common/jni/jni_support.h"
#include "library/common/jni/jni_utility.h"
#include "library/common/jni/jni_version.h"
#include "test/common/integration/quic_test_server.h"


// Quic Test ServerJniLibrary
Envoy::QuicTestServer quic_test_server;

extern "C" JNIEXPORT void JNICALL Java_org_chromium_net_testing_QuicTestServer_nativeStartQuicTestServer(JNIEnv * env,
                                                                        jclass clazz,
                                                                        jstring file_path,
                                                                        jstring test_data_dir) {
  // TODO: implement nativeStartQuicTestServer()
  jni_log("[QTS]", "starting server");
  quic_test_server.startQuicTestServer();
}

extern "C" JNIEXPORT jint
JNICALL
Java_org_chromium_net_testing_QuicTestServer_nativeGetServerPort(JNIEnv* env,
                                                                 jclass clazz) {
  // TODO: implement nativeGetServerPort()
  jni_log("[QTS]", "getting server port");
  return quic_test_server.getServerPort();
}


extern "C" JNIEXPORT void JNICALL
Java_org_chromium_net_testing_QuicTestServer_nativeShutdownQuicTestServer(JNIEnv * env,
                                                                          jclass clazz) {
// TODO: implement nativeShutdownQuicTestServer()
jni_log("[QTS]", "shutting down server");
  quic_test_server.shutdownQuicTestServer();
}