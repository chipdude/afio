/* Information about the volume storing a file
(C) 2015-2017 Niall Douglas <http://www.nedproductions.biz/> (8 commits)
File Created: Dec 2015


Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License in the accompanying file
Licence.txt or at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


Distributed under the Boost Software License, Version 1.0.
    (See accompanying file Licence.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AFIO_STATFS_H
#define AFIO_STATFS_H

#ifndef AFIO_CONFIG_HPP
#error You must include the master afio.hpp, not individual header files directly
#endif
#include "config.hpp"

//! \file statfs.hpp Provides statfs

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)  // dll interface
#endif

AFIO_V2_NAMESPACE_EXPORT_BEGIN

class handle;

/*! \struct statfs_t
\brief Metadata about a filing system. Unsupported entries are all bits set.
*/
struct AFIO_DECL statfs_t
{
  struct f_flags_t
  {
    uint32_t rdonly : 1;           //!< Filing system is read only                                      (Windows, POSIX)
    uint32_t noexec : 1;           //!< Filing system cannot execute programs                           (POSIX)
    uint32_t nosuid : 1;           //!< Filing system cannot superuser                                  (POSIX)
    uint32_t acls : 1;             //!< Filing system provides ACLs                                     (Windows, POSIX)
    uint32_t xattr : 1;            //!< Filing system provides extended attributes                      (Windows, POSIX)
    uint32_t compression : 1;      //!< Filing system provides whole volume compression                 (Windows, POSIX)
    uint32_t extents : 1;          //!< Filing system provides extent based file storage (sparse files) (Windows, POSIX)
    uint32_t filecompression : 1;  //!< Filing system provides per-file selectable compression          (Windows)
  } f_flags;                       /*!< copy of mount exported flags       (Windows, POSIX) */
  uint64_t f_bsize;                /*!< fundamental filesystem block size  (Windows, POSIX) */
  uint64_t f_iosize;               /*!< optimal transfer block size        (Windows, POSIX) */
  uint64_t f_blocks;               /*!< total data blocks in filesystem    (Windows, POSIX) */
  uint64_t f_bfree;                /*!< free blocks in filesystem          (Windows, POSIX) */
  uint64_t f_bavail;               /*!< free blocks avail to non-superuser (Windows, POSIX) */
  uint64_t f_files;                /*!< total file nodes in filesystem     (POSIX) */
  uint64_t f_ffree;                /*!< free nodes avail to non-superuser  (POSIX) */
  uint32_t f_namemax;              /*!< maximum filename length            (Windows, POSIX) */
#ifndef _WIN32
  int16_t f_owner; /*!< user that mounted the filesystem   (BSD, OS X) */
#endif
  uint64_t f_fsid[2];        /*!< filesystem id                      (Windows, POSIX) */
  std::string f_fstypename;  /*!< filesystem type name               (Windows, POSIX) */
  std::string f_mntfromname; /*!< mounted filesystem                 (Windows, POSIX) */
  fixme_path f_mntonname;    /*!< directory on which mounted         (Windows, POSIX) */

  //! Used to indicate what metadata should be filled in
  QUICKCPPLIB_BITFIELD_BEGIN(want) { flags = 1 << 0, bsize = 1 << 1, iosize = 1 << 2, blocks = 1 << 3, bfree = 1 << 4, bavail = 1 << 5, files = 1 << 6, ffree = 1 << 7, namemax = 1 << 8, owner = 1 << 9, fsid = 1 << 10, fstypename = 1 << 11, mntfromname = 1 << 12, mntonname = 1 << 13, all = (unsigned) -1 }
  QUICKCPPLIB_BITFIELD_END(want)
  //! Constructs a default initialised instance (all bits set)
  statfs_t()
  {
    size_t frontbytes = ((char *) &f_fstypename) - ((char *) this);
    memset(this, 0xff, frontbytes);
    memset(this, 0, sizeof(f_flags));
  }
#ifdef __cpp_exceptions
  //! Constructs a filled instance, throwing as an exception any error which might occur
  statfs_t(const handle &h, want wanted = want::all)
      : statfs_t()
  {
    auto v(fill(h, wanted));
    if(v.has_error())
      throw std::system_error(v.error());
  }
#endif
  //! Fills in the structure with metadata, returning number of items filled in
  AFIO_HEADERS_ONLY_MEMFUNC_SPEC result<size_t> fill(const handle &h, want wanted = want::all) noexcept;
};

AFIO_V2_NAMESPACE_END

#if AFIO_HEADERS_ONLY == 1 && !defined(DOXYGEN_SHOULD_SKIP_THIS)
#define AFIO_INCLUDED_BY_HEADER 1
#ifdef _WIN32
#include "detail/impl/windows/statfs.ipp"
#else
#include "detail/impl/posix/statfs.ipp"
#endif
#undef AFIO_INCLUDED_BY_HEADER
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif
