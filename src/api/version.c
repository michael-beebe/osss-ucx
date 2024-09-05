/* For license: see LICENSE file at top-level */

/* Check if the configuration file exists and include it if available */
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include "shmemu.h"  /* Include for SHMEM utility functions and error checking */

#ifdef ENABLE_PSHMEM
/*
 * If profiling is enabled, create weak symbols for the profiling versions
 * of shmem_info_get_version and shmem_info_get_name, allowing the 
 * implementation to use either the default or profiling versions.
 */
#pragma weak shmem_info_get_version = pshmem_info_get_version
#define shmem_info_get_version pshmem_info_get_version
#pragma weak shmem_info_get_name = pshmem_info_get_name
#define shmem_info_get_name pshmem_info_get_name
#endif /* ENABLE_PSHMEM */

/*
 * Function to retrieve the SHMEM API version, returning the major and minor
 * version numbers.
 * 
 * @param major: Pointer to an integer where the major version will be stored.
 * @param minor: Pointer to an integer where the minor version will be stored.
 */
void
shmem_info_get_version(int *major, int *minor)
{
    /* Ensure SHMEM is initialized before querying the version */
    SHMEMU_CHECK_INIT();

    /* Check that the provided major and minor pointers are not NULL */
    SHMEMU_CHECK_NOT_NULL(major, 1);
    SHMEMU_CHECK_NOT_NULL(minor, 2);

    /* Set the major and minor versions from SHMEM constants */
    *major = SHMEM_MAJOR_VERSION;
    *minor = SHMEM_MINOR_VERSION;
}

/*
 * Function to retrieve the SHMEM vendor's name.
 * 
 * @param name: Pointer to a character array where the vendor's name will be stored.
 */
void
shmem_info_get_name(char *name)
{
    /* Ensure SHMEM is initialized before querying the vendor name */
    SHMEMU_CHECK_INIT();

    /* Check that the provided name pointer is not NULL */
    SHMEMU_CHECK_NOT_NULL(name, 1);

    /* Copy the vendor name to the provided buffer, ensuring it is within max length */
    STRNCPY_SAFE(name, SHMEM_VENDOR_STRING, SHMEM_MAX_NAME_LEN);
}

#ifdef PR463

/*
 * Reference to GitHub issue #463 in the OpenSHMEM specification repository,
 * which involves additional versioning functions.
 * URL: https://github.com/openshmem-org/specification/issues/463
 */

#define SHMEM_VERSION_CVT(_maj, _min)           \
    ( (100 * (_maj)) + (_min) )

/* Convert the vendor version using major, minor, and patch values */
#define SHMEM_VENDOR_VERSION_CVT(_maj, _min, _pth)          \
    ( ( 100 * SHMEM_VERSION_CVT(_maj, _min) ) + (_pth) )

/* Save the current SHMEM API version */
static const int saved_version =
    SHMEM_VERSION_CVT(SHMEM_MAJOR_VERSION, SHMEM_MINOR_VERSION);

/* Save the current vendor version */
static const int saved_vendor_version =
    SHMEM_VENDOR_VERSION_CVT(SHMEM_VENDOR_MAJOR_VERSION,
                             SHMEM_VENDOR_MINOR_VERSION,
                             SHMEM_VENDOR_PATCH_VERSION);

#ifdef ENABLE_PSHMEM
/* Weak symbols for profiling versions of the version number functions */
#pragma weak shmem_info_get_version_number = pshmem_info_get_version_number
#define shmem_info_get_version_number pshmem_info_get_version_number
#endif /* ENABLE_PSHMEM */

/*
 * Function to get the SHMEM API version as a single integer.
 * 
 * @param version: Pointer to an integer where the version number will be stored.
 */
void
shmem_info_get_version_number(int *version)
{
    /* Ensure SHMEM is initialized before querying the version number */
    SHMEMU_CHECK_INIT();

    /* Check that the provided version pointer is not NULL */
    SHMEMU_CHECK_NOT_NULL(version, 1);

    /* Store the saved version (major + minor combined) in the provided pointer */
    *version = saved_version;
}

#ifdef ENABLE_PSHMEM
/* Weak symbol for profiling version of the vendor version function */
#pragma weak shmem_info_get_vendor_version = pshmem_info_get_vendor_version
#define shmem_info_get_vendor_version pshmem_info_get_vendor_version
#endif /* ENABLE_PSHMEM */

/*
 * Function to get the SHMEM vendor version, including major, minor, and patch values.
 * 
 * @param major: Pointer to an integer where the major version will be stored.
 * @param minor: Pointer to an integer where the minor version will be stored.
 * @param patch: Pointer to an integer where the patch version will be stored.
 */
void
shmem_info_get_vendor_version(int *major, int *minor, int *patch)
{
    /* Ensure SHMEM is initialized before querying the vendor version */
    SHMEMU_CHECK_INIT();

    /* Check that the major, minor, and patch pointers are not NULL */
    SHMEMU_CHECK_NOT_NULL(major, 1);
    SHMEMU_CHECK_NOT_NULL(minor, 2);
    SHMEMU_CHECK_NOT_NULL(patch, 3);

    /* Set the vendor's version values */
    *major = SHMEM_VENDOR_MAJOR_VERSION;
    *minor = SHMEM_VENDOR_MINOR_VERSION;
    *patch = SHMEM_VENDOR_PATCH_VERSION;
}

#ifdef ENABLE_PSHMEM
/* Weak symbol for profiling version of the vendor version number function */
#pragma weak shmem_info_get_vendor_version_number = pshmem_info_get_vendor_version_number
#define shmem_info_get_vendor_version_number pshmem_info_get_vendor_version_number
#endif /* ENABLE_PSHMEM */

/*
 * Function to get the SHMEM vendor version as a single integer.
 * 
 * @param version: Pointer to an integer where the vendor version number will be stored.
 */
void
shmem_info_get_vendor_version_number(int *version)
{
    /* Ensure SHMEM is initialized before querying the vendor version number */
    SHMEMU_CHECK_INIT();

    /* Check that the provided version pointer is not NULL */
    SHMEMU_CHECK_NOT_NULL(version, 1);

    /* Store the saved vendor version (major + minor + patch combined) */
    *version = saved_vendor_version;
}

#endif
