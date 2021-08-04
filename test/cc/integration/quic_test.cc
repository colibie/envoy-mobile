//#include "envoy/test/integration/integration.h"
//#include "envoy/test/integration/utility.h"
//#include "envoy/test/server/utility.h"
//#include "envoy/test/test_common/utility.h"
#include "quic_test_1.cc"

namespace Envoy {

class QuicTest : public testing::TestWithParam<Network::Address::IpVersion> {
 public:
  QuicTestServer server_;
  QuicTest() {}

  void SetUp() { server_.start(); }

  void TearDown() { server_.shutdown(); }
};


INSTANTIATE_TEST_SUITE_P(IpVersions, QuicTest,
    testing::ValuesIn(TestEnvironment::getIpVersionsForTest()),
    TestUtility::ipTestParamsToString
);

TEST_P(QuicTest, Hello) {
      std::cout << "hello";
      ASSERT_EQ("EPPR", "XCD");
}
//  std::string response;
//  auto connection = createConnectionDriver(
//      lookupPort("cronvoy_quic_listener"), "hello",
//      [&response](Network::ClientConnection& conn,
//                  const Buffer::Instance& data) -> void {
//        response.append(data.toString());
//        conn.close(Network::ConnectionCloseType::FlushWrite);
//      });
//  connection->run();
//  EXPECT_EQ("hello", response);
//}
};