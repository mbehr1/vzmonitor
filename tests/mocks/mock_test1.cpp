#include <gmock/gmock.h>
  using ::testing::Eq;
  using ::testing::Return;
  using ::testing::InSequence;
  using ::testing::AtLeast;
  using ::testing::Invoke;
  using ::testing::Ge;
  using ::testing::_;
  using ::testing::SetArrayArgument;

#include <gtest/gtest.h>
  using ::testing::Test;

namespace mock_test1
{

TEST(mock_test1, basic)
{
	EXPECT_TRUE(true);
}

}

int main(int argc, char** argv) {
  // The following line must be executed to initialize Google Mock
  // (and Google Test) before running the tests.
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
