/*
 * xen_unified.c: Unified Xen driver.
 *
 * Copyright (C) 2007 Red Hat, Inc.
 *
 * See COPYING.LIB for the License of this software
 *
 * Richard W.M. Jones <rjones@redhat.com>
 */

#ifdef WITH_XEN

/* Note:
 *
 * This driver provides a unified interface to the five
 * separate underlying Xen drivers (xen_internal, proxy_internal,
 * xend_internal, xs_internal and xm_internal).  Historically
 * the body of libvirt.c handled the five Xen drivers,
 * and contained Xen-specific code.
 *
 * The interface between Xen drivers and xen_unified is
 * the same as for "ordinary" libvirt drivers (ie. virDriverPtr),
 * however this is just for convenience and may be changed
 * in future.  Libvirt.c should no longer call directly
 * to the five underlying Xen drivers.
 */

#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <xen/dom0_ops.h>

#include "internal.h"

#include "xen_unified.h"

#include "xen_internal.h"
#include "proxy_internal.h"
#include "xend_internal.h"
#include "xs_internal.h"
#include "xm_internal.h"

/* The five Xen drivers below us. */
static virDriverPtr drivers[XEN_UNIFIED_NR_DRIVERS] = {
    &xenHypervisorDriver,
    &xenProxyDriver,
    &xenDaemonDriver,
    &xenStoreDriver,
    &xenXMDriver
};
static const int hypervisor_offset = 0;
static const int proxy_offset = 1;

/**
 * xenUnifiedError:
 * @conn: the connection
 * @error: the error number
 * @info: extra information string
 *
 * Handle an error at the xend daemon interface
 */
static void
xenUnifiedError (virConnectPtr conn, virErrorNumber error, const char *info)
{
    const char *errmsg;

    errmsg = __virErrorMsg (error, info);
    __virRaiseError (conn, NULL, NULL, VIR_FROM_XEN, error, VIR_ERR_ERROR,
                     errmsg, info, NULL, 0, 0, errmsg, info);
}

/*----- Dispatch functions. -----*/

/* These dispatch functions follow the model used historically
 * by libvirt.c -- trying each low-level Xen driver in turn
 * until one succeeds.  However since we know what low-level
 * drivers can perform which functions, it is probably better
 * in future to optimise these dispatch functions to just call
 * the single function (or small number of appropriate functions)
 * in the low level drivers directly.
 */

static int
xenUnifiedOpen (virConnectPtr conn, const char *name, int flags)
{
    int i, j;
    xenUnifiedPrivatePtr priv;

    /* If name == NULL, name == "", or begins with "xen", then it's for us. */
    if (!name || name[0] == '\0')
        name = "Xen";
    if (strncasecmp (name, "Xen", 3) != 0)
        return VIR_DRV_OPEN_DECLINED;

    /* Allocate per-connection private data. */
    priv = malloc (sizeof *priv);
    if (!priv) {
        xenUnifiedError (conn, VIR_ERR_NO_MEMORY, "allocating private data");
        return VIR_DRV_OPEN_ERROR;
    }
    conn->privateData = priv;

    priv->handle = -1;
    priv->xendConfigVersion = -1;
    priv->type = -1;
    priv->len = -1;
    priv->addr = NULL;
    priv->xshandle = NULL;
    priv->proxy = -1;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i) {
        priv->opened[i] = 0;

        /* Ignore proxy for root */
        if (i == proxy_offset && getuid() == 0)
            continue;

        if (drivers[i]->open &&
            drivers[i]->open (conn, name, flags) == VIR_DRV_OPEN_SUCCESS)
            priv->opened[i] = 1;

        /* If as root, then all drivers must succeed.
           If non-root, then only proxy must succeed */
        if (!priv->opened[i] && (getuid() == 0 || i == proxy_offset)) {
            for (j = 0; j < i; ++j)
                if (priv->opened[j]) drivers[j]->close (conn);
            return VIR_DRV_OPEN_ERROR;
        }
    }

    return VIR_DRV_OPEN_SUCCESS;
}

#define GET_PRIVATE(conn) \
    xenUnifiedPrivatePtr priv = (xenUnifiedPrivatePtr) (conn)->privateData

static int
xenUnifiedClose (virConnectPtr conn)
{
    GET_PRIVATE(conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->close)
            (void) drivers[i]->close (conn);

    free (conn->privateData);
    conn->privateData = NULL;

    return 0;
}

