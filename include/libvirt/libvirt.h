/* -*- c -*-
 * libvirt.h: Core interfaces for the libvirt library
 * Summary: core interfaces for the libvirt library
 * Description: Provides the interfaces of the libvirt library to handle
 *              virtualized domains
 *
 * Copyright (C) 2005-2006, 2010-2014 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Author: Daniel Veillard <veillard@redhat.com>
 */

#ifndef __VIR_VIRLIB_H__
#define __VIR_VIRLIB_H__

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VIR_DEPRECATED
  /* The feature is present in gcc-3.1 and newer.  */
# if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
#  define VIR_DEPRECATED __attribute__((__deprecated__))
# else
#  define VIR_DEPRECATED /* nothing */
# endif
#endif /* VIR_DEPRECATED */

#ifdef WIN32
# ifdef LIBVIRT_STATIC
#  define VIR_EXPORT_VAR extern
# else
#  ifdef IN_LIBVIRT
#   define VIR_EXPORT_VAR __declspec(dllexport)
#  else
#   define VIR_EXPORT_VAR __declspec(dllimport) extern
#  endif
# endif
#else
# define VIR_EXPORT_VAR extern
#endif

/* General note - throughout this file, any linear enumeration which
 * might be expanded in the future has an optional *_LAST value that
 * gives the size of the enum at the time of compilation, if the user
 * defines VIR_ENUM_SENTINELS.  Enumerations for bit values do not
 * have a *_LAST value, but additional bits may be defined.  */

/*
 * virFreeCallback:
 * @opaque: opaque user data provided at registration
 *
 * Type for a callback cleanup function to be paired with a callback.  This
 * function will be called as a final chance to clean up the @opaque
 * registered with the primary callback, at the time when the primary
 * callback is deregistered.
 *
 * It is forbidden to call any other libvirt APIs from an
 * implementation of this callback, since it can be invoked
 * from a context which is not re-entrant safe. Failure to
 * abide by this requirement may lead to application deadlocks
 * or crashes.
 */
typedef void (*virFreeCallback)(void *opaque);


/**
 * virConnect:
 *
 * a virConnect is a private structure representing a connection to
 * the Hypervisor.
 */
typedef struct _virConnect virConnect;

/**
 * virConnectPtr:
 *
 * a virConnectPtr is pointer to a virConnect private structure, this is the
 * type used to reference a connection to the Hypervisor in the API.
 */
typedef virConnect *virConnectPtr;

/**
 * virDomain:
 *
 * a virDomain is a private structure representing a domain.
 */
typedef struct _virDomain virDomain;

/**
 * virDomainPtr:
 *
 * a virDomainPtr is pointer to a virDomain private structure, this is the
 * type used to reference a domain in the API.
 */
typedef virDomain *virDomainPtr;

/**
 * virDomainState:
 *
 * A domain may be in different states at a given point in time
 */
typedef enum {
    VIR_DOMAIN_NOSTATE = 0,     /* no state */
    VIR_DOMAIN_RUNNING = 1,     /* the domain is running */
    VIR_DOMAIN_BLOCKED = 2,     /* the domain is blocked on resource */
    VIR_DOMAIN_PAUSED  = 3,     /* the domain is paused by user */
    VIR_DOMAIN_SHUTDOWN= 4,     /* the domain is being shut down */
    VIR_DOMAIN_SHUTOFF = 5,     /* the domain is shut off */
    VIR_DOMAIN_CRASHED = 6,     /* the domain is crashed */
    VIR_DOMAIN_PMSUSPENDED = 7, /* the domain is suspended by guest
                                   power management */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_LAST
    /*
     * NB: this enum value will increase over time as new events are
     * added to the libvirt API. It reflects the last state supported
     * by this version of the libvirt API.
     */
#endif
} virDomainState;

typedef enum {
    VIR_DOMAIN_NOSTATE_UNKNOWN = 0,

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_NOSTATE_LAST
#endif
} virDomainNostateReason;

typedef enum {
    VIR_DOMAIN_RUNNING_UNKNOWN = 0,
    VIR_DOMAIN_RUNNING_BOOTED = 1,          /* normal startup from boot */
    VIR_DOMAIN_RUNNING_MIGRATED = 2,        /* migrated from another host */
    VIR_DOMAIN_RUNNING_RESTORED = 3,        /* restored from a state file */
    VIR_DOMAIN_RUNNING_FROM_SNAPSHOT = 4,   /* restored from snapshot */
    VIR_DOMAIN_RUNNING_UNPAUSED = 5,        /* returned from paused state */
    VIR_DOMAIN_RUNNING_MIGRATION_CANCELED = 6,  /* returned from migration */
    VIR_DOMAIN_RUNNING_SAVE_CANCELED = 7,   /* returned from failed save process */
    VIR_DOMAIN_RUNNING_WAKEUP = 8,          /* returned from pmsuspended due to
                                               wakeup event */
    VIR_DOMAIN_RUNNING_CRASHED = 9,         /* resumed from crashed */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_RUNNING_LAST
#endif
} virDomainRunningReason;

typedef enum {
    VIR_DOMAIN_BLOCKED_UNKNOWN = 0,     /* the reason is unknown */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_BLOCKED_LAST
#endif
} virDomainBlockedReason;

typedef enum {
    VIR_DOMAIN_PAUSED_UNKNOWN = 0,      /* the reason is unknown */
    VIR_DOMAIN_PAUSED_USER = 1,         /* paused on user request */
    VIR_DOMAIN_PAUSED_MIGRATION = 2,    /* paused for offline migration */
    VIR_DOMAIN_PAUSED_SAVE = 3,         /* paused for save */
    VIR_DOMAIN_PAUSED_DUMP = 4,         /* paused for offline core dump */
    VIR_DOMAIN_PAUSED_IOERROR = 5,      /* paused due to a disk I/O error */
    VIR_DOMAIN_PAUSED_WATCHDOG = 6,     /* paused due to a watchdog event */
    VIR_DOMAIN_PAUSED_FROM_SNAPSHOT = 7, /* paused after restoring from snapshot */
    VIR_DOMAIN_PAUSED_SHUTTING_DOWN = 8, /* paused during shutdown process */
    VIR_DOMAIN_PAUSED_SNAPSHOT = 9,      /* paused while creating a snapshot */
    VIR_DOMAIN_PAUSED_CRASHED = 10,     /* paused due to a guest crash */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_PAUSED_LAST
#endif
} virDomainPausedReason;

typedef enum {
    VIR_DOMAIN_SHUTDOWN_UNKNOWN = 0,    /* the reason is unknown */
    VIR_DOMAIN_SHUTDOWN_USER = 1,       /* shutting down on user request */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_SHUTDOWN_LAST
#endif
} virDomainShutdownReason;

typedef enum {
    VIR_DOMAIN_SHUTOFF_UNKNOWN = 0,     /* the reason is unknown */
    VIR_DOMAIN_SHUTOFF_SHUTDOWN = 1,    /* normal shutdown */
    VIR_DOMAIN_SHUTOFF_DESTROYED = 2,   /* forced poweroff */
    VIR_DOMAIN_SHUTOFF_CRASHED = 3,     /* domain crashed */
    VIR_DOMAIN_SHUTOFF_MIGRATED = 4,    /* migrated to another host */
    VIR_DOMAIN_SHUTOFF_SAVED = 5,       /* saved to a file */
    VIR_DOMAIN_SHUTOFF_FAILED = 6,      /* domain failed to start */
    VIR_DOMAIN_SHUTOFF_FROM_SNAPSHOT = 7, /* restored from a snapshot which was
                                           * taken while domain was shutoff */
#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_SHUTOFF_LAST
#endif
} virDomainShutoffReason;

typedef enum {
    VIR_DOMAIN_CRASHED_UNKNOWN = 0,     /* crashed for unknown reason */
    VIR_DOMAIN_CRASHED_PANICKED = 1,    /* domain panicked */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_CRASHED_LAST
#endif
} virDomainCrashedReason;

typedef enum {
    VIR_DOMAIN_PMSUSPENDED_UNKNOWN = 0,

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_PMSUSPENDED_LAST
#endif
} virDomainPMSuspendedReason;

typedef enum {
    VIR_DOMAIN_PMSUSPENDED_DISK_UNKNOWN = 0,

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_PMSUSPENDED_DISK_LAST
#endif
} virDomainPMSuspendedDiskReason;

/**
 * virDomainControlState:
 *
 * Current state of a control interface to the domain.
 */
typedef enum {
    VIR_DOMAIN_CONTROL_OK = 0,       /* operational, ready to accept commands */
    VIR_DOMAIN_CONTROL_JOB = 1,      /* background job is running (can be
                                        monitored by virDomainGetJobInfo); only
                                        limited set of commands may be allowed */
    VIR_DOMAIN_CONTROL_OCCUPIED = 2, /* occupied by a running command */
    VIR_DOMAIN_CONTROL_ERROR = 3,    /* unusable, domain cannot be fully operated */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_CONTROL_LAST
#endif
} virDomainControlState;

/**
 * virDomainControlInfo:
 *
 * Structure filled in by virDomainGetControlInfo and providing details about
 * current state of control interface to a domain.
 */
typedef struct _virDomainControlInfo virDomainControlInfo;
struct _virDomainControlInfo {
    unsigned int state;     /* control state, one of virDomainControlState */
    unsigned int details;   /* state details, currently 0 */
    unsigned long long stateTime; /* for how long (in msec) control interface
                                     has been in current state (except for OK
                                     and ERROR states) */
};

/**
 * virDomainControlInfoPtr:
 *
 * Pointer to virDomainControlInfo structure.
 */
typedef virDomainControlInfo *virDomainControlInfoPtr;

/**
 * virDomainModificationImpact:
 *
 * Several modification APIs take flags to determine whether a change
 * to the domain affects just the running instance, just the
 * persistent definition, or both at the same time.  The counterpart
 * query APIs also take the same flags to determine whether to query
 * the running instance or persistent definition, although both cannot
 * be queried at once.
 *
 * The use of VIR_DOMAIN_AFFECT_CURRENT will resolve to either
 * VIR_DOMAIN_AFFECT_LIVE or VIR_DOMAIN_AFFECT_CONFIG according to
 * current domain state. VIR_DOMAIN_AFFECT_LIVE requires a running
 * domain, and VIR_DOMAIN_AFFECT_CONFIG requires a persistent domain
 * (whether or not it is running).
 *
 * These enums should not conflict with those of virTypedParameterFlags.
 */
typedef enum {
    VIR_DOMAIN_AFFECT_CURRENT = 0,      /* Affect current domain state.  */
    VIR_DOMAIN_AFFECT_LIVE    = 1 << 0, /* Affect running domain state.  */
    VIR_DOMAIN_AFFECT_CONFIG  = 1 << 1, /* Affect persistent domain state.  */
    /* 1 << 2 is reserved for virTypedParameterFlags */
} virDomainModificationImpact;

/**
 * virDomainInfoPtr:
 *
 * a virDomainInfo is a structure filled by virDomainGetInfo() and extracting
 * runtime information for a given active Domain
 */

typedef struct _virDomainInfo virDomainInfo;

struct _virDomainInfo {
    unsigned char state;        /* the running state, one of virDomainState */
    unsigned long maxMem;       /* the maximum memory in KBytes allowed */
    unsigned long memory;       /* the memory in KBytes used by the domain */
    unsigned short nrVirtCpu;   /* the number of virtual CPUs for the domain */
    unsigned long long cpuTime; /* the CPU time used in nanoseconds */
};

/**
 * virDomainInfoPtr:
 *
 * a virDomainInfoPtr is a pointer to a virDomainInfo structure.
 */

typedef virDomainInfo *virDomainInfoPtr;

/**
 * virDomainCreateFlags:
 *
 * Flags OR'ed together to provide specific behaviour when creating a
 * Domain.
 */
typedef enum {
    VIR_DOMAIN_NONE               = 0,      /* Default behavior */
    VIR_DOMAIN_START_PAUSED       = 1 << 0, /* Launch guest in paused state */
    VIR_DOMAIN_START_AUTODESTROY  = 1 << 1, /* Automatically kill guest when virConnectPtr is closed */
    VIR_DOMAIN_START_BYPASS_CACHE = 1 << 2, /* Avoid file system cache pollution */
    VIR_DOMAIN_START_FORCE_BOOT   = 1 << 3, /* Boot, discarding any managed save */
} virDomainCreateFlags;


/**
 * virNodeSuspendTarget:
 *
 * Flags to indicate which system-wide sleep state the host must be
 * transitioned to.
 */
typedef enum {
    VIR_NODE_SUSPEND_TARGET_MEM     = 0,
    VIR_NODE_SUSPEND_TARGET_DISK    = 1,
    VIR_NODE_SUSPEND_TARGET_HYBRID  = 2,

#ifdef VIR_ENUM_SENTINELS
    VIR_NODE_SUSPEND_TARGET_LAST /* This constant is subject to change */
#endif
} virNodeSuspendTarget;

/**
 * virStream:
 *
 * a virStream is a private structure representing a data stream.
 */
typedef struct _virStream virStream;

/**
 * virStreamPtr:
 *
 * a virStreamPtr is pointer to a virStream private structure, this is the
 * type used to reference a data stream in the API.
 */
typedef virStream *virStreamPtr;

/**
 * VIR_SECURITY_LABEL_BUFLEN:
 *
 * Macro providing the maximum length of the virSecurityLabel label string.
 * Note that this value is based on that used by Labeled NFS.
 */
#define VIR_SECURITY_LABEL_BUFLEN (4096 + 1)

/**
 * virSecurityLabel:
 *
 * a virSecurityLabel is a structure filled by virDomainGetSecurityLabel(),
 * providing the security label and associated attributes for the specified
 * domain.
 *
 */
typedef struct _virSecurityLabel {
    char label[VIR_SECURITY_LABEL_BUFLEN];    /* security label string */
    int enforcing;                            /* 1 if security policy is being enforced for domain */
} virSecurityLabel;

/**
 * virSecurityLabelPtr:
 *
 * a virSecurityLabelPtr is a pointer to a virSecurityLabel.
 */
typedef virSecurityLabel *virSecurityLabelPtr;

/**
 * VIR_SECURITY_MODEL_BUFLEN:
 *
 * Macro providing the maximum length of the virSecurityModel model string.
 */
#define VIR_SECURITY_MODEL_BUFLEN (256 + 1)

/**
 * VIR_SECURITY_DOI_BUFLEN:
 *
 * Macro providing the maximum length of the virSecurityModel doi string.
 */
#define VIR_SECURITY_DOI_BUFLEN (256 + 1)

/**
 * virSecurityModel:
 *
 * a virSecurityModel is a structure filled by virNodeGetSecurityModel(),
 * providing the per-hypervisor security model and DOI attributes for the
 * specified domain.
 *
 */
typedef struct _virSecurityModel {
    char model[VIR_SECURITY_MODEL_BUFLEN];      /* security model string */
    char doi[VIR_SECURITY_DOI_BUFLEN];          /* domain of interpetation */
} virSecurityModel;

/**
 * virSecurityModelPtr:
 *
 * a virSecurityModelPtr is a pointer to a virSecurityModel.
 */
typedef virSecurityModel *virSecurityModelPtr;

/* Common data types shared among interfaces with name/type/value lists.  */

/**
 * virTypedParameterType:
 *
 * Express the type of a virTypedParameter
 */
typedef enum {
    VIR_TYPED_PARAM_INT     = 1, /* integer case */
    VIR_TYPED_PARAM_UINT    = 2, /* unsigned integer case */
    VIR_TYPED_PARAM_LLONG   = 3, /* long long case */
    VIR_TYPED_PARAM_ULLONG  = 4, /* unsigned long long case */
    VIR_TYPED_PARAM_DOUBLE  = 5, /* double case */
    VIR_TYPED_PARAM_BOOLEAN = 6, /* boolean(character) case */
    VIR_TYPED_PARAM_STRING  = 7, /* string case */

#ifdef VIR_ENUM_SENTINELS
    VIR_TYPED_PARAM_LAST
#endif
} virTypedParameterType;

/**
 * virTypedParameterFlags:
 *
 * Flags related to libvirt APIs that use virTypedParameter.
 *
 * These enums should not conflict with those of virDomainModificationImpact.
 */
typedef enum {
    /* 1 << 0 is reserved for virDomainModificationImpact */
    /* 1 << 1 is reserved for virDomainModificationImpact */

    /* Older servers lacked the ability to handle string typed
     * parameters.  Attempts to set a string parameter with an older
     * server will fail at the client, but attempts to retrieve
     * parameters must not return strings from a new server to an
     * older client, so this flag exists to identify newer clients to
     * newer servers.  This flag is automatically set when needed, so
     * the user does not have to worry about it; however, manually
     * setting the flag can be used to reject servers that cannot
     * return typed strings, even if no strings would be returned.
     */
    VIR_TYPED_PARAM_STRING_OKAY = 1 << 2,

} virTypedParameterFlags;

/**
 * VIR_TYPED_PARAM_FIELD_LENGTH:
 *
 * Macro providing the field length of virTypedParameter name
 */
#define VIR_TYPED_PARAM_FIELD_LENGTH 80

/**
 * virTypedParameter:
 *
 * A named parameter, including a type and value.
 *
 * The types virSchedParameter, virBlkioParameter, and
 * virMemoryParameter are aliases of this type, for use when
 * targetting libvirt earlier than 0.9.2.
 */
typedef struct _virTypedParameter virTypedParameter;

struct _virTypedParameter {
    char field[VIR_TYPED_PARAM_FIELD_LENGTH];  /* parameter name */
    int type;   /* parameter type, virTypedParameterType */
    union {
        int i;                      /* type is INT */
        unsigned int ui;            /* type is UINT */
        long long int l;            /* type is LLONG */
        unsigned long long int ul;  /* type is ULLONG */
        double d;                   /* type is DOUBLE */
        char b;                     /* type is BOOLEAN */
        char *s;                    /* type is STRING, may not be NULL */
    } value; /* parameter value */
};

/**
 * virTypedParameterPtr:
 *
 * a pointer to a virTypedParameter structure.
 */
typedef virTypedParameter *virTypedParameterPtr;


virTypedParameterPtr
virTypedParamsGet       (virTypedParameterPtr params,
                         int nparams,
                         const char *name);
int
virTypedParamsGetInt    (virTypedParameterPtr params,
                         int nparams,
                         const char *name,
                         int *value);
int
virTypedParamsGetUInt   (virTypedParameterPtr params,
                         int nparams,
                         const char *name,
                         unsigned int *value);
int
virTypedParamsGetLLong  (virTypedParameterPtr params,
                         int nparams,
                         const char *name,
                         long long *value);
int
virTypedParamsGetULLong (virTypedParameterPtr params,
                         int nparams,
                         const char *name,
                         unsigned long long *value);
int
virTypedParamsGetDouble (virTypedParameterPtr params,
                         int nparams,
                         const char *name,
                         double *value);
int
virTypedParamsGetBoolean(virTypedParameterPtr params,
                         int nparams,
                         const char *name,
                         int *value);
int
virTypedParamsGetString (virTypedParameterPtr params,
                         int nparams,
                         const char *name,
                         const char **value);
int
virTypedParamsAddInt    (virTypedParameterPtr *params,
                         int *nparams,
                         int *maxparams,
                         const char *name,
                         int value);
int
virTypedParamsAddUInt   (virTypedParameterPtr *params,
                         int *nparams,
                         int *maxparams,
                         const char *name,
                         unsigned int value);
int
virTypedParamsAddLLong  (virTypedParameterPtr *params,
                         int *nparams,
                         int *maxparams,
                         const char *name,
                         long long value);
int
virTypedParamsAddULLong (virTypedParameterPtr *params,
                         int *nparams,
                         int *maxparams,
                         const char *name,
                         unsigned long long value);
int
virTypedParamsAddDouble (virTypedParameterPtr *params,
                         int *nparams,
                         int *maxparams,
                         const char *name,
                         double value);
int
virTypedParamsAddBoolean(virTypedParameterPtr *params,
                         int *nparams,
                         int *maxparams,
                         const char *name,
                         int value);
int
virTypedParamsAddString (virTypedParameterPtr *params,
                         int *nparams,
                         int *maxparams,
                         const char *name,
                         const char *value);
int
virTypedParamsAddFromString(virTypedParameterPtr *params,
                         int *nparams,
                         int *maxparams,
                         const char *name,
                         int type,
                         const char *value);
void
virTypedParamsClear     (virTypedParameterPtr params,
                         int nparams);
void
virTypedParamsFree      (virTypedParameterPtr params,
                         int nparams);

/* data types related to virNodePtr */

/**
 * virNodeInfoPtr:
 *
 * a virNodeInfo is a structure filled by virNodeGetInfo() and providing
 * the information for the Node.
 */

typedef struct _virNodeInfo virNodeInfo;

struct _virNodeInfo {
    char model[32];       /* string indicating the CPU model */
    unsigned long memory; /* memory size in kilobytes */
    unsigned int cpus;    /* the number of active CPUs */
    unsigned int mhz;     /* expected CPU frequency */
    unsigned int nodes;   /* the number of NUMA cell, 1 for unusual NUMA
                             topologies or uniform memory access; check
                             capabilities XML for the actual NUMA topology */
    unsigned int sockets; /* number of CPU sockets per node if nodes > 1,
                             1 in case of unusual NUMA topology */
    unsigned int cores;   /* number of cores per socket, total number of
                             processors in case of unusual NUMA topology*/
    unsigned int threads; /* number of threads per core, 1 in case of
                             unusual numa topology */
};

/**
 * VIR_NODE_CPU_STATS_FIELD_LENGTH:
 *
 * Macro providing the field length of virNodeCPUStats
 */
#define VIR_NODE_CPU_STATS_FIELD_LENGTH 80

/**
 * VIR_NODE_CPU_STATS_ALL_CPUS:
 *
 * Value for specifying request for the total CPU time/utilization
 */
typedef enum {
    VIR_NODE_CPU_STATS_ALL_CPUS = -1,
} virNodeGetCPUStatsAllCPUs;

/**
 * VIR_NODE_CPU_STATS_KERNEL:
 *
 * Macro for the cumulative CPU time which was spent by the kernel,
 * since the node booting up (in nanoseconds).
 */
#define VIR_NODE_CPU_STATS_KERNEL "kernel"

/**
 * VIR_NODE_CPU_STATS_USER:
 *
 * The cumulative CPU time which was spent by user processes,
 * since the node booting up (in nanoseconds).
 */
#define VIR_NODE_CPU_STATS_USER "user"

/**
 * VIR_NODE_CPU_STATS_IDLE:
 *
 * The cumulative idle CPU time,
 * since the node booting up (in nanoseconds).
 */
#define VIR_NODE_CPU_STATS_IDLE "idle"

/**
 * VIR_NODE_CPU_STATS_IOWAIT:
 *
 * The cumulative I/O wait CPU time,
 * since the node booting up (in nanoseconds).
 */
#define VIR_NODE_CPU_STATS_IOWAIT "iowait"

/**
 * VIR_NODE_CPU_STATS_INTR:
 *
 * The cumulative interrupt CPU time,
 * since the node booting up (in nanoseconds).
 */
#define VIR_NODE_CPU_STATS_INTR "intr"

/**
 * VIR_NODE_CPU_STATS_UTILIZATION:
 *
 * The CPU utilization of a node.
 * The usage value is in percent and 100% represents all CPUs of
 * the node.
 */
#define VIR_NODE_CPU_STATS_UTILIZATION "utilization"

/**
 * virNodeCPUStats:
 *
 * a virNodeCPUStats is a structure filled by virNodeGetCPUStats()
 * providing information about the CPU stats of the node.
 */
typedef struct _virNodeCPUStats virNodeCPUStats;

struct _virNodeCPUStats {
    char field[VIR_NODE_CPU_STATS_FIELD_LENGTH];
    unsigned long long value;
};

/**
 * VIR_NODE_MEMORY_STATS_FIELD_LENGTH:
 *
 * Macro providing the field length of virNodeMemoryStats
 */
#define VIR_NODE_MEMORY_STATS_FIELD_LENGTH 80

/**
 * VIR_NODE_MEMORY_STATS_ALL_CELLS:
 *
 * Value for specifying request for the total memory of all cells.
 */
typedef enum {
    VIR_NODE_MEMORY_STATS_ALL_CELLS = -1,
} virNodeGetMemoryStatsAllCells;

/**
 * VIR_NODE_MEMORY_STATS_TOTAL:
 *
 * Macro for the total memory of specified cell:
 * it represents the maximum memory.
 */
#define VIR_NODE_MEMORY_STATS_TOTAL "total"

/**
 * VIR_NODE_MEMORY_STATS_FREE:
 *
 * Macro for the free memory of specified cell:
 * On Linux, it includes buffer and cached memory, in case of
 * VIR_NODE_MEMORY_STATS_ALL_CELLS.
 */
#define VIR_NODE_MEMORY_STATS_FREE "free"

/**
 * VIR_NODE_MEMORY_STATS_BUFFERS:
 *
 * Macro for the buffer memory: On Linux, it is only returned in case of
 * VIR_NODE_MEMORY_STATS_ALL_CELLS.
 */
#define VIR_NODE_MEMORY_STATS_BUFFERS "buffers"

/**
 * VIR_NODE_MEMORY_STATS_CACHED:
 *
 * Macro for the cached memory: On Linux, it is only returned in case of
 * VIR_NODE_MEMORY_STATS_ALL_CELLS.
 */
#define VIR_NODE_MEMORY_STATS_CACHED "cached"

/**
 * virNodeMemoryStats:
 *
 * a virNodeMemoryStats is a structure filled by virNodeGetMemoryStats()
 * providing information about the memory of the node.
 */
typedef struct _virNodeMemoryStats virNodeMemoryStats;

struct _virNodeMemoryStats {
    char field[VIR_NODE_MEMORY_STATS_FIELD_LENGTH];
    unsigned long long value;
};

/*
 * VIR_NODE_MEMORY_SHARED_PAGES_TO_SCAN:
 *
 * Macro for typed parameter that represents how many present pages
 * to scan before the shared memory service goes to sleep.
 */
