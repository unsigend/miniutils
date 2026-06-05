#ifndef FS_GENERAL_H
#define FS_GENERAL_H

#include <errno.h>
#include <fcntl.h>
#include <fs.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utest.h>

static const char *tmpd = "./build/tmp/";

#define TMP_PATH(buf, suffix) snprintf((buf), sizeof(buf), "%s" suffix, tmpd)

#define TMP_ENSURE() (void)rmdirr(tmpd)

#define TMP_INIT()                                                             \
  do {                                                                         \
    (void)rmdirr(tmpd);                                                        \
    EXPECT_EQ_INT(mkdirp(tmpd, 0755), 0);                                      \
  } while (0)

#define EXPECT_FAIL(call, err)                                                 \
  do {                                                                         \
    errno = 0;                                                                 \
    EXPECT_EQ_INT((call), -1);                                                 \
    EXPECT_EQ_INT(errno, (err));                                               \
  } while (0)

#define TOUCH_FILE(path)                                                       \
  do {                                                                         \
    int _fd = open((path), O_CREAT | O_WRONLY | O_TRUNC, 0644);                \
    EXPECT_GE_INT(_fd, 0);                                                     \
    EXPECT_EQ_INT(close(_fd), 0);                                              \
  } while (0)

#define WRITE_FILE(path, data)                                                 \
  do {                                                                         \
    int _fd = open((path), O_CREAT | O_WRONLY | O_TRUNC, 0644);                \
    EXPECT_GE_INT(_fd, 0);                                                     \
    EXPECT_EQ_INT(write(_fd, (data), strlen(data)), (ssize_t)strlen(data));    \
    EXPECT_EQ_INT(close(_fd), 0);                                              \
  } while (0)

#define EXPECT_FILE_EQ(path, data)                                             \
  do {                                                                         \
    char _buf[64] = {0};                                                       \
    int _fd = open((path), O_RDONLY);                                          \
    EXPECT_GE_INT(_fd, 0);                                                     \
    EXPECT_EQ_INT(read(_fd, _buf, sizeof(_buf) - 1), (ssize_t)strlen(data));   \
    EXPECT_EQ_INT(close(_fd), 0);                                              \
    EXPECT_EQ_STR(_buf, (data));                                               \
  } while (0)

#define EXPECT_ABSPATH_SAME_REALPATH(path)                                     \
  do {                                                                         \
    char _expect[PATH_MAX];                                                    \
    char _buf[PATH_MAX];                                                       \
    EXPECT_NOTNULL(realpath((path), _expect));                                 \
    EXPECT_EQ_INT(abspath((path), _buf), 0);                                   \
    EXPECT_EQ_STR(_buf, _expect);                                              \
  } while (0)

#define EXPECT_ABSPATH_EQ(path, expect)                                        \
  do {                                                                         \
    char _buf[PATH_MAX];                                                       \
    EXPECT_EQ_INT(abspath((path), _buf), 0);                                   \
    EXPECT_EQ_STR(_buf, (expect));                                             \
  } while (0)

#endif