static const char *
xenUnifiedType (virConnectPtr conn)
{
    GET_PRIVATE(conn);
    int i;
    const char *ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->type) {
            ret = drivers[i]->type (conn);
            if (ret) return ret;
        }

    return NULL;
}

static int
xenUnifiedVersion (virConnectPtr conn, unsigned long *hvVer)
{
    GET_PRIVATE(conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] &&
            drivers[i]->version &&
            drivers[i]->version (conn, hvVer) == 0)
            return 0;

    return -1;
}

static int
xenUnifiedGetMaxVcpus (virConnectPtr conn, const char *type)
{
    GET_PRIVATE(conn);
    int i;

    if (!type)
        type = "Xen";

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && strcmp (drivers[i]->name, type) == 0)
            return drivers[i]->getMaxVcpus (conn, type);

    return -1;
}

static int
xenUnifiedNodeGetInfo (virConnectPtr conn, virNodeInfoPtr info)
{
    GET_PRIVATE(conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] &&
            drivers[i]->nodeGetInfo &&
            drivers[i]->nodeGetInfo (conn, info) == 0)
            return 0;

    return -1;
}

static char *
xenUnifiedGetCapabilities (virConnectPtr conn)
{
    GET_PRIVATE(conn);
    int i;
    char *ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->getCapabilities) {
            ret = drivers[i]->getCapabilities (conn);
            if (ret) return ret;
        }

    return NULL;
}

static int
xenUnifiedListDomains (virConnectPtr conn, int *ids, int maxids)
{
    GET_PRIVATE(conn);
    int i, ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->listDomains) {
            ret = drivers[i]->listDomains (conn, ids, maxids);
            if (ret >= 0) return ret;
        }

    return -1;
}

static int
xenUnifiedNumOfDomains (virConnectPtr conn)
{
    GET_PRIVATE(conn);
    int i, ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->numOfDomains) {
            ret = drivers[i]->numOfDomains (conn);
            if (ret >= 0) return ret;
        }

    return -1;
}

static virDomainPtr
xenUnifiedDomainCreateLinux (virConnectPtr conn,
                             const char *xmlDesc, unsigned int flags)
{
    GET_PRIVATE(conn);
    int i;
    virDomainPtr ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainCreateLinux) {
            ret = drivers[i]->domainCreateLinux (conn, xmlDesc, flags);
            if (ret) return ret;
        }

    return NULL;
}

static virDomainPtr
xenUnifiedDomainLookupByID (virConnectPtr conn, int id)
{
    GET_PRIVATE(conn);
    int i;
    virDomainPtr ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainLookupByID) {
            ret = drivers[i]->domainLookupByID (conn, id);
            if (ret) return ret;
        }

    return NULL;
}

static virDomainPtr
xenUnifiedDomainLookupByUUID (virConnectPtr conn,
                              const unsigned char *uuid)
{
    GET_PRIVATE(conn);
    int i;
    virDomainPtr ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainLookupByUUID) {
            ret = drivers[i]->domainLookupByUUID (conn, uuid);
            if (ret) return ret;
        }

    return NULL;
}

static virDomainPtr
xenUnifiedDomainLookupByName (virConnectPtr conn,
                              const char *name)
{
    GET_PRIVATE(conn);
    int i;
    virDomainPtr ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainLookupByName) {
            ret = drivers[i]->domainLookupByName (conn, name);
            if (ret) return ret;
        }

    return NULL;
}

static int
xenUnifiedDomainSuspend (virDomainPtr dom)
{
    GET_PRIVATE(dom->conn);
    int i;

    /* Try non-hypervisor methods first, then hypervisor direct method
     * as a last resort.
     */
    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (i != hypervisor_offset &&
            priv->opened[i] &&
            drivers[i]->domainSuspend &&
            drivers[i]->domainSuspend (dom) == 0)
            return 0;

    if (priv->opened[hypervisor_offset] &&
        drivers[hypervisor_offset]->domainSuspend &&
        drivers[hypervisor_offset]->domainSuspend (dom) == 0)
        return 0;

    return -1;
}

