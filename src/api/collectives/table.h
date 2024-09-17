#ifndef _TABLE_H
#define _TABLE_H 1

#define COLL_NAME_MAX 64  /* Maximum length of a collective operation name */

/* Function pointer type for collective operations */
typedef void (*coll_fn_t)();

/* Structure to represent a sized collective operation (32-bit and 64-bit variants) */
typedef struct sized_op {
    const char op[COLL_NAME_MAX]; /* Collective operation name */
    coll_fn_t f32;                /* Function pointer for 32-bit version */
    coll_fn_t f64;                /* Function pointer for 64-bit version */
} sized_op_t;

/* Structure to represent an unsized collective operation */
typedef struct unsized_op {
    const char op[COLL_NAME_MAX]; /* Collective operation name */
    coll_fn_t f;                  /* Function pointer */
} unsized_op_t;

/* Structure for a typed collective operation */
typedef struct unsized_typed_op {
    const char op[COLL_NAME_MAX];   /* Collective operation name */
    const char type[COLL_NAME_MAX]; /* Data type name */
    coll_fn_t f;                    /* Function pointer */
} unsized_typed_op_t;

/* Global registry for collective operations (both sized and unsized) */
typedef struct coll_ops {
    unsized_typed_op_t alltoall;   /* Typed all-to-all operation */
    sized_op_t alltoalls;          /* Sized all-to-all strided operation */
    sized_op_t collect;            /* Sized collect operation */
    sized_op_t fcollect;           /* Sized familiar collect operation */
    sized_op_t broadcast;          /* Sized broadcast operation */
    unsized_op_t barrier;          /* Unsized barrier operation */
    unsized_op_t barrier_all;      /* Unsized barrier_all operation */
    unsized_op_t sync;             /* Unsized sync operation */
    unsized_op_t sync_all;         /* Unsized sync_all operation */
} coll_ops_t;

/* External declaration of the global collective operations registry */
extern coll_ops_t colls;

/* Function declarations for registering collective operations */
int register_barrier_all(const char *name);
int register_sync_all(const char *name);
int register_barrier(const char *name);
int register_sync(const char *name);
int register_broadcast(const char *name);
int register_alltoall(const char *name, const char *type);
int register_alltoalls(const char *name);
int register_collect(const char *name);
int register_fcollect(const char *name);

#endif /* _TABLE_H */
