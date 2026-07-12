#include "general.h"

UTEST_CASE(mkdirp)
{
  char path[PATH_MAX];
  char longpath[PATH_MAX + 2];
  struct stat st;

  TMP_ENSURE();
  EXPECT_FALSE(dir_exists(tmpd));
  EXPECT_EQ_INT(mkdirp(tmpd, 0755), 0);
  EXPECT_EQ_INT(mkdirp(tmpd, 0755), 0);
  EXPECT_TRUE(dir_exists(tmpd));
  EXPECT_EQ_INT(rmdir(tmpd), 0);

  TMP_PATH(path, "a/b/c/d/e");
  EXPECT_EQ_INT(mkdirp(path, 0755), 0);
  EXPECT_TRUE(dir_exists(path));
  EXPECT_EQ_INT(stat(path, &st), 0);
  EXPECT_TRUE(S_ISDIR(st.st_mode));
  EXPECT_EQ_INT(rmdirr(tmpd), 0);

  EXPECT_EQ_INT(mkdir(tmpd, 0755), 0);
  TMP_PATH(path, "a");
  EXPECT_EQ_INT(mkdir(path, 0755), 0);
  TMP_PATH(path, "a/b/c");
  EXPECT_EQ_INT(mkdirp(path, 0755), 0);
  EXPECT_TRUE(dir_exists(path));
  EXPECT_EQ_INT(rmdirr(tmpd), 0);

  EXPECT_EQ_INT(mkdir(tmpd, 0755), 0);
  TMP_PATH(path, "x/y/z");
  EXPECT_EQ_INT(mkdirp(path, 0755), 0);
  EXPECT_EQ_INT(mkdirp(path, 0755), 0);
  EXPECT_TRUE(dir_exists(path));
  EXPECT_EQ_INT(rmdirr(tmpd), 0);

  EXPECT_EQ_INT(mkdir(tmpd, 0755), 0);
  TMP_PATH(path, "block");
  TOUCH_FILE(path);
  TMP_PATH(path, "block/sub");
  errno = 0;
  EXPECT_EQ_INT(mkdirp(path, 0755), -1);
  TMP_PATH(path, "block");
  EXPECT_EQ_INT(unlink(path), 0);
  EXPECT_EQ_INT(rmdir(tmpd), 0);

  EXPECT_EQ_INT(mkdirp("nodir", 0755), 0);
  EXPECT_TRUE(dir_exists("nodir"));
  EXPECT_EQ_INT(rmdir("nodir"), 0);

  EXPECT_EQ_INT(mkdirp(tmpd, 0755), 0);
  TMP_PATH(path, "leaf");
  EXPECT_EQ_INT(mkdirp(path, 0755), 0);
  EXPECT_EQ_INT(mkdirp(path, 0700), 0);
  EXPECT_TRUE(dir_exists(path));
  EXPECT_EQ_INT(rmdirr(tmpd), 0);

  memset(longpath, 'a', PATH_MAX);
  longpath[0] = '/';
  longpath[PATH_MAX] = '\0';
  EXPECT_FAIL(mkdirp(longpath, 0755), ENAMETOOLONG);
}
