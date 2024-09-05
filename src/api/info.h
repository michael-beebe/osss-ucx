/* For license: see LICENSE file at top-level */

#ifndef _SHMEM_OSH_INFO_H
#define _SHMEM_OSH_INFO_H 1

#include <stdio.h>

/*
 * Function: info_output_spec_version
 * ---------------------------------
 * Outputs the OpenSHMEM specification version.
 *
 * strm:   The file stream to output to.
 * prefix: Prefix string for the tag.
 * suffix: Suffix string for the value.
 */
void info_output_spec_version(FILE *strm,
                              const char *prefix, const char *suffix);

/*
 * Function: info_output_package_name
 * ---------------------------------
 * Outputs the OpenSHMEM package name.
 *
 * strm:   The file stream to output to.
 * prefix: Prefix string for the tag.
 * suffix: Suffix string for the value.
 */
void info_output_package_name(FILE *strm,
                              const char *prefix, const char *suffix);

/*
 * Function: info_output_package_contact
 * ------------------------------------
 * Outputs the OpenSHMEM package contact and bug report information.
 *
 * strm:   The file stream to output to.
 * prefix: Prefix string for the tag.
 * suffix: Suffix string for the value.
 */
void info_output_package_contact(FILE *strm,
                                 const char *prefix, const char *suffix);

/*
 * Function: info_output_package_version
 * ------------------------------------
 * Outputs the OpenSHMEM package version.
 *
 * strm:   The file stream to output to.
 * prefix: Prefix string for the tag.
 * suffix: Suffix string for the value.
 * terse:  If non-zero, omits the tag and outputs only the version value.
 */
void info_output_package_version(FILE *strm,
                                 const char *prefix, const char *suffix,
                                 int terse);

/*
 * Function: info_output_build_env
 * ------------------------------
 * Outputs information about the build environment.
 *
 * strm:   The file stream to output to.
 * prefix: Prefix string for the tag.
 * suffix: Suffix string for the value.
 */
void info_output_build_env(FILE *strm,
                           const char *prefix, const char *suffix);

/*
 * Function: info_output_features
 * -----------------------------
 * Outputs a list of enabled or disabled OpenSHMEM features.
 *
 * strm:   The file stream to output to.
 * prefix: Prefix string for the tag.
 * suffix: Suffix string for the value.
 */
void info_output_features(FILE *strm,
                          const char *prefix, const char *suffix);

/*
 * Function: info_output_comms
 * --------------------------
 * Outputs information about the communications environment (e.g., UCX, PMIx).
 *
 * strm:   The file stream to output to.
 * prefix: Prefix string for the tag.
 * suffix: Suffix string for the value.
 */
void info_output_comms(FILE *strm,
                       const char *prefix, const char *suffix);

/*
 * Function: info_output_help
 * -------------------------
 * Outputs help or usage information.
 *
 * strm:   The file stream to output to.
 * prefix: Prefix string for the tag.
 * suffix: Suffix string for the value.
 */
void info_output_help(FILE *strm,
                      const char *prefix, const char *suffix);

#endif /* ! _SHMEM_OSH_INFO_H */
