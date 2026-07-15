#include <utest.h>

#include "fileutils/abspath.h"
#include "fileutils/basename.h"
#include "fileutils/copy.h"
#include "fileutils/exist.h"
#include "fileutils/io.h"
#include "fileutils/mkdirp.h"
#include "fileutils/rmdirr.h"

UTEST_SUITE(fileutils)
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