# define VIR_NODE_MEMORY_SHARED_PAGES_TO_SCAN      "shm_pages_to_scan"

/*
 * VIR_NODE_MEMORY_SHARED_SLEEP_MILLISECS:
 *
 * Macro for typed parameter that represents how many milliseconds
 * the shared memory service should sleep before next scan.
 */
# define VIR_NODE_MEMORY_SHARED_SLEEP_MILLISECS    "shm_sleep_millisecs"

/*
 * VIR_NODE_MEMORY_SHARED_PAGES_SHARED:
 *
 * Macro for typed parameter that represents how many the shared
 * memory pages are being used.
 */
# define VIR_NODE_MEMORY_SHARED_PAGES_SHARED       "shm_pages_shared"

/*
 * VIR_NODE_MEMORY_SHARED_PAGES_SHARING:
 *
 * Macro for typed parameter that represents how many sites are
 * sharing the pages i.e. how much saved.
 */
# define VIR_NODE_MEMORY_SHARED_PAGES_SHARING      "shm_pages_sharing"

/* VIR_NODE_MEMORY_SHARED_PAGES_UNSHARED:
 *
 * Macro for typed parameter that represents how many pages unique
 * but repeatedly checked for merging.
 */
# define VIR_NODE_MEMORY_SHARED_PAGES_UNSHARED     "shm_pages_unshared"

/* VIR_NODE_MEMORY_SHARED_PAGES_VOLATILE:
 *
 * Macro for typed parameter that represents how many pages changing
 * too fast to be placed in a tree.
 */
# define VIR_NODE_MEMORY_SHARED_PAGES_VOLATILE     "shm_pages_volatile"

/* VIR_NODE_MEMORY_SHARED_FULL_SCAN:
 *
 * Macro for typed parameter that represents how many times all
 * mergeable areas have been scanned.
 */
# define VIR_NODE_MEMORY_SHARED_FULL_SCANS         "shm_full_scans"

/* VIR_NODE_MEMORY_SHARED_MERGE_ACROSS_NODES:
 *
 * Macro for typed parameter that represents whether pages from
 * different NUMA nodes can be merged. The parameter has type int,
 * when its value is 0, only pages which physically reside in the
 * memory area of same NUMA node are merged; When its value is 1,
 * pages from all nodes can be merged. Other values are reserved
 * for future use.
 */
# define VIR_NODE_MEMORY_SHARED_MERGE_ACROSS_NODES "shm_merge_across_nodes"


int virNodeGetMemoryParameters(virConnectPtr conn,
                               virTypedParameterPtr params,
                               int *nparams,
                               unsigned int flags);

int virNodeSetMemoryParameters(virConnectPtr conn,
                               virTypedParameterPtr params,
                               int nparams,
                               unsigned int flags);

/*
 *  node CPU map
 */
int virNodeGetCPUMap(virConnectPtr conn,
                     unsigned char **cpumap,
                     unsigned int *online,
                     unsigned int flags);


/* Management of scheduler parameters */

/**
 * VIR_DOMAIN_SCHEDULER_CPU_SHARES:
 *
 * Macro represents proportional weight of the scheduler used on the
 * host cpu, when using the posix scheduler, as a ullong.
 */
#define VIR_DOMAIN_SCHEDULER_CPU_SHARES "cpu_shares"

/**
 * VIR_DOMAIN_SCHEDULER_VCPU_PERIOD:
 *
 * Macro represents the enforcement period for a quota, in microseconds,
 * for vcpus only, when using the posix scheduler, as a ullong.
 */
#define VIR_DOMAIN_SCHEDULER_VCPU_PERIOD "vcpu_period"

/**
 * VIR_DOMAIN_SCHEDULER_VCPU_QUOTA:
 *
 * Macro represents the maximum bandwidth to be used within a period for
 * vcpus only, when using the posix scheduler, as an llong.
 */
#define VIR_DOMAIN_SCHEDULER_VCPU_QUOTA "vcpu_quota"

/**
 * VIR_DOMAIN_SCHEDULER_EMULATOR_PERIOD:
 *
 * Macro represents the enforcement period for a quota in microseconds,
 * when using the posix scheduler, for all emulator activity not tied to
 * vcpus, as a ullong.
 */
#define VIR_DOMAIN_SCHEDULER_EMULATOR_PERIOD "emulator_period"

/**
 * VIR_DOMAIN_SCHEDULER_EMULATOR_QUOTA:
 *
 * Macro represents the maximum bandwidth to be used within a period for
 * all emulator activity not tied to vcpus, when using the posix scheduler,
 * as an llong.
 */
#define VIR_DOMAIN_SCHEDULER_EMULATOR_QUOTA "emulator_quota"

/**
 * VIR_DOMAIN_SCHEDULER_WEIGHT:
 *
 * Macro represents the relative weight,  when using the credit
 * scheduler, as a uint.
 */
#define VIR_DOMAIN_SCHEDULER_WEIGHT "weight"

/**
 * VIR_DOMAIN_SCHEDULER_CAP:
 *
 * Macro represents the maximum scheduler cap, when using the credit
 * scheduler, as a uint.
 */
#define VIR_DOMAIN_SCHEDULER_CAP "cap"

/**
 * VIR_DOMAIN_SCHEDULER_RESERVATION:
 *
 * Macro represents the scheduler reservation value, when using the
 * allocation scheduler, as an llong.
 */
#define VIR_DOMAIN_SCHEDULER_RESERVATION "reservation"

/**
 * VIR_DOMAIN_SCHEDULER_LIMIT:
 *
 * Macro represents the scheduler limit value, when using the
 * allocation scheduler, as an llong.
 */
#define VIR_DOMAIN_SCHEDULER_LIMIT "limit"

/**
 * VIR_DOMAIN_SCHEDULER_SHARES:
 *
 * Macro represents the scheduler shares value, when using the
 * allocation scheduler, as an int.
 */
#define VIR_DOMAIN_SCHEDULER_SHARES "shares"

/*
 * Fetch scheduler parameters, caller allocates 'params' field of size 'nparams'
 */
int     virDomainGetSchedulerParameters (virDomainPtr domain,
                                         virTypedParameterPtr params,
                                         int *nparams);
int     virDomainGetSchedulerParametersFlags (virDomainPtr domain,
                                              virTypedParameterPtr params,
                                              int *nparams,
                                              unsigned int flags);

/*
 * Change scheduler parameters
 */
int     virDomainSetSchedulerParameters (virDomainPtr domain,
                                         virTypedParameterPtr params,
                                         int nparams);
int     virDomainSetSchedulerParametersFlags (virDomainPtr domain,
                                              virTypedParameterPtr params,
                                              int nparams,
                                              unsigned int flags);

/**
 * virDomainBlockStats:
 *
 * Block device stats for virDomainBlockStats.
 *
 * Hypervisors may return a field set to ((long long)-1) which indicates
 * that the hypervisor does not support that statistic.
 *
 * NB. Here 'long long' means 64 bit integer.
 */
typedef struct _virDomainBlockStats virDomainBlockStatsStruct;

struct _virDomainBlockStats {
    long long rd_req; /* number of read requests */
    long long rd_bytes; /* number of read bytes */
    long long wr_req; /* number of write requests */
    long long wr_bytes; /* number of written bytes */
    long long errs;   /* In Xen this returns the mysterious 'oo_req'. */
};

/**
 * virDomainBlockStatsPtr:
 *
 * A pointer to a virDomainBlockStats structure
 */
typedef virDomainBlockStatsStruct *virDomainBlockStatsPtr;


/**
 * VIR_DOMAIN_BLOCK_STATS_FIELD_LENGTH:
 *
 * Macro providing the field length of parameter names when using
 * virDomainBlockStatsFlags().
 */
#define VIR_DOMAIN_BLOCK_STATS_FIELD_LENGTH VIR_TYPED_PARAM_FIELD_LENGTH

/**
 * VIR_DOMAIN_BLOCK_STATS_READ_BYTES:
 *
 * Macro represents the total number of read bytes of the
 * block device, as an llong.
 */
#define VIR_DOMAIN_BLOCK_STATS_READ_BYTES "rd_bytes"

/**
 * VIR_DOMAIN_BLOCK_STATS_READ_REQ:
 *
 * Macro represents the total read requests of the
 * block device, as an llong.
 */
#define VIR_DOMAIN_BLOCK_STATS_READ_REQ "rd_operations"

/**
 * VIR_DOMAIN_BLOCK_STATS_READ_TOTAL_TIMES:
 *
 * Macro represents the total time spend on cache reads in
 * nano-seconds of the block device, as an llong.
 */
#define VIR_DOMAIN_BLOCK_STATS_READ_TOTAL_TIMES "rd_total_times"

/**
 * VIR_DOMAIN_BLOCK_STATS_WRITE_BYTES:
 *
 * Macro represents the total number of write bytes of the
 * block device, as an llong.
 */
#define VIR_DOMAIN_BLOCK_STATS_WRITE_BYTES "wr_bytes"

/**
 * VIR_DOMAIN_BLOCK_STATS_WRITE_REQ:
 *
 * Macro represents the total write requests of the
 * block device, as an llong.
 */
#define VIR_DOMAIN_BLOCK_STATS_WRITE_REQ "wr_operations"

/**
 * VIR_DOMAIN_BLOCK_STATS_WRITE_TOTAL_TIMES:
 *
 * Macro represents the total time spend on cache writes in
 * nano-seconds of the block device, as an llong.
 */
#define VIR_DOMAIN_BLOCK_STATS_WRITE_TOTAL_TIMES "wr_total_times"

/**
 * VIR_DOMAIN_BLOCK_STATS_FLUSH_REQ:
 *
 * Macro represents the total flush requests of the
 * block device, as an llong.
 */
#define VIR_DOMAIN_BLOCK_STATS_FLUSH_REQ "flush_operations"

/**
 * VIR_DOMAIN_BLOCK_STATS_FLUSH_TOTAL_TIMES:
 *
 * Macro represents the total time spend on cache flushing in
 * nano-seconds of the block device, as an llong.
 */
#define VIR_DOMAIN_BLOCK_STATS_FLUSH_TOTAL_TIMES "flush_total_times"

/**
 * VIR_DOMAIN_BLOCK_STATS_ERRS:
 *
 * In Xen this returns the mysterious 'oo_req', as an llong.
 */
#define VIR_DOMAIN_BLOCK_STATS_ERRS "errs"

/**
 * virDomainInterfaceStats:
 *
 * Network interface stats for virDomainInterfaceStats.
 *
 * Hypervisors may return a field set to ((long long)-1) which indicates
 * that the hypervisor does not support that statistic.
 *
 * NB. Here 'long long' means 64 bit integer.
 */
typedef struct _virDomainInterfaceStats virDomainInterfaceStatsStruct;

struct _virDomainInterfaceStats {
    long long rx_bytes;
    long long rx_packets;
    long long rx_errs;
    long long rx_drop;
    long long tx_bytes;
    long long tx_packets;
    long long tx_errs;
    long long tx_drop;
};

/**
 * virDomainInterfaceStatsPtr:
 *
 * A pointer to a virDomainInterfaceStats structure
 */
typedef virDomainInterfaceStatsStruct *virDomainInterfaceStatsPtr;

/**
 * Memory Statistics Tags:
 */
typedef enum {
    /* The total amount of data read from swap space (in kB). */
    VIR_DOMAIN_MEMORY_STAT_SWAP_IN         = 0,
    /* The total amount of memory written out to swap space (in kB). */
    VIR_DOMAIN_MEMORY_STAT_SWAP_OUT        = 1,

    /*
     * Page faults occur when a process makes a valid access to virtual memory
     * that is not available.  When servicing the page fault, if disk IO is
     * required, it is considered a major fault.  If not, it is a minor fault.
     * These are expressed as the number of faults that have occurred.
     */
    VIR_DOMAIN_MEMORY_STAT_MAJOR_FAULT     = 2,
    VIR_DOMAIN_MEMORY_STAT_MINOR_FAULT     = 3,

    /*
     * The amount of memory left completely unused by the system.  Memory that
     * is available but used for reclaimable caches should NOT be reported as
     * free.  This value is expressed in kB.
     */
    VIR_DOMAIN_MEMORY_STAT_UNUSED          = 4,

    /*
     * The total amount of usable memory as seen by the domain.  This value
     * may be less than the amount of memory assigned to the domain if a
     * balloon driver is in use or if the guest OS does not initialize all
     * assigned pages.  This value is expressed in kB.
     */
    VIR_DOMAIN_MEMORY_STAT_AVAILABLE       = 5,

    /* Current balloon value (in KB). */
    VIR_DOMAIN_MEMORY_STAT_ACTUAL_BALLOON  = 6,

    /* Resident Set Size of the process running the domain. This value
     * is in kB */
    VIR_DOMAIN_MEMORY_STAT_RSS             = 7,

    /*
     * The number of statistics supported by this version of the interface.
     * To add new statistics, add them to the enum and increase this value.
     */
    VIR_DOMAIN_MEMORY_STAT_NR              = 8,

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_MEMORY_STAT_LAST = VIR_DOMAIN_MEMORY_STAT_NR
#endif
} virDomainMemoryStatTags;

typedef struct _virDomainMemoryStat virDomainMemoryStatStruct;

struct _virDomainMemoryStat {
    int tag;
    unsigned long long val;
};

typedef virDomainMemoryStatStruct *virDomainMemoryStatPtr;


/* Domain core dump flags. */
typedef enum {
    VIR_DUMP_CRASH        = (1 << 0), /* crash after dump */
    VIR_DUMP_LIVE         = (1 << 1), /* live dump */
    VIR_DUMP_BYPASS_CACHE = (1 << 2), /* avoid file system cache pollution */
    VIR_DUMP_RESET        = (1 << 3), /* reset domain after dump finishes */
    VIR_DUMP_MEMORY_ONLY  = (1 << 4), /* use dump-guest-memory */
} virDomainCoreDumpFlags;

/**
 * virDomainCoreDumpFormat:
 *
 * Values for specifying different formats of domain core dumps.
 */
typedef enum {
    VIR_DOMAIN_CORE_DUMP_FORMAT_RAW,          /* dump guest memory in raw format */
    VIR_DOMAIN_CORE_DUMP_FORMAT_KDUMP_ZLIB,   /* kdump-compressed format, with
                                               * zlib compression */
    VIR_DOMAIN_CORE_DUMP_FORMAT_KDUMP_LZO,    /* kdump-compressed format, with
                                               * lzo compression */
    VIR_DOMAIN_CORE_DUMP_FORMAT_KDUMP_SNAPPY, /* kdump-compressed format, with
                                               * snappy compression */
#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_CORE_DUMP_FORMAT_LAST
    /*
     * NB: this enum value will increase over time as new events are
     * added to the libvirt API. It reflects the last state supported
     * by this version of the libvirt API.
     */
#endif
} virDomainCoreDumpFormat;

/* Domain migration flags. */
typedef enum {
    VIR_MIGRATE_LIVE              = (1 << 0), /* live migration */
    VIR_MIGRATE_PEER2PEER         = (1 << 1), /* direct source -> dest host control channel */
    /* Note the less-common spelling that we're stuck with:
       VIR_MIGRATE_TUNNELLED should be VIR_MIGRATE_TUNNELED */
    VIR_MIGRATE_TUNNELLED         = (1 << 2), /* tunnel migration data over libvirtd connection */
    VIR_MIGRATE_PERSIST_DEST      = (1 << 3), /* persist the VM on the destination */
    VIR_MIGRATE_UNDEFINE_SOURCE   = (1 << 4), /* undefine the VM on the source */
    VIR_MIGRATE_PAUSED            = (1 << 5), /* pause on remote side */
    VIR_MIGRATE_NON_SHARED_DISK   = (1 << 6), /* migration with non-shared storage with full disk copy */
    VIR_MIGRATE_NON_SHARED_INC    = (1 << 7), /* migration with non-shared storage with incremental copy */
                                              /* (same base image shared between source and destination) */
    VIR_MIGRATE_CHANGE_PROTECTION = (1 << 8), /* protect for changing domain configuration through the
                                               * whole migration process; this will be used automatically
                                               * when supported */
    VIR_MIGRATE_UNSAFE            = (1 << 9), /* force migration even if it is considered unsafe */
    VIR_MIGRATE_OFFLINE           = (1 << 10), /* offline migrate */
    VIR_MIGRATE_COMPRESSED        = (1 << 11), /* compress data during migration */
    VIR_MIGRATE_ABORT_ON_ERROR    = (1 << 12), /* abort migration on I/O errors happened during migration */
    VIR_MIGRATE_AUTO_CONVERGE     = (1 << 13), /* force convergence */
} virDomainMigrateFlags;


/**
 * VIR_MIGRATE_PARAM_URI:
 *
 * virDomainMigrate* params field: URI to use for initiating domain migration
 * as VIR_TYPED_PARAM_STRING. It takes a hypervisor specific format. The
 * uri_transports element of the hypervisor capabilities XML includes details
 * of the supported URI schemes. When omitted libvirt will auto-generate
 * suitable default URI. It is typically only necessary to specify this URI if
 * the destination host has multiple interfaces and a specific interface is
 * required to transmit migration data.
 *
 * This filed may not be used when VIR_MIGRATE_TUNNELLED flag is set.
 */
#define VIR_MIGRATE_PARAM_URI               "migrate_uri"

/**
 * VIR_MIGRATE_PARAM_DEST_NAME:
 *
 * virDomainMigrate* params field: the name to be used for the domain on the
 * destination host as VIR_TYPED_PARAM_STRING. Omitting this parameter keeps
 * the domain name the same. This field is only allowed to be used with
 * hypervisors that support domain renaming during migration.
 */
#define VIR_MIGRATE_PARAM_DEST_NAME         "destination_name"

/**
 * VIR_MIGRATE_PARAM_DEST_XML:
 *
 * virDomainMigrate* params field: the new configuration to be used for the
 * domain on the destination host as VIR_TYPED_PARAM_STRING. The configuration
 * must include an identical set of virtual devices, to ensure a stable guest
 * ABI across migration. Only parameters related to host side configuration
 * can be changed in the XML. Hypervisors which support this field will forbid
 * migration if the provided XML would cause a change in the guest ABI. This
 * field cannot be used to rename the domain during migration (use
 * VIR_MIGRATE_PARAM_DEST_NAME field for that purpose). Domain name in the
 * destination XML must match the original domain name.
 *
 * Omitting this parameter keeps the original domain configuration. Using this
 * field with hypervisors that do not support changing domain configuration
 * during migration will result in a failure.
 */
#define VIR_MIGRATE_PARAM_DEST_XML          "destination_xml"

/**
 * VIR_MIGRATE_PARAM_BANDWIDTH:
 *
 * virDomainMigrate* params field: the maximum bandwidth (in MiB/s) that will
 * be used for migration as VIR_TYPED_PARAM_ULLONG. If set to 0 or omitted,
 * libvirt will choose a suitable default. Some hypervisors do not support this
 * feature and will return an error if this field is used and is not 0.
 */
#define VIR_MIGRATE_PARAM_BANDWIDTH         "bandwidth"

/**
 * VIR_MIGRATE_PARAM_GRAPHICS_URI:
 *
 * virDomainMigrate* params field: URI to use for migrating client's connection
 * to domain's graphical console as VIR_TYPED_PARAM_STRING. If specified, the
 * client will be asked to automatically reconnect using these parameters
 * instead of the automatically computed ones. This can be useful if, e.g., the
 * client does not have a direct access to the network virtualization hosts are
 * connected to and needs to connect through a proxy. The URI is formed as
 * follows:
 *
 *      protocol://hostname[:port]/[?parameters]
 *
 * where protocol is either "spice" or "vnc" and parameters is a list of
 * protocol specific parameters separated by '&'. Currently recognized
 * parameters are "tlsPort" and "tlsSubject". For example,
 *
 *      spice://target.host.com:1234/?tlsPort=4567
 */
#define VIR_MIGRATE_PARAM_GRAPHICS_URI      "graphics_uri"

/**
 * VIR_MIGRATE_PARAM_LISTEN_ADDRESS:
 *
 * virDomainMigrate* params field: The listen address that hypervisor on the
 * destination side should bind to for incoming migration. Both IPv4 and IPv6
 * addresses are accepted as well as hostnames (the resolving is done on
 * destination). Some hypervisors do not support this feature and will return
 * an error if this field is used.
 */
#define VIR_MIGRATE_PARAM_LISTEN_ADDRESS    "listen_address"

/* Domain migration. */
virDomainPtr virDomainMigrate (virDomainPtr domain, virConnectPtr dconn,
                               unsigned long flags, const char *dname,
                               const char *uri, unsigned long bandwidth);
virDomainPtr virDomainMigrate2(virDomainPtr domain, virConnectPtr dconn,
                               const char *dxml,
                               unsigned long flags, const char *dname,
                               const char *uri, unsigned long bandwidth);
virDomainPtr virDomainMigrate3(virDomainPtr domain,
                               virConnectPtr dconn,
                               virTypedParameterPtr params,
                               unsigned int nparams,
                               unsigned int flags);

int virDomainMigrateToURI (virDomainPtr domain, const char *duri,
                           unsigned long flags, const char *dname,
                           unsigned long bandwidth);

int virDomainMigrateToURI2(virDomainPtr domain,
                           const char *dconnuri,
                           const char *miguri,
                           const char *dxml,
                           unsigned long flags,
                           const char *dname,
                           unsigned long bandwidth);
int virDomainMigrateToURI3(virDomainPtr domain,
                           const char *dconnuri,
                           virTypedParameterPtr params,
                           unsigned int nparams,
                           unsigned int flags);

int virDomainMigrateSetMaxDowntime (virDomainPtr domain,
                                    unsigned long long downtime,
                                    unsigned int flags);

int virDomainMigrateGetCompressionCache(virDomainPtr domain,
                                        unsigned long long *cacheSize,
                                        unsigned int flags);
int virDomainMigrateSetCompressionCache(virDomainPtr domain,
                                        unsigned long long cacheSize,
                                        unsigned int flags);

int virDomainMigrateSetMaxSpeed(virDomainPtr domain,
                                unsigned long bandwidth,
                                unsigned int flags);

int virDomainMigrateGetMaxSpeed(virDomainPtr domain,
                                unsigned long *bandwidth,
                                unsigned int flags);

/**
 * VIR_NODEINFO_MAXCPUS:
 * @nodeinfo: virNodeInfo instance
 *
 * This macro is to calculate the total number of CPUs supported
 * but not necessary active in the host.
 */


#define VIR_NODEINFO_MAXCPUS(nodeinfo) ((nodeinfo).nodes*(nodeinfo).sockets*(nodeinfo).cores*(nodeinfo).threads)

/**
 * virNodeInfoPtr:
 *
 * a virNodeInfoPtr is a pointer to a virNodeInfo structure.
 */

typedef virNodeInfo *virNodeInfoPtr;

/**
 * virNodeCPUStatsPtr:
 *
 * a virNodeCPUStatsPtr is a pointer to a virNodeCPUStats structure.
 */

typedef virNodeCPUStats *virNodeCPUStatsPtr;

/**
 * virNodeMemoryStatsPtr:
 *
 * a virNodeMemoryStatsPtr is a pointer to a virNodeMemoryStats structure.
 */

typedef virNodeMemoryStats *virNodeMemoryStatsPtr;

/**
 * virConnectFlags
 *
 * Flags when opening a connection to a hypervisor
 */
typedef enum {
    VIR_CONNECT_RO         = (1 << 0),  /* A readonly connection */
    VIR_CONNECT_NO_ALIASES = (1 << 1),  /* Don't try to resolve URI aliases */
} virConnectFlags;


typedef enum {
    VIR_CRED_USERNAME = 1,     /* Identity to act as */
    VIR_CRED_AUTHNAME = 2,     /* Identify to authorize as */
    VIR_CRED_LANGUAGE = 3,     /* RFC 1766 languages, comma separated */
    VIR_CRED_CNONCE = 4,       /* client supplies a nonce */
    VIR_CRED_PASSPHRASE = 5,   /* Passphrase secret */
    VIR_CRED_ECHOPROMPT = 6,   /* Challenge response */
    VIR_CRED_NOECHOPROMPT = 7, /* Challenge response */
    VIR_CRED_REALM = 8,        /* Authentication realm */
    VIR_CRED_EXTERNAL = 9,     /* Externally managed credential */

#ifdef VIR_ENUM_SENTINELS
    VIR_CRED_LAST              /* More may be added - expect the unexpected */
#endif
} virConnectCredentialType;

struct _virConnectCredential {
    int type; /* One of virConnectCredentialType constants */
    const char *prompt; /* Prompt to show to user */
    const char *challenge; /* Additional challenge to show */
    const char *defresult; /* Optional default result */
    char *result; /* Result to be filled with user response (or defresult) */
    unsigned int resultlen; /* Length of the result */
};

typedef struct _virConnectCredential virConnectCredential;
typedef virConnectCredential *virConnectCredentialPtr;


/**
 * virConnectAuthCallbackPtr:
 * @cred: list of virConnectCredential object to fetch from user
 * @ncred: size of cred list
 * @cbdata: opaque data passed to virConnectOpenAuth
 *
 * When authentication requires one or more interactions, this callback
 * is invoked. For each interaction supplied, data must be gathered
 * from the user and filled in to the 'result' and 'resultlen' fields.
 * If an interaction cannot be filled, fill in NULL and 0.
 *
 * Returns 0 if all interactions were filled, or -1 upon error
 */
typedef int (*virConnectAuthCallbackPtr)(virConnectCredentialPtr cred,
                                         unsigned int ncred,
                                         void *cbdata);

struct _virConnectAuth {
    int *credtype; /* List of supported virConnectCredentialType values */
    unsigned int ncredtype;

    virConnectAuthCallbackPtr cb; /* Callback used to collect credentials */
    void *cbdata;
};


typedef struct _virConnectAuth virConnectAuth;
typedef virConnectAuth *virConnectAuthPtr;

VIR_EXPORT_VAR virConnectAuthPtr virConnectAuthPtrDefault;

