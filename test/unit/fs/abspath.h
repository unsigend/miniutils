#include "general.h"

UTEST_CASE(abspath)
{
  char cwd[PATH_MAX];
  char expect[PATH_MAX];
  char buf[PATH_MAX];
  char longpath[PATH_MAX + 2];
  size_t n;

  EXPECT_NOTNULL(getcwd(cwd, sizeof(cwd)));

  EXPECT_ABSPATH_SAME_REALPATH(".");
  EXPECT_ABSPATH_SAME_REALPATH("..");
  EXPECT_ABSPATH_SAME_REALPATH("/");
  EXPECT_ABSPATH_SAME_REALPATH("./Makefile");
  EXPECT_ABSPATH_SAME_REALPATH("Makefile");
  EXPECT_ABSPATH_SAME_REALPATH("build");
  EXPECT_ABSPATH_SAME_REALPATH("build/../Makefile");
  EXPECT_ABSPATH_SAME_REALPATH("external/includes/utest");

  snprintf(expect, sizeof(expect), "%s/nonexistent", cwd);
  EXPECT_ABSPATH_EQ("nonexistent", expect);

  snprintf(expect, sizeof(expect), "%s/a/b/c", cwd);
  EXPECT_ABSPATH_EQ("a/b/c", expect);

  snprintf(expect, sizeof(expect), "%s/foo/../bar", cwd);
  EXPECT_ABSPATH_EQ("foo/../bar", expect);

  EXPECT_ABSPATH_EQ("/no/such/path", "/no/such/path");
  EXPECT_ABSPATH_EQ("/tmp/missing/file", "/tmp/missing/file");

  EXPECT_EQ_INT(abspath("/no/such/path", buf), 0);
  EXPECT_EQ_INT(abspath("/no/such/path", expect), 0);
  EXPECT_EQ_STR(buf, expect);

  memset(longpath, 'a', PATH_MAX);
  longpath[0] = '/';
  longpath[PATH_MAX] = '\0';
  EXPECT_FAIL(abspath(longpath, buf), ENAMETOOLONG);

  n = PATH_MAX - strlen(cwd) - 1;
  memset(longpath, 'a', n);
  longpath[n] = '\0';
  EXPECT_FAIL(abspath(longpath, buf), ENAMETOOLONG);
}
