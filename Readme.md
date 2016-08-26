This is the beginnings of the post-peer-review AFIO
v2 rewrite. You can view its documentation at https://ned14.github.io/boost.afio/

<b>master branch test status</b> Linux: [![Build Status](https://travis-ci.org/ned14/boost.afio.svg?branch=master)](https://travis-ci.org/ned14/boost.afio) Windows: [![Build status](https://ci.appveyor.com/api/projects/status/ox59o2r276xbmef7/branch/master?svg=true)](https://ci.appveyor.com/project/ned14/boost-afio/branch/master) Coverage: Boost.KernelTest support for coveralls.io still todo <b>CMake dashboard</b>: http://my.cdash.org/index.php?project=Boost.AFIO

Tarballs of source and prebuilt binaries with all unit tests passing: https://dedi4.nedprod.com/static/files/


CppCon 2016 todos:
- Fix remaining test failures.
- Raise the sanitisers on per-commit CI via ctest.
- Rename all ParseProjectVersionFromHpp etc to parse_project_version_from_hpp etc
- DLL library edition appears to not be encoding extended error code detail because
it's not sharing a single ringbuffer_log. Hard to fix given Outcome could be being
used by multiple libraries as a header only library, need to figure out some global
fix e.g. named shared memory.



Later:
- Each test runner needs to be compiled into many sanitising build variants
using the header only library
  - Also generate a DLL for each test kernel
- Single include generation
- Make updating revision.hpp updated by the pre-commit git hook


Notes on experimental automatic unit test framework:
- kernel.cpp needs to be separate from runner.cpp
  - kernel.cpp gets compiled using asan, lsan, msan and ubsan using AFIO as
    a header only library into a DLL/SO
    - ALSO compile normally to determine code bloat for some kernel (subtract
      an empty main() obviously)
- runner.cpp contains the handwritten parameter permutations to use
- kernel.cpp can be compiled using `-fsanitize-coverage=trace-pc -O3` on clang
3.9 or GCC 6.1 and linked against the edge support library. It will spit out
a percentage of total edges executed which needs to be reported to cdash somehow.
  - What is total edges? It can't be the *actual* total because virtual functions
    will implement edges never called by a kernel. I guess what we really want is:
    
    "Percentage of edges executed not skipped per function"
    
    But even this isn't particularly accurate as for 100% test coverage you need
    to exercise every possible call graph between edges that is possible. The
    compiler can tell some of this, but even then not all because the inputs
    may be guaranteed to be bounded which prevents some call graphs being possible.
    I guess for this we cannot avoid a fuzzer.
    
  - FUTURE Every edge needs to be mapped back onto the source and each line
    of source code given an intensity of green vs red to indicate coverage
  - FUTURE Need to generate a graph of executed edges and display it on the
    right hand side of a HTML source code listing. https://jsplumbtoolkit.com/
    has a community edition letting you draw graphs on a web page.
- FUTURE write a custom DLL/SO loader which returns error codes from
system calls to exercise even more branch points.
- FUTURE automatic possible error returns calculator for the documentation



- tests/<test_name>/auto_tests.cpp is what is generated by the clang AST tool
from statically inspecting the AST generated by kernel.cpp.
  - For each returned result<T> whose state might be set to errored:
    - For each possible error_code it might be set to:
      - Generate a permutation matrix of every combination of error_code
      value possible for every error_code set in the AST

Therefore:
- [ ] Need some lightweight opt-compile-in mechanism of having every result<T>
in a call sequence be overridden with some errored return.
  - How do I identify which result<T> in a sequence? Do I rewrite and output
  a custom AST per permutation?
  - Integration test would be expected to fail during these permutations, but
  we probably have to still call the syscalls in case they have side effects

  
  
  
Todo:
- [ ] algorithm::atomic_append needs improvements:
  - Trap if append exceeds 2^63 and do something useful with that
  - Fix the known inefficiencies in the implementation:
    - We should use byte range locks when concurrency*entities is low
    - We have an extra read() during locking between the scanning for our lock
    request and scanning for other lock requests
    - During scan when hashes mismatch we reload a suboptimal range
    - We should use memory maps until a SMB/NFS/QNX/OpenBSD lock_request comes
    in whereafter we should degrade to normal i/o gracefully
- [ ] In DEBUG builds, have io_handle always not fill buffers passed to remind
people to use pointers returned!

- [ ] Port AFIO v2 back to POSIX
  - [ ] delete on close on Linux could be implemented using a clone() and monitoring
parent process for exit, then trying to take a write oplock and if success
unlinking the file.
  - [ ] Maybe best actually to create a delete_on_close_file_handle to encapsulate
all the hefty code for POSIX.


- [ ] Add mapped_file_handle
  - Use one-two-three level page system, so 4Kb/2Mb/?. Files under 2Mb need just
one indirection.
  - Page tables need to also live in a potentially mapped file
  - Need some way of explicitly converting a file_handle into a mapped_file_handle
and vice versa.
  - Could speculatively map 4Kb chunks lazily and keep an internal map of 4Kb
offsets to map. This allows more optimal handing of growing files.
  - WOULD BE NICE: Copy on Write support which collates a list of dirtied 4Kb
pages and could write those out as a delta.
    - LATER: Use guard pages to toggle dirty flag per initial COW
- [ ] Rewrite correctness test in benchmark_locking to use mapped_file handle.


- [ ] Outcome's error logging needs to record current thread id ideally.
- [ ] Move caching into native_handle_type.
- [ ] Add layer between io_handle and (file|async_file)_handle for locking?
- [ ] delete_on_close really needs an oplocks API to work somewhat like on Windows
(with races if every user doesn't turn on the oplocks emulation however)
  - oplocks API can be simulated by range locks on some common byte offset
on POSIX not Linux. Linux has proper kernel oplocks API, but there is a race between
taking the write oplock and unlinking - a breaking open() may end up with a deleted
file entry.

- [ ] Implement [[bindlib::make_free]] which injects member functions into the enclosing
namespace.
- [ ] Add macro helpers to Outcome for returning outcomes out of things
which cannot return values like constructors, and convert said exceptions/TLS
back into outcomes.
 - Make use of std::system_error(errno, system_category, "custom error message");
- [ ] Get Outcome to work perfectly with exceptions and RTTI disabled, this makes
Outcome useful in the games/audio world.
  - When exceptions are disabled, disable outcome<T>? Just have result<T>?
  - [ ] Add unit tests proving it for all platforms.
  - [ ] Move AFIO to being tested with exceptions and RTTI disabled. Where AFIO 
throws, have it detect __cpp_exceptions and skip those implementations.
- [ ] There is much duplicate and sloppy code in AFIO v2. Reduce and eliminate.

- [ ] C bindings for all AFIO v2 APIs. Write libclang parser which autogenerates
SWIG interface files from the .hpp files.


- [ ] Add native BSD kqueues to POSIX AIO backend as is vastly more efficient.
  - http://www.informit.com/articles/article.aspx?p=607373&seqNum=4 is a
very useful programming guide for POSIX AIO.
- [ ] Port to Linux KAIO
  - http://linux.die.net/man/2/io_getevents would be in the run() loop.
pthread_sigqueue() can be used by post() to cause aio_suspend() to break
early to run user supplied functions.
- [ ] Add to docs for every API the number of malloc + free performed.
  - Unit test op codes generated per set of i/o calls 
- [ ] Don't run the cpu and sys tests if cpu and sys ids already in fs_probe_results.yaml
  - Need to uniquely fingerprint a machine somehow?
- [ ] Fatter afio::path. We probably need to allow relative paths
based on a handle and fragment in afio::path, therefore might as well encapsulate
NT kernel vs win32 paths in there too.
- [ ] Add monitoring of CPU usage to tests. See GetThreadTimes. Make sure
worker thread times are added into results.
- [ ] Configurable tracking of op latency and throughput (bytes) for all
handles on some storage i.e. storage needs to be kept in a global map.
  - Something which strongly resembles the memory bandwidth test
  - [ ] Should have decile bucketing e.g. percentage in bottom 10%, percentage
  in next 10% etc. Plus mean and stddev.
  - [ ] Should either be resettable or subtractable i.e. points can be diffed.
  - [ ] Add IOPS QD=1..N storage profile test
  - [ ] Add throughput storage profile test
- [ ] Output into YAML comparable hashes for OS + device + FS + flags
so we can merge partial results for some combo into the results database.
- [ ] Write YAML parsing tool which merges fs_probe_results.yaml into
the results directory where flags and OS get its own directory and each YAML file
is named FS + device e.g.
  - results/win64 direct=1 sync=0/NTFS + WDC WD30EFRX-68EUZN0
- [ ] virtual handle::path_type handle::path(bool refresh=false) should be added using
GetFinalPathNameByHandle(FILE_NAME_OPENED). VOLUME_NAME_DOS vs VOLUME_NAME_NT should
depend on the current afio::path setting.
- [ ] directory_handle
- [ ] symlink_handle
- [ ] pipe_handle? If so, child_process can use that instead of doing its own
thing. Would be nice purely for conformance checking that io_handle layers
downwards are correct.
- [ ] Missing functions on handle/file_handle from AFIO v1
- [ ] Proper temporary file support
  - [ ] Need discovery routine - may need directory_handle support first.
  - [ ] Need to do something about file creation permissions as temp files
probably need to be user access only

boost::afio::algorithm::todo:

- [ ] Store in EA or a file called .spookyhashes or .spookyhash the 128 bit hash of
a file and the time it was calculated. This can save lots of hashing work later.
- [ ] Correct directory hierarchy delete
  i.e.:
  - Delete files first tips to trunk, retrying for some given timeout. If fail to
  immediately delete, rename to base directory under a long random hex name, try
  to delete again.
  - Only after all files have been deleted, delete directories. If new files appear
  during directory deletion, loop.
  - Options:
    - Rename base directory(ies) to something random to atomically prevent lookup.
    - Delete all or nothing (i.e. rename all files into another tree checking
    permissions etc, if successful only then delete)
- [ ] Correct directory hierarchy copy
  - Optional backup semantics (i.e. copy all ACLs, metadata etc)
  - Intelligent retry for some given timeout before failing.
  - Optional fixup of any symbolic links pointing into copied tree.
  - Optional copy directory structure but hard or symbolic linking files.
    - Symbolic links optionally are always absolute paths instead of relative.
  - Optional deference all hard links and/or symbolic links into real files.
- [ ] Correct directory hierarchy move
- [ ] Correct directory hierarchy update (i.e. changes only)
- [ ] Make directory tree C by cloning tree B to tree B, and then updating tree C
with changes from tree A. The idea is for an incremental backup of changes over
time but saving storage where possible.
- [ ] Replace all content (including EA) duplicate files in a tree with hardlinks.
- [ ] Figure out all hard linked file entries for some inode.
- [ ] Generate list of all hard linked files in a tree (i.e. refcount>1) and which
are the same inode.

## Commits and tags in this git repository can be verified using:
<pre>
-----BEGIN PGP PUBLIC KEY BLOCK-----
Version: GnuPG v2

mDMEVvMacRYJKwYBBAHaRw8BAQdAp+Qn6djfxWQYtAEvDmv4feVmGALEQH/pYpBC
llaXNQe0WE5pYWxsIERvdWdsYXMgKHMgW3VuZGVyc2NvcmVdIHNvdXJjZWZvcmdl
IHthdH0gbmVkcHJvZCBbZG90XSBjb20pIDxzcGFtdHJhcEBuZWRwcm9kLmNvbT6I
eQQTFggAIQUCVvMacQIbAwULCQgHAgYVCAkKCwIEFgIDAQIeAQIXgAAKCRCELDV4
Zvkgx4vwAP9gxeQUsp7ARMFGxfbR0xPf6fRbH+miMUg2e7rYNuHtLQD9EUoR32We
V8SjvX4r/deKniWctvCi5JccgfUwXkVzFAk=
=puFk
-----END PGP PUBLIC KEY BLOCK-----
</pre>