/**
 * VIR_UUID_BUFLEN:
 *
 * This macro provides the length of the buffer required
 * for virDomainGetUUID()
 */

#define VIR_UUID_BUFLEN (16)

/**
 * VIR_UUID_STRING_BUFLEN:
 *
 * This macro provides the length of the buffer required
 * for virDomainGetUUIDString()
 */

#define VIR_UUID_STRING_BUFLEN (36+1)

/* library versioning */

/**
 * LIBVIR_VERSION_NUMBER:
 *
 * Macro providing the version of the library as
 * version * 1,000,000 + minor * 1000 + micro
 */

#define LIBVIR_VERSION_NUMBER 1002004

/**
 * LIBVIR_CHECK_VERSION:
 * @major: major component of the version number
 * @minor: minor component of the version number
 * @micro: micro component of the version number
 *
 * Macro for developers to easily check what version of the library
 * their code is compiling against.
 * e.g.
 *   #if LIBVIR_CHECK_VERSION(1,1,3)
 *     // some code that only works in 1.1.3 and newer
 *   #endif
 */
#define LIBVIR_CHECK_VERSION(major, minor, micro) \
    ((major) * 1000000 + (minor) * 1000 + (micro) <= LIBVIR_VERSION_NUMBER)

int                     virGetVersion           (unsigned long *libVer,
                                                 const char *type,
                                                 unsigned long *typeVer);

/*
 * Connection and disconnections to the Hypervisor
 */
int                     virInitialize           (void);

virConnectPtr           virConnectOpen          (const char *name);
virConnectPtr           virConnectOpenReadOnly  (const char *name);
virConnectPtr           virConnectOpenAuth      (const char *name,
                                                 virConnectAuthPtr auth,
                                                 unsigned int flags);
int                     virConnectRef           (virConnectPtr conn);
int                     virConnectClose         (virConnectPtr conn);
const char *            virConnectGetType       (virConnectPtr conn);
int                     virConnectGetVersion    (virConnectPtr conn,
                                                 unsigned long *hvVer);
int                     virConnectGetLibVersion (virConnectPtr conn,
                                                 unsigned long *libVer);
char *                  virConnectGetHostname   (virConnectPtr conn);
char *                  virConnectGetURI        (virConnectPtr conn);
char *                  virConnectGetSysinfo    (virConnectPtr conn,
                                                 unsigned int flags);

int virConnectSetKeepAlive(virConnectPtr conn,
                           int interval,
                           unsigned int count);

typedef enum {
    VIR_CONNECT_CLOSE_REASON_ERROR     = 0, /* Misc I/O error */
    VIR_CONNECT_CLOSE_REASON_EOF       = 1, /* End-of-file from server */
    VIR_CONNECT_CLOSE_REASON_KEEPALIVE = 2, /* Keepalive timer triggered */
    VIR_CONNECT_CLOSE_REASON_CLIENT    = 3, /* Client requested it */

# ifdef VIR_ENUM_SENTINELS
    VIR_CONNECT_CLOSE_REASON_LAST
# endif
} virConnectCloseReason;

/**
 * virConnectCloseFunc:
 * @conn: virConnect connection
 * @reason: reason why the connection was closed (one of virConnectCloseReason)
 * @opaque: opaque user data
 *
 * A callback function to be registered, and called when the connection
 * is closed.
 */
typedef void (*virConnectCloseFunc)(virConnectPtr conn,
                                    int reason,
                                    void *opaque);

int virConnectRegisterCloseCallback(virConnectPtr conn,
                                    virConnectCloseFunc cb,
                                    void *opaque,
                                    virFreeCallback freecb);
int virConnectUnregisterCloseCallback(virConnectPtr conn,
                                      virConnectCloseFunc cb);

/*
 * Capabilities of the connection / driver.
 */

int                     virConnectGetMaxVcpus   (virConnectPtr conn,
                                                 const char *type);
int                     virNodeGetInfo          (virConnectPtr conn,
                                                 virNodeInfoPtr info);
char *                  virConnectGetCapabilities (virConnectPtr conn);

int                     virNodeGetCPUStats (virConnectPtr conn,
                                            int cpuNum,
                                            virNodeCPUStatsPtr params,
                                            int *nparams,
                                            unsigned int flags);

int                     virNodeGetMemoryStats (virConnectPtr conn,
                                               int cellNum,
                                               virNodeMemoryStatsPtr params,
                                               int *nparams,
                                               unsigned int flags);

unsigned long long      virNodeGetFreeMemory    (virConnectPtr conn);

int                     virNodeGetSecurityModel (virConnectPtr conn,
                                                 virSecurityModelPtr secmodel);

int                     virNodeSuspendForDuration (virConnectPtr conn,
                                                   unsigned int target,
                                                   unsigned long long duration,
                                                   unsigned int flags);

/*
 * Gather list of running domains
 */
int                     virConnectListDomains   (virConnectPtr conn,
                                                 int *ids,
                                                 int maxids);

/*
 * Number of domains
 */
int                     virConnectNumOfDomains  (virConnectPtr conn);


/*
 * Get connection from domain.
 */
virConnectPtr           virDomainGetConnect     (virDomainPtr domain);

/*
 * Domain creation and destruction
 */

virDomainPtr            virDomainCreateXML      (virConnectPtr conn,
                                                 const char *xmlDesc,
                                                 unsigned int flags);
virDomainPtr            virDomainCreateXMLWithFiles(virConnectPtr conn,
                                                    const char *xmlDesc,
                                                    unsigned int nfiles,
                                                    int *files,
                                                    unsigned int flags);
virDomainPtr            virDomainLookupByName   (virConnectPtr conn,
                                                 const char *name);
virDomainPtr            virDomainLookupByID     (virConnectPtr conn,
                                                 int id);
virDomainPtr            virDomainLookupByUUID   (virConnectPtr conn,
                                                 const unsigned char *uuid);
virDomainPtr            virDomainLookupByUUIDString     (virConnectPtr conn,
                                                         const char *uuid);

typedef enum {
    VIR_DOMAIN_SHUTDOWN_DEFAULT        = 0,        /* hypervisor choice */
    VIR_DOMAIN_SHUTDOWN_ACPI_POWER_BTN = (1 << 0), /* Send ACPI event */
    VIR_DOMAIN_SHUTDOWN_GUEST_AGENT    = (1 << 1), /* Use guest agent */
    VIR_DOMAIN_SHUTDOWN_INITCTL        = (1 << 2), /* Use initctl */
    VIR_DOMAIN_SHUTDOWN_SIGNAL         = (1 << 3), /* Send a signal */
} virDomainShutdownFlagValues;

int                     virDomainShutdown       (virDomainPtr domain);
int                     virDomainShutdownFlags  (virDomainPtr domain,
                                                 unsigned int flags);

typedef enum {
    VIR_DOMAIN_REBOOT_DEFAULT        = 0,        /* hypervisor choice */
    VIR_DOMAIN_REBOOT_ACPI_POWER_BTN = (1 << 0), /* Send ACPI event */
    VIR_DOMAIN_REBOOT_GUEST_AGENT    = (1 << 1), /* Use guest agent */
    VIR_DOMAIN_REBOOT_INITCTL        = (1 << 2), /* Use initctl */
    VIR_DOMAIN_REBOOT_SIGNAL         = (1 << 3), /* Send a signal */
} virDomainRebootFlagValues;

int                     virDomainReboot         (virDomainPtr domain,
                                                 unsigned int flags);
int                     virDomainReset          (virDomainPtr domain,
                                                 unsigned int flags);

int                     virDomainDestroy        (virDomainPtr domain);

/**
 * virDomainDestroyFlagsValues:
 *
 * Flags used to provide specific behaviour to the
 * virDomainDestroyFlags() function
 */
typedef enum {
    VIR_DOMAIN_DESTROY_DEFAULT   = 0,      /* Default behavior - could lead to data loss!! */
    VIR_DOMAIN_DESTROY_GRACEFUL  = 1 << 0, /* only SIGTERM, no SIGKILL */
} virDomainDestroyFlagsValues;

int                     virDomainDestroyFlags   (virDomainPtr domain,
                                                 unsigned int flags);
int                     virDomainRef            (virDomainPtr domain);
int                     virDomainFree           (virDomainPtr domain);

/*
 * Domain suspend/resume
 */
int                     virDomainSuspend        (virDomainPtr domain);
int                     virDomainResume         (virDomainPtr domain);
int                     virDomainPMSuspendForDuration (virDomainPtr domain,
                                                       unsigned int target,
                                                       unsigned long long duration,
                                                       unsigned int flags);
int                     virDomainPMWakeup       (virDomainPtr domain,
                                                 unsigned int flags);
/*
 * Domain save/restore
 */

/**
 * virDomainSaveRestoreFlags:
 * Flags for use in virDomainSaveFlags(), virDomainManagedSave(),
 * virDomainRestoreFlags(), and virDomainSaveImageDefineXML().  Not all
 * flags apply to all these functions.
 */
typedef enum {
    VIR_DOMAIN_SAVE_BYPASS_CACHE = 1 << 0, /* Avoid file system cache pollution */
    VIR_DOMAIN_SAVE_RUNNING      = 1 << 1, /* Favor running over paused */
    VIR_DOMAIN_SAVE_PAUSED       = 1 << 2, /* Favor paused over running */
} virDomainSaveRestoreFlags;

int                     virDomainSave           (virDomainPtr domain,
                                                 const char *to);
int                     virDomainSaveFlags      (virDomainPtr domain,
                                                 const char *to,
                                                 const char *dxml,
                                                 unsigned int flags);
int                     virDomainRestore        (virConnectPtr conn,
                                                 const char *from);
int                     virDomainRestoreFlags   (virConnectPtr conn,
                                                 const char *from,
                                                 const char *dxml,
                                                 unsigned int flags);

char *          virDomainSaveImageGetXMLDesc    (virConnectPtr conn,
                                                 const char *file,
                                                 unsigned int flags);
int             virDomainSaveImageDefineXML     (virConnectPtr conn,
                                                 const char *file,
                                                 const char *dxml,
                                                 unsigned int flags);

/*
 * Managed domain save
 */
int                    virDomainManagedSave     (virDomainPtr dom,
                                                 unsigned int flags);
int                    virDomainHasManagedSaveImage(virDomainPtr dom,
                                                    unsigned int flags);
int                    virDomainManagedSaveRemove(virDomainPtr dom,
                                                  unsigned int flags);

/*
 * Domain core dump
 */
int                     virDomainCoreDump       (virDomainPtr domain,
                                                 const char *to,
                                                 unsigned int flags);

/*
 * Domain core dump with format specified
 */
int                 virDomainCoreDumpWithFormat (virDomainPtr domain,
                                                 const char *to,
                                                 unsigned int dumpformat,
                                                 unsigned int flags);

/*
 * Screenshot of current domain console
 */
char *                  virDomainScreenshot     (virDomainPtr domain,
                                                 virStreamPtr stream,
                                                 unsigned int screen,
                                                 unsigned int flags);

/*
 * Domain runtime information, and collecting CPU statistics
 */

int                     virDomainGetInfo        (virDomainPtr domain,
                                                 virDomainInfoPtr info);
int                     virDomainGetState       (virDomainPtr domain,
                                                 int *state,
                                                 int *reason,
                                                 unsigned int flags);

/**
 * VIR_DOMAIN_CPU_STATS_CPUTIME:
 * cpu usage (sum of both vcpu and hypervisor usage) in nanoseconds,
 * as a ullong
 */
#define VIR_DOMAIN_CPU_STATS_CPUTIME "cpu_time"

/**
 * VIR_DOMAIN_CPU_STATS_USERTIME:
 * cpu time charged to user instructions in nanoseconds, as a ullong
 */
#define VIR_DOMAIN_CPU_STATS_USERTIME "user_time"

/**
 * VIR_DOMAIN_CPU_STATS_SYSTEMTIME:
 * cpu time charged to system instructions in nanoseconds, as a ullong
 */
#define VIR_DOMAIN_CPU_STATS_SYSTEMTIME "system_time"

/**
 * VIR_DOMAIN_CPU_STATS_VCPUTIME:
 * vcpu usage in nanoseconds (cpu_time excluding hypervisor time),
 * as a ullong
 */
#define VIR_DOMAIN_CPU_STATS_VCPUTIME "vcpu_time"

int virDomainGetCPUStats(virDomainPtr domain,
                         virTypedParameterPtr params,
                         unsigned int nparams,
                         int start_cpu,
                         unsigned int ncpus,
                         unsigned int flags);

int                     virDomainGetControlInfo (virDomainPtr domain,
                                                 virDomainControlInfoPtr info,
                                                 unsigned int flags);

/*
 * Return scheduler type in effect 'sedf', 'credit', 'linux'
 */
char *                  virDomainGetSchedulerType(virDomainPtr domain,
                                                  int *nparams);


/* Manage blkio parameters.  */

/**
 * VIR_DOMAIN_BLKIO_WEIGHT:
 *
 * Macro for the Blkio tunable weight: it represents the io weight
 * the guest can use, as a uint.
 */

#define VIR_DOMAIN_BLKIO_WEIGHT "weight"

/**
 * VIR_DOMAIN_BLKIO_DEVICE_WEIGHT:
 *
 * Macro for the blkio tunable weight_device: it represents the
 * per-device weight, as a string.  The string is parsed as a
 * series of /path/to/device,weight elements, separated by ','.
 */

#define VIR_DOMAIN_BLKIO_DEVICE_WEIGHT "device_weight"

/**
 * VIR_DOMAIN_BLKIO_DEVICE_READ_IOPS:
 *
 * Macro for the blkio tunable throttle.read_iops_device: it represents
 * the number of reading the block device per second, as a string. The
 * string is parsed as a series of /path/to/device, read_iops elements,
 * separated by ','.
 */

#define VIR_DOMAIN_BLKIO_DEVICE_READ_IOPS "device_read_iops_sec"


/**
 * VIR_DOMAIN_BLKIO_DEVICE_WRITE_IOPS:
 *
 * Macro for the blkio tunable throttle.write_iops_device: it represents
 * the number of writing the block device per second, as a string. The
 * string is parsed as a series of /path/to/device, write_iops elements,
 * separated by ','.
 */
#define VIR_DOMAIN_BLKIO_DEVICE_WRITE_IOPS "device_write_iops_sec"


/**
 * VIR_DOMAIN_BLKIO_DEVICE_READ_BPS:
 *
 * Macro for the blkio tunable throttle.read_iops_device: it represents
 * the bytes of reading the block device per second, as a string. The
 * string is parsed as a series of /path/to/device, read_bps elements,
 * separated by ','.
 */
#define VIR_DOMAIN_BLKIO_DEVICE_READ_BPS "device_read_bytes_sec"


/**
 * VIR_DOMAIN_BLKIO_DEVICE_WRITE_BPS:
 *
 * Macro for the blkio tunable throttle.read_iops_device: it represents
 * the number of reading the block device per second, as a string. The
 * string is parsed as a series of /path/to/device, write_bps elements,
 * separated by ','.
 */
#define VIR_DOMAIN_BLKIO_DEVICE_WRITE_BPS "device_write_bytes_sec"


/* Set Blkio tunables for the domain*/
int     virDomainSetBlkioParameters(virDomainPtr domain,
                                    virTypedParameterPtr params,
                                    int nparams, unsigned int flags);
int     virDomainGetBlkioParameters(virDomainPtr domain,
                                    virTypedParameterPtr params,
                                    int *nparams, unsigned int flags);

/* Manage memory parameters.  */

/**
 * VIR_DOMAIN_MEMORY_PARAM_UNLIMITED:
 *
 * Macro providing the virMemoryParameter value that indicates "unlimited"
 */

#define VIR_DOMAIN_MEMORY_PARAM_UNLIMITED 9007199254740991LL /* = INT64_MAX >> 10 */

/**
 * VIR_DOMAIN_MEMORY_HARD_LIMIT:
 *
 * Macro for the memory tunable hard_limit: it represents the maximum memory
 * the guest can use, as a ullong.
 */

#define VIR_DOMAIN_MEMORY_HARD_LIMIT "hard_limit"

/**
 * VIR_DOMAIN_MEMORY_SOFT_LIMIT:
 *
 * Macro for the memory tunable soft_limit: it represents the memory upper
 * limit enforced during memory contention, as a ullong.
 */

#define VIR_DOMAIN_MEMORY_SOFT_LIMIT "soft_limit"

/**
 * VIR_DOMAIN_MEMORY_MIN_GUARANTEE:
 *
 * Macro for the memory tunable min_guarantee: it represents the minimum
 * memory guaranteed to be reserved for the guest, as a ullong.
 */

#define VIR_DOMAIN_MEMORY_MIN_GUARANTEE "min_guarantee"

/**
 * VIR_DOMAIN_MEMORY_SWAP_HARD_LIMIT:
 *
 * Macro for the swap tunable swap_hard_limit: it represents the maximum swap
 * plus memory the guest can use, as a ullong. This limit has to be more than
 * VIR_DOMAIN_MEMORY_HARD_LIMIT.
 */

#define VIR_DOMAIN_MEMORY_SWAP_HARD_LIMIT "swap_hard_limit"

/* Set memory tunables for the domain*/
int     virDomainSetMemoryParameters(virDomainPtr domain,
                                     virTypedParameterPtr params,
                                     int nparams, unsigned int flags);
int     virDomainGetMemoryParameters(virDomainPtr domain,
                                     virTypedParameterPtr params,
                                     int *nparams, unsigned int flags);

/* Memory size modification flags. */
typedef enum {
    /* See virDomainModificationImpact for these flags.  */
    VIR_DOMAIN_MEM_CURRENT = VIR_DOMAIN_AFFECT_CURRENT,
    VIR_DOMAIN_MEM_LIVE    = VIR_DOMAIN_AFFECT_LIVE,
    VIR_DOMAIN_MEM_CONFIG  = VIR_DOMAIN_AFFECT_CONFIG,

    /* Additionally, these flags may be bitwise-OR'd in.  */
    VIR_DOMAIN_MEM_MAXIMUM = (1 << 2), /* affect Max rather than current */
} virDomainMemoryModFlags;


/* Manage numa parameters */

/**
 * virDomainNumatuneMemMode:
 * Representation of the various modes in the <numatune> element of
 * a domain.
 */
typedef enum {
    VIR_DOMAIN_NUMATUNE_MEM_STRICT      = 0,
    VIR_DOMAIN_NUMATUNE_MEM_PREFERRED   = 1,
    VIR_DOMAIN_NUMATUNE_MEM_INTERLEAVE  = 2,

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_NUMATUNE_MEM_LAST /* This constant is subject to change */
#endif
} virDomainNumatuneMemMode;

/**
 * VIR_DOMAIN_NUMA_NODESET:
 *
 * Macro for typed parameter name that lists the numa nodeset of a
 * domain, as a string.
 */
#define VIR_DOMAIN_NUMA_NODESET "numa_nodeset"

/**
 * VIR_DOMAIN_NUMA_MODE:
 *
 * Macro for typed parameter name that lists the numa mode of a domain,
 * as an int containing a virDomainNumatuneMemMode value.
 */
#define VIR_DOMAIN_NUMA_MODE "numa_mode"

int     virDomainSetNumaParameters(virDomainPtr domain,
                                   virTypedParameterPtr params,
                                   int nparams, unsigned int flags);
int     virDomainGetNumaParameters(virDomainPtr domain,
                                   virTypedParameterPtr params,
                                   int *nparams, unsigned int flags);

/*
 * Dynamic control of domains
 */
const char *            virDomainGetName        (virDomainPtr domain);
unsigned int            virDomainGetID          (virDomainPtr domain);
int                     virDomainGetUUID        (virDomainPtr domain,
                                                 unsigned char *uuid);
int                     virDomainGetUUIDString  (virDomainPtr domain,
                                                 char *buf);
char *                  virDomainGetOSType      (virDomainPtr domain);
unsigned long           virDomainGetMaxMemory   (virDomainPtr domain);
int                     virDomainSetMaxMemory   (virDomainPtr domain,
                                                 unsigned long memory);
int                     virDomainSetMemory      (virDomainPtr domain,
                                                 unsigned long memory);
int                     virDomainSetMemoryFlags (virDomainPtr domain,
                                                 unsigned long memory,
                                                 unsigned int flags);
int                     virDomainSetMemoryStatsPeriod (virDomainPtr domain,
                                                       int period,
                                                       unsigned int flags);
int                     virDomainGetMaxVcpus    (virDomainPtr domain);
int                     virDomainGetSecurityLabel (virDomainPtr domain,
                                                   virSecurityLabelPtr seclabel);
char *                  virDomainGetHostname    (virDomainPtr domain,
                                                 unsigned int flags);
int                     virDomainGetSecurityLabelList (virDomainPtr domain,
                                                       virSecurityLabelPtr* seclabels);

typedef enum {
    VIR_DOMAIN_METADATA_DESCRIPTION = 0, /* Operate on <description> */
    VIR_DOMAIN_METADATA_TITLE       = 1, /* Operate on <title> */
    VIR_DOMAIN_METADATA_ELEMENT     = 2, /* Operate on <metadata> */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_METADATA_LAST
#endif
} virDomainMetadataType;

int
virDomainSetMetadata(virDomainPtr domain,
                     int type,
                     const char *metadata,
                     const char *key,
                     const char *uri,
                     unsigned int flags);

char *
virDomainGetMetadata(virDomainPtr domain,
                     int type,
                     const char *uri,
                     unsigned int flags);

/*
 * XML domain description
 */
/**
 * virDomainXMLFlags:
 *
 * Flags available for virDomainGetXMLDesc
 */

typedef enum {
    VIR_DOMAIN_XML_SECURE       = (1 << 0), /* dump security sensitive information too */
    VIR_DOMAIN_XML_INACTIVE     = (1 << 1), /* dump inactive domain information */
    VIR_DOMAIN_XML_UPDATE_CPU   = (1 << 2), /* update guest CPU requirements according to host CPU */
    VIR_DOMAIN_XML_MIGRATABLE   = (1 << 3), /* dump XML suitable for migration */
} virDomainXMLFlags;

char *                  virDomainGetXMLDesc     (virDomainPtr domain,
                                                 unsigned int flags);


char *                  virConnectDomainXMLFromNative(virConnectPtr conn,
                                                      const char *nativeFormat,
                                                      const char *nativeConfig,
                                                      unsigned int flags);
char *                  virConnectDomainXMLToNative(virConnectPtr conn,
                                                    const char *nativeFormat,
                                                    const char *domainXml,
                                                    unsigned int flags);

int                     virDomainBlockStats     (virDomainPtr dom,
                                                 const char *disk,
                                                 virDomainBlockStatsPtr stats,
                                                 size_t size);
int                     virDomainBlockStatsFlags (virDomainPtr dom,
                                                  const char *disk,
                                                  virTypedParameterPtr params,
                                                  int *nparams,
                                                  unsigned int flags);
int                     virDomainInterfaceStats (virDomainPtr dom,
                                                 const char *path,
                                                 virDomainInterfaceStatsPtr stats,
                                                 size_t size);

/* Management of interface parameters */

/**
 * VIR_DOMAIN_BANDWIDTH_IN_AVERAGE:
 *
 * Macro represents the inbound average of NIC bandwidth, as a uint.
 */
#define VIR_DOMAIN_BANDWIDTH_IN_AVERAGE "inbound.average"

/**
 * VIR_DOMAIN_BANDWIDTH_IN_PEAK:
 *
 * Macro represents the inbound peak of NIC bandwidth, as a uint.
 */
#define VIR_DOMAIN_BANDWIDTH_IN_PEAK "inbound.peak"

/**
 * VIR_DOMAIN_BANDWIDTH_IN_BURST:
 *
 * Macro represents the inbound burst of NIC bandwidth, as a uint.
 */
#define VIR_DOMAIN_BANDWIDTH_IN_BURST "inbound.burst"

/**
 * VIR_DOMAIN_BANDWIDTH_OUT_AVERAGE:
 *
 * Macro represents the outbound average of NIC bandwidth, as a uint.
 */
#define VIR_DOMAIN_BANDWIDTH_OUT_AVERAGE "outbound.average"

/**
 * VIR_DOMAIN_BANDWIDTH_OUT_PEAK:
 *
 * Macro represents the outbound peak of NIC bandwidth, as a uint.
 */
#define VIR_DOMAIN_BANDWIDTH_OUT_PEAK "outbound.peak"

/**
 * VIR_DOMAIN_BANDWIDTH_OUT_BURST:
 *
 * Macro represents the outbound burst of NIC bandwidth, as a uint.
 */
#define VIR_DOMAIN_BANDWIDTH_OUT_BURST "outbound.burst"

int                     virDomainSetInterfaceParameters (virDomainPtr dom,
                                                         const char *device,
                                                         virTypedParameterPtr params,
                                                         int nparams, unsigned int flags);
int                     virDomainGetInterfaceParameters (virDomainPtr dom,
                                                         const char *device,
                                                         virTypedParameterPtr params,
                                                         int *nparams, unsigned int flags);

/* Management of domain block devices */

int                     virDomainBlockPeek (virDomainPtr dom,
                                            const char *disk,
                                            unsigned long long offset,
                                            size_t size,
                                            void *buffer,
                                            unsigned int flags);

/**
 * virDomainBlockResizeFlags:
 *
 * Flags available for virDomainBlockResize().
 */
typedef enum {
    VIR_DOMAIN_BLOCK_RESIZE_BYTES = 1 << 0, /* size in bytes instead of KiB */
} virDomainBlockResizeFlags;

int                     virDomainBlockResize (virDomainPtr dom,
                                              const char *disk,
                                              unsigned long long size,
                                              unsigned int flags);

/** virDomainBlockInfo:
 *
 * This struct provides information about the size of a block device
 * backing store
 *
 * Examples:
 *
 *  - Fully allocated raw file in filesystem:
 *       * capacity, allocation, physical: All the same
 *
 *  - Sparse raw file in filesystem:
 *       * capacity: logical size of the file
 *       * allocation, physical: number of blocks allocated to file
 *
 *  - qcow2 file in filesystem
 *       * capacity: logical size from qcow2 header
 *       * allocation, physical: logical size of the file /
 *                               highest qcow extent (identical)
 *
 *  - qcow2 file in a block device
 *       * capacity: logical size from qcow2 header
 *       * allocation: highest qcow extent written for an active domain
 *       * physical: size of the block device container
 */