static int
xenUnifiedDomainResume (virDomainPtr dom)
{
    GET_PRIVATE(dom->conn);
    int i;

    /* Try non-hypervisor methods first, then hypervisor direct method
     * as a last resort.
     */
    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (i != hypervisor_offset &&
            priv->opened[i] &&
            drivers[i]->domainResume &&
            drivers[i]->domainResume (dom) == 0)
            return 0;

    if (priv->opened[hypervisor_offset] &&
        drivers[hypervisor_offset]->domainResume &&
        drivers[hypervisor_offset]->domainResume (dom) == 0)
        return 0;

    return -1;
}

static int
xenUnifiedDomainShutdown (virDomainPtr dom)
{
    GET_PRIVATE(dom->conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] &&
            drivers[i]->domainShutdown &&
            drivers[i]->domainShutdown (dom) == 0)
            return 0;

    return -1;
}

static int
xenUnifiedDomainReboot (virDomainPtr dom, unsigned int flags)
{
    GET_PRIVATE(dom->conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] &&
            drivers[i]->domainReboot &&
            drivers[i]->domainReboot (dom, flags) == 0)
            return 0;

    return -1;
}

static int
xenUnifiedDomainDestroy (virDomainPtr dom)
{
    GET_PRIVATE(dom->conn);
    int i;

    /* Try non-hypervisor methods first, then hypervisor direct method
     * as a last resort.
     */
    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (i != hypervisor_offset &&
            priv->opened[i] &&
            drivers[i]->domainDestroy &&
            drivers[i]->domainDestroy (dom) == 0)
            return 0;

    if (priv->opened[i] &&
        drivers[hypervisor_offset]->domainDestroy &&
        drivers[hypervisor_offset]->domainDestroy (dom) == 0)
        return 0;

    return -1;
}

static char *
xenUnifiedDomainGetOSType (virDomainPtr dom)
{
    GET_PRIVATE(dom->conn);
    int i;
    char *ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainGetOSType) {
            ret = drivers[i]->domainGetOSType (dom);
            if (ret) return ret;
        }

    return NULL;
}

static unsigned long
xenUnifiedDomainGetMaxMemory (virDomainPtr dom)
{
    GET_PRIVATE(dom->conn);
    int i;
    unsigned long ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainGetMaxMemory) {
            ret = drivers[i]->domainGetMaxMemory (dom);
            if (ret != 0) return ret;
        }

    return 0;
}

static int
xenUnifiedDomainSetMaxMemory (virDomainPtr dom, unsigned long memory)
{
    GET_PRIVATE(dom->conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] &&
            drivers[i]->domainSetMaxMemory &&
            drivers[i]->domainSetMaxMemory (dom, memory) == 0)
            return 0;

    return -1;
}

static int
xenUnifiedDomainSetMemory (virDomainPtr dom, unsigned long memory)
{
    GET_PRIVATE(dom->conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] &&
            drivers[i]->domainSetMemory &&
            drivers[i]->domainSetMemory (dom, memory) == 0)
            return 0;

    return -1;
}

static int
xenUnifiedDomainGetInfo (virDomainPtr dom, virDomainInfoPtr info)
{
    GET_PRIVATE(dom->conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] &&
            drivers[i]->domainGetInfo &&
            drivers[i]->domainGetInfo (dom, info) == 0)
            return 0;

    return -1;
}

static int
xenUnifiedDomainSave (virDomainPtr dom, const char *to)
{
    GET_PRIVATE(dom->conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] &&
            drivers[i]->domainSave &&
            drivers[i]->domainSave (dom, to) == 0)
            return 0;

    return -1;
}

static int
xenUnifiedDomainRestore (virConnectPtr conn, const char *from)
{
    GET_PRIVATE(conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] &&
            drivers[i]->domainRestore &&
            drivers[i]->domainRestore (conn, from) == 0)
            return 0;

    return -1;
}

static int
xenUnifiedDomainCoreDump (virDomainPtr dom, const char *to, int flags)
{
    GET_PRIVATE(dom->conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] &&
            drivers[i]->domainCoreDump &&
            drivers[i]->domainCoreDump (dom, to, flags) == 0)
            return 0;

    return -1;
}

