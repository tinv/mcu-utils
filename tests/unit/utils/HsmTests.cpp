
#include "CustomGtestMacros.h"
#include "utils/hsm.h"
#include <string.h>

class HsmFixture : public ::testing::Test {
protected:
  void SetUp() {}

  void TearDown() {}
};

TEST_F(HsmFixture, eventStringAssignedOnDefinitionUsingMacro)
{
  static const int Ev_0 = 0;
  mu_hsmEvent_t event = MU_HSM_EVENT( Ev_0 );
  EXPECT_ARRAY_EQ(char, "Ev_0", event.name, 4);
}


