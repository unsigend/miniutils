#include "general.h"

UTEST_CASE(rmdirr)
{
  char path[PATH_MAX];
  char src[PATH_MAX];
  char dst[PATH_MAX];

  TMP_PATH(path, "missing");
  EXPECT_FAIL(rmdirr(path), ENOENT);

  TMP_INIT();

  TMP_PATH(path, "file.txt");
  WRITE_FILE(path, "ok");
  EXPECT_EQ_INT(rmdirr(path), 0);
  EXPECT_FALSE(file_exists(path));

  TMP_PATH(path, "empty");
  EXPECT_EQ_INT(mkdir(path, 0755), 0);
  EXPECT_EQ_INT(rmdirr(path), 0);
  EXPECT_FALSE(dir_exists(path));

  TMP_PATH(path, "dirs");
  EXPECT_EQ_INT(mkdirp(path, 0755), 0);
  snprintf(path, sizeof(path), "%sdirs/a", tmpd);
  EXPECT_EQ_INT(mkdir(path, 0755), 0);
  snprintf(path, sizeof(path), "%sdirs/a/b", tmpd);
  EXPECT_EQ_INT(mkdir(path, 0755), 0);
  TMP_PATH(path, "dirs");
  EXPECT_EQ_INT(rmdirr(path), 0);
  EXPECT_FALSE(path_exists(path));

  TMP_PATH(path, "tree");
  EXPECT_EQ_INT(mkdirp(path, 0755), 0);
  snprintf(path, sizeof(path), "%stree/a.txt", tmpd);
  TOUCH_FILE(path);
  snprintf(path, sizeof(path), "%stree/sub/deep", tmpd);
  EXPECT_EQ_INT(mkdirp(path, 0755), 0);
  snprintf(path, sizeof(path), "%stree/sub/deep/b.txt", tmpd);
  TOUCH_FILE(path);
  TMP_PATH(path, "tree");
  EXPECT_EQ_INT(rmdirr(path), 0);
  EXPECT_FALSE(path_exists(path));

  TMP_PATH(src, "src");
  TMP_PATH(dst, "dst");
  EXPECT_EQ_INT(mkdirp(src, 0755), 0);
  EXPECT_EQ_INT(mkdirp(dst, 0755), 0);
  snprintf(path, sizeof(path), "%s/x.txt", src);
  TOUCH_FILE(path);
  snprintf(path, sizeof(path), "%s/nested", src);
  EXPECT_EQ_INT(mkdirp(path, 0755), 0);
  snprintf(path, sizeof(path), "%s/nested/y.txt", src);
  TOUCH_FILE(path);
  EXPECT_EQ_INT(copy_dir(src, dst), 0);
  EXPECT_EQ_INT(rmdirr(dst), 0);
  EXPECT_FALSE(dir_exists(dst));
  EXPECT_EQ_INT(rmdirr(src), 0);
  EXPECT_FALSE(dir_exists(src));

  TMP_PATH(src, "copy");
  TMP_PATH(dst, "copydir");
  EXPECT_EQ_INT(mkdirp(dst, 0755), 0);
  WRITE_FILE(src, "ok");
  snprintf(path, sizeof(path), "%s/b.txt", dst);
  EXPECT_EQ_INT(copy_file(src, path), 0);
  EXPECT_EQ_INT(rmdirr(dst), 0);
  EXPECT_FALSE(path_exists(dst));
  EXPECT_EQ_INT(unlink(src), 0);

  EXPECT_EQ_INT(rmdirr(tmpd), 0);
  EXPECT_FALSE(path_exists(tmpd));
}