static int
xenUnifiedDomainSetVcpus (virDomainPtr dom, unsigned int nvcpus)
{
    GET_PRIVATE(dom->conn);
    int i;

    /* Try non-hypervisor methods first, then hypervisor direct method
     * as a last resort.
     */
    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (i != hypervisor_offset &&
            priv->opened[i] &&
            drivers[i]->domainSetVcpus &&
            drivers[i]->domainSetVcpus (dom, nvcpus) == 0)
            return 0;

    if (priv->opened[hypervisor_offset] &&
        drivers[hypervisor_offset]->domainSetVcpus &&
        drivers[hypervisor_offset]->domainSetVcpus (dom, nvcpus) == 0)
        return 0;

    return -1;
}

static int
xenUnifiedDomainPinVcpu (virDomainPtr dom, unsigned int vcpu,
                         unsigned char *cpumap, int maplen)
{
    GET_PRIVATE(dom->conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] &&
            drivers[i]->domainPinVcpu &&
            drivers[i]->domainPinVcpu (dom, vcpu, cpumap, maplen) == 0)
            return 0;

    return -1;
}

static int
xenUnifiedDomainGetVcpus (virDomainPtr dom,
                          virVcpuInfoPtr info, int maxinfo,
                          unsigned char *cpumaps, int maplen)
{
    GET_PRIVATE(dom->conn);
    int i, ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainGetVcpus) {
            ret = drivers[i]->domainGetVcpus (dom, info, maxinfo, cpumaps, maplen);
            if (ret > 0)
                return ret;
        }
    return -1;
}

static int
xenUnifiedDomainGetMaxVcpus (virDomainPtr dom)
{
    GET_PRIVATE(dom->conn);
    int i, ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainGetMaxVcpus) {
            ret = drivers[i]->domainGetMaxVcpus (dom);
            if (ret != 0) return ret;
        }

    return -1;
}

static char *
xenUnifiedDomainDumpXML (virDomainPtr dom, int flags)
{
    GET_PRIVATE(dom->conn);
    int i;
    char *ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainDumpXML) {
            ret = drivers[i]->domainDumpXML (dom, flags);
            if (ret) return ret;
        }

    return NULL;
}

static int
xenUnifiedListDefinedDomains (virConnectPtr conn, char **const names,
                              int maxnames)
{
    GET_PRIVATE(conn);
    int i;
    int ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->listDefinedDomains) {
            ret = drivers[i]->listDefinedDomains (conn, names, maxnames);
            if (ret >= 0) return ret;
        }

    return -1;
}

static int
xenUnifiedNumOfDefinedDomains (virConnectPtr conn)
{
    GET_PRIVATE(conn);
    int i;
    int ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->numOfDefinedDomains) {
            ret = drivers[i]->numOfDefinedDomains (conn);
            if (ret >= 0) return ret;
        }

    return -1;
}

static int
xenUnifiedDomainCreate (virDomainPtr dom)
{
    GET_PRIVATE(dom->conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainCreate &&
            drivers[i]->domainCreate (dom) == 0)
            return 0;

    return -1;
}

static virDomainPtr
xenUnifiedDomainDefineXML (virConnectPtr conn, const char *xml)
{
    GET_PRIVATE(conn);
    int i;
    virDomainPtr ret;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainDefineXML) {
            ret = drivers[i]->domainDefineXML (conn, xml);
            if (ret) return ret;
        }

    return NULL;
}

static int
xenUnifiedDomainUndefine (virDomainPtr dom)
{
    GET_PRIVATE(dom->conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainUndefine &&
            drivers[i]->domainUndefine (dom) == 0)
            return 0;

    return -1;
}

static int
xenUnifiedDomainAttachDevice (virDomainPtr dom, char *xml)
{
    GET_PRIVATE(dom->conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainAttachDevice &&
            drivers[i]->domainAttachDevice (dom, xml) == 0)
            return 0;

    return -1;
}

static int
xenUnifiedDomainDetachDevice (virDomainPtr dom, char *xml)
{
    GET_PRIVATE(dom->conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainDetachDevice &&
            drivers[i]->domainDetachDevice (dom, xml) == 0)
            return 0;

    return -1;
}

static int
xenUnifiedDomainGetAutostart (virDomainPtr dom, int *autostart)
{
    GET_PRIVATE(dom->conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainGetAutostart &&
            drivers[i]->domainGetAutostart (dom, autostart) == 0)
            return 0;

    return -1;
}

