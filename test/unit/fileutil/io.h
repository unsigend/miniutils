#include "general.h"

UTEST_CASE(read_all)
{
  char path[PATH_MAX];
  char buf[64] = {0};
  int fd;
  int pipefd[2];

  TMP_INIT();
  TMP_PATH(path, "data");
  WRITE_FILE(path, "hello");

  fd = open(path, O_RDONLY);
  EXPECT_GE_INT(fd, 0);
  EXPECT_EQ_INT(read_all(fd, buf, 5), 5);
  EXPECT_EQ_STR(buf, "hello");
  EXPECT_EQ_INT(close(fd), 0);

  fd = open(path, O_RDONLY);
  EXPECT_GE_INT(fd, 0);
  memset(buf, 0, sizeof(buf));
  EXPECT_EQ_INT(read_all(fd, buf, 64), 5);
  EXPECT_EQ_STR(buf, "hello");
  EXPECT_EQ_INT(close(fd), 0);

  EXPECT_EQ_INT(pipe(pipefd), 0);
  EXPECT_EQ_INT(write(pipefd[1], "pipe", 4), 4);
  EXPECT_EQ_INT(close(pipefd[1]), 0);
  memset(buf, 0, sizeof(buf));
  EXPECT_EQ_INT(read_all(pipefd[0], buf, 4), 4);
  EXPECT_EQ_STR(buf, "pipe");
  EXPECT_EQ_INT(close(pipefd[0]), 0);

  EXPECT_FAIL(read_all(-1, buf, 1), EBADF);

  fd = open(path, O_RDONLY);
  EXPECT_GE_INT(fd, 0);
  EXPECT_EQ_INT(read_all(fd, buf, 0), 0);
  EXPECT_EQ_INT(close(fd), 0);

  EXPECT_EQ_INT(unlink(path), 0);
  EXPECT_EQ_INT(rmdir(tmpd), 0);
}

UTEST_CASE(write_all)
{
  char path[PATH_MAX];
  char buf[16] = {0};
  int fd;
  int pipefd[2];

  TMP_INIT();
  TMP_PATH(path, "out");
  fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
  EXPECT_GE_INT(fd, 0);
  EXPECT_EQ_INT(write_all(fd, "abcd", 4), 4);
  EXPECT_EQ_INT(close(fd), 0);
  EXPECT_FILE_EQ(path, "abcd");

  EXPECT_EQ_INT(pipe(pipefd), 0);
  EXPECT_EQ_INT(write_all(pipefd[1], "xyz", 3), 3);
  EXPECT_EQ_INT(close(pipefd[1]), 0);
  EXPECT_EQ_INT(read(pipefd[0], buf, sizeof(buf)), 3);
  EXPECT_EQ_STR(buf, "xyz");
  EXPECT_EQ_INT(close(pipefd[0]), 0);

  EXPECT_FAIL(write_all(-1, "x", 1), EBADF);

  EXPECT_EQ_INT(write_all(1, "", 0), 0);

  EXPECT_EQ_INT(unlink(path), 0);
  EXPECT_EQ_INT(rmdir(tmpd), 0);
}

UTEST_CASE(read_file)
{
  char path[PATH_MAX];
  void *buf = NULL;
  size_t len = 0;
  char big[4096];

  TMP_INIT();
  TMP_PATH(path, "data.txt");

  WRITE_FILE(path, "hello");
  EXPECT_EQ_INT(read_file(path, &buf, &len), 0);
  EXPECT_NOTNULL(buf);
  EXPECT_EQ_INT((int)len, 5);
  EXPECT_EQ_STR((char *)buf, "hello");
  free(buf);
  buf = NULL;

  TOUCH_FILE(path);
  EXPECT_EQ_INT(read_file(path, &buf, &len), 0);
  EXPECT_NULL(buf);
  EXPECT_EQ_INT((int)len, 0);

  memset(big, 'z', sizeof(big) - 1);
  big[sizeof(big) - 1] = '\0';
  WRITE_FILE(path, big);
  EXPECT_EQ_INT(read_file(path, &buf, &len), 0);
  EXPECT_NOTNULL(buf);
  EXPECT_EQ_INT((int)len, (int)(sizeof(big) - 1));
  EXPECT_EQ_STR((char *)buf, big);
  free(buf);
  buf = NULL;
  EXPECT_EQ_INT(unlink(path), 0);

  TMP_PATH(path, "missing");
  EXPECT_FAIL(read_file(path, &buf, &len), ENOENT);

  EXPECT_FAIL(read_file(tmpd, &buf, &len), EINVAL);
  EXPECT_FAIL(read_file(".", &buf, &len), EINVAL);

  EXPECT_EQ_INT(rmdir(tmpd), 0);
}

UTEST_CASE(write_file)
{
  char path[PATH_MAX];
  char longpath[PATH_MAX];
  void *buf = NULL;
  size_t len = 0;

  TMP_INIT();
  TMP_PATH(path, "out.txt");

  EXPECT_EQ_INT(write_file(path, "hello", 5), 0);
  EXPECT_TRUE(file_exists(path));
  EXPECT_FILE_EQ(path, "hello");

  EXPECT_EQ_INT(write_file(path, "world", 5), 0);
  EXPECT_FILE_EQ(path, "world");

  EXPECT_EQ_INT(read_file(path, &buf, &len), 0);
  EXPECT_EQ_INT((int)len, 5);
  EXPECT_EQ_STR((char *)buf, "world");
  free(buf);
  buf = NULL;
  EXPECT_EQ_INT(unlink(path), 0);

  EXPECT_FAIL(write_file(path, "x", 0), EINVAL);
  EXPECT_FAIL(write_file(path, NULL, 1), EINVAL);
  EXPECT_FALSE(file_exists(path));

  TMP_PATH(path, "missing/out.txt");
  EXPECT_FAIL(write_file(path, "x", 1), ENOENT);

  memset(longpath, 'a', PATH_MAX - 1);
  longpath[0] = '/';
  longpath[PATH_MAX - 4] = '\0';
  EXPECT_FAIL(write_file(longpath, "x", 1), ENAMETOOLONG);

  EXPECT_EQ_INT(rmdir(tmpd), 0);
}