typedef struct _virDomainBlockInfo virDomainBlockInfo;
typedef virDomainBlockInfo *virDomainBlockInfoPtr;
struct _virDomainBlockInfo {
    unsigned long long capacity;   /* logical size in bytes of the block device backing image */
    unsigned long long allocation; /* highest allocated extent in bytes of the block device backing image */
    unsigned long long physical;   /* physical size in bytes of the container of the backing image */
};

int                     virDomainGetBlockInfo(virDomainPtr dom,
                                              const char *disk,
                                              virDomainBlockInfoPtr info,
                                              unsigned int flags);

/* Management of domain memory */

int                     virDomainMemoryStats (virDomainPtr dom,
                                              virDomainMemoryStatPtr stats,
                                              unsigned int nr_stats,
                                              unsigned int flags);

/* Memory peeking flags. */

typedef enum {
    VIR_MEMORY_VIRTUAL            = 1 << 0, /* addresses are virtual addresses */
    VIR_MEMORY_PHYSICAL           = 1 << 1, /* addresses are physical addresses */
} virDomainMemoryFlags;

int                     virDomainMemoryPeek (virDomainPtr dom,
                                             unsigned long long start,
                                             size_t size,
                                             void *buffer,
                                             unsigned int flags);

/*
 * defined but not running domains
 */
virDomainPtr            virDomainDefineXML      (virConnectPtr conn,
                                                 const char *xml);
int                     virDomainUndefine       (virDomainPtr domain);

typedef enum {
    VIR_DOMAIN_UNDEFINE_MANAGED_SAVE       = (1 << 0), /* Also remove any
                                                          managed save */
    VIR_DOMAIN_UNDEFINE_SNAPSHOTS_METADATA = (1 << 1), /* If last use of domain,
                                                          then also remove any
                                                          snapshot metadata */

    /* Future undefine control flags should come here. */
} virDomainUndefineFlagsValues;


int                     virDomainUndefineFlags   (virDomainPtr domain,
                                                  unsigned int flags);
int                     virConnectNumOfDefinedDomains  (virConnectPtr conn);
int                     virConnectListDefinedDomains (virConnectPtr conn,
                                                      char **const names,
                                                      int maxnames);
/**
 * virConnectListAllDomainsFlags:
 *
 * Flags used to tune which domains are listed by virConnectListAllDomains().
 * Note that these flags come in groups; if all bits from a group are 0,
 * then that group is not used to filter results.
 */
typedef enum {
    VIR_CONNECT_LIST_DOMAINS_ACTIVE         = 1 << 0,
    VIR_CONNECT_LIST_DOMAINS_INACTIVE       = 1 << 1,

    VIR_CONNECT_LIST_DOMAINS_PERSISTENT     = 1 << 2,
    VIR_CONNECT_LIST_DOMAINS_TRANSIENT      = 1 << 3,

    VIR_CONNECT_LIST_DOMAINS_RUNNING        = 1 << 4,
    VIR_CONNECT_LIST_DOMAINS_PAUSED         = 1 << 5,
    VIR_CONNECT_LIST_DOMAINS_SHUTOFF        = 1 << 6,
    VIR_CONNECT_LIST_DOMAINS_OTHER          = 1 << 7,

    VIR_CONNECT_LIST_DOMAINS_MANAGEDSAVE    = 1 << 8,
    VIR_CONNECT_LIST_DOMAINS_NO_MANAGEDSAVE = 1 << 9,

    VIR_CONNECT_LIST_DOMAINS_AUTOSTART      = 1 << 10,
    VIR_CONNECT_LIST_DOMAINS_NO_AUTOSTART   = 1 << 11,

    VIR_CONNECT_LIST_DOMAINS_HAS_SNAPSHOT   = 1 << 12,
    VIR_CONNECT_LIST_DOMAINS_NO_SNAPSHOT    = 1 << 13,
} virConnectListAllDomainsFlags;

int                     virConnectListAllDomains (virConnectPtr conn,
                                                  virDomainPtr **domains,
                                                  unsigned int flags);
int                     virDomainCreate         (virDomainPtr domain);
int                     virDomainCreateWithFlags (virDomainPtr domain,
                                                  unsigned int flags);

int                     virDomainCreateWithFiles (virDomainPtr domain,
                                                  unsigned int nfiles,
                                                  int *files,
                                                  unsigned int flags);

int                     virDomainGetAutostart   (virDomainPtr domain,
                                                 int *autostart);
int                     virDomainSetAutostart   (virDomainPtr domain,
                                                 int autostart);

/**
 * virVcpuInfo: structure for information about a virtual CPU in a domain.
 */

typedef enum {
    VIR_VCPU_OFFLINE    = 0,    /* the virtual CPU is offline */
    VIR_VCPU_RUNNING    = 1,    /* the virtual CPU is running */
    VIR_VCPU_BLOCKED    = 2,    /* the virtual CPU is blocked on resource */

#ifdef VIR_ENUM_SENTINELS
    VIR_VCPU_LAST
#endif
} virVcpuState;

typedef struct _virVcpuInfo virVcpuInfo;
struct _virVcpuInfo {
    unsigned int number;        /* virtual CPU number */
    int state;                  /* value from virVcpuState */
    unsigned long long cpuTime; /* CPU time used, in nanoseconds */
    int cpu;                    /* real CPU number, or -1 if offline */
};
typedef virVcpuInfo *virVcpuInfoPtr;

/* Flags for controlling virtual CPU hot-plugging.  */
typedef enum {
    /* See virDomainModificationImpact for these flags.  */
    VIR_DOMAIN_VCPU_CURRENT = VIR_DOMAIN_AFFECT_CURRENT,
    VIR_DOMAIN_VCPU_LIVE    = VIR_DOMAIN_AFFECT_LIVE,
    VIR_DOMAIN_VCPU_CONFIG  = VIR_DOMAIN_AFFECT_CONFIG,

    /* Additionally, these flags may be bitwise-OR'd in.  */
    VIR_DOMAIN_VCPU_MAXIMUM = (1 << 2), /* Max rather than current count */
    VIR_DOMAIN_VCPU_GUEST   = (1 << 3), /* Modify state of the cpu in the guest */
} virDomainVcpuFlags;

int                     virDomainSetVcpus       (virDomainPtr domain,
                                                 unsigned int nvcpus);
int                     virDomainSetVcpusFlags  (virDomainPtr domain,
                                                 unsigned int nvcpus,
                                                 unsigned int flags);
int                     virDomainGetVcpusFlags  (virDomainPtr domain,
                                                 unsigned int flags);

int                     virDomainPinVcpu        (virDomainPtr domain,
                                                 unsigned int vcpu,
                                                 unsigned char *cpumap,
                                                 int maplen);
int                     virDomainPinVcpuFlags   (virDomainPtr domain,
                                                 unsigned int vcpu,
                                                 unsigned char *cpumap,
                                                 int maplen,
                                                 unsigned int flags);

int                     virDomainGetVcpuPinInfo (virDomainPtr domain,
                                                 int ncpumaps,
                                                 unsigned char *cpumaps,
                                                 int maplen,
                                                 unsigned int flags);

int                     virDomainPinEmulator   (virDomainPtr domain,
                                                unsigned char *cpumap,
                                                int maplen,
                                                unsigned int flags);

int                     virDomainGetEmulatorPinInfo (virDomainPtr domain,
                                                     unsigned char *cpumaps,
                                                     int maplen,
                                                     unsigned int flags);

/**
 * VIR_USE_CPU:
 * @cpumap: pointer to a bit map of real CPUs (in 8-bit bytes) (IN/OUT)
 * @cpu: the physical CPU number
 *
 * This macro is to be used in conjunction with virDomainPinVcpu() API.
 * It sets the bit (CPU usable) of the related cpu in cpumap.
 */

#define VIR_USE_CPU(cpumap, cpu) ((cpumap)[(cpu) / 8] |= (1 << ((cpu) % 8)))

/**
 * VIR_UNUSE_CPU:
 * @cpumap: pointer to a bit map of real CPUs (in 8-bit bytes) (IN/OUT)
 * @cpu: the physical CPU number
 *
 * This macro is to be used in conjunction with virDomainPinVcpu() API.
 * It resets the bit (CPU not usable) of the related cpu in cpumap.
 */

#define VIR_UNUSE_CPU(cpumap, cpu) ((cpumap)[(cpu) / 8] &= ~(1 << ((cpu) % 8)))

/**
 * VIR_CPU_USED:
 * @cpumap: pointer to a bit map of real CPUs (in 8-bit bytes) (IN)
 * @cpu: the physical CPU number
 *
 * This macro can be used in conjunction with virNodeGetCPUMap() API.
 * It returns non-zero if the bit of the related CPU is set.
 */

#define VIR_CPU_USED(cpumap, cpu) ((cpumap)[(cpu) / 8] & (1 << ((cpu) % 8)))

/**
 * VIR_CPU_MAPLEN:
 * @cpu: number of physical CPUs
 *
 * This macro is to be used in conjunction with virDomainPinVcpu() API.
 * It returns the length (in bytes) required to store the complete
 * CPU map between a single virtual & all physical CPUs of a domain.
 */

#define VIR_CPU_MAPLEN(cpu) (((cpu) + 7) / 8)


int                     virDomainGetVcpus       (virDomainPtr domain,
                                                 virVcpuInfoPtr info,
                                                 int maxinfo,
                                                 unsigned char *cpumaps,
                                                 int maplen);

/**
 * VIR_CPU_USABLE:
 * @cpumaps: pointer to an array of cpumap (in 8-bit bytes) (IN)
 * @maplen: the length (in bytes) of one cpumap
 * @vcpu: the virtual CPU number
 * @cpu: the physical CPU number
 *
 * This macro is to be used in conjunction with virDomainGetVcpus() API.
 * VIR_CPU_USABLE macro returns a non-zero value (true) if the cpu
 * is usable by the vcpu, and 0 otherwise.
 */

#define VIR_CPU_USABLE(cpumaps, maplen, vcpu, cpu) \
    VIR_CPU_USED(VIR_GET_CPUMAP(cpumaps, maplen, vcpu), cpu)

/**
 * VIR_COPY_CPUMAP:
 * @cpumaps: pointer to an array of cpumap (in 8-bit bytes) (IN)
 * @maplen: the length (in bytes) of one cpumap
 * @vcpu: the virtual CPU number
 * @cpumap: pointer to a cpumap (in 8-bit bytes) (OUT)
 *      This cpumap must be previously allocated by the caller
 *      (ie: malloc(maplen))
 *
 * This macro is to be used in conjunction with virDomainGetVcpus() and
 * virDomainPinVcpu() APIs. VIR_COPY_CPUMAP macro extracts the cpumap of
 * the specified vcpu from cpumaps array and copies it into cpumap to be used
 * later by virDomainPinVcpu() API.
 */
#define VIR_COPY_CPUMAP(cpumaps, maplen, vcpu, cpumap) \
    memcpy(cpumap, VIR_GET_CPUMAP(cpumaps, maplen, vcpu), maplen)


/**
 * VIR_GET_CPUMAP:
 * @cpumaps: pointer to an array of cpumap (in 8-bit bytes) (IN)
 * @maplen: the length (in bytes) of one cpumap
 * @vcpu: the virtual CPU number
 *
 * This macro is to be used in conjunction with virDomainGetVcpus() and
 * virDomainPinVcpu() APIs. VIR_GET_CPUMAP macro returns a pointer to the
 * cpumap of the specified vcpu from cpumaps array.
 */
#define VIR_GET_CPUMAP(cpumaps, maplen, vcpu) (&((cpumaps)[(vcpu) * (maplen)]))


typedef enum {
    /* See virDomainModificationImpact for these flags.  */
    VIR_DOMAIN_DEVICE_MODIFY_CURRENT = VIR_DOMAIN_AFFECT_CURRENT,
    VIR_DOMAIN_DEVICE_MODIFY_LIVE    = VIR_DOMAIN_AFFECT_LIVE,
    VIR_DOMAIN_DEVICE_MODIFY_CONFIG  = VIR_DOMAIN_AFFECT_CONFIG,

    /* Additionally, these flags may be bitwise-OR'd in.  */
    VIR_DOMAIN_DEVICE_MODIFY_FORCE = (1 << 2), /* Forcibly modify device
                                                  (ex. force eject a cdrom) */
} virDomainDeviceModifyFlags;

int virDomainAttachDevice(virDomainPtr domain, const char *xml);
int virDomainDetachDevice(virDomainPtr domain, const char *xml);

int virDomainAttachDeviceFlags(virDomainPtr domain,
                               const char *xml, unsigned int flags);
int virDomainDetachDeviceFlags(virDomainPtr domain,
                               const char *xml, unsigned int flags);
int virDomainUpdateDeviceFlags(virDomainPtr domain,
                               const char *xml, unsigned int flags);

/*
 * BlockJob API
 */

/**
 * virDomainBlockJobType:
 *
 * VIR_DOMAIN_BLOCK_JOB_TYPE_PULL: Block Pull (virDomainBlockPull, or
 * virDomainBlockRebase without flags), job ends on completion
 * VIR_DOMAIN_BLOCK_JOB_TYPE_COPY: Block Copy (virDomainBlockRebase with
 * flags), job exists as long as mirroring is active
 * VIR_DOMAIN_BLOCK_JOB_TYPE_COMMIT: Block Commit (virDomainBlockCommit),
 * job ends on completion
 */
typedef enum {
    VIR_DOMAIN_BLOCK_JOB_TYPE_UNKNOWN = 0,
    VIR_DOMAIN_BLOCK_JOB_TYPE_PULL = 1,
    VIR_DOMAIN_BLOCK_JOB_TYPE_COPY = 2,
    VIR_DOMAIN_BLOCK_JOB_TYPE_COMMIT = 3,

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_BLOCK_JOB_TYPE_LAST
#endif
} virDomainBlockJobType;

/**
 * virDomainBlockJobAbortFlags:
 *
 * VIR_DOMAIN_BLOCK_JOB_ABORT_ASYNC: Request only, do not wait for completion
 * VIR_DOMAIN_BLOCK_JOB_ABORT_PIVOT: Pivot to mirror when ending a copy job
 */
typedef enum {
    VIR_DOMAIN_BLOCK_JOB_ABORT_ASYNC = 1 << 0,
    VIR_DOMAIN_BLOCK_JOB_ABORT_PIVOT = 1 << 1,
} virDomainBlockJobAbortFlags;

/* An iterator for monitoring block job operations */
typedef unsigned long long virDomainBlockJobCursor;

typedef struct _virDomainBlockJobInfo virDomainBlockJobInfo;
struct _virDomainBlockJobInfo {
    virDomainBlockJobType type;
    unsigned long bandwidth;
    /*
     * The following fields provide an indication of block job progress.  @cur
     * indicates the current position and will be between 0 and @end.  @end is
     * the final cursor position for this operation and represents completion.
     * To approximate progress, divide @cur by @end.
     */
    virDomainBlockJobCursor cur;
    virDomainBlockJobCursor end;
};
typedef virDomainBlockJobInfo *virDomainBlockJobInfoPtr;

int       virDomainBlockJobAbort(virDomainPtr dom, const char *disk,
                                 unsigned int flags);
int     virDomainGetBlockJobInfo(virDomainPtr dom, const char *disk,
                                 virDomainBlockJobInfoPtr info,
                                 unsigned int flags);
int    virDomainBlockJobSetSpeed(virDomainPtr dom, const char *disk,
                                 unsigned long bandwidth, unsigned int flags);

int           virDomainBlockPull(virDomainPtr dom, const char *disk,
                                 unsigned long bandwidth, unsigned int flags);

/**
 * virDomainBlockRebaseFlags:
 *
 * Flags available for virDomainBlockRebase().
 */
typedef enum {
    VIR_DOMAIN_BLOCK_REBASE_SHALLOW   = 1 << 0, /* Limit copy to top of source
                                                   backing chain */
    VIR_DOMAIN_BLOCK_REBASE_REUSE_EXT = 1 << 1, /* Reuse existing external
                                                   file for a copy */
    VIR_DOMAIN_BLOCK_REBASE_COPY_RAW  = 1 << 2, /* Make destination file raw */
    VIR_DOMAIN_BLOCK_REBASE_COPY      = 1 << 3, /* Start a copy job */
} virDomainBlockRebaseFlags;

int           virDomainBlockRebase(virDomainPtr dom, const char *disk,
                                   const char *base, unsigned long bandwidth,
                                   unsigned int flags);

/**
 * virDomainBlockCommitFlags:
 *
 * Flags available for virDomainBlockCommit().
 */
typedef enum {
    VIR_DOMAIN_BLOCK_COMMIT_SHALLOW = 1 << 0, /* NULL base means next backing
                                                 file, not whole chain */
    VIR_DOMAIN_BLOCK_COMMIT_DELETE  = 1 << 1, /* Delete any files that are now
                                                 invalid after their contents
                                                 have been committed */
} virDomainBlockCommitFlags;

int virDomainBlockCommit(virDomainPtr dom, const char *disk, const char *base,
                         const char *top, unsigned long bandwidth,
                         unsigned int flags);


/* Block I/O throttling support */

/**
 * VIR_DOMAIN_BLOCK_IOTUNE_TOTAL_BYTES_SEC:
 *
 * Macro for the BlockIoTune tunable weight: it represents the total
 * bytes per second permitted through a block device, as a ullong.
 */
#define VIR_DOMAIN_BLOCK_IOTUNE_TOTAL_BYTES_SEC "total_bytes_sec"

/**
 * VIR_DOMAIN_BLOCK_IOTUNE_READ_BYTES_SEC:
 *
 * Macro for the BlockIoTune tunable weight: it represents the read
 * bytes per second permitted through a block device, as a ullong.
 */
#define VIR_DOMAIN_BLOCK_IOTUNE_READ_BYTES_SEC "read_bytes_sec"

/**
 * VIR_DOMAIN_BLOCK_IOTUNE_WRITE_BYTES_SEC:
 *
 * Macro for the BlockIoTune tunable weight: it represents the write
 * bytes per second permitted through a block device, as a ullong.
 */
#define VIR_DOMAIN_BLOCK_IOTUNE_WRITE_BYTES_SEC "write_bytes_sec"

/**
 * VIR_DOMAIN_BLOCK_IOTUNE_TOTAL_IOPS_SEC:
 *
 * Macro for the BlockIoTune tunable weight: it represents the total
 * I/O operations per second permitted through a block device, as a ullong.
 */
#define VIR_DOMAIN_BLOCK_IOTUNE_TOTAL_IOPS_SEC "total_iops_sec"

/**
 * VIR_DOMAIN_BLOCK_IOTUNE_READ_IOPS_SEC:
 *
 * Macro for the BlockIoTune tunable weight: it represents the read
 * I/O operations per second permitted through a block device, as a ullong.
 */
#define VIR_DOMAIN_BLOCK_IOTUNE_READ_IOPS_SEC "read_iops_sec"

/**
 * VIR_DOMAIN_BLOCK_IOTUNE_WRITE_IOPS_SEC:
 * Macro for the BlockIoTune tunable weight: it represents the write
 * I/O operations per second permitted through a block device, as a ullong.
 */
#define VIR_DOMAIN_BLOCK_IOTUNE_WRITE_IOPS_SEC "write_iops_sec"

int
virDomainSetBlockIoTune(virDomainPtr dom,
                        const char *disk,
                        virTypedParameterPtr params,
                        int nparams,
                        unsigned int flags);
int
virDomainGetBlockIoTune(virDomainPtr dom,
                        const char *disk,
                        virTypedParameterPtr params,
                        int *nparams,
                        unsigned int flags);

/**
 * virDomainDiskErrorCode:
 *
 * Disk I/O error.
 */
typedef enum {
    VIR_DOMAIN_DISK_ERROR_NONE      = 0, /* no error */
    VIR_DOMAIN_DISK_ERROR_UNSPEC    = 1, /* unspecified I/O error */
    VIR_DOMAIN_DISK_ERROR_NO_SPACE  = 2, /* no space left on the device */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_DISK_ERROR_LAST
#endif
} virDomainDiskErrorCode;

/**
 * virDomainDiskError:
 *
 */
typedef struct _virDomainDiskError virDomainDiskError;
typedef virDomainDiskError *virDomainDiskErrorPtr;

struct _virDomainDiskError {
    char *disk; /* disk target */
    int error;  /* virDomainDiskErrorCode */
};

int virDomainGetDiskErrors(virDomainPtr dom,
                           virDomainDiskErrorPtr errors,
                           unsigned int maxerrors,
                           unsigned int flags);


/*
 * NUMA support
 */

int                      virNodeGetCellsFreeMemory(virConnectPtr conn,
                                                   unsigned long long *freeMems,
                                                   int startCell,
                                                   int maxCells);

/*
 * Virtual Networks API
 */

typedef enum {
    VIR_NETWORK_XML_INACTIVE = (1 << 0), /* dump inactive network information */
} virNetworkXMLFlags;

/**
 * virNetwork:
 *
 * a virNetwork is a private structure representing a virtual network.
 */
typedef struct _virNetwork virNetwork;

/**
 * virNetworkPtr:
 *
 * a virNetworkPtr is pointer to a virNetwork private structure, this is the
 * type used to reference a virtual network in the API.
 */
typedef virNetwork *virNetworkPtr;

/*
 * Get connection from network.
 */
virConnectPtr           virNetworkGetConnect    (virNetworkPtr network);

/*
 * List active networks
 */
int                     virConnectNumOfNetworks (virConnectPtr conn);
int                     virConnectListNetworks  (virConnectPtr conn,
                                                 char **const names,
                                                 int maxnames);

/*
 * List inactive networks
 */
int                     virConnectNumOfDefinedNetworks  (virConnectPtr conn);
int                     virConnectListDefinedNetworks   (virConnectPtr conn,
                                                         char **const names,
                                                         int maxnames);
/*
 * virConnectListAllNetworks:
 *
 * Flags used to filter the returned networks. Flags in each group
 * are exclusive attributes of a network.
 */
typedef enum {
    VIR_CONNECT_LIST_NETWORKS_INACTIVE      = 1 << 0,
    VIR_CONNECT_LIST_NETWORKS_ACTIVE        = 1 << 1,

    VIR_CONNECT_LIST_NETWORKS_PERSISTENT    = 1 << 2,
    VIR_CONNECT_LIST_NETWORKS_TRANSIENT     = 1 << 3,

    VIR_CONNECT_LIST_NETWORKS_AUTOSTART     = 1 << 4,
    VIR_CONNECT_LIST_NETWORKS_NO_AUTOSTART  = 1 << 5,
} virConnectListAllNetworksFlags;

int                     virConnectListAllNetworks       (virConnectPtr conn,
                                                         virNetworkPtr **nets,
                                                         unsigned int flags);

/*
 * Lookup network by name or uuid
 */
virNetworkPtr           virNetworkLookupByName          (virConnectPtr conn,
                                                         const char *name);
virNetworkPtr           virNetworkLookupByUUID          (virConnectPtr conn,
                                                         const unsigned char *uuid);
virNetworkPtr           virNetworkLookupByUUIDString    (virConnectPtr conn,
                                                         const char *uuid);

/*
 * Create active transient network
 */
virNetworkPtr           virNetworkCreateXML     (virConnectPtr conn,
                                                 const char *xmlDesc);

/*
 * Define inactive persistent network
 */
virNetworkPtr           virNetworkDefineXML     (virConnectPtr conn,
                                                 const char *xmlDesc);

/*
 * Delete persistent network
 */
int                     virNetworkUndefine      (virNetworkPtr network);

/**
 * virNetworkUpdateCommand:
 *
 * describes which type of update to perform on a <network>
 * definition.
 *
 */
typedef enum {
    VIR_NETWORK_UPDATE_COMMAND_NONE      = 0, /* (invalid) */
    VIR_NETWORK_UPDATE_COMMAND_MODIFY    = 1, /* modify an existing element */
    VIR_NETWORK_UPDATE_COMMAND_DELETE    = 2, /* delete an existing element */
    VIR_NETWORK_UPDATE_COMMAND_ADD_LAST  = 3, /* add an element at end of list */
    VIR_NETWORK_UPDATE_COMMAND_ADD_FIRST = 4, /* add an element at start of list */
#ifdef VIR_ENUM_SENTINELS
    VIR_NETWORK_UPDATE_COMMAND_LAST
#endif
} virNetworkUpdateCommand;

/**
 * virNetworkUpdateSection:
 *
 * describes which section of a <network> definition the provided
 * xml should be applied to.
 *
 */
typedef enum {
    VIR_NETWORK_SECTION_NONE              =  0, /* (invalid) */
    VIR_NETWORK_SECTION_BRIDGE            =  1, /* <bridge> */
    VIR_NETWORK_SECTION_DOMAIN            =  2, /* <domain> */
    VIR_NETWORK_SECTION_IP                =  3, /* <ip> */
    VIR_NETWORK_SECTION_IP_DHCP_HOST      =  4, /* <ip>/<dhcp>/<host> */
    VIR_NETWORK_SECTION_IP_DHCP_RANGE     =  5, /* <ip>/<dhcp>/<range> */
    VIR_NETWORK_SECTION_FORWARD           =  6, /* <forward> */
    VIR_NETWORK_SECTION_FORWARD_INTERFACE =  7, /* <forward>/<interface> */
    VIR_NETWORK_SECTION_FORWARD_PF        =  8, /* <forward>/<pf> */
    VIR_NETWORK_SECTION_PORTGROUP         =  9, /* <portgroup> */
    VIR_NETWORK_SECTION_DNS_HOST          = 10, /* <dns>/<host> */
    VIR_NETWORK_SECTION_DNS_TXT           = 11, /* <dns>/<txt> */
    VIR_NETWORK_SECTION_DNS_SRV           = 12, /* <dns>/<srv> */
#ifdef VIR_ENUM_SENTINELS
    VIR_NETWORK_SECTION_LAST
#endif
} virNetworkUpdateSection;

