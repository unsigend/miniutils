#include <fcntl.h>
#include <fs.h>
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utest.h>

static const char *tmpd = "./build/tmp/";

UTEST_CASE(path_exists)
{
  char buf[PATH_MAX];

  EXPECT_TRUE(path_exists("."));
  EXPECT_TRUE(path_exists(".."));
  EXPECT_TRUE(path_exists("/"));
  EXPECT_TRUE(path_exists("build"));
  EXPECT_FALSE(path_exists("nonexistent"));
  EXPECT_TRUE(path_exists("Makefile"));
  EXPECT_TRUE(path_exists("external/includes/utest"));
  EXPECT_TRUE(path_exists("external/src/"));

  EXPECT_FALSE(dir_exists(tmpd));
  EXPECT_EQ_INT(mkdir(tmpd, 0755), 0);
  EXPECT_TRUE(dir_exists(tmpd));
  EXPECT_EQ_INT(rmdir(tmpd), 0);
  EXPECT_FALSE(dir_exists(tmpd));

  snprintf(buf, sizeof(buf), "%s/test", tmpd);
  EXPECT_FALSE(path_exists(buf));

  snprintf(buf, sizeof(buf), "%s/a/b/c/d/e", tmpd);
  EXPECT_FALSE(path_exists(buf));
}

UTEST_CASE(dir_exists)
{
  char buf[PATH_MAX];

  EXPECT_TRUE(dir_exists("."));
  EXPECT_TRUE(dir_exists(".."));
  EXPECT_TRUE(dir_exists("/"));
  EXPECT_TRUE(dir_exists("build"));
  EXPECT_FALSE(dir_exists("nonexistent"));
  EXPECT_TRUE(dir_exists("external/includes/utest"));
  EXPECT_TRUE(dir_exists("external/src"));

  EXPECT_FALSE(dir_exists(tmpd));
  EXPECT_EQ_INT(mkdir(tmpd, 0755), 0);
  EXPECT_TRUE(dir_exists(tmpd));
  EXPECT_EQ_INT(rmdir(tmpd), 0);
  EXPECT_FALSE(dir_exists(tmpd));

  snprintf(buf, sizeof(buf), "%s/test", tmpd);
  EXPECT_FALSE(path_exists(buf));

  snprintf(buf, sizeof(buf), "%s/a/b/c/d/e", tmpd);
  EXPECT_FALSE(path_exists(buf));
}

UTEST_CASE(file_exists)
{
  char buf[PATH_MAX];

  EXPECT_FALSE(file_exists("."));
  EXPECT_FALSE(file_exists("/"));
  EXPECT_FALSE(file_exists(".."));
  EXPECT_FALSE(file_exists("nonexistent"));
  EXPECT_TRUE(file_exists("Makefile"));
  EXPECT_FALSE(file_exists("build"));
  EXPECT_TRUE(file_exists("external/includes/utest/core.h"));
  EXPECT_TRUE(file_exists("external/src/utest.c"));

  EXPECT_FALSE(dir_exists(tmpd));
  EXPECT_EQ_INT(mkdir(tmpd, 0755), 0);
  EXPECT_TRUE(dir_exists(tmpd));
  snprintf(buf, sizeof(buf), "%s/test.txt", tmpd);
  EXPECT_FALSE(file_exists(buf));
  int fd = open(buf, O_CREAT | O_WRONLY | O_TRUNC, 0644);
  EXPECT_GE_INT(fd, 0);
  EXPECT_TRUE(file_exists(buf));
  EXPECT_EQ_INT(close(fd), 0);
  EXPECT_EQ_INT(unlink(buf), 0);
  EXPECT_FALSE(file_exists(buf));
  EXPECT_TRUE(dir_exists(tmpd));
  EXPECT_EQ_INT(rmdir(tmpd), 0);
  EXPECT_FALSE(dir_exists(tmpd));
}