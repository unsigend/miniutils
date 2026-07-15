#include "general.h"

UTEST_CASE(fbasename)
{
  char buf[PATH_MAX];
  char *ret;

  ret = fbasename("file.txt", buf);
  EXPECT_NOTNULL(ret);
  EXPECT_EQ_STR(ret, "file.txt");

  ret = fbasename("/usr/lib", buf);
  EXPECT_NOTNULL(ret);
  EXPECT_EQ_STR(ret, "lib");

  ret = fbasename("/usr/lib/", buf);
  EXPECT_NOTNULL(ret);
  EXPECT_EQ_STR(ret, "lib");

  ret = fbasename("/usr", buf);
  EXPECT_NOTNULL(ret);
  EXPECT_EQ_STR(ret, "usr");

  ret = fbasename("/", buf);
  EXPECT_NOTNULL(ret);
  EXPECT_EQ_STR(ret, "/");

  ret = fbasename(".", buf);
  EXPECT_NOTNULL(ret);
  EXPECT_EQ_STR(ret, ".");

  ret = fbasename("..", buf);
  EXPECT_NOTNULL(ret);
  EXPECT_EQ_STR(ret, "..");

  ret = fbasename("./a/b/c.txt", buf);
  EXPECT_NOTNULL(ret);
  EXPECT_EQ_STR(ret, "c.txt");

  ret = fbasename("a//b///c", buf);
  EXPECT_NOTNULL(ret);
  EXPECT_EQ_STR(ret, "c");

  EXPECT_EQ_PTR(fbasename("Makefile", buf), buf);
}
