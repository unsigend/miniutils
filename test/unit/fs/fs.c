#include <utest.h>

#include "fs/exist.h"

UTEST_SUITE(fs)
{
  UTEST_RUNCASE(dir_exists);
  UTEST_RUNCASE(path_exists);
  UTEST_RUNCASE(file_exists);
}