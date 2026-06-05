#include <utest.h>

#include "fs/abspath.h"
#include "fs/copy.h"
#include "fs/exist.h"
#include "fs/io.h"
#include "fs/mkdirp.h"
#include "fs/rmdirr.h"

UTEST_SUITE(fs)
{
  UTEST_RUNCASE(path_exists);
  UTEST_RUNCASE(dir_exists);
  UTEST_RUNCASE(file_exists);

  UTEST_RUNCASE(abspath);

  UTEST_RUNCASE(read_all);
  UTEST_RUNCASE(write_all);

  UTEST_RUNCASE(mkdirp);
  UTEST_RUNCASE(copy_file);
  UTEST_RUNCASE(copy_dir);
  UTEST_RUNCASE(rmdirr);
}