static int
xenUnifiedDomainSetAutostart (virDomainPtr dom, int autostart)
{
    GET_PRIVATE(dom->conn);
    int i;

    for (i = 0; i < XEN_UNIFIED_NR_DRIVERS; ++i)
        if (priv->opened[i] && drivers[i]->domainSetAutostart &&
            drivers[i]->domainSetAutostart (dom, autostart) == 0)
            return 0;

    return -1;
}

/*----- Register with libvirt.c, and initialise Xen drivers. -----*/

#define VERSION ((DOM0_INTERFACE_VERSION >> 24) * 1000000 +         \
                 ((DOM0_INTERFACE_VERSION >> 16) & 0xFF) * 1000 +	\
                 (DOM0_INTERFACE_VERSION & 0xFFFF))

/* The interface which we export upwards to libvirt.c. */
static virDriver xenUnifiedDriver = {
    .no = VIR_DRV_XEN_UNIFIED,
    .name = "Xen",
    .ver = VERSION,
    .open 			= xenUnifiedOpen,
    .close 			= xenUnifiedClose,
    .type 			= xenUnifiedType,
    .version 			= xenUnifiedVersion,
    .getMaxVcpus 			= xenUnifiedGetMaxVcpus,
    .nodeGetInfo 			= xenUnifiedNodeGetInfo,
    .getCapabilities 		= xenUnifiedGetCapabilities,
    .listDomains 			= xenUnifiedListDomains,
    .numOfDomains 		= xenUnifiedNumOfDomains,
    .domainCreateLinux 		= xenUnifiedDomainCreateLinux,
    .domainLookupByID 		= xenUnifiedDomainLookupByID,
    .domainLookupByUUID 		= xenUnifiedDomainLookupByUUID,
    .domainLookupByName 		= xenUnifiedDomainLookupByName,
    .domainSuspend 		= xenUnifiedDomainSuspend,
    .domainResume 		= xenUnifiedDomainResume,
    .domainShutdown 		= xenUnifiedDomainShutdown,
    .domainReboot 		= xenUnifiedDomainReboot,
    .domainDestroy 		= xenUnifiedDomainDestroy,
    .domainGetOSType 		= xenUnifiedDomainGetOSType,
    .domainGetMaxMemory 		= xenUnifiedDomainGetMaxMemory,
    .domainSetMaxMemory 		= xenUnifiedDomainSetMaxMemory,
    .domainSetMemory 		= xenUnifiedDomainSetMemory,
    .domainGetInfo 		= xenUnifiedDomainGetInfo,
    .domainSave 			= xenUnifiedDomainSave,
    .domainRestore 		= xenUnifiedDomainRestore,
    .domainCoreDump 		= xenUnifiedDomainCoreDump,
    .domainSetVcpus 		= xenUnifiedDomainSetVcpus,
    .domainPinVcpu 		= xenUnifiedDomainPinVcpu,
    .domainGetVcpus 		= xenUnifiedDomainGetVcpus,
    .domainGetMaxVcpus 		= xenUnifiedDomainGetMaxVcpus,
    .domainDumpXML 		= xenUnifiedDomainDumpXML,
    .listDefinedDomains 		= xenUnifiedListDefinedDomains,
    .numOfDefinedDomains 		= xenUnifiedNumOfDefinedDomains,
    .domainCreate 		= xenUnifiedDomainCreate,
    .domainDefineXML 		= xenUnifiedDomainDefineXML,
    .domainUndefine 		= xenUnifiedDomainUndefine,
    .domainAttachDevice 		= xenUnifiedDomainAttachDevice,
    .domainDetachDevice 		= xenUnifiedDomainDetachDevice,
    .domainGetAutostart 		= xenUnifiedDomainGetAutostart,
    .domainSetAutostart 		= xenUnifiedDomainSetAutostart,
};

/**
 * xenUnifiedRegister:
 *
 * Register xen related drivers
 *
 * Returns the driver priority or -1 in case of error.
 */
int
xenUnifiedRegister (void)
{
    /* Ignore failures here. */
    (void) xenHypervisorInit ();
    (void) xenProxyInit ();
    (void) xenDaemonInit ();
    (void) xenStoreInit ();
    (void) xenXMInit ();

    return virRegisterDriver (&xenUnifiedDriver);
}

#endif /* WITH_XEN */

/*
 * vim: set tabstop=4:
 * vim: set shiftwidth=4:
 * vim: set expandtab:
 */
/*
 * Local variables:
 *  indent-tabs-mode: nil
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  tab-width: 4
 * End:
 */
