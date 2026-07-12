#include <utest.h>

#include "fileutil/abspath.h"
#include "fileutil/basename.h"
#include "fileutil/copy.h"
#include "fileutil/exist.h"
#include "fileutil/io.h"
#include "fileutil/mkdirp.h"
#include "fileutil/rmdirr.h"

UTEST_SUITE(fileutil)
{
    UTEST_RUNCASE(path_exists);
    UTEST_RUNCASE(dir_exists);
    UTEST_RUNCASE(file_exists);

    UTEST_RUNCASE(fabspath);
    UTEST_RUNCASE(fbasename);

    UTEST_RUNCASE(read_all);
    UTEST_RUNCASE(write_all);
    UTEST_RUNCASE(read_file);
    UTEST_RUNCASE(write_file);

    UTEST_RUNCASE(mkdirp);
    UTEST_RUNCASE(copy_file);
    UTEST_RUNCASE(copy_dir);
    UTEST_RUNCASE(rmdirr);
}
