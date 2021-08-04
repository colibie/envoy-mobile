#include "envoy/test/integration/server.h"

namespace cronet {

namespace {

static const int kServerPort = 6121;

Envoy::Thread::Thread* g_quic_server_thread = nullptr;
// quic::QuicMemoryCacheBackend* g_quic_memory_cache_backend = nullptr;
Envoy::IntegrationTestServerPtr g_quic_server = nullptr;

void StartOnServerThread(const base::FilePath& test_files_root,
                         const base::FilePath& test_data_dir) {
  DCHECK(g_quic_server_thread->task_runner()->BelongsToCurrentThread());
  DCHECK(!g_quic_server);

  // Set up in-memory cache.
  base::FilePath file_dir = test_files_root.Append("quic_data");
  CHECK(base::PathExists(file_dir)) << "Quic data does not exist";
  g_quic_memory_cache_backend = new quic::QuicMemoryCacheBackend();
  g_quic_memory_cache_backend->InitializeBackend(file_dir.value());
  quic::QuicConfig config;

  // Set up server certs.
  base::FilePath directory = test_data_dir.Append("net/data/ssl/certificates");
  std::unique_ptr<net::ProofSourceChromium> proof_source(
      new net::ProofSourceChromium());
  CHECK(proof_source->Initialize(
      directory.Append("quic-chain.pem"),
      directory.Append("quic-leaf-cert.key"),
      base::FilePath()));
  g_quic_server = new net::QuicSimpleServer(
      std::move(proof_source), config,
      quic::QuicCryptoServerConfig::ConfigOptions(),
      quic::AllSupportedVersions(), g_quic_memory_cache_backend);

  // Start listening.
  int rv = g_quic_server->Listen(
      net::IPEndPoint(net::IPAddress::IPv4AllZeros(), kServerPort));
  CHECK_GE(rv, 0) << "Quic server fails to start";
  JNIEnv* env = base::android::AttachCurrentThread();
  Java_QuicTestServer_onServerStarted(env);
}

void ShutdownOnServerThread() {
  DCHECK(g_quic_server_thread->task_runner()->BelongsToCurrentThread());
  g_quic_server->Shutdown();
  delete g_quic_server;
  delete g_quic_memory_cache_backend;
}

}  // namespace

// Quic server is currently hardcoded to run on port 6121 of the localhost on
// the device.
void JNI_QuicTestServer_StartQuicTestServer(
    JNIEnv* env,
    const JavaParamRef<jstring>& jtest_files_root,
    const JavaParamRef<jstring>& jtest_data_dir) {
  DCHECK(!g_quic_server_thread);
  base::FilePath test_data_dir(
      base::android::ConvertJavaStringToUTF8(env, jtest_data_dir));
  base::InitAndroidTestPaths(test_data_dir);

  g_quic_server_thread = new base::Thread("quic server thread");
  base::Thread::Options thread_options;
  thread_options.message_pump_type = base::MessagePumpType::IO;
  bool started =
      g_quic_server_thread->StartWithOptions(std::move(thread_options));
  DCHECK(started);
  base::FilePath test_files_root(
      base::android::ConvertJavaStringToUTF8(env, jtest_files_root));
  g_quic_server_thread->task_runner()->PostTask(
      FROM_HERE,
      base::BindOnce(&StartOnServerThread, test_files_root, test_data_dir));
}

void JNI_QuicTestServer_ShutdownQuicTestServer(JNIEnv* env) {
  DCHECK(!g_quic_server_thread->task_runner()->BelongsToCurrentThread());
  g_quic_server_thread->task_runner()->PostTask(
      FROM_HERE, base::BindOnce(&ShutdownOnServerThread));
  delete g_quic_server_thread;
}

int JNI_QuicTestServer_GetServerPort(JNIEnv* env) {
  return kServerPort;
}

}  // namespace cronet

