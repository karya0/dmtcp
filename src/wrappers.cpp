/* ptsname_r is defined with "always_inline" attribute. GCC>=4.7 disallows us
 * to define the ptsname_r wrapper if compiled with -O0. Here we are renaming
 * ptsname_r so that later code does not see the declaration of ptsname_r as
 * inline. Normal user code from other files will continue to invoke ptsname_r
 * as inline as an inline function calling __ptsname_r_chk. Later in this file
 * we define __ptsname_r_chk to call the original ptsname_r symbol.
 * Similarly, for ttyname_r, etc.
 *
 * Also, on some machines (e.g. SLES 10), readlink has conflicting return types
 * (ssize_t and int).
 *     In general, we rename the functions below, since any type declarations
 * may vary on different systems, and so we ignore these type declarations.
*/
#define open     open_always_inline
#define open64   open64_always_inline

#include <fcntl.h>
#include <stdarg.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#undef open
#undef open64
#undef openat
#undef openat64

#include "dmtcp.h"

namespace dmtcp
{

extern DmtcpWrappers localWrappers;

extern "C" void __attribute__((constructor(101)))
dmtcp_initialize_entry_point();

static int
open_work(const char *path, int flags, mode_t mode)
{
  dmtcp_initialize_entry_point();

  DMTCP_PLUGIN_DISABLE_CKPT();

  int fd = localWrappers.real_open(path, flags, mode);

  DMTCP_PLUGIN_ENABLE_CKPT();

  return fd;
}


extern "C" int
open(const char *path, int flags, ...)
{
  mode_t mode = 0;

  if (flags & O_CREAT) {
    va_list arg;
    va_start(arg, flags);
    mode = va_arg(arg, int);
    va_end(arg);
  }

  return open_work(path, flags, mode);
}


extern "C" int
__open_2(const char *path, int flags)
{
  return open_work(path, flags, 0);
}


// FIXME: The 'fn64' version of functions is defined only when within
// __USE_LARGEFILE64 is #defined. The wrappers in this file need to consider
// this fact. The problem can occur, for example, when DMTCP is not compiled
// with __USE_LARGEFILE64 whereas the user-binary is. In that case the open64()
// call from user will come to DMTCP and DMTCP might fail to execute it
// properly.

// FIXME: Add the 'fn64' wrapper test cases to dmtcp test suite.
extern "C" int
open64(const char *path, int flags, ...)
{
  mode_t mode = 0;

  // Handling the variable number of arguments
  if (flags & O_CREAT) {
    va_list arg;
    va_start(arg, flags);
    mode = va_arg(arg, int);
    va_end(arg);
  }

  return open_work(path, flags | O_LARGEFILE, mode);
}


extern "C" int
__open64_2(const char *path, int flags)
{
  return open_work(path, flags | O_LARGEFILE, 0);
}


static FILE *
fopen_work(const char *path, const char *mode)
{
  dmtcp_initialize_entry_point();

  DMTCP_PLUGIN_DISABLE_CKPT();

  FILE *fp = localWrappers.real_fopen(path, mode);

  DMTCP_PLUGIN_ENABLE_CKPT();

  return fp;
}


extern "C" FILE *
fopen(const char *path, const char *mode)
{
  return fopen_work(path, mode);
}


extern "C" FILE *
fopen64(const char *path, const char *mode)
{
  return fopen_work(path, mode);
}


static FILE *
freopen_work(const char *path, const char *mode, FILE * stream)
{
  dmtcp_initialize_entry_point();

  DMTCP_PLUGIN_DISABLE_CKPT();

  FILE *file = localWrappers.real_freopen(path, mode, stream);

  DMTCP_PLUGIN_ENABLE_CKPT();
  return file;
}


extern "C" FILE *
freopen(const char *path, const char *mode, FILE * stream)
{
  return freopen_work(path, mode, stream);
}


extern "C" FILE *
freopen64(const char *path, const char *mode, FILE * stream)
{
  return freopen_work(path, mode, stream);
}


static int
openat_work(int dirfd, const char *path, int flags, mode_t mode)
{
  dmtcp_initialize_entry_point();

  DMTCP_PLUGIN_DISABLE_CKPT();

  int fd = localWrappers.real_openat(dirfd, path, flags, mode);

  DMTCP_PLUGIN_ENABLE_CKPT();

  return fd;
}


extern "C" int
openat(int dirfd, const char *path, int flags, ...)
{
  va_list arg;

  va_start(arg, flags);
  mode_t mode = va_arg(arg, int);
  va_end(arg);

  return openat_work(dirfd, path, flags, mode);
}


extern "C" int
openat_2(int dirfd, const char *path, int flags)
{
  return openat_work(dirfd, path, flags, 0);
}


extern "C" int
__openat_2(int dirfd, const char *path, int flags)
{
  return openat_work(dirfd, path, flags, 0);
}


extern "C" int
openat64(int dirfd, const char *path, int flags, ...)
{
  va_list arg;

  va_start(arg, flags);
  mode_t mode = va_arg(arg, int);
  va_end(arg);

  return openat_work(dirfd, path, flags | O_LARGEFILE, mode);
}


extern "C" int
openat64_2(int dirfd, const char *path, int flags)
{
  return openat_work(dirfd, path, flags | O_LARGEFILE, 0);
}


extern "C" int
__openat64_2(int dirfd, const char *path, int flags)
{
  return openat_work(dirfd, path, flags | O_LARGEFILE, 0);
}


extern "C" int
creat(const char *path, mode_t mode)
{
  // creat() is equivalent to open() with flags equal to
  // O_CREAT|O_WRONLY|O_TRUNC
  return open_work(path, O_CREAT | O_WRONLY | O_TRUNC, mode);
}


extern "C" int
creat64(const char *path, mode_t mode)
{
  // creat() is equivalent to open() with flags equal to
  // O_CREAT|O_WRONLY|O_TRUNC
  return open_work(path, O_CREAT | O_WRONLY | O_TRUNC | O_LARGEFILE, mode);
}

}