/**
 * virNetworkUpdateFlags:
 *
 * Flags to control options for virNetworkUpdate()
 */
typedef enum {
    VIR_NETWORK_UPDATE_AFFECT_CURRENT = 0,      /* affect live if network is active,
                                                   config if it's not active */
    VIR_NETWORK_UPDATE_AFFECT_LIVE    = 1 << 0, /* affect live state of network only */
    VIR_NETWORK_UPDATE_AFFECT_CONFIG  = 1 << 1, /* affect persistent config only */
} virNetworkUpdateFlags;

/*
 * Update an existing network definition
 */
int                     virNetworkUpdate(virNetworkPtr network,
                                         unsigned int command, /* virNetworkUpdateCommand */
                                         unsigned int section, /* virNetworkUpdateSection */
                                         int parentIndex,
                                         const char *xml,
                                         unsigned int flags);

/*
 * Activate persistent network
 */
int                     virNetworkCreate        (virNetworkPtr network);

/*
 * Network destroy/free
 */
int                     virNetworkDestroy       (virNetworkPtr network);
int                     virNetworkRef           (virNetworkPtr network);
int                     virNetworkFree          (virNetworkPtr network);

/*
 * Network information
 */
const char*             virNetworkGetName       (virNetworkPtr network);
int                     virNetworkGetUUID       (virNetworkPtr network,
                                                 unsigned char *uuid);
int                     virNetworkGetUUIDString (virNetworkPtr network,
                                                 char *buf);
char *                  virNetworkGetXMLDesc    (virNetworkPtr network,
                                                 unsigned int flags);
char *                  virNetworkGetBridgeName (virNetworkPtr network);

int                     virNetworkGetAutostart  (virNetworkPtr network,
                                                 int *autostart);
int                     virNetworkSetAutostart  (virNetworkPtr network,
                                                 int autostart);

/*
 * Physical host interface configuration API
 */

/**
 * virInterface:
 *
 * a virInterface is a private structure representing a virtual interface.
 */
typedef struct _virInterface virInterface;

/**
 * virInterfacePtr:
 *
 * a virInterfacePtr is pointer to a virInterface private structure, this is the
 * type used to reference a virtual interface in the API.
 */
typedef virInterface *virInterfacePtr;

virConnectPtr           virInterfaceGetConnect    (virInterfacePtr iface);

int                     virConnectNumOfInterfaces (virConnectPtr conn);
int                     virConnectListInterfaces  (virConnectPtr conn,
                                                   char **const names,
                                                   int maxnames);

int                     virConnectNumOfDefinedInterfaces (virConnectPtr conn);
int                     virConnectListDefinedInterfaces  (virConnectPtr conn,
                                                          char **const names,
                                                          int maxnames);
/*
 * virConnectListAllInterfaces:
 *
 * Flags used to filter the returned interfaces.
 */
typedef enum {
    VIR_CONNECT_LIST_INTERFACES_INACTIVE      = 1 << 0,
    VIR_CONNECT_LIST_INTERFACES_ACTIVE        = 1 << 1,
} virConnectListAllInterfacesFlags;

int                     virConnectListAllInterfaces (virConnectPtr conn,
                                                     virInterfacePtr **ifaces,
                                                     unsigned int flags);

virInterfacePtr         virInterfaceLookupByName  (virConnectPtr conn,
                                                   const char *name);
virInterfacePtr         virInterfaceLookupByMACString (virConnectPtr conn,
                                                       const char *mac);

const char*             virInterfaceGetName       (virInterfacePtr iface);
const char*             virInterfaceGetMACString  (virInterfacePtr iface);

typedef enum {
    VIR_INTERFACE_XML_INACTIVE = 1 << 0 /* dump inactive interface information */
} virInterfaceXMLFlags;

char *                  virInterfaceGetXMLDesc    (virInterfacePtr iface,
                                                   unsigned int flags);
virInterfacePtr         virInterfaceDefineXML     (virConnectPtr conn,
                                                   const char *xmlDesc,
                                                   unsigned int flags);

int                     virInterfaceUndefine      (virInterfacePtr iface);

int                     virInterfaceCreate        (virInterfacePtr iface,
                                                   unsigned int flags);

int                     virInterfaceDestroy       (virInterfacePtr iface,
                                                   unsigned int flags);

int                     virInterfaceRef           (virInterfacePtr iface);
int                     virInterfaceFree          (virInterfacePtr iface);

int                     virInterfaceChangeBegin   (virConnectPtr conn,
                                                   unsigned int flags);
int                     virInterfaceChangeCommit  (virConnectPtr conn,
                                                   unsigned int flags);
int                     virInterfaceChangeRollback(virConnectPtr conn,
                                                   unsigned int flags);

/**
 * virStoragePool:
 *
 * a virStoragePool is a private structure representing a storage pool
 */
typedef struct _virStoragePool virStoragePool;

/**
 * virStoragePoolPtr:
 *
 * a virStoragePoolPtr is pointer to a virStoragePool private structure, this is the
 * type used to reference a storage pool in the API.
 */
typedef virStoragePool *virStoragePoolPtr;


typedef enum {
    VIR_STORAGE_POOL_INACTIVE = 0, /* Not running */
    VIR_STORAGE_POOL_BUILDING = 1, /* Initializing pool, not available */
    VIR_STORAGE_POOL_RUNNING = 2,  /* Running normally */
    VIR_STORAGE_POOL_DEGRADED = 3, /* Running degraded */
    VIR_STORAGE_POOL_INACCESSIBLE = 4, /* Running, but not accessible */

#ifdef VIR_ENUM_SENTINELS
    VIR_STORAGE_POOL_STATE_LAST
#endif
} virStoragePoolState;


typedef enum {
    VIR_STORAGE_POOL_BUILD_NEW  = 0,   /* Regular build from scratch */
    VIR_STORAGE_POOL_BUILD_REPAIR = (1 << 0), /* Repair / reinitialize */
    VIR_STORAGE_POOL_BUILD_RESIZE = (1 << 1),  /* Extend existing pool */
    VIR_STORAGE_POOL_BUILD_NO_OVERWRITE = (1 << 2),  /* Do not overwrite existing pool */
    VIR_STORAGE_POOL_BUILD_OVERWRITE = (1 << 3),  /* Overwrite data */
} virStoragePoolBuildFlags;

typedef enum {
    VIR_STORAGE_POOL_DELETE_NORMAL = 0, /* Delete metadata only    (fast) */
    VIR_STORAGE_POOL_DELETE_ZEROED = 1 << 0,  /* Clear all data to zeros (slow) */
} virStoragePoolDeleteFlags;

typedef struct _virStoragePoolInfo virStoragePoolInfo;

struct _virStoragePoolInfo {
    int state;                     /* virStoragePoolState flags */
    unsigned long long capacity;   /* Logical size bytes */
    unsigned long long allocation; /* Current allocation bytes */
    unsigned long long available;  /* Remaining free space bytes */
};

typedef virStoragePoolInfo *virStoragePoolInfoPtr;


/**
 * virStorageVol:
 *
 * a virStorageVol is a private structure representing a storage volume
 */
typedef struct _virStorageVol virStorageVol;

/**
 * virStorageVolPtr:
 *
 * a virStorageVolPtr is pointer to a virStorageVol private structure, this is the
 * type used to reference a storage volume in the API.
 */
typedef virStorageVol *virStorageVolPtr;


typedef enum {
    VIR_STORAGE_VOL_FILE = 0,     /* Regular file based volumes */
    VIR_STORAGE_VOL_BLOCK = 1,    /* Block based volumes */
    VIR_STORAGE_VOL_DIR = 2,      /* Directory-passthrough based volume */
    VIR_STORAGE_VOL_NETWORK = 3,  /* Network volumes like RBD (RADOS Block Device) */
    VIR_STORAGE_VOL_NETDIR = 4,   /* Network accessible directory that can
                                   * contain other network volumes */

#ifdef VIR_ENUM_SENTINELS
    VIR_STORAGE_VOL_LAST
#endif
} virStorageVolType;

typedef enum {
    VIR_STORAGE_VOL_DELETE_NORMAL = 0, /* Delete metadata only    (fast) */
    VIR_STORAGE_VOL_DELETE_ZEROED = 1 << 0,  /* Clear all data to zeros (slow) */
} virStorageVolDeleteFlags;

typedef enum {
    VIR_STORAGE_VOL_WIPE_ALG_ZERO = 0, /* 1-pass, all zeroes */
    VIR_STORAGE_VOL_WIPE_ALG_NNSA = 1, /* 4-pass  NNSA Policy Letter
                                          NAP-14.1-C (XVI-8) */
    VIR_STORAGE_VOL_WIPE_ALG_DOD = 2, /* 4-pass DoD 5220.22-M section
                                         8-306 procedure */
    VIR_STORAGE_VOL_WIPE_ALG_BSI = 3, /* 9-pass method recommended by the
                                         German Center of Security in
                                         Information Technologies */
    VIR_STORAGE_VOL_WIPE_ALG_GUTMANN = 4, /* The canonical 35-pass sequence */
    VIR_STORAGE_VOL_WIPE_ALG_SCHNEIER = 5, /* 7-pass method described by
                                              Bruce Schneier in "Applied
                                              Cryptography" (1996) */
    VIR_STORAGE_VOL_WIPE_ALG_PFITZNER7 = 6, /* 7-pass random */

    VIR_STORAGE_VOL_WIPE_ALG_PFITZNER33 = 7, /* 33-pass random */

    VIR_STORAGE_VOL_WIPE_ALG_RANDOM = 8, /* 1-pass random */

#ifdef VIR_ENUM_SENTINELS
    VIR_STORAGE_VOL_WIPE_ALG_LAST
    /*
     * NB: this enum value will increase over time as new algorithms are
     * added to the libvirt API. It reflects the last algorithm supported
     * by this version of the libvirt API.
     */
#endif
} virStorageVolWipeAlgorithm;

typedef struct _virStorageVolInfo virStorageVolInfo;

struct _virStorageVolInfo {
    int type;                      /* virStorageVolType flags */
    unsigned long long capacity;   /* Logical size bytes */
    unsigned long long allocation; /* Current allocation bytes */
};

typedef virStorageVolInfo *virStorageVolInfoPtr;

typedef enum {
    VIR_STORAGE_XML_INACTIVE    = (1 << 0), /* dump inactive pool/volume information */
} virStorageXMLFlags;

/*
 * Get connection from pool.
 */
virConnectPtr           virStoragePoolGetConnect        (virStoragePoolPtr pool);

/*
 * List active storage pools
 */
int                     virConnectNumOfStoragePools     (virConnectPtr conn);
int                     virConnectListStoragePools      (virConnectPtr conn,
                                                         char **const names,
                                                         int maxnames);

/*
 * List inactive storage pools
 */
int                     virConnectNumOfDefinedStoragePools(virConnectPtr conn);
int                     virConnectListDefinedStoragePools(virConnectPtr conn,
                                                          char **const names,
                                                          int maxnames);

/*
 * virConnectListAllStoragePoolsFlags:
 *
 * Flags used to tune pools returned by virConnectListAllStoragePools().
 * Note that these flags come in groups; if all bits from a group are 0,
 * then that group is not used to filter results.
 */
typedef enum {
    VIR_CONNECT_LIST_STORAGE_POOLS_INACTIVE      = 1 << 0,
    VIR_CONNECT_LIST_STORAGE_POOLS_ACTIVE        = 1 << 1,

    VIR_CONNECT_LIST_STORAGE_POOLS_PERSISTENT    = 1 << 2,
    VIR_CONNECT_LIST_STORAGE_POOLS_TRANSIENT     = 1 << 3,

    VIR_CONNECT_LIST_STORAGE_POOLS_AUTOSTART     = 1 << 4,
    VIR_CONNECT_LIST_STORAGE_POOLS_NO_AUTOSTART  = 1 << 5,

    /* List pools by type */
    VIR_CONNECT_LIST_STORAGE_POOLS_DIR           = 1 << 6,
    VIR_CONNECT_LIST_STORAGE_POOLS_FS            = 1 << 7,
    VIR_CONNECT_LIST_STORAGE_POOLS_NETFS         = 1 << 8,
    VIR_CONNECT_LIST_STORAGE_POOLS_LOGICAL       = 1 << 9,
    VIR_CONNECT_LIST_STORAGE_POOLS_DISK          = 1 << 10,
    VIR_CONNECT_LIST_STORAGE_POOLS_ISCSI         = 1 << 11,
    VIR_CONNECT_LIST_STORAGE_POOLS_SCSI          = 1 << 12,
    VIR_CONNECT_LIST_STORAGE_POOLS_MPATH         = 1 << 13,
    VIR_CONNECT_LIST_STORAGE_POOLS_RBD           = 1 << 14,
    VIR_CONNECT_LIST_STORAGE_POOLS_SHEEPDOG      = 1 << 15,
    VIR_CONNECT_LIST_STORAGE_POOLS_GLUSTER       = 1 << 16,
} virConnectListAllStoragePoolsFlags;

int                     virConnectListAllStoragePools(virConnectPtr conn,
                                                      virStoragePoolPtr **pools,
                                                      unsigned int flags);
/*
 * Query a host for storage pools of a particular type
 */
char *                  virConnectFindStoragePoolSources(virConnectPtr conn,
                                                         const char *type,
                                                         const char *srcSpec,
                                                         unsigned int flags);

/*
 * Lookup pool by name or uuid
 */
virStoragePoolPtr       virStoragePoolLookupByName      (virConnectPtr conn,
                                                         const char *name);
virStoragePoolPtr       virStoragePoolLookupByUUID      (virConnectPtr conn,
                                                         const unsigned char *uuid);
virStoragePoolPtr       virStoragePoolLookupByUUIDString(virConnectPtr conn,
                                                         const char *uuid);
virStoragePoolPtr       virStoragePoolLookupByVolume    (virStorageVolPtr vol);

/*
 * Creating/destroying pools
 */
virStoragePoolPtr       virStoragePoolCreateXML         (virConnectPtr conn,
                                                         const char *xmlDesc,
                                                         unsigned int flags);
virStoragePoolPtr       virStoragePoolDefineXML         (virConnectPtr conn,
                                                         const char *xmlDesc,
                                                         unsigned int flags);
int                     virStoragePoolBuild             (virStoragePoolPtr pool,
                                                         unsigned int flags);
int                     virStoragePoolUndefine          (virStoragePoolPtr pool);
int                     virStoragePoolCreate            (virStoragePoolPtr pool,
                                                         unsigned int flags);
int                     virStoragePoolDestroy           (virStoragePoolPtr pool);
int                     virStoragePoolDelete            (virStoragePoolPtr pool,
                                                         unsigned int flags);
int                     virStoragePoolRef               (virStoragePoolPtr pool);
int                     virStoragePoolFree              (virStoragePoolPtr pool);
int                     virStoragePoolRefresh           (virStoragePoolPtr pool,
                                                         unsigned int flags);

/*
 * StoragePool information
 */
const char*             virStoragePoolGetName           (virStoragePoolPtr pool);
int                     virStoragePoolGetUUID           (virStoragePoolPtr pool,
                                                         unsigned char *uuid);
int                     virStoragePoolGetUUIDString     (virStoragePoolPtr pool,
                                                         char *buf);

int                     virStoragePoolGetInfo           (virStoragePoolPtr vol,
                                                         virStoragePoolInfoPtr info);

char *                  virStoragePoolGetXMLDesc        (virStoragePoolPtr pool,
                                                         unsigned int flags);

int                     virStoragePoolGetAutostart      (virStoragePoolPtr pool,
                                                         int *autostart);
int                     virStoragePoolSetAutostart      (virStoragePoolPtr pool,
                                                         int autostart);

/*
 * List/lookup storage volumes within a pool
 */
int                     virStoragePoolNumOfVolumes      (virStoragePoolPtr pool);
int                     virStoragePoolListVolumes       (virStoragePoolPtr pool,
                                                         char **const names,
                                                         int maxnames);
int                     virStoragePoolListAllVolumes    (virStoragePoolPtr pool,
                                                         virStorageVolPtr **vols,
                                                         unsigned int flags);

virConnectPtr           virStorageVolGetConnect         (virStorageVolPtr vol);

/*
 * Lookup volumes based on various attributes
 */
virStorageVolPtr        virStorageVolLookupByName       (virStoragePoolPtr pool,
                                                         const char *name);
virStorageVolPtr        virStorageVolLookupByKey        (virConnectPtr conn,
                                                         const char *key);
virStorageVolPtr        virStorageVolLookupByPath       (virConnectPtr conn,
                                                         const char *path);


const char*             virStorageVolGetName            (virStorageVolPtr vol);
const char*             virStorageVolGetKey             (virStorageVolPtr vol);

typedef enum {
    VIR_STORAGE_VOL_CREATE_PREALLOC_METADATA = 1 << 0,
} virStorageVolCreateFlags;

virStorageVolPtr        virStorageVolCreateXML          (virStoragePoolPtr pool,
                                                         const char *xmldesc,
                                                         unsigned int flags);
virStorageVolPtr        virStorageVolCreateXMLFrom      (virStoragePoolPtr pool,
                                                         const char *xmldesc,
                                                         virStorageVolPtr clonevol,
                                                         unsigned int flags);
int                     virStorageVolDownload           (virStorageVolPtr vol,
                                                         virStreamPtr stream,
                                                         unsigned long long offset,
                                                         unsigned long long length,
                                                         unsigned int flags);
int                     virStorageVolUpload             (virStorageVolPtr vol,
                                                         virStreamPtr stream,
                                                         unsigned long long offset,
                                                         unsigned long long length,
                                                         unsigned int flags);
int                     virStorageVolDelete             (virStorageVolPtr vol,
                                                         unsigned int flags);
int                     virStorageVolWipe               (virStorageVolPtr vol,
                                                         unsigned int flags);
int                     virStorageVolWipePattern        (virStorageVolPtr vol,
                                                         unsigned int algorithm,
                                                         unsigned int flags);
int                     virStorageVolRef                (virStorageVolPtr vol);
int                     virStorageVolFree               (virStorageVolPtr vol);

int                     virStorageVolGetInfo            (virStorageVolPtr vol,
                                                         virStorageVolInfoPtr info);
char *                  virStorageVolGetXMLDesc         (virStorageVolPtr pool,
                                                         unsigned int flags);

char *                  virStorageVolGetPath            (virStorageVolPtr vol);

typedef enum {
    VIR_STORAGE_VOL_RESIZE_ALLOCATE = 1 << 0, /* force allocation of new size */
    VIR_STORAGE_VOL_RESIZE_DELTA    = 1 << 1, /* size is relative to current */
    VIR_STORAGE_VOL_RESIZE_SHRINK   = 1 << 2, /* allow decrease in capacity */
} virStorageVolResizeFlags;

int                     virStorageVolResize             (virStorageVolPtr vol,
                                                         unsigned long long capacity,
                                                         unsigned int flags);


/**
 * virKeycodeSet:
 *
 * Enum to specify which keycode mapping is in use for virDomainSendKey().
 */
typedef enum {
    VIR_KEYCODE_SET_LINUX          = 0,
    VIR_KEYCODE_SET_XT             = 1,
    VIR_KEYCODE_SET_ATSET1         = 2,
    VIR_KEYCODE_SET_ATSET2         = 3,
    VIR_KEYCODE_SET_ATSET3         = 4,
    VIR_KEYCODE_SET_OSX            = 5,
    VIR_KEYCODE_SET_XT_KBD         = 6,
    VIR_KEYCODE_SET_USB            = 7,
    VIR_KEYCODE_SET_WIN32          = 8,
    VIR_KEYCODE_SET_RFB            = 9,

#ifdef VIR_ENUM_SENTINELS
    VIR_KEYCODE_SET_LAST
    /*
     * NB: this enum value will increase over time as new events are
     * added to the libvirt API. It reflects the last keycode set supported
     * by this version of the libvirt API.
     */
#endif
} virKeycodeSet;

/**
 * VIR_DOMAIN_SEND_KEY_MAX_KEYS:
 *
 * Maximum number of keycodes that can be sent in one virDomainSendKey() call.
 */
#define VIR_DOMAIN_SEND_KEY_MAX_KEYS  16

int virDomainSendKey(virDomainPtr domain,
                     unsigned int codeset,
                     unsigned int holdtime,
                     unsigned int *keycodes,
                     int nkeycodes,
                     unsigned int flags);

/*
 * These just happen to match Linux signal numbers. The numbers
 * will be mapped to whatever the SIGNUM is in the guest OS in
 * question by the agent delivering the signal. The names are
 * based on the POSIX / XSI signal standard though.
 *
 * Do not rely on all values matching Linux though. It is possible
 * this enum might be extended with new signals which have no
 * mapping in Linux.
 */
typedef enum {
    VIR_DOMAIN_PROCESS_SIGNAL_NOP        =  0, /* No constant in POSIX/Linux */
    VIR_DOMAIN_PROCESS_SIGNAL_HUP        =  1, /* SIGHUP */
    VIR_DOMAIN_PROCESS_SIGNAL_INT        =  2, /* SIGINT */
    VIR_DOMAIN_PROCESS_SIGNAL_QUIT       =  3, /* SIGQUIT */
    VIR_DOMAIN_PROCESS_SIGNAL_ILL        =  4, /* SIGILL */
    VIR_DOMAIN_PROCESS_SIGNAL_TRAP       =  5, /* SIGTRAP */
    VIR_DOMAIN_PROCESS_SIGNAL_ABRT       =  6, /* SIGABRT */
    VIR_DOMAIN_PROCESS_SIGNAL_BUS        =  7, /* SIGBUS */
    VIR_DOMAIN_PROCESS_SIGNAL_FPE        =  8, /* SIGFPE */
    VIR_DOMAIN_PROCESS_SIGNAL_KILL       =  9, /* SIGKILL */

    VIR_DOMAIN_PROCESS_SIGNAL_USR1       = 10, /* SIGUSR1 */
    VIR_DOMAIN_PROCESS_SIGNAL_SEGV       = 11, /* SIGSEGV */
    VIR_DOMAIN_PROCESS_SIGNAL_USR2       = 12, /* SIGUSR2 */
    VIR_DOMAIN_PROCESS_SIGNAL_PIPE       = 13, /* SIGPIPE */
    VIR_DOMAIN_PROCESS_SIGNAL_ALRM       = 14, /* SIGALRM */
    VIR_DOMAIN_PROCESS_SIGNAL_TERM       = 15, /* SIGTERM */
    VIR_DOMAIN_PROCESS_SIGNAL_STKFLT     = 16, /* Not in POSIX (SIGSTKFLT on Linux )*/
    VIR_DOMAIN_PROCESS_SIGNAL_CHLD       = 17, /* SIGCHLD */
    VIR_DOMAIN_PROCESS_SIGNAL_CONT       = 18, /* SIGCONT */
    VIR_DOMAIN_PROCESS_SIGNAL_STOP       = 19, /* SIGSTOP */

    VIR_DOMAIN_PROCESS_SIGNAL_TSTP       = 20, /* SIGTSTP */
    VIR_DOMAIN_PROCESS_SIGNAL_TTIN       = 21, /* SIGTTIN */
    VIR_DOMAIN_PROCESS_SIGNAL_TTOU       = 22, /* SIGTTOU */
    VIR_DOMAIN_PROCESS_SIGNAL_URG        = 23, /* SIGURG */
    VIR_DOMAIN_PROCESS_SIGNAL_XCPU       = 24, /* SIGXCPU */
    VIR_DOMAIN_PROCESS_SIGNAL_XFSZ       = 25, /* SIGXFSZ */
    VIR_DOMAIN_PROCESS_SIGNAL_VTALRM     = 26, /* SIGVTALRM */
    VIR_DOMAIN_PROCESS_SIGNAL_PROF       = 27, /* SIGPROF */
    VIR_DOMAIN_PROCESS_SIGNAL_WINCH      = 28, /* Not in POSIX (SIGWINCH on Linux) */
    VIR_DOMAIN_PROCESS_SIGNAL_POLL       = 29, /* SIGPOLL (also known as SIGIO on Linux) */

    VIR_DOMAIN_PROCESS_SIGNAL_PWR        = 30, /* Not in POSIX (SIGPWR on Linux) */
    VIR_DOMAIN_PROCESS_SIGNAL_SYS        = 31, /* SIGSYS (also known as SIGUNUSED on Linux) */
    VIR_DOMAIN_PROCESS_SIGNAL_RT0        = 32, /* SIGRTMIN */
    VIR_DOMAIN_PROCESS_SIGNAL_RT1        = 33, /* SIGRTMIN + 1 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT2        = 34, /* SIGRTMIN + 2 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT3        = 35, /* SIGRTMIN + 3 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT4        = 36, /* SIGRTMIN + 4 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT5        = 37, /* SIGRTMIN + 5 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT6        = 38, /* SIGRTMIN + 6 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT7        = 39, /* SIGRTMIN + 7 */

    VIR_DOMAIN_PROCESS_SIGNAL_RT8        = 40, /* SIGRTMIN + 8 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT9        = 41, /* SIGRTMIN + 9 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT10       = 42, /* SIGRTMIN + 10 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT11       = 43, /* SIGRTMIN + 11 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT12       = 44, /* SIGRTMIN + 12 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT13       = 45, /* SIGRTMIN + 13 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT14       = 46, /* SIGRTMIN + 14 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT15       = 47, /* SIGRTMIN + 15 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT16       = 48, /* SIGRTMIN + 16 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT17       = 49, /* SIGRTMIN + 17 */

    VIR_DOMAIN_PROCESS_SIGNAL_RT18       = 50, /* SIGRTMIN + 18 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT19       = 51, /* SIGRTMIN + 19 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT20       = 52, /* SIGRTMIN + 20 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT21       = 53, /* SIGRTMIN + 21 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT22       = 54, /* SIGRTMIN + 22 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT23       = 55, /* SIGRTMIN + 23 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT24       = 56, /* SIGRTMIN + 24 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT25       = 57, /* SIGRTMIN + 25 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT26       = 58, /* SIGRTMIN + 26 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT27       = 59, /* SIGRTMIN + 27 */

    VIR_DOMAIN_PROCESS_SIGNAL_RT28       = 60, /* SIGRTMIN + 28 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT29       = 61, /* SIGRTMIN + 29 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT30       = 62, /* SIGRTMIN + 30 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT31       = 63, /* SIGRTMIN + 31 */
    VIR_DOMAIN_PROCESS_SIGNAL_RT32       = 64, /* SIGRTMIN + 32 / SIGRTMAX */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_PROCESS_SIGNAL_LAST
#endif
} virDomainProcessSignal;

