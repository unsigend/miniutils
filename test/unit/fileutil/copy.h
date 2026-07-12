#include "general.h"

UTEST_CASE(copy_file)
{
  char src[PATH_MAX];
  char dst[PATH_MAX];
  char big[5000];

  TMP_INIT();

  TMP_PATH(src, "src.txt");
  TMP_PATH(dst, "dst.txt");
  WRITE_FILE(src, "ok");
  EXPECT_EQ_INT(copy_file(dst, src), 0);
  EXPECT_TRUE(file_exists(dst));
  EXPECT_FILE_EQ(dst, "ok");
  EXPECT_EQ_INT(unlink(src), 0);
  EXPECT_EQ_INT(unlink(dst), 0);

  TMP_PATH(dst, "copy");
  EXPECT_EQ_INT(copy_file(dst, "Makefile"), 0);
  EXPECT_TRUE(file_exists(dst));
  EXPECT_EQ_INT(unlink(dst), 0);

  WRITE_FILE(src, "new");
  WRITE_FILE(dst, "old");
  EXPECT_EQ_INT(copy_file(dst, src), 0);
  EXPECT_FILE_EQ(dst, "new");
  EXPECT_EQ_INT(unlink(src), 0);
  EXPECT_EQ_INT(unlink(dst), 0);

  TMP_PATH(src, "empty");
  TMP_PATH(dst, "empty.out");
  TOUCH_FILE(src);
  EXPECT_EQ_INT(copy_file(dst, src), 0);
  EXPECT_TRUE(file_exists(dst));
  EXPECT_EQ_INT(unlink(src), 0);
  EXPECT_EQ_INT(unlink(dst), 0);

  memset(big, 'x', sizeof(big) - 1);
  big[sizeof(big) - 1] = '\0';
  WRITE_FILE(src, big);
  EXPECT_EQ_INT(copy_file(dst, src), 0);
  EXPECT_TRUE(file_exists(dst));
  EXPECT_EQ_INT(unlink(src), 0);
  EXPECT_EQ_INT(unlink(dst), 0);

  TMP_PATH(src, "missing");
  TMP_PATH(dst, "nope");
  EXPECT_FAIL(copy_file(dst, src), ENOENT);

  TMP_PATH(src, "src.txt");
  TOUCH_FILE(src);
  TMP_PATH(dst, "missing/dst.txt");
  EXPECT_FAIL(copy_file(dst, src), ENOENT);
  EXPECT_EQ_INT(unlink(src), 0);

  TMP_PATH(dst, "a/b/dst");
  EXPECT_FAIL(copy_file(dst, "Makefile"), ENOENT);

  TMP_PATH(dst, "dir");
  EXPECT_EQ_INT(mkdir(dst, 0755), 0);
  EXPECT_FAIL(copy_file(dst, "Makefile"), EISDIR);
  EXPECT_EQ_INT(rmdir(dst), 0);

  TMP_PATH(dst, "out");
  errno = 0;
  EXPECT_EQ_INT(copy_file(dst, "build"), -1);

  EXPECT_EQ_INT(rmdir(tmpd), 0);
}

