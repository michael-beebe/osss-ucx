/* For license: see LICENSE file at top-level */

#ifndef _SHMEM_DEFS_SUBST_H
#define _SHMEM_DEFS_SUBST_H 1

#define SHMEM_MAJOR_VERSION 1
#define SHMEM_MINOR_VERSION 4
#define SHMEM_MAX_NAME_LEN  64
#define SHMEM_VENDOR_STRING "osss-ucx"

#ifdef PR463

/*
 * https://github.com/openshmem-org/specification/issues/463
 *
 */

#define SHMEM_VENDOR_MAJOR_VERSION 1
#define SHMEM_VENDOR_MINOR_VERSION 0
#define SHMEM_VENDOR_PATCH_VERSION 0

#endif

#endif  /* ! _SHMEM_DEFS_SUBST_H */