int virDomainSendProcessSignal(virDomainPtr domain,
                               long long pid_value,
                               unsigned int signum,
                               unsigned int flags);

/*
 * Deprecated calls
 */
virDomainPtr            virDomainCreateLinux    (virConnectPtr conn,
                                                 const char *xmlDesc,
                                                 unsigned int flags);

/*
 * Host device enumeration
 */

/**
 * virNodeDevice:
 *
 * A virNodeDevice contains a node (host) device details.
 */

typedef struct _virNodeDevice virNodeDevice;

/**
 * virNodeDevicePtr:
 *
 * A virNodeDevicePtr is a pointer to a virNodeDevice structure.  Get
 * one via virNodeDeviceLookupByName, or virNodeDeviceCreate.  Be sure
 * to call virNodeDeviceFree when done using a virNodeDevicePtr obtained
 * from any of the above functions to avoid leaking memory.
 */

typedef virNodeDevice *virNodeDevicePtr;


int                     virNodeNumOfDevices     (virConnectPtr conn,
                                                 const char *cap,
                                                 unsigned int flags);

int                     virNodeListDevices      (virConnectPtr conn,
                                                 const char *cap,
                                                 char **const names,
                                                 int maxnames,
                                                 unsigned int flags);
/*
 * virConnectListAllNodeDevices:
 *
 * Flags used to filter the returned node devices. Flags in each group
 * are exclusive. Currently only one group to filter the devices by cap
 * type.
 */
typedef enum {
    VIR_CONNECT_LIST_NODE_DEVICES_CAP_SYSTEM        = 1 << 0,  /* System capability */
    VIR_CONNECT_LIST_NODE_DEVICES_CAP_PCI_DEV       = 1 << 1,  /* PCI device */
    VIR_CONNECT_LIST_NODE_DEVICES_CAP_USB_DEV       = 1 << 2,  /* USB device */
    VIR_CONNECT_LIST_NODE_DEVICES_CAP_USB_INTERFACE = 1 << 3,  /* USB interface */
    VIR_CONNECT_LIST_NODE_DEVICES_CAP_NET           = 1 << 4,  /* Network device */
    VIR_CONNECT_LIST_NODE_DEVICES_CAP_SCSI_HOST     = 1 << 5,  /* SCSI Host Bus Adapter */
    VIR_CONNECT_LIST_NODE_DEVICES_CAP_SCSI_TARGET   = 1 << 6,  /* SCSI Target */
    VIR_CONNECT_LIST_NODE_DEVICES_CAP_SCSI          = 1 << 7,  /* SCSI device */
    VIR_CONNECT_LIST_NODE_DEVICES_CAP_STORAGE       = 1 << 8,  /* Storage device */
    VIR_CONNECT_LIST_NODE_DEVICES_CAP_FC_HOST       = 1 << 9,  /* FC Host Bus Adapter */
    VIR_CONNECT_LIST_NODE_DEVICES_CAP_VPORTS        = 1 << 10, /* Capable of vport */
    VIR_CONNECT_LIST_NODE_DEVICES_CAP_SCSI_GENERIC  = 1 << 11, /* Capable of scsi_generic */
} virConnectListAllNodeDeviceFlags;

int                     virConnectListAllNodeDevices (virConnectPtr conn,
                                                      virNodeDevicePtr **devices,
                                                      unsigned int flags);

virNodeDevicePtr        virNodeDeviceLookupByName (virConnectPtr conn,
                                                   const char *name);

virNodeDevicePtr        virNodeDeviceLookupSCSIHostByWWN (virConnectPtr conn,
                                                          const char *wwnn,
                                                          const char *wwpn,
                                                          unsigned int flags);

const char *            virNodeDeviceGetName     (virNodeDevicePtr dev);

const char *            virNodeDeviceGetParent   (virNodeDevicePtr dev);

int                     virNodeDeviceNumOfCaps   (virNodeDevicePtr dev);

int                     virNodeDeviceListCaps    (virNodeDevicePtr dev,
                                                  char **const names,
                                                  int maxnames);

char *                  virNodeDeviceGetXMLDesc (virNodeDevicePtr dev,
                                                 unsigned int flags);

int                     virNodeDeviceRef        (virNodeDevicePtr dev);
int                     virNodeDeviceFree       (virNodeDevicePtr dev);

int                     virNodeDeviceDettach    (virNodeDevicePtr dev);
int                     virNodeDeviceDetachFlags(virNodeDevicePtr dev,
                                                 const char *driverName,
                                                 unsigned int flags);
int                     virNodeDeviceReAttach   (virNodeDevicePtr dev);
int                     virNodeDeviceReset      (virNodeDevicePtr dev);

virNodeDevicePtr        virNodeDeviceCreateXML  (virConnectPtr conn,
                                                 const char *xmlDesc,
                                                 unsigned int flags);

int                     virNodeDeviceDestroy    (virNodeDevicePtr dev);

/*
 * Domain Event Notification
 */

/**
 * virDomainEventType:
 *
 * a virDomainEventType is emitted during domain lifecycle events
 */
typedef enum {
    VIR_DOMAIN_EVENT_DEFINED = 0,
    VIR_DOMAIN_EVENT_UNDEFINED = 1,
    VIR_DOMAIN_EVENT_STARTED = 2,
    VIR_DOMAIN_EVENT_SUSPENDED = 3,
    VIR_DOMAIN_EVENT_RESUMED = 4,
    VIR_DOMAIN_EVENT_STOPPED = 5,
    VIR_DOMAIN_EVENT_SHUTDOWN = 6,
    VIR_DOMAIN_EVENT_PMSUSPENDED = 7,
    VIR_DOMAIN_EVENT_CRASHED = 8,

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_LAST
#endif
} virDomainEventType;

/**
 * virDomainEventDefinedDetailType:
 *
 * Details on the cause of a 'defined' lifecycle event
 */
typedef enum {
    VIR_DOMAIN_EVENT_DEFINED_ADDED = 0,     /* Newly created config file */
    VIR_DOMAIN_EVENT_DEFINED_UPDATED = 1,   /* Changed config file */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_DEFINED_LAST
#endif
} virDomainEventDefinedDetailType;

/**
 * virDomainEventUndefinedDetailType:
 *
 * Details on the cause of an 'undefined' lifecycle event
 */
typedef enum {
    VIR_DOMAIN_EVENT_UNDEFINED_REMOVED = 0, /* Deleted the config file */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_UNDEFINED_LAST
#endif
} virDomainEventUndefinedDetailType;

/**
 * virDomainEventStartedDetailType:
 *
 * Details on the cause of a 'started' lifecycle event
 */
typedef enum {
    VIR_DOMAIN_EVENT_STARTED_BOOTED = 0,   /* Normal startup from boot */
    VIR_DOMAIN_EVENT_STARTED_MIGRATED = 1, /* Incoming migration from another host */
    VIR_DOMAIN_EVENT_STARTED_RESTORED = 2, /* Restored from a state file */
    VIR_DOMAIN_EVENT_STARTED_FROM_SNAPSHOT = 3, /* Restored from snapshot */
    VIR_DOMAIN_EVENT_STARTED_WAKEUP = 4,   /* Started due to wakeup event */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_STARTED_LAST
#endif
} virDomainEventStartedDetailType;

/**
 * virDomainEventSuspendedDetailType:
 *
 * Details on the cause of a 'suspended' lifecycle event
 */
typedef enum {
    VIR_DOMAIN_EVENT_SUSPENDED_PAUSED = 0,   /* Normal suspend due to admin pause */
    VIR_DOMAIN_EVENT_SUSPENDED_MIGRATED = 1, /* Suspended for offline migration */
    VIR_DOMAIN_EVENT_SUSPENDED_IOERROR = 2,  /* Suspended due to a disk I/O error */
    VIR_DOMAIN_EVENT_SUSPENDED_WATCHDOG = 3,  /* Suspended due to a watchdog firing */
    VIR_DOMAIN_EVENT_SUSPENDED_RESTORED = 4,  /* Restored from paused state file */
    VIR_DOMAIN_EVENT_SUSPENDED_FROM_SNAPSHOT = 5, /* Restored from paused snapshot */
    VIR_DOMAIN_EVENT_SUSPENDED_API_ERROR = 6, /* suspended after failure during libvirt API call */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_SUSPENDED_LAST
#endif
} virDomainEventSuspendedDetailType;

/**
 * virDomainEventResumedDetailType:
 *
 * Details on the cause of a 'resumed' lifecycle event
 */
typedef enum {
    VIR_DOMAIN_EVENT_RESUMED_UNPAUSED = 0,   /* Normal resume due to admin unpause */
    VIR_DOMAIN_EVENT_RESUMED_MIGRATED = 1,   /* Resumed for completion of migration */
    VIR_DOMAIN_EVENT_RESUMED_FROM_SNAPSHOT = 2, /* Resumed from snapshot */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_RESUMED_LAST
#endif
} virDomainEventResumedDetailType;

/**
 * virDomainEventStoppedDetailType:
 *
 * Details on the cause of a 'stopped' lifecycle event
 */
typedef enum {
    VIR_DOMAIN_EVENT_STOPPED_SHUTDOWN = 0,  /* Normal shutdown */
    VIR_DOMAIN_EVENT_STOPPED_DESTROYED = 1, /* Forced poweroff from host */
    VIR_DOMAIN_EVENT_STOPPED_CRASHED = 2,   /* Guest crashed */
    VIR_DOMAIN_EVENT_STOPPED_MIGRATED = 3,  /* Migrated off to another host */
    VIR_DOMAIN_EVENT_STOPPED_SAVED = 4,     /* Saved to a state file */
    VIR_DOMAIN_EVENT_STOPPED_FAILED = 5,    /* Host emulator/mgmt failed */
    VIR_DOMAIN_EVENT_STOPPED_FROM_SNAPSHOT = 6, /* offline snapshot loaded */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_STOPPED_LAST
#endif
} virDomainEventStoppedDetailType;


/**
 * virDomainEventShutdownDetailType:
 *
 * Details on the cause of a 'shutdown' lifecycle event
 */
typedef enum {
    VIR_DOMAIN_EVENT_SHUTDOWN_FINISHED = 0, /* Guest finished shutdown sequence */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_SHUTDOWN_LAST
#endif
} virDomainEventShutdownDetailType;

/**
 * virDomainEventPMSuspendedDetailType:
 *
 * Details on the cause of a 'pmsuspended' lifecycle event
 */
typedef enum {
    VIR_DOMAIN_EVENT_PMSUSPENDED_MEMORY = 0, /* Guest was PM suspended to memory */
    VIR_DOMAIN_EVENT_PMSUSPENDED_DISK = 1, /* Guest was PM suspended to disk */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_PMSUSPENDED_LAST
#endif
} virDomainEventPMSuspendedDetailType;

/**
 * virDomainEventCrashedDetailType:
 *
 * Details on the cause of a 'crashed' lifecycle event
 */
typedef enum {
    VIR_DOMAIN_EVENT_CRASHED_PANICKED = 0, /* Guest was panicked */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_CRASHED_LAST
#endif
} virDomainEventCrashedDetailType;

/**
 * virConnectDomainEventCallback:
 * @conn: virConnect connection
 * @dom: The domain on which the event occurred
 * @event: The specific virDomainEventType which occurred
 * @detail: event specific detail information
 * @opaque: opaque user data
 *
 * A callback function to be registered, and called when a domain event occurs
 *
 * Returns 0 (the return value is currently ignored)
 */
typedef int (*virConnectDomainEventCallback)(virConnectPtr conn,
                                             virDomainPtr dom,
                                             int event,
                                             int detail,
                                             void *opaque);

int virConnectDomainEventRegister(virConnectPtr conn,
                                  virConnectDomainEventCallback cb,
                                  void *opaque,
                                  virFreeCallback freecb);

int virConnectDomainEventDeregister(virConnectPtr conn,
                                    virConnectDomainEventCallback cb);

/*
 * Events Implementation
 */

/**
 * virEventHandleType:
 *
 * a virEventHandleType is used similar to POLLxxx FD events, but is specific
 * to libvirt. A client app must translate to, and from POLL events when using
 * this construct.
 */
typedef enum {
    VIR_EVENT_HANDLE_READABLE  = (1 << 0),
    VIR_EVENT_HANDLE_WRITABLE  = (1 << 1),
    VIR_EVENT_HANDLE_ERROR     = (1 << 2),
    VIR_EVENT_HANDLE_HANGUP    = (1 << 3),
} virEventHandleType;

/**
 * virEventHandleCallback:
 *
 * @watch: watch on which the event occurred
 * @fd: file handle on which the event occurred
 * @events: bitset of events from virEventHandleType constants
 * @opaque: user data registered with handle
 *
 * Callback for receiving file handle events. The callback will
 * be invoked once for each event which is pending.
 */
typedef void (*virEventHandleCallback)(int watch, int fd, int events, void *opaque);

/**
 * virEventAddHandleFunc:
 * @fd: file descriptor to listen on
 * @event: bitset of events on which to fire the callback
 * @cb: the callback to be called when an event occurrs
 * @opaque: user data to pass to the callback
 * @ff: the callback invoked to free opaque data blob
 *
 * Part of the EventImpl, this callback adds a file handle callback to
 * listen for specific events. The same file handle can be registered
 * multiple times provided the requested event sets are non-overlapping
 *
 * If the opaque user data requires free'ing when the handle
 * is unregistered, then a 2nd callback can be supplied for
 * this purpose. This callback needs to be invoked from a clean stack.
 * If 'ff' callbacks are invoked directly from the virEventRemoveHandleFunc
 * they will likely deadlock in libvirt.
 *
 * Returns -1 if the file handle cannot be registered, otherwise a handle
 * watch number to be used for updating and unregistering for events
 */
typedef int (*virEventAddHandleFunc)(int fd, int event,
                                     virEventHandleCallback cb,
                                     void *opaque,
                                     virFreeCallback ff);

/**
 * virEventUpdateHandleFunc:
 * @watch: file descriptor watch to modify
 * @event: new events to listen on
 *
 * Part of the EventImpl, this user-provided callback is notified when
 * events to listen on change
 */
typedef void (*virEventUpdateHandleFunc)(int watch, int event);

/**
 * virEventRemoveHandleFunc:
 * @watch: file descriptor watch to stop listening on
 *
 * Part of the EventImpl, this user-provided callback is notified when
 * an fd is no longer being listened on.
 *
 * If a virEventHandleFreeFunc was supplied when the handle was
 * registered, it will be invoked some time during, or after this
 * function call, when it is safe to release the user data.
 *
 * Returns -1 if the file handle was not registered, 0 upon success
 */
typedef int (*virEventRemoveHandleFunc)(int watch);

/**
 * virEventTimeoutCallback:
 *
 * @timer: timer id emitting the event
 * @opaque: user data registered with handle
 *
 * callback for receiving timer events
 */
typedef void (*virEventTimeoutCallback)(int timer, void *opaque);

/**
 * virEventAddTimeoutFunc:
 * @timeout: The timeout to monitor
 * @cb: the callback to call when timeout has expired
 * @opaque: user data to pass to the callback
 * @ff: the callback invoked to free opaque data blob
 *
 * Part of the EventImpl, this user-defined callback handles adding an
 * event timeout.
 *
 * If the opaque user data requires free'ing when the handle
 * is unregistered, then a 2nd callback can be supplied for
 * this purpose.
 *
 * Returns a timer value
 */
typedef int (*virEventAddTimeoutFunc)(int timeout,
                                      virEventTimeoutCallback cb,
                                      void *opaque,
                                      virFreeCallback ff);

/**
 * virEventUpdateTimeoutFunc:
 * @timer: the timer to modify
 * @timeout: the new timeout value
 *
 * Part of the EventImpl, this user-defined callback updates an
 * event timeout.
 */
typedef void (*virEventUpdateTimeoutFunc)(int timer, int timeout);

/**
 * virEventRemoveTimeoutFunc:
 * @timer: the timer to remove
 *
 * Part of the EventImpl, this user-defined callback removes a timer
 *
 * If a virEventTimeoutFreeFunc was supplied when the handle was
 * registered, it will be invoked some time during, or after this
 * function call, when it is safe to release the user data.
 *
 * Returns 0 on success, -1 on failure
 */
typedef int (*virEventRemoveTimeoutFunc)(int timer);

void virEventRegisterImpl(virEventAddHandleFunc addHandle,
                          virEventUpdateHandleFunc updateHandle,
                          virEventRemoveHandleFunc removeHandle,
                          virEventAddTimeoutFunc addTimeout,
                          virEventUpdateTimeoutFunc updateTimeout,
                          virEventRemoveTimeoutFunc removeTimeout);

int virEventRegisterDefaultImpl(void);
int virEventRunDefaultImpl(void);

int virEventAddHandle(int fd, int events,
                      virEventHandleCallback cb,
                      void *opaque,
                      virFreeCallback ff);
void virEventUpdateHandle(int watch, int events);
int virEventRemoveHandle(int watch);

int virEventAddTimeout(int frequency,
                       virEventTimeoutCallback cb,
                       void *opaque,
                       virFreeCallback ff);
void virEventUpdateTimeout(int timer, int frequency);
int virEventRemoveTimeout(int timer);

/*
 * Secret manipulation API
 */

/**
 * virSecret:
 *
 * A virSecret stores a secret value (e.g. a passphrase or encryption key)
 * and associated metadata.
 */
typedef struct _virSecret virSecret;
typedef virSecret *virSecretPtr;

typedef enum {
    VIR_SECRET_USAGE_TYPE_NONE = 0,
    VIR_SECRET_USAGE_TYPE_VOLUME = 1,
    VIR_SECRET_USAGE_TYPE_CEPH = 2,
    VIR_SECRET_USAGE_TYPE_ISCSI = 3,

#ifdef VIR_ENUM_SENTINELS
    VIR_SECRET_USAGE_TYPE_LAST
    /*
     * NB: this enum value will increase over time as new events are
     * added to the libvirt API. It reflects the last secret owner ID
     * supported by this version of the libvirt API.
     */
#endif
} virSecretUsageType;

virConnectPtr           virSecretGetConnect     (virSecretPtr secret);
int                     virConnectNumOfSecrets  (virConnectPtr conn);
int                     virConnectListSecrets   (virConnectPtr conn,
                                                 char **uuids,
                                                 int maxuuids);

/*
 * virConnectListAllSecrets:
 *
 * Flags used to filter the returned secrets. Flags in each group
 * are exclusive attributes of a secret.
 */
typedef enum {
    VIR_CONNECT_LIST_SECRETS_EPHEMERAL    = 1 << 0, /* kept in memory, never
                                                       stored persistently */
    VIR_CONNECT_LIST_SECRETS_NO_EPHEMERAL = 1 << 1,

    VIR_CONNECT_LIST_SECRETS_PRIVATE      = 1 << 2, /* not revealed to any caller
                                                       of libvirt, nor to any other
                                                       node */
    VIR_CONNECT_LIST_SECRETS_NO_PRIVATE   = 1 << 3,
} virConnectListAllSecretsFlags;

int                     virConnectListAllSecrets(virConnectPtr conn,
                                                 virSecretPtr **secrets,
                                                 unsigned int flags);
virSecretPtr            virSecretLookupByUUID(virConnectPtr conn,
                                              const unsigned char *uuid);
virSecretPtr            virSecretLookupByUUIDString(virConnectPtr conn,
                                                    const char *uuid);
virSecretPtr            virSecretLookupByUsage(virConnectPtr conn,
                                               int usageType,
                                               const char *usageID);
virSecretPtr            virSecretDefineXML      (virConnectPtr conn,
                                                 const char *xml,
                                                 unsigned int flags);
int                     virSecretGetUUID        (virSecretPtr secret,
                                                 unsigned char *buf);
int                     virSecretGetUUIDString  (virSecretPtr secret,
                                                 char *buf);
int                     virSecretGetUsageType   (virSecretPtr secret);
const char *            virSecretGetUsageID     (virSecretPtr secret);
char *                  virSecretGetXMLDesc     (virSecretPtr secret,
                                                 unsigned int flags);
int                     virSecretSetValue       (virSecretPtr secret,
                                                 const unsigned char *value,
                                                 size_t value_size,
                                                 unsigned int flags);
unsigned char *         virSecretGetValue       (virSecretPtr secret,
                                                 size_t *value_size,
                                                 unsigned int flags);
int                     virSecretUndefine       (virSecretPtr secret);
int                     virSecretRef            (virSecretPtr secret);
int                     virSecretFree           (virSecretPtr secret);

typedef enum {
    VIR_STREAM_NONBLOCK = (1 << 0),
} virStreamFlags;

virStreamPtr virStreamNew(virConnectPtr conn,
                          unsigned int flags);
int virStreamRef(virStreamPtr st);

int virStreamSend(virStreamPtr st,
                  const char *data,
                  size_t nbytes);

int virStreamRecv(virStreamPtr st,
                  char *data,
                  size_t nbytes);


/**
 * virStreamSourceFunc:
 *
 * @st: the stream object
 * @data: preallocated array to be filled with data
 * @nbytes: size of the data array
 * @opaque: optional application provided data
 *
 * The virStreamSourceFunc callback is used together
 * with the virStreamSendAll function for libvirt to
 * obtain the data that is to be sent.
 *
 * The callback will be invoked multiple times,
 * fetching data in small chunks. The application
 * should fill the 'data' array with up to 'nbytes'
 * of data and then return the number actual number
 * of bytes. The callback will continue to be
 * invoked until it indicates the end of the source
 * has been reached by returning 0. A return value
 * of -1 at any time will abort the send operation
 *
 * Returns the number of bytes filled, 0 upon end
 * of file, or -1 upon error
 */
typedef int (*virStreamSourceFunc)(virStreamPtr st,
                                   char *data,
                                   size_t nbytes,
                                   void *opaque);

int virStreamSendAll(virStreamPtr st,
                     virStreamSourceFunc handler,
                     void *opaque);

/**
 * virStreamSinkFunc:
 *
 * @st: the stream object
 * @data: preallocated array to be filled with data
 * @nbytes: size of the data array
 * @opaque: optional application provided data
 *
 * The virStreamSinkFunc callback is used together
 * with the virStreamRecvAll function for libvirt to
 * provide the data that has been received.
 *
 * The callback will be invoked multiple times,
 * providing data in small chunks. The application
 * should consume up 'nbytes' from the 'data' array
 * of data and then return the number actual number
 * of bytes consumed. The callback will continue to be
 * invoked until it indicates the end of the stream
 * has been reached. A return value of -1 at any time
 * will abort the receive operation
 *
 * Returns the number of bytes consumed or -1 upon
 * error
 */
typedef int (*virStreamSinkFunc)(virStreamPtr st,
                                 const char *data,
                                 size_t nbytes,
                                 void *opaque);

int virStreamRecvAll(virStreamPtr st,
                     virStreamSinkFunc handler,
                     void *opaque);

typedef enum {
    VIR_STREAM_EVENT_READABLE  = (1 << 0),
    VIR_STREAM_EVENT_WRITABLE  = (1 << 1),
    VIR_STREAM_EVENT_ERROR     = (1 << 2),
    VIR_STREAM_EVENT_HANGUP    = (1 << 3),
} virStreamEventType;


/**
 * virStreamEventCallback:
 *
 * @stream: stream on which the event occurred
 * @events: bitset of events from virEventHandleType constants
 * @opaque: user data registered with handle
 *
 * Callback for receiving stream events. The callback will
 * be invoked once for each event which is pending.
 */
typedef void (*virStreamEventCallback)(virStreamPtr stream, int events, void *opaque);

int virStreamEventAddCallback(virStreamPtr stream,
                              int events,
                              virStreamEventCallback cb,
                              void *opaque,
                              virFreeCallback ff);

int virStreamEventUpdateCallback(virStreamPtr stream,
                                 int events);

int virStreamEventRemoveCallback(virStreamPtr stream);


int virStreamFinish(virStreamPtr st);
int virStreamAbort(virStreamPtr st);

int virStreamFree(virStreamPtr st);


int virDomainIsActive(virDomainPtr dom);
int virDomainIsPersistent(virDomainPtr dom);
int virDomainIsUpdated(virDomainPtr dom);

int virNetworkIsActive(virNetworkPtr net);
int virNetworkIsPersistent(virNetworkPtr net);

int virStoragePoolIsActive(virStoragePoolPtr pool);
int virStoragePoolIsPersistent(virStoragePoolPtr pool);

int virInterfaceIsActive(virInterfacePtr iface);

int virConnectIsEncrypted(virConnectPtr conn);
int virConnectIsSecure(virConnectPtr conn);
int virConnectIsAlive(virConnectPtr conn);

/*
 * CPU specification API
 */

typedef enum {
    VIR_CPU_COMPARE_ERROR           = -1,
    VIR_CPU_COMPARE_INCOMPATIBLE    = 0,
    VIR_CPU_COMPARE_IDENTICAL       = 1,
    VIR_CPU_COMPARE_SUPERSET        = 2,

#ifdef VIR_ENUM_SENTINELS
    VIR_CPU_COMPARE_LAST
#endif
} virCPUCompareResult;

/**
 * virConnectCompareCPU:
 *
 * @conn: virConnect connection
 * @xmlDesc: XML description of either guest or host cpu with <cpu> root tag
 * @flags: comparison flags
 *
 * Compares given CPU with host cpu.
 *
 * Returns virCPUCompareResult.
 */
int virConnectCompareCPU(virConnectPtr conn,
                         const char *xmlDesc,
                         unsigned int flags);

