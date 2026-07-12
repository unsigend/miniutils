#include "general.h"

UTEST_CASE(path_exists)
{
  char buf[PATH_MAX];

  EXPECT_TRUE(path_exists("."));
  EXPECT_TRUE(path_exists(".."));
  EXPECT_TRUE(path_exists("/"));
  EXPECT_TRUE(path_exists("build"));
  EXPECT_TRUE(path_exists("Makefile"));
  EXPECT_TRUE(path_exists("external/includes/utest"));
  EXPECT_FALSE(path_exists("nonexistent"));

  TMP_PATH(buf, "a/b/c");
  EXPECT_FALSE(path_exists(buf));

  TMP_INIT();
  TMP_PATH(buf, "probe");
  TOUCH_FILE(buf);
  EXPECT_TRUE(path_exists(buf));
  EXPECT_TRUE(file_exists(buf));
  EXPECT_FALSE(dir_exists(buf));
  EXPECT_EQ_INT(unlink(buf), 0);
  EXPECT_EQ_INT(rmdir(tmpd), 0);
}

UTEST_CASE(dir_exists)
{
  char buf[PATH_MAX];

  EXPECT_TRUE(dir_exists("."));
  EXPECT_TRUE(dir_exists(".."));
  EXPECT_TRUE(dir_exists("build"));
  EXPECT_TRUE(dir_exists("external/includes/utest"));
  EXPECT_FALSE(dir_exists("nonexistent"));
  EXPECT_FALSE(dir_exists("Makefile"));
  EXPECT_FALSE(dir_exists("external/src/utest.c"));

  TMP_INIT();
  TMP_PATH(buf, "sub");
  EXPECT_EQ_INT(mkdir(buf, 0755), 0);
  EXPECT_TRUE(dir_exists(buf));
  EXPECT_TRUE(path_exists(buf));
  EXPECT_FALSE(file_exists(buf));
  EXPECT_EQ_INT(rmdir(buf), 0);
  EXPECT_EQ_INT(rmdir(tmpd), 0);
}

UTEST_CASE(file_exists)
{
  char buf[PATH_MAX];

  EXPECT_TRUE(file_exists("Makefile"));
  EXPECT_TRUE(file_exists("external/src/utest.c"));
  EXPECT_FALSE(file_exists("."));
  EXPECT_FALSE(file_exists("build"));
  EXPECT_FALSE(file_exists("nonexistent"));

  TMP_INIT();
  TMP_PATH(buf, "test.txt");
  EXPECT_FALSE(file_exists(buf));
  TOUCH_FILE(buf);
  EXPECT_TRUE(file_exists(buf));
  EXPECT_TRUE(path_exists(buf));
  EXPECT_FALSE(dir_exists(buf));
  EXPECT_EQ_INT(unlink(buf), 0);
  EXPECT_EQ_INT(rmdir(tmpd), 0);
}