UTEST_CASE(copy_dir)
{
  char src[PATH_MAX];
  char dst[PATH_MAX];
  char path[PATH_MAX];

  TMP_ENSURE();
  TMP_PATH(src, "src");
  TMP_PATH(dst, "dst");
  EXPECT_EQ_INT(mkdirp(src, 0755), 0);
  EXPECT_EQ_INT(mkdirp(dst, 0755), 0);

  snprintf(path, sizeof(path), "%s/a.txt", src);
  WRITE_FILE(path, "ok");
  snprintf(path, sizeof(path), "%s/sub/deep", src);
  EXPECT_EQ_INT(mkdirp(path, 0755), 0);
  snprintf(path, sizeof(path), "%s/sub/b.txt", src);
  WRITE_FILE(path, "hi");
  snprintf(path, sizeof(path), "%s/emptydir", src);
  EXPECT_EQ_INT(mkdir(path, 0755), 0);

  EXPECT_EQ_INT(copy_dir(dst, src), 0);
  snprintf(path, sizeof(path), "%s/a.txt", dst);
  EXPECT_FILE_EQ(path, "ok");
  snprintf(path, sizeof(path), "%s/sub/b.txt", dst);
  EXPECT_TRUE(file_exists(path));
  snprintf(path, sizeof(path), "%s/sub/deep", dst);
  EXPECT_TRUE(dir_exists(path));
  snprintf(path, sizeof(path), "%s/emptydir", dst);
  EXPECT_TRUE(dir_exists(path));
  EXPECT_EQ_INT(rmdirr(tmpd), 0);

  EXPECT_EQ_INT(mkdirp(src, 0755), 0);
  EXPECT_EQ_INT(mkdirp(dst, 0755), 0);
  EXPECT_EQ_INT(copy_dir(dst, src), 0);
  TMP_PATH(path, "dst/a.txt");
  EXPECT_FALSE(file_exists(path));
  EXPECT_EQ_INT(rmdirr(tmpd), 0);

  TMP_PATH(dst, "dst");
  EXPECT_EQ_INT(mkdirp(dst, 0755), 0);
  TMP_PATH(src, "missing");
  EXPECT_FAIL(copy_dir(dst, src), ENOENT);
  EXPECT_EQ_INT(rmdir(dst), 0);

  TMP_PATH(src, "src");
  EXPECT_EQ_INT(mkdirp(src, 0755), 0);
  TMP_PATH(dst, "missing");
  EXPECT_FAIL(copy_dir(dst, src), ENOENT);
  EXPECT_EQ_INT(rmdir(src), 0);

  TMP_PATH(dst, "dst");
  EXPECT_EQ_INT(mkdirp(dst, 0755), 0);
  TMP_PATH(path, "file");
  TOUCH_FILE(path);
  EXPECT_FAIL(copy_dir(dst, path), ENOTDIR);
  EXPECT_EQ_INT(unlink(path), 0);
  EXPECT_EQ_INT(rmdir(dst), 0);

  TMP_PATH(dst, "file");
  TOUCH_FILE(dst);
  EXPECT_EQ_INT(mkdirp(src, 0755), 0);
  EXPECT_FAIL(copy_dir(dst, src), ENOTDIR);
  EXPECT_EQ_INT(unlink(dst), 0);
  EXPECT_EQ_INT(rmdir(src), 0);

  TMP_PATH(src, "src");
  TMP_PATH(dst, "dst");
  EXPECT_EQ_INT(mkdirp(src, 0755), 0);
  EXPECT_EQ_INT(mkdirp(dst, 0755), 0);
  snprintf(path, sizeof(path), "%s/only.txt", src);
  WRITE_FILE(path, "src");
  snprintf(path, sizeof(path), "%s/extra.txt", dst);
  WRITE_FILE(path, "extra");
  EXPECT_EQ_INT(copy_dir(dst, src), 0);
  snprintf(path, sizeof(path), "%s/only.txt", dst);
  EXPECT_FILE_EQ(path, "src");
  snprintf(path, sizeof(path), "%s/extra.txt", dst);
  EXPECT_FILE_EQ(path, "extra");
  EXPECT_EQ_INT(rmdirr(tmpd), 0);

  TMP_PATH(src, "src");
  TMP_PATH(dst, "dst");
  EXPECT_EQ_INT(mkdirp(src, 0755), 0);
  EXPECT_EQ_INT(mkdirp(dst, 0755), 0);
  snprintf(path, sizeof(path), "%s/a/b/c", src);
  EXPECT_EQ_INT(mkdirp(path, 0755), 0);
  snprintf(path, sizeof(path), "%s/a/b/c/deep.txt", src);
  WRITE_FILE(path, "deep");
  EXPECT_EQ_INT(copy_dir(dst, src), 0);
  snprintf(path, sizeof(path), "%s/a/b/c/deep.txt", dst);
  EXPECT_FILE_EQ(path, "deep");
  EXPECT_EQ_INT(rmdirr(tmpd), 0);

  TMP_PATH(src, "src");
  TMP_PATH(dst, "dst");
  EXPECT_EQ_INT(mkdirp(src, 0755), 0);
  EXPECT_EQ_INT(mkdirp(dst, 0755), 0);
  snprintf(path, sizeof(path), "%s/over.txt", dst);
  WRITE_FILE(path, "old");
  snprintf(path, sizeof(path), "%s/over.txt", src);
  WRITE_FILE(path, "new");
  EXPECT_EQ_INT(copy_dir(dst, src), 0);
  snprintf(path, sizeof(path), "%s/over.txt", dst);
  EXPECT_FILE_EQ(path, "new");
  EXPECT_EQ_INT(rmdirr(tmpd), 0);
}
