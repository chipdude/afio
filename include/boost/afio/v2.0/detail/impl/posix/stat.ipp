/* stat.hpp
Information about a file
(C) 2015-2017 Niall Douglas http://www.nedprod.com/
File Created: Apr 2017


Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include "../../../handle.hpp"
#include "../../../stat.hpp"
#include "import.hpp"

BOOST_AFIO_V2_NAMESPACE_BEGIN

BOOST_AFIO_HEADERS_ONLY_MEMFUNC_SPEC result<size_t> stat_t::fill(handle &h, stat_t::want wanted) noexcept
{
  BOOST_AFIO_LOG_FUNCTION_CALL(h.native_handle().fd);
  struct stat_t s;
  memset(&s, 0, sizeof(s));
  if(-1 == ::fstat(h.native_handle().fd, &s))
    return make_errored_result<size_t>(errno, last190(h.path()));
  if(wanted&want::dev) { st_dev=s.st_dev; ++ret; }
  if(wanted&want::ino) { st_ino=s.st_ino; ++ret; }
  if(wanted&want::type) { st_type=to_st_type(s.st_mode); ++ret; }
  if(wanted&want::perms) { st_perms=s.st_mode & 0xfff; ++ret; }
  if(wanted&want::nlink) { st_nlink=s.st_nlink; ++ret; }
  if(wanted&want::uid) { st_uid=s.st_uid; ++ret; }
  if(wanted&want::gid) { st_gid=s.st_gid; ++ret; }
  if(wanted&want::rdev) { st_rdev=s.st_rdev; ++ret; }
#ifdef __ANDROID__
  if(wanted&want::atim) { st_atim=to_timepoint(*((struct timespec *)&s.st_atime)); ++ret; }
  if(wanted&want::mtim) { st_mtim=to_timepoint(*((struct timespec *)&s.st_mtime)); ++ret; }
  if(wanted&want::ctim) { st_ctim=to_timepoint(*((struct timespec *)&s.st_ctime)); ++ret; }
#elif defined(__APPLE__)
  if(wanted&want::atim) { st_atim=to_timepoint(s.st_atimespec); ++ret; }
  if(wanted&want::mtim) { st_mtim=to_timepoint(s.st_mtimespec); ++ret; }
  if(wanted&want::ctim) { st_ctim=to_timepoint(s.st_ctimespec); ++ret; }
#else  // Linux and BSD
  if(wanted&want::atim) { st_atim=to_timepoint(s.st_atim); ++ret; }
  if(wanted&want::mtim) { st_mtim=to_timepoint(s.st_mtim); ++ret; }
  if(wanted&want::ctim) { st_ctim=to_timepoint(s.st_ctim); ++ret; }
#endif
  if(wanted&want::size) { st_size=s.st_size; ++ret; }
  if(wanted&want::allocated) { st_allocated=(handle::extent_type) s.st_blocks*512; ++ret; }
  if(wanted&want::blocks) { st_blocks=s.st_blocks; ++ret; }
  if(wanted&want::blksize) { st_blksize=s.st_blksize; ++ret; }
#ifdef HAVE_STAT_FLAGS
  if(wanted&want::flags) { st_flags=s.st_flags; ++ret; }
#endif
#ifdef HAVE_STAT_GEN
  if(wanted&want::gen) { st_gen=s.st_gen; ++ret; }
#endif
#ifdef HAVE_BIRTHTIMESPEC
#if defined(__APPLE__)
  if(wanted&want::birthtim) { st_birthtim=to_timepoint(s.st_birthtimespec); ++ret; }
#else
  if(wanted&want::birthtim) { st_birthtim=to_timepoint(s.st_birthtim); ++ret; }
#endif
#endif
  if(wanted&want::sparse) { st_sparse=((handle::extent_type) s.st_blocks*512)<(handle::extent_type) s.st_size; ++ret; }
  return ret;
}

BOOST_AFIO_V2_NAMESPACE_END