
#include <stdio.h>
#include <gmock/gmock.h>
#include <posix_board_if.h>
#include <cmdline.h>

extern "C" int main( void )
{
  int ret = 0;
  int argc;
  char **argv;

  // googletext expects two arguments otherwise it doesn't parse gtest_filter.
  // native_get_test_cmd_line_args returns only the options after the -testargs argument thus avoiding the exe name

  // int argc and char* argv[] provided by native_posix board
  native_get_test_cmd_line_args(&argc, &argv);

  std::vector<char *> newArgv(argc+1); // vector containing one element more than required
  newArgv[0] = "test";
  for(int arg = 0; arg < argc; ++arg)
  {
    newArgv[arg+1] = argv[arg];
  }

  testing::InitGoogleTest( &argc, newArgv.data() );
  testing::InitGoogleMock( &argc, newArgv.data() );

  ret = RUN_ALL_TESTS();
  printf("Tests returned: %d\n", ret);
  posix_exit(ret);
}