int virConnectGetCPUModelNames(virConnectPtr conn,
                               const char *arch,
                               char ***models,
                               unsigned int flags);

/**
 * virConnectBaselineCPUFlags
 *
 * Flags when getting XML description of a computed CPU
 */
typedef enum {
    VIR_CONNECT_BASELINE_CPU_EXPAND_FEATURES  = (1 << 0),  /* show all features */
} virConnectBaselineCPUFlags;

/**
 * virConnectBaselineCPU:
 *
 * @conn: virConnect connection
 * @ncpus: number of CPUs in xmlCPUs
 * @xmlCPUs: array of XML descriptions of host CPUs
 * @flags: fine-tuning flags
 *
 * Computes the most feature-rich CPU which is compatible with all given
 * host CPUs.
 *
 * Returns XML description of the computed CPU or NULL on error.
 */
char *virConnectBaselineCPU(virConnectPtr conn,
                            const char **xmlCPUs,
                            unsigned int ncpus,
                            unsigned int flags);

typedef enum {
    VIR_DOMAIN_JOB_NONE      = 0, /* No job is active */
    VIR_DOMAIN_JOB_BOUNDED   = 1, /* Job with a finite completion time */
    VIR_DOMAIN_JOB_UNBOUNDED = 2, /* Job without a finite completion time */
    VIR_DOMAIN_JOB_COMPLETED = 3, /* Job has finished, but isn't cleaned up */
    VIR_DOMAIN_JOB_FAILED    = 4, /* Job hit error, but isn't cleaned up */
    VIR_DOMAIN_JOB_CANCELLED = 5, /* Job was aborted, but isn't cleaned up */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_JOB_LAST
#endif
} virDomainJobType;

typedef struct _virDomainJobInfo virDomainJobInfo;
typedef virDomainJobInfo *virDomainJobInfoPtr;
struct _virDomainJobInfo {
    /* One of virDomainJobType */
    int type;

    /* Time is measured in milliseconds */
    unsigned long long timeElapsed;    /* Always set */
    unsigned long long timeRemaining;  /* Only for VIR_DOMAIN_JOB_BOUNDED */

    /* Data is measured in bytes unless otherwise specified
     * and is measuring the job as a whole.
     *
     * For VIR_DOMAIN_JOB_UNBOUNDED, dataTotal may be less
     * than the final sum of dataProcessed + dataRemaining
     * in the event that the hypervisor has to repeat some
     * data, such as due to dirtied pages during migration.
     *
     * For VIR_DOMAIN_JOB_BOUNDED, dataTotal shall always
     * equal the sum of dataProcessed + dataRemaining.
     */
    unsigned long long dataTotal;
    unsigned long long dataProcessed;
    unsigned long long dataRemaining;

    /* As above, but only tracking guest memory progress */
    unsigned long long memTotal;
    unsigned long long memProcessed;
    unsigned long long memRemaining;

    /* As above, but only tracking guest disk file progress */
    unsigned long long fileTotal;
    unsigned long long fileProcessed;
    unsigned long long fileRemaining;
};

int virDomainGetJobInfo(virDomainPtr dom,
                        virDomainJobInfoPtr info);
int virDomainGetJobStats(virDomainPtr domain,
                         int *type,
                         virTypedParameterPtr *params,
                         int *nparams,
                         unsigned int flags);
int virDomainAbortJob(virDomainPtr dom);

/**
 * VIR_DOMAIN_JOB_TIME_ELAPSED:
 *
 * virDomainGetJobStats field: time (ms) since the beginning of the
 * job, as VIR_TYPED_PARAM_ULLONG.
 *
 * This field corresponds to timeElapsed field in virDomainJobInfo.
 */
#define VIR_DOMAIN_JOB_TIME_ELAPSED             "time_elapsed"

/**
 * VIR_DOMAIN_JOB_TIME_REMAINING:
 *
 * virDomainGetJobStats field: remaining time (ms) for VIR_DOMAIN_JOB_BOUNDED
 * jobs, as VIR_TYPED_PARAM_ULLONG.
 *
 * This field corresponds to timeRemaining field in virDomainJobInfo.
 */
#define VIR_DOMAIN_JOB_TIME_REMAINING           "time_remaining"

/**
 * VIR_DOMAIN_JOB_DOWNTIME:
 *
 * virDomainGetJobStats field: downtime (ms) that is expected to happen
 * during migration, as VIR_TYPED_PARAM_ULLONG.
 */
#define VIR_DOMAIN_JOB_DOWNTIME                 "downtime"

/**
 * VIR_DOMAIN_JOB_DATA_TOTAL:
 *
 * virDomainGetJobStats field: total number of bytes supposed to be
 * transferred, as VIR_TYPED_PARAM_ULLONG. For VIR_DOMAIN_JOB_UNBOUNDED
 * jobs, this may be less than the sum of VIR_DOMAIN_JOB_DATA_PROCESSED and
 * VIR_DOMAIN_JOB_DATA_REMAINING in the event that the hypervisor has to
 * repeat some data, e.g., due to dirtied pages during migration. For
 * VIR_DOMAIN_JOB_BOUNDED jobs, VIR_DOMAIN_JOB_DATA_TOTAL shall always equal
 * VIR_DOMAIN_JOB_DATA_PROCESSED + VIR_DOMAIN_JOB_DATA_REMAINING.
 *
 * This field corresponds to dataTotal field in virDomainJobInfo.
 */
#define VIR_DOMAIN_JOB_DATA_TOTAL               "data_total"

/**
 * VIR_DOMAIN_JOB_DATA_PROCESSED:
 *
 * virDomainGetJobStats field: number of bytes transferred from the
 * beginning of the job, as VIR_TYPED_PARAM_ULLONG.
 *
 * This field corresponds to dataProcessed field in virDomainJobInfo.
 */
#define VIR_DOMAIN_JOB_DATA_PROCESSED           "data_processed"

/**
 * VIR_DOMAIN_JOB_DATA_REMAINING:
 *
 * virDomainGetJobStats field: number of bytes that still need to be
 * transferred, as VIR_TYPED_PARAM_ULLONG.
 *
 * This field corresponds to dataRemaining field in virDomainJobInfo.
 */
#define VIR_DOMAIN_JOB_DATA_REMAINING           "data_remaining"

/**
 * VIR_DOMAIN_JOB_MEMORY_TOTAL:
 *
 * virDomainGetJobStats field: as VIR_DOMAIN_JOB_DATA_TOTAL but only
 * tracking guest memory progress, as VIR_TYPED_PARAM_ULLONG.
 *
 * This field corresponds to memTotal field in virDomainJobInfo.
 */
#define VIR_DOMAIN_JOB_MEMORY_TOTAL             "memory_total"

/**
 * VIR_DOMAIN_JOB_MEMORY_PROCESSED:
 *
 * virDomainGetJobStats field: as VIR_DOMAIN_JOB_DATA_PROCESSED but only
 * tracking guest memory progress, as VIR_TYPED_PARAM_ULLONG.
 *
 * This field corresponds to memProcessed field in virDomainJobInfo.
 */
#define VIR_DOMAIN_JOB_MEMORY_PROCESSED         "memory_processed"

/**
 * VIR_DOMAIN_JOB_MEMORY_REMAINING:
 *
 * virDomainGetJobStats field: as VIR_DOMAIN_JOB_DATA_REMAINING but only
 * tracking guest memory progress, as VIR_TYPED_PARAM_ULLONG.
 *
 * This field corresponds to memRemaining field in virDomainJobInfo.
 */
#define VIR_DOMAIN_JOB_MEMORY_REMAINING         "memory_remaining"

/**
 * VIR_DOMAIN_JOB_MEMORY_CONSTANT:
 *
 * virDomainGetJobStats field: number of pages filled with a constant
 * byte (all bytes in a single page are identical) transferred since the
 * beginning of the migration job, as VIR_TYPED_PARAM_ULLONG.
 *
 * The most common example of such pages are zero pages, i.e., pages filled
 * with zero bytes.
 */
#define VIR_DOMAIN_JOB_MEMORY_CONSTANT          "memory_constant"

/**
 * VIR_DOMAIN_JOB_MEMORY_NORMAL:
 *
 * virDomainGetJobStats field: number of pages that were transferred without
 * any kind of compression (i.e., pages which were not filled with a constant
 * byte and which could not be compressed) transferred since the beginning
 * of the migration job, as VIR_TYPED_PARAM_ULLONG.
 */
#define VIR_DOMAIN_JOB_MEMORY_NORMAL            "memory_normal"

/**
 * VIR_DOMAIN_JOB_MEMORY_NORMAL_BYTES:
 *
 * virDomainGetJobStats field: number of bytes transferred as normal pages,
 * as VIR_TYPED_PARAM_ULLONG.
 *
 * See VIR_DOMAIN_JOB_MEMORY_NORMAL for more details.
 */
#define VIR_DOMAIN_JOB_MEMORY_NORMAL_BYTES      "memory_normal_bytes"

/**
 * VIR_DOMAIN_JOB_DISK_TOTAL:
 *
 * virDomainGetJobStats field: as VIR_DOMAIN_JOB_DATA_TOTAL but only
 * tracking guest disk progress, as VIR_TYPED_PARAM_ULLONG.
 *
 * This field corresponds to fileTotal field in virDomainJobInfo.
 */
#define VIR_DOMAIN_JOB_DISK_TOTAL               "disk_total"

/**
 * VIR_DOMAIN_JOB_DISK_PROCESSED:
 *
 * virDomainGetJobStats field: as VIR_DOMAIN_JOB_DATA_PROCESSED but only
 * tracking guest disk progress, as VIR_TYPED_PARAM_ULLONG.
 *
 * This field corresponds to fileProcessed field in virDomainJobInfo.
 */
#define VIR_DOMAIN_JOB_DISK_PROCESSED           "disk_processed"

/**
 * VIR_DOMAIN_JOB_DISK_REMAINING:
 *
 * virDomainGetJobStats field: as VIR_DOMAIN_JOB_DATA_REMAINING but only
 * tracking guest disk progress, as VIR_TYPED_PARAM_ULLONG.
 *
 * This field corresponds to fileRemaining field in virDomainJobInfo.
 */
#define VIR_DOMAIN_JOB_DISK_REMAINING           "disk_remaining"

/**
 * VIR_DOMAIN_JOB_COMPRESSION_CACHE:
 *
 * virDomainGetJobStats field: size of the cache (in bytes) used for
 * compressing repeatedly transferred memory pages during live migration,
 * as VIR_TYPED_PARAM_ULLONG.
 */
#define VIR_DOMAIN_JOB_COMPRESSION_CACHE        "compression_cache"

/**
 * VIR_DOMAIN_JOB_COMPRESSION_BYTES:
 *
 * virDomainGetJobStats field: number of compressed bytes transferred
 * since the beginning of migration, as VIR_TYPED_PARAM_ULLONG.
 */
#define VIR_DOMAIN_JOB_COMPRESSION_BYTES        "compression_bytes"

/**
 * VIR_DOMAIN_JOB_COMPRESSION_PAGES:
 *
 * virDomainGetJobStats field: number of compressed pages transferred
 * since the beginning of migration, as VIR_TYPED_PARAM_ULLONG.
 */
#define VIR_DOMAIN_JOB_COMPRESSION_PAGES        "compression_pages"

/**
 * VIR_DOMAIN_JOB_COMPRESSION_CACHE_MISSES:
 *
 * virDomainGetJobStats field: number of repeatedly changing pages that
 * were not found in compression cache and thus could not be compressed,
 * as VIR_TYPED_PARAM_ULLONG.
 */
#define VIR_DOMAIN_JOB_COMPRESSION_CACHE_MISSES "compression_cache_misses"

/**
 * VIR_DOMAIN_JOB_COMPRESSION_OVERFLOW:
 *
 * virDomainGetJobStats field: number of repeatedly changing pages that
 * were found in compression cache but were sent uncompressed because
 * the result of compression was larger than the original page as a whole,
 * as VIR_TYPED_PARAM_ULLONG.
 */
#define VIR_DOMAIN_JOB_COMPRESSION_OVERFLOW     "compression_overflow"


/**
 * virDomainSnapshot:
 *
 * a virDomainSnapshot is a private structure representing a snapshot of
 * a domain.
 */
typedef struct _virDomainSnapshot virDomainSnapshot;

/**
 * virDomainSnapshotPtr:
 *
 * a virDomainSnapshotPtr is pointer to a virDomainSnapshot private structure,
 * and is the type used to reference a domain snapshot in the API.
 */
typedef virDomainSnapshot *virDomainSnapshotPtr;

const char *virDomainSnapshotGetName(virDomainSnapshotPtr snapshot);
virDomainPtr virDomainSnapshotGetDomain(virDomainSnapshotPtr snapshot);
virConnectPtr virDomainSnapshotGetConnect(virDomainSnapshotPtr snapshot);

typedef enum {
    VIR_DOMAIN_SNAPSHOT_CREATE_REDEFINE    = (1 << 0), /* Restore or alter
                                                          metadata */
    VIR_DOMAIN_SNAPSHOT_CREATE_CURRENT     = (1 << 1), /* With redefine, make
                                                          snapshot current */
    VIR_DOMAIN_SNAPSHOT_CREATE_NO_METADATA = (1 << 2), /* Make snapshot without
                                                          remembering it */
    VIR_DOMAIN_SNAPSHOT_CREATE_HALT        = (1 << 3), /* Stop running guest
                                                          after snapshot */
    VIR_DOMAIN_SNAPSHOT_CREATE_DISK_ONLY   = (1 << 4), /* disk snapshot, not
                                                          system checkpoint */
    VIR_DOMAIN_SNAPSHOT_CREATE_REUSE_EXT   = (1 << 5), /* reuse any existing
                                                          external files */
    VIR_DOMAIN_SNAPSHOT_CREATE_QUIESCE     = (1 << 6), /* use guest agent to
                                                          quiesce all mounted
                                                          file systems within
                                                          the domain */
    VIR_DOMAIN_SNAPSHOT_CREATE_ATOMIC      = (1 << 7), /* atomically avoid
                                                          partial changes */
    VIR_DOMAIN_SNAPSHOT_CREATE_LIVE        = (1 << 8), /* create the snapshot
                                                          while the guest is
                                                          running */
} virDomainSnapshotCreateFlags;

/* Take a snapshot of the current VM state */
virDomainSnapshotPtr virDomainSnapshotCreateXML(virDomainPtr domain,
                                                const char *xmlDesc,
                                                unsigned int flags);

/* Dump the XML of a snapshot */
char *virDomainSnapshotGetXMLDesc(virDomainSnapshotPtr snapshot,
                                  unsigned int flags);

/**
 * virDomainSnapshotListFlags:
 *
 * Flags valid for virDomainSnapshotNum(),
 * virDomainSnapshotListNames(), virDomainSnapshotNumChildren(), and
 * virDomainSnapshotListChildrenNames(), virDomainListAllSnapshots(),
 * and virDomainSnapshotListAllChildren().  Note that the interpretation
 * of flag (1<<0) depends on which function it is passed to; but serves
 * to toggle the per-call default of whether the listing is shallow or
 * recursive.  Remaining bits come in groups; if all bits from a group are
 * 0, then that group is not used to filter results.  */
typedef enum {
    VIR_DOMAIN_SNAPSHOT_LIST_ROOTS       = (1 << 0), /* Filter by snapshots
                                                        with no parents, when
                                                        listing a domain */
    VIR_DOMAIN_SNAPSHOT_LIST_DESCENDANTS = (1 << 0), /* List all descendants,
                                                        not just children, when
                                                        listing a snapshot */

    /* For historical reasons, groups do not use contiguous bits.  */

    VIR_DOMAIN_SNAPSHOT_LIST_LEAVES      = (1 << 2), /* Filter by snapshots
                                                        with no children */
    VIR_DOMAIN_SNAPSHOT_LIST_NO_LEAVES   = (1 << 3), /* Filter by snapshots
                                                        that have children */

    VIR_DOMAIN_SNAPSHOT_LIST_METADATA    = (1 << 1), /* Filter by snapshots
                                                        which have metadata */
    VIR_DOMAIN_SNAPSHOT_LIST_NO_METADATA = (1 << 4), /* Filter by snapshots
                                                        with no metadata */

    VIR_DOMAIN_SNAPSHOT_LIST_INACTIVE    = (1 << 5), /* Filter by snapshots
                                                        taken while guest was
                                                        shut off */
    VIR_DOMAIN_SNAPSHOT_LIST_ACTIVE      = (1 << 6), /* Filter by snapshots
                                                        taken while guest was
                                                        active, and with
                                                        memory state */
    VIR_DOMAIN_SNAPSHOT_LIST_DISK_ONLY   = (1 << 7), /* Filter by snapshots
                                                        taken while guest was
                                                        active, but without
                                                        memory state */

    VIR_DOMAIN_SNAPSHOT_LIST_INTERNAL    = (1 << 8), /* Filter by snapshots
                                                        stored internal to
                                                        disk images */
    VIR_DOMAIN_SNAPSHOT_LIST_EXTERNAL    = (1 << 9), /* Filter by snapshots
                                                        that use files external
                                                        to disk images */
} virDomainSnapshotListFlags;

/* Return the number of snapshots for this domain */
int virDomainSnapshotNum(virDomainPtr domain, unsigned int flags);

/* Get the names of all snapshots for this domain */
int virDomainSnapshotListNames(virDomainPtr domain, char **names, int nameslen,
                               unsigned int flags);

/* Get all snapshot objects for this domain */
int virDomainListAllSnapshots(virDomainPtr domain,
                              virDomainSnapshotPtr **snaps,
                              unsigned int flags);

/* Return the number of child snapshots for this snapshot */
int virDomainSnapshotNumChildren(virDomainSnapshotPtr snapshot,
                                 unsigned int flags);

/* Get the names of all child snapshots for this snapshot */
int virDomainSnapshotListChildrenNames(virDomainSnapshotPtr snapshot,
                                       char **names, int nameslen,
                                       unsigned int flags);

/* Get all snapshot object children for this snapshot */
int virDomainSnapshotListAllChildren(virDomainSnapshotPtr snapshot,
                                     virDomainSnapshotPtr **snaps,
                                     unsigned int flags);

/* Get a handle to a named snapshot */
virDomainSnapshotPtr virDomainSnapshotLookupByName(virDomainPtr domain,
                                                   const char *name,
                                                   unsigned int flags);

/* Check whether a domain has a snapshot which is currently used */
int virDomainHasCurrentSnapshot(virDomainPtr domain, unsigned int flags);

/* Get a handle to the current snapshot */
virDomainSnapshotPtr virDomainSnapshotCurrent(virDomainPtr domain,
                                              unsigned int flags);

/* Get a handle to the parent snapshot, if one exists */
virDomainSnapshotPtr virDomainSnapshotGetParent(virDomainSnapshotPtr snapshot,
                                                unsigned int flags);

/* Determine if a snapshot is the current snapshot of its domain.  */
int virDomainSnapshotIsCurrent(virDomainSnapshotPtr snapshot,
                               unsigned int flags);

/* Determine if a snapshot has associated libvirt metadata that would
 * prevent the deletion of its domain.  */
int virDomainSnapshotHasMetadata(virDomainSnapshotPtr snapshot,
                                 unsigned int flags);

typedef enum {
    VIR_DOMAIN_SNAPSHOT_REVERT_RUNNING = 1 << 0, /* Run after revert */
    VIR_DOMAIN_SNAPSHOT_REVERT_PAUSED  = 1 << 1, /* Pause after revert */
    VIR_DOMAIN_SNAPSHOT_REVERT_FORCE   = 1 << 2, /* Allow risky reverts */
} virDomainSnapshotRevertFlags;

/* Revert the domain to a point-in-time snapshot.  The
 * state of the guest after this call will be the state
 * of the guest when the snapshot in question was taken
 */
int virDomainRevertToSnapshot(virDomainSnapshotPtr snapshot,
                              unsigned int flags);

/* Delete a snapshot */
typedef enum {
    VIR_DOMAIN_SNAPSHOT_DELETE_CHILDREN      = (1 << 0), /* Also delete children */
    VIR_DOMAIN_SNAPSHOT_DELETE_METADATA_ONLY = (1 << 1), /* Delete just metadata */
    VIR_DOMAIN_SNAPSHOT_DELETE_CHILDREN_ONLY = (1 << 2), /* Delete just children */
} virDomainSnapshotDeleteFlags;

int virDomainSnapshotDelete(virDomainSnapshotPtr snapshot,
                            unsigned int flags);

int virDomainSnapshotRef(virDomainSnapshotPtr snapshot);
int virDomainSnapshotFree(virDomainSnapshotPtr snapshot);

/**
 * virConnectDomainEventGenericCallback:
 * @conn: the connection pointer
 * @dom: the domain pointer
 * @opaque: application specified data
 *
 * A generic domain event callback handler, for use with
 * virConnectDomainEventRegisterAny(). Specific events usually
 * have a customization with extra parameters, often with @opaque being
 * passed in a different parameter position; use VIR_DOMAIN_EVENT_CALLBACK()
 * when registering an appropriate handler.
 */
typedef void (*virConnectDomainEventGenericCallback)(virConnectPtr conn,
                                                     virDomainPtr dom,
                                                     void *opaque);

/**
 * virConnectDomainEventRTCChangeCallback:
 * @conn: connection object
 * @dom: domain on which the event occurred
 * @utcoffset: the new RTC offset from UTC, measured in seconds
 * @opaque: application specified data
 *
 * The callback signature to use when registering for an event of type
 * VIR_DOMAIN_EVENT_ID_RTC_CHANGE with virConnectDomainEventRegisterAny()
 */
typedef void (*virConnectDomainEventRTCChangeCallback)(virConnectPtr conn,
                                                       virDomainPtr dom,
                                                       long long utcoffset,
                                                       void *opaque);

/**
 * virDomainEventWatchdogAction:
 *
 * The action that is to be taken due to the watchdog device firing
 */
typedef enum {
    VIR_DOMAIN_EVENT_WATCHDOG_NONE = 0, /* No action, watchdog ignored */
    VIR_DOMAIN_EVENT_WATCHDOG_PAUSE,    /* Guest CPUs are paused */
    VIR_DOMAIN_EVENT_WATCHDOG_RESET,    /* Guest CPUs are reset */
    VIR_DOMAIN_EVENT_WATCHDOG_POWEROFF, /* Guest is forcibly powered off */
    VIR_DOMAIN_EVENT_WATCHDOG_SHUTDOWN, /* Guest is requested to gracefully shutdown */
    VIR_DOMAIN_EVENT_WATCHDOG_DEBUG,    /* No action, a debug message logged */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_WATCHDOG_LAST
#endif
} virDomainEventWatchdogAction;

/**
 * virConnectDomainEventWatchdogCallback:
 * @conn: connection object
 * @dom: domain on which the event occurred
 * @action: action that is to be taken due to watchdog firing
 * @opaque: application specified data
 *
 * The callback signature to use when registering for an event of type
 * VIR_DOMAIN_EVENT_ID_WATCHDOG with virConnectDomainEventRegisterAny()
 *
 */
typedef void (*virConnectDomainEventWatchdogCallback)(virConnectPtr conn,
                                                      virDomainPtr dom,
                                                      int action,
                                                      void *opaque);

/**
 * virDomainEventIOErrorAction:
 *
 * The action that is to be taken due to an IO error occurring
 */
typedef enum {
    VIR_DOMAIN_EVENT_IO_ERROR_NONE = 0,  /* No action, IO error ignored */
    VIR_DOMAIN_EVENT_IO_ERROR_PAUSE,     /* Guest CPUs are paused */
    VIR_DOMAIN_EVENT_IO_ERROR_REPORT,    /* IO error reported to guest OS */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_IO_ERROR_LAST
#endif
} virDomainEventIOErrorAction;


/**
 * virConnectDomainEventIOErrorCallback:
 * @conn: connection object
 * @dom: domain on which the event occurred
 * @srcPath: The host file on which the IO error occurred
 * @devAlias: The guest device alias associated with the path
 * @action: action that is to be taken due to the IO error
 * @opaque: application specified data
 *
 * The callback signature to use when registering for an event of type
 * VIR_DOMAIN_EVENT_ID_IO_ERROR with virConnectDomainEventRegisterAny()
 */
typedef void (*virConnectDomainEventIOErrorCallback)(virConnectPtr conn,
                                                     virDomainPtr dom,
                                                     const char *srcPath,
                                                     const char *devAlias,
                                                     int action,
                                                     void *opaque);

/**
 * virConnectDomainEventIOErrorReasonCallback:
 * @conn: connection object
 * @dom: domain on which the event occurred
 * @srcPath: The host file on which the IO error occurred
 * @devAlias: The guest device alias associated with the path
 * @action: action that is to be taken due to the IO error
 * @reason: the cause of the IO error
 * @opaque: application specified data
 *
 * The callback signature to use when registering for an event of type
 * VIR_DOMAIN_EVENT_ID_IO_ERROR_REASON with virConnectDomainEventRegisterAny()
 *
 */
typedef void (*virConnectDomainEventIOErrorReasonCallback)(virConnectPtr conn,
                                                           virDomainPtr dom,
                                                           const char *srcPath,
                                                           const char *devAlias,
                                                           int action,
                                                           const char *reason,
                                                           void *opaque);

/**
 * virDomainEventGraphicsPhase:
 *
 * The phase of the graphics client connection
 */
typedef enum {
    VIR_DOMAIN_EVENT_GRAPHICS_CONNECT = 0,  /* Initial socket connection established */
    VIR_DOMAIN_EVENT_GRAPHICS_INITIALIZE,   /* Authentication & setup completed */
    VIR_DOMAIN_EVENT_GRAPHICS_DISCONNECT,   /* Final socket disconnection */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_GRAPHICS_LAST
#endif
} virDomainEventGraphicsPhase;

/**
 * virDomainEventGraphicsAddressType:
 *
 * The type of address for the connection
 */
