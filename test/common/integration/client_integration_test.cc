#include "source/extensions/http/header_formatters/preserve_case/preserve_case_formatter.h"

#include "test/common/http/common.h"
#include "test/integration/autonomous_upstream.h"
#include "test/integration/integration.h"
#include "test/server/utility.h"
#include "test/test_common/environment.h"
#include "test/test_common/utility.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "library/common/data/utility.h"
#include "library/common/http/client.h"
#include "library/common/http/header_utility.h"
#include "library/common/types/c_types.h"

using testing::ReturnRef;

namespace Envoy {
namespace {


// TODO(junr03): move this to derive from the ApiListenerIntegrationTest after moving that class
// into a test lib.
class ClientIntegrationTest : public BaseIntegrationTest,
                              public testing::TestWithParam<Network::Address::IpVersion> {
public:
  ClientIntegrationTest() : BaseIntegrationTest(GetParam(), bootstrap_config()) {
    use_lds_ = false;
    autonomous_upstream_ = true;
  }

  void SetUp() override {
  }

  void TearDown() override {
    test_server_.reset();
    fake_upstreams_.clear();
  }

  static std::string bootstrap_config() {
    // At least one empty filter chain needs to be specified.
    return ConfigHelper::baseConfig() + R"EOF(
    filter_chains:
      filters:
    )EOF";
  }
};

INSTANTIATE_TEST_SUITE_P(IpVersions, ClientIntegrationTest,
                         testing::ValuesIn(TestEnvironment::getIpVersionsForTest()),
                         TestUtility::ipTestParamsToString);

TEST_P(ClientIntegrationTest, Basic) {
  initialize();
  std::cerr << "address is" << test_server_->adminAddress()->ip()->addressAsString();

  sleep(100000);
  FAIL();
}
} // namespace
} // namespace Envoy
