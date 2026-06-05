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

  EXPECT_EQ_INT(unlink(path), 0);
  EXPECT_EQ_INT(rmdir(tmpd), 0);
}