typedef enum {
    VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_IPV4,  /* IPv4 address */
    VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_IPV6,  /* IPv6 address */
    VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_UNIX,  /* UNIX socket path */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_GRAPHICS_ADDRESS_LAST
#endif
} virDomainEventGraphicsAddressType;


/**
 * virDomainEventGraphicsAddress:
 *
 * The data structure containing connection address details
 *
 */
struct _virDomainEventGraphicsAddress {
    int family;               /* Address family, virDomainEventGraphicsAddressType */
    char *node;               /* Address of node (eg IP address, or UNIX path) */
    char *service;            /* Service name/number (eg TCP port, or NULL) */
};
typedef struct _virDomainEventGraphicsAddress virDomainEventGraphicsAddress;
typedef virDomainEventGraphicsAddress *virDomainEventGraphicsAddressPtr;


/**
 * virDomainEventGraphicsSubjectIdentity:
 *
 * The data structure representing a single identity
 *
 * The types of identity differ according to the authentication scheme,
 * some examples are 'x509dname' and 'saslUsername'.
 */
struct _virDomainEventGraphicsSubjectIdentity {
    char *type;     /* Type of identity */
    char *name;     /* Identity value */
};
typedef struct _virDomainEventGraphicsSubjectIdentity virDomainEventGraphicsSubjectIdentity;
typedef virDomainEventGraphicsSubjectIdentity *virDomainEventGraphicsSubjectIdentityPtr;


/**
 * virDomainEventGraphicsSubject:
 *
 * The data structure representing an authenticated subject
 *
 * A subject will have zero or more identities. The types of
 * identity differ according to the authentication scheme
 */
struct _virDomainEventGraphicsSubject {
    int nidentity;                                /* Number of identities in array*/
    virDomainEventGraphicsSubjectIdentityPtr identities; /* Array of identities for subject */
};
typedef struct _virDomainEventGraphicsSubject virDomainEventGraphicsSubject;
typedef virDomainEventGraphicsSubject *virDomainEventGraphicsSubjectPtr;


/**
 * virConnectDomainEventGraphicsCallback:
 * @conn: connection object
 * @dom: domain on which the event occurred
 * @phase: the phase of the connection
 * @local: the local server address
 * @remote: the remote client address
 * @authScheme: the authentication scheme activated
 * @subject: the authenticated subject (user)
 * @opaque: application specified data
 *
 * The callback signature to use when registering for an event of type
 * VIR_DOMAIN_EVENT_ID_GRAPHICS with virConnectDomainEventRegisterAny()
 */
typedef void (*virConnectDomainEventGraphicsCallback)(virConnectPtr conn,
                                                      virDomainPtr dom,
                                                      int phase,
                                                      const virDomainEventGraphicsAddress *local,
                                                      const virDomainEventGraphicsAddress *remote,
                                                      const char *authScheme,
                                                      const virDomainEventGraphicsSubject *subject,
                                                      void *opaque);

/**
 * virConnectDomainEventBlockJobStatus:
 *
 * The final status of a virDomainBlockPull() or virDomainBlockRebase()
 * operation
 */
typedef enum {
    VIR_DOMAIN_BLOCK_JOB_COMPLETED = 0,
    VIR_DOMAIN_BLOCK_JOB_FAILED = 1,
    VIR_DOMAIN_BLOCK_JOB_CANCELED = 2,
    VIR_DOMAIN_BLOCK_JOB_READY = 3,

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_BLOCK_JOB_LAST
#endif
} virConnectDomainEventBlockJobStatus;

/**
 * virConnectDomainEventBlockJobCallback:
 * @conn: connection object
 * @dom: domain on which the event occurred
 * @disk: fully-qualified filename of the affected disk
 * @type: type of block job (virDomainBlockJobType)
 * @status: final status of the operation (virConnectDomainEventBlockJobStatus)
 * @opaque: application specified data
 *
 * The callback signature to use when registering for an event of type
 * VIR_DOMAIN_EVENT_ID_BLOCK_JOB with virConnectDomainEventRegisterAny()
 */
typedef void (*virConnectDomainEventBlockJobCallback)(virConnectPtr conn,
                                                      virDomainPtr dom,
                                                      const char *disk,
                                                      int type,
                                                      int status,
                                                      void *opaque);

/**
 * virConnectDomainEventDiskChangeReason:
 *
 * The reason describing why this callback is called
 */
typedef enum {
    VIR_DOMAIN_EVENT_DISK_CHANGE_MISSING_ON_START = 0, /* oldSrcPath is set */
    VIR_DOMAIN_EVENT_DISK_DROP_MISSING_ON_START = 1,

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_DISK_CHANGE_LAST
#endif
} virConnectDomainEventDiskChangeReason;

/**
 * virConnectDomainEventDiskChangeCallback:
 * @conn: connection object
 * @dom: domain on which the event occurred
 * @oldSrcPath: old source path
 * @newSrcPath: new source path
 * @devAlias: device alias name
 * @reason: reason why this callback was called; any of
 *          virConnectDomainEventDiskChangeReason
 * @opaque: application specified data
 *
 * This callback occurs when disk gets changed. However,
 * not all @reason will cause both @oldSrcPath and @newSrcPath
 * to be non-NULL. Please see virConnectDomainEventDiskChangeReason
 * for more details.
 *
 * The callback signature to use when registering for an event of type
 * VIR_DOMAIN_EVENT_ID_DISK_CHANGE with virConnectDomainEventRegisterAny()
 */
typedef void (*virConnectDomainEventDiskChangeCallback)(virConnectPtr conn,
                                                        virDomainPtr dom,
                                                        const char *oldSrcPath,
                                                        const char *newSrcPath,
                                                        const char *devAlias,
                                                        int reason,
                                                        void *opaque);

/**
 * virConnectDomainEventTrayChangeReason:
 *
 * The reason describing why the callback was called
 */
typedef enum {
    VIR_DOMAIN_EVENT_TRAY_CHANGE_OPEN = 0,
    VIR_DOMAIN_EVENT_TRAY_CHANGE_CLOSE,

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_TRAY_CHANGE_LAST
#endif
} virDomainEventTrayChangeReason;

/**
 * virConnectDomainEventTrayChangeCallback:
 * @conn: connection object
 * @dom: domain on which the event occurred
 * @devAlias: device alias
 * @reason: why the tray status was changed?
 * @opaque: application specified data
 *
 * This callback occurs when the tray of a removable device is moved.
 *
 * The callback signature to use when registering for an event of type
 * VIR_DOMAIN_EVENT_ID_TRAY_CHANGE with virConnectDomainEventRegisterAny()
 */
typedef void (*virConnectDomainEventTrayChangeCallback)(virConnectPtr conn,
                                                        virDomainPtr dom,
                                                        const char *devAlias,
                                                        int reason,
                                                        void *opaque);

/**
 * virConnectDomainEventPMWakeupCallback:
 * @conn: connection object
 * @dom: domain on which the event occurred
 * @reason: reason why the callback was called, unused currently,
 *          always passes 0
 * @opaque: application specified data
 *
 * This callback occurs when the guest is woken up.
 *
 * The callback signature to use when registering for an event of type
 * VIR_DOMAIN_EVENT_ID_PMWAKEUP with virConnectDomainEventRegisterAny()
 */
typedef void (*virConnectDomainEventPMWakeupCallback)(virConnectPtr conn,
                                                      virDomainPtr dom,
                                                      int reason,
                                                      void *opaque);

/**
 * virConnectDomainEventPMSuspendCallback:
 * @conn: connection object
 * @dom: domain on which the event occurred
 * @reason: reason why the callback was called, unused currently,
 *          always passes 0
 * @opaque: application specified data
 *
 * This callback occurs when the guest is suspended.
 *
 * The callback signature to use when registering for an event of type
 * VIR_DOMAIN_EVENT_ID_PMSUSPEND with virConnectDomainEventRegisterAny()
 */
typedef void (*virConnectDomainEventPMSuspendCallback)(virConnectPtr conn,
                                                       virDomainPtr dom,
                                                       int reason,
                                                       void *opaque);


/**
 * virConnectDomainEventBalloonChangeCallback:
 * @conn: connection object
 * @dom: domain on which the event occurred
 * @actual: the new balloon level measured in kibibytes(blocks of 1024 bytes)
 * @opaque: application specified data
 *
 * The callback signature to use when registering for an event of type
 * VIR_DOMAIN_EVENT_ID_BALLOON_CHANGE with virConnectDomainEventRegisterAny()
 */
typedef void (*virConnectDomainEventBalloonChangeCallback)(virConnectPtr conn,
                                                           virDomainPtr dom,
                                                           unsigned long long actual,
                                                           void *opaque);

/**
 * virConnectDomainEventPMSuspendDiskCallback:
 * @conn: connection object
 * @dom: domain on which the event occurred
 * @reason: reason why the callback was called, unused currently,
 *          always passes 0
 * @opaque: application specified data
 *
 * This callback occurs when the guest is suspended to disk.
 *
 * The callback signature to use when registering for an event of type
 * VIR_DOMAIN_EVENT_ID_PMSUSPEND_DISK with virConnectDomainEventRegisterAny()
 */
typedef void (*virConnectDomainEventPMSuspendDiskCallback)(virConnectPtr conn,
                                                           virDomainPtr dom,
                                                           int reason,
                                                           void *opaque);

/**
 * virConnectDomainEventDeviceRemovedCallback:
 * @conn: connection object
 * @dom: domain on which the event occurred
 * @devAlias: device alias
 * @opaque: application specified data
 *
 * This callback occurs when a device is removed from the domain.
 *
 * The callback signature to use when registering for an event of type
 * VIR_DOMAIN_EVENT_ID_DEVICE_REMOVED with virConnectDomainEventRegisterAny()
 */
typedef void (*virConnectDomainEventDeviceRemovedCallback)(virConnectPtr conn,
                                                           virDomainPtr dom,
                                                           const char *devAlias,
                                                           void *opaque);


/**
 * VIR_DOMAIN_EVENT_CALLBACK:
 *
 * Used to cast the event specific callback into the generic one
 * for use for virConnectDomainEventRegisterAny()
 */
#define VIR_DOMAIN_EVENT_CALLBACK(cb) ((virConnectDomainEventGenericCallback)(cb))


/**
 * virDomainEventID:
 *
 * An enumeration of supported eventId parameters for
 * virConnectDomainEventRegisterAny().  Each event id determines which
 * signature of callback function will be used.
 */
typedef enum {
    VIR_DOMAIN_EVENT_ID_LIFECYCLE = 0,       /* virConnectDomainEventCallback */
    VIR_DOMAIN_EVENT_ID_REBOOT = 1,          /* virConnectDomainEventGenericCallback */
    VIR_DOMAIN_EVENT_ID_RTC_CHANGE = 2,      /* virConnectDomainEventRTCChangeCallback */
    VIR_DOMAIN_EVENT_ID_WATCHDOG = 3,        /* virConnectDomainEventWatchdogCallback */
    VIR_DOMAIN_EVENT_ID_IO_ERROR = 4,        /* virConnectDomainEventIOErrorCallback */
    VIR_DOMAIN_EVENT_ID_GRAPHICS = 5,        /* virConnectDomainEventGraphicsCallback */
    VIR_DOMAIN_EVENT_ID_IO_ERROR_REASON = 6, /* virConnectDomainEventIOErrorReasonCallback */
    VIR_DOMAIN_EVENT_ID_CONTROL_ERROR = 7,   /* virConnectDomainEventGenericCallback */
    VIR_DOMAIN_EVENT_ID_BLOCK_JOB = 8,       /* virConnectDomainEventBlockJobCallback */
    VIR_DOMAIN_EVENT_ID_DISK_CHANGE = 9,     /* virConnectDomainEventDiskChangeCallback */
    VIR_DOMAIN_EVENT_ID_TRAY_CHANGE = 10,    /* virConnectDomainEventTrayChangeCallback */
    VIR_DOMAIN_EVENT_ID_PMWAKEUP = 11,       /* virConnectDomainEventPMWakeupCallback */
    VIR_DOMAIN_EVENT_ID_PMSUSPEND = 12,      /* virConnectDomainEventPMSuspendCallback */
    VIR_DOMAIN_EVENT_ID_BALLOON_CHANGE = 13, /* virConnectDomainEventBalloonChangeCallback */
    VIR_DOMAIN_EVENT_ID_PMSUSPEND_DISK = 14, /* virConnectDomainEventPMSuspendDiskCallback */
    VIR_DOMAIN_EVENT_ID_DEVICE_REMOVED = 15, /* virConnectDomainEventDeviceRemovedCallback */

#ifdef VIR_ENUM_SENTINELS
    VIR_DOMAIN_EVENT_ID_LAST
    /*
     * NB: this enum value will increase over time as new events are
     * added to the libvirt API. It reflects the last event ID supported
     * by this version of the libvirt API.
     */
#endif
} virDomainEventID;


/* Use VIR_DOMAIN_EVENT_CALLBACK() to cast the 'cb' parameter  */
int virConnectDomainEventRegisterAny(virConnectPtr conn,
                                     virDomainPtr dom, /* Optional, to filter */
                                     int eventID,
                                     virConnectDomainEventGenericCallback cb,
                                     void *opaque,
                                     virFreeCallback freecb);

int virConnectDomainEventDeregisterAny(virConnectPtr conn,
                                       int callbackID);

/**
 * virNetworkEventLifecycleType:
 *
 * a virNetworkEventLifecycleType is emitted during network lifecycle events
 */
typedef enum {
    VIR_NETWORK_EVENT_DEFINED = 0,
    VIR_NETWORK_EVENT_UNDEFINED = 1,
    VIR_NETWORK_EVENT_STARTED = 2,
    VIR_NETWORK_EVENT_STOPPED = 3,

#ifdef VIR_ENUM_SENTINELS
    VIR_NETWORK_EVENT_LAST
#endif
} virNetworkEventLifecycleType;

/**
 * virConnectNetworkEventLifecycleCallback:
 * @conn: connection object
 * @net: network on which the event occurred
 * @event: The specific virNetworkEventLifeCycleType which occurred
 * @detail: contains some details on the reason of the event.
 *          It will be 0 for the while.
 * @opaque: application specified data
 *
 * This callback occurs when the network is started or stopped.
 *
 * The callback signature to use when registering for an event of type
 * VIR_NETWORK_EVENT_ID_LIFECYCLE with virConnectNetworkEventRegisterAny()
 */
typedef void (*virConnectNetworkEventLifecycleCallback)(virConnectPtr conn,
                                                        virNetworkPtr net,
                                                        int event,
                                                        int detail,
                                                        void *opaque);

/**
 * VIR_NETWORK_EVENT_CALLBACK:
 *
 * Used to cast the event specific callback into the generic one
 * for use for virConnectNetworkEventRegisterAny()
 */
#define VIR_NETWORK_EVENT_CALLBACK(cb) ((virConnectNetworkEventGenericCallback)(cb))

/**
 * virNetworkEventID:
 *
 * An enumeration of supported eventId parameters for
 * virConnectNetworkEventRegisterAny().  Each event id determines which
 * signature of callback function will be used.
 */
typedef enum {
    VIR_NETWORK_EVENT_ID_LIFECYCLE = 0,       /* virConnectNetworkEventLifecycleCallback */

#ifdef VIR_ENUM_SENTINELS
    VIR_NETWORK_EVENT_ID_LAST
    /*
     * NB: this enum value will increase over time as new events are
     * added to the libvirt API. It reflects the last event ID supported
     * by this version of the libvirt API.
     */
#endif
} virNetworkEventID;

/**
 * virConnectNetworkEventGenericCallback:
 * @conn: the connection pointer
 * @net: the network pointer
 * @opaque: application specified data
 *
 * A generic network event callback handler, for use with
 * virConnectNetworkEventRegisterAny(). Specific events usually
 * have a customization with extra parameters, often with @opaque being
 * passed in a different parameter position; use VIR_NETWORK_EVENT_CALLBACK()
 * when registering an appropriate handler.
 */
typedef void (*virConnectNetworkEventGenericCallback)(virConnectPtr conn,
                                                      virNetworkPtr net,
                                                      void *opaque);

/* Use VIR_NETWORK_EVENT_CALLBACK() to cast the 'cb' parameter  */
int virConnectNetworkEventRegisterAny(virConnectPtr conn,
                                      virNetworkPtr net, /* Optional, to filter */
                                      int eventID,
                                      virConnectNetworkEventGenericCallback cb,
                                      void *opaque,
                                      virFreeCallback freecb);

int virConnectNetworkEventDeregisterAny(virConnectPtr conn,
                                        int callbackID);

/**
 * virNWFilter:
 *
 * a virNWFilter is a private structure representing a network filter
 */
typedef struct _virNWFilter virNWFilter;

/**
 * virNWFilterPtr:
 *
 * a virNWFilterPtr is pointer to a virNWFilter private structure,
 * this is the type used to reference a network filter in the API.
 */
typedef virNWFilter *virNWFilterPtr;


/*
 * List NWFilters
 */
int                     virConnectNumOfNWFilters (virConnectPtr conn);
int                     virConnectListNWFilters  (virConnectPtr conn,
                                                  char **const names,
                                                  int maxnames);
int                     virConnectListAllNWFilters(virConnectPtr conn,
                                                   virNWFilterPtr **filters,
                                                   unsigned int flags);
/*
 * Lookup nwfilter by name or uuid
 */
virNWFilterPtr          virNWFilterLookupByName       (virConnectPtr conn,
                                                       const char *name);
virNWFilterPtr          virNWFilterLookupByUUID       (virConnectPtr conn,
                                                       const unsigned char *uuid);
virNWFilterPtr          virNWFilterLookupByUUIDString (virConnectPtr conn,
                                                       const char *uuid);

/*
 * Define persistent nwfilter
 */
virNWFilterPtr          virNWFilterDefineXML    (virConnectPtr conn,
                                                 const char *xmlDesc);

/*
 * Delete persistent nwfilter
 */
int                     virNWFilterUndefine     (virNWFilterPtr nwfilter);

/*
 * NWFilter destroy/free
 */
int                     virNWFilterRef          (virNWFilterPtr nwfilter);
int                     virNWFilterFree         (virNWFilterPtr nwfilter);

/*
 * NWFilter information
 */
const char*             virNWFilterGetName       (virNWFilterPtr nwfilter);
int                     virNWFilterGetUUID       (virNWFilterPtr nwfilter,
                                                  unsigned char *uuid);
int                     virNWFilterGetUUIDString (virNWFilterPtr nwfilter,
                                                  char *buf);
char *                  virNWFilterGetXMLDesc    (virNWFilterPtr nwfilter,
                                                  unsigned int flags);
/**
 * virDomainConsoleFlags
 *
 * Since 0.9.10
 */
typedef enum {

    VIR_DOMAIN_CONSOLE_FORCE = (1 << 0), /* abort a (possibly) active console
                                            connection to force a new
                                            connection */
    VIR_DOMAIN_CONSOLE_SAFE = (1 << 1), /* check if the console driver supports
                                           safe console operations */
} virDomainConsoleFlags;

int virDomainOpenConsole(virDomainPtr dom,
                         const char *devname,
                         virStreamPtr st,
                         unsigned int flags);

/**
 * virDomainChannelFlags
 *
 * Since 1.0.2
 */
typedef enum {
    VIR_DOMAIN_CHANNEL_FORCE = (1 << 0), /* abort a (possibly) active channel
                                            connection to force a new
                                            connection */
} virDomainChannelFlags;

int virDomainOpenChannel(virDomainPtr dom,
                         const char *name,
                         virStreamPtr st,
                         unsigned int flags);

typedef enum {
    VIR_DOMAIN_OPEN_GRAPHICS_SKIPAUTH = (1 << 0),
} virDomainOpenGraphicsFlags;

int virDomainOpenGraphics(virDomainPtr dom,
                          unsigned int idx,
                          int fd,
                          unsigned int flags);

int virDomainInjectNMI(virDomainPtr domain, unsigned int flags);

int virDomainFSTrim(virDomainPtr dom,
                    const char *mountPoint,
                    unsigned long long minimum,
                    unsigned int flags);

/**
 * virSchedParameterType:
 *
 * A scheduler parameter field type.  Provided for backwards
 * compatibility; virTypedParameterType is the preferred enum since
 * 0.9.2.
 */
typedef enum {
    VIR_DOMAIN_SCHED_FIELD_INT     = VIR_TYPED_PARAM_INT,
    VIR_DOMAIN_SCHED_FIELD_UINT    = VIR_TYPED_PARAM_UINT,
    VIR_DOMAIN_SCHED_FIELD_LLONG   = VIR_TYPED_PARAM_LLONG,
    VIR_DOMAIN_SCHED_FIELD_ULLONG  = VIR_TYPED_PARAM_ULLONG,
    VIR_DOMAIN_SCHED_FIELD_DOUBLE  = VIR_TYPED_PARAM_DOUBLE,
    VIR_DOMAIN_SCHED_FIELD_BOOLEAN = VIR_TYPED_PARAM_BOOLEAN,
} virSchedParameterType;

/**
 * VIR_DOMAIN_SCHED_FIELD_LENGTH:
 *
 * Macro providing the field length of virSchedParameter.  Provided
 * for backwards compatibility; VIR_TYPED_PARAM_FIELD_LENGTH is the
 * preferred value since 0.9.2.
 */
#define VIR_DOMAIN_SCHED_FIELD_LENGTH VIR_TYPED_PARAM_FIELD_LENGTH

/**
 * virSchedParameter:
 *
 * a virSchedParameter is the set of scheduler parameters.
 * Provided for backwards compatibility; virTypedParameter is the
 * preferred alias since 0.9.2.
 */
#define _virSchedParameter _virTypedParameter
typedef struct _virTypedParameter virSchedParameter;

/**
 * virSchedParameterPtr:
 *
 * a virSchedParameterPtr is a pointer to a virSchedParameter structure.
 * Provided for backwards compatibility; virTypedParameterPtr is the
 * preferred alias since 0.9.2.
 */
typedef virSchedParameter *virSchedParameterPtr;

/**
 * virBlkioParameterType:
 *
 * A blkio parameter field type.  Provided for backwards
 * compatibility; virTypedParameterType is the preferred enum since
 * 0.9.2.
 */
typedef enum {
    VIR_DOMAIN_BLKIO_PARAM_INT     = VIR_TYPED_PARAM_INT,
    VIR_DOMAIN_BLKIO_PARAM_UINT    = VIR_TYPED_PARAM_UINT,
    VIR_DOMAIN_BLKIO_PARAM_LLONG   = VIR_TYPED_PARAM_LLONG,
    VIR_DOMAIN_BLKIO_PARAM_ULLONG  = VIR_TYPED_PARAM_ULLONG,
    VIR_DOMAIN_BLKIO_PARAM_DOUBLE  = VIR_TYPED_PARAM_DOUBLE,
    VIR_DOMAIN_BLKIO_PARAM_BOOLEAN = VIR_TYPED_PARAM_BOOLEAN,
} virBlkioParameterType;

/**
 * VIR_DOMAIN_BLKIO_FIELD_LENGTH:
 *
 * Macro providing the field length of virBlkioParameter.  Provided
 * for backwards compatibility; VIR_TYPED_PARAM_FIELD_LENGTH is the
 * preferred value since 0.9.2.
 */
#define VIR_DOMAIN_BLKIO_FIELD_LENGTH VIR_TYPED_PARAM_FIELD_LENGTH

/**
 * virBlkioParameter:
 *
 * a virBlkioParameter is the set of blkio parameters.
 * Provided for backwards compatibility; virTypedParameter is the
 * preferred alias since 0.9.2.
 */
#define _virBlkioParameter _virTypedParameter
typedef struct _virTypedParameter virBlkioParameter;

/**
 * virBlkioParameterPtr:
 *
 * a virBlkioParameterPtr is a pointer to a virBlkioParameter structure.
 * Provided for backwards compatibility; virTypedParameterPtr is the
 * preferred alias since 0.9.2.
 */
typedef virBlkioParameter *virBlkioParameterPtr;

/**
 * virMemoryParameterType:
 *
 * A memory parameter field type.  Provided for backwards
 * compatibility; virTypedParameterType is the preferred enum since
 * 0.9.2.
 */
typedef enum {
    VIR_DOMAIN_MEMORY_PARAM_INT     = VIR_TYPED_PARAM_INT,
    VIR_DOMAIN_MEMORY_PARAM_UINT    = VIR_TYPED_PARAM_UINT,
    VIR_DOMAIN_MEMORY_PARAM_LLONG   = VIR_TYPED_PARAM_LLONG,
    VIR_DOMAIN_MEMORY_PARAM_ULLONG  = VIR_TYPED_PARAM_ULLONG,
    VIR_DOMAIN_MEMORY_PARAM_DOUBLE  = VIR_TYPED_PARAM_DOUBLE,
    VIR_DOMAIN_MEMORY_PARAM_BOOLEAN = VIR_TYPED_PARAM_BOOLEAN,
} virMemoryParameterType;

/**
 * VIR_DOMAIN_MEMORY_FIELD_LENGTH:
 *
 * Macro providing the field length of virMemoryParameter.  Provided
 * for backwards compatibility; VIR_TYPED_PARAM_FIELD_LENGTH is the
 * preferred value since 0.9.2.
 */
#define VIR_DOMAIN_MEMORY_FIELD_LENGTH VIR_TYPED_PARAM_FIELD_LENGTH

/**
 * virMemoryParameter:
 *
 * a virMemoryParameter is the set of scheduler parameters.
 * Provided for backwards compatibility; virTypedParameter is the
 * preferred alias since 0.9.2.
 */
#define _virMemoryParameter _virTypedParameter
typedef struct _virTypedParameter virMemoryParameter;

/**
 * virMemoryParameterPtr:
 *
 * a virMemoryParameterPtr is a pointer to a virMemoryParameter structure.
 * Provided for backwards compatibility; virTypedParameterPtr is the
 * preferred alias since 0.9.2.
 */
typedef virMemoryParameter *virMemoryParameterPtr;

/* Add new interfaces to the appropriate sections earlier in this
 * file; the end of the file is reserved for deprecated names.  */

#ifdef __cplusplus
}
#endif

#endif /* __VIR_VIRLIB_H__ */
