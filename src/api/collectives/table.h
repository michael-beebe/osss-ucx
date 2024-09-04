/* For license: see LICENSE file at top-level */

#ifndef _TABLE_H
#define _TABLE_H 1

/*
 * The maximum length of a collective operation name.
 * This is used to ensure that the operation names fit within the defined limit.
 */
#define COLL_NAME_MAX 64         /* longer than any name */

/*
 * Most collectives are either a single operation or have
 * 32-bit and 64-bit variants. These function pointer types
 * and structures help to organize and manage the different
 * types of collective operations.
 */

/*
 * Function pointer type for collective operations.
 * This type will be used to point to the functions that implement the collective operations.
 */
typedef void (*coll_fn_t)();

/*
 * Structure to represent a sized collective operation,
 * which includes 32-bit and 64-bit versions.
 */
typedef struct sized_op {
    const char op[COLL_NAME_MAX]; /* Name of the collective operation */
    coll_fn_t f32;                /* Function pointer for the 32-bit version */
    coll_fn_t f64;                /* Function pointer for the 64-bit version */
} sized_op_t;

/*
 * Structure to represent an unsized collective operation,
 * which does not differentiate between 32-bit and 64-bit versions.
 */
typedef struct unsized_op {
    const char op[COLL_NAME_MAX]; /* Name of the collective operation */
    coll_fn_t f;                  /* Function pointer for the operation */
} unsized_op_t;

/*
 * This structure holds the function pointers for all collective operations
 * that are supported by the library. It includes both sized and unsized
 * operations. Each member of this structure corresponds to a specific
 * collective operation (e.g., all-to-all, barrier).
 */

typedef struct coll_ops {
    sized_op_t   alltoall;      /* Sized all-to-all operation */
    sized_op_t   alltoalls;     /* Sized all-to-all strided operation */
    sized_op_t   collect;       /* Sized collect operation */
    sized_op_t   fcollect;      /* Sized familiar collect operation */
    sized_op_t   broadcast;     /* Sized broadcast operation */
    unsized_op_t barrier;       /* Unsized barrier operation */
    unsized_op_t barrier_all;   /* Unsized barrier_all operation */
    unsized_op_t sync;          /* Unsized sync operation */
    unsized_op_t sync_all;      /* Unsized sync_all operation */
} coll_ops_t;

/*
 * The global registry of collective operations.
 * This external declaration allows other parts of the program to access
 * the registered collective operations.
 */
extern coll_ops_t colls;

/*
 * These function declarations are for registering the collective
 * operations. Each function attempts to register a particular
 * collective operation by its name. The functions return zero
 * if the operation is registered successfully, and non-zero otherwise.
 */

int register_barrier_all(const char *name);
int register_sync_all(const char *name);
int register_barrier(const char *name);
int register_sync(const char *name);
int register_broadcast(const char *name);
int register_alltoall(const char *name);
int register_alltoalls(const char *name);
int register_collect(const char *name);
int register_fcollect(const char *name);

#endif /* _TABLE_H */
