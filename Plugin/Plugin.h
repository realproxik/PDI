#ifndef PDI_PLUGIN_H
#define PDI_PLUGIN_H

#ifdef __cplusplus
extern "C" {
#endif

#define PDI_ABI_VERSION 1

/* Opaque handle to the host context. Plugins never see its internals. */
typedef struct PDI_Context PDI_Context;

/* Standard return codes */
typedef enum {
    PDI_OK              = 0,
    PDI_ERR_INIT_FAILED  = -1,
    PDI_ERR_EXEC_FAILED  = -2,
    PDI_ERR_BAD_ARG      = -3,
    PDI_ERR_NOT_FOUND    = -4
} PDI_Status;

/*
 * The plugin interface every plugin must implement.
 * Function pointers may be NULL only for `exec` if the plugin is
 * purely event-driven (see pdi_event.h) -- init/shutdown are required.
 */
typedef struct PDI_Plugin {
    const char* name;           /* unique plugin identifier            */
    const char* version;        /* semantic version string             */
    int         abi_version;    /* must match PDI_ABI_VERSION           */

    /* Called once after load. Return PDI_OK to proceed, else abort load. */
    int  (*init)(PDI_Context* ctx);

    /* Optional: direct synchronous invocation by name/data.
       May be NULL if the plugin only responds to events. */
    int  (*exec)(PDI_Context* ctx, const char* op, void* in, void* out);

    /* Called once before unload. Must not fail. */
    void (*shutdown)(PDI_Context* ctx);
} PDI_Plugin;

/*
 * Every plugin shared library MUST export exactly one symbol with
 * this name and signature. This is the single entry point the loader
 * looks up via dlsym/GetProcAddress.
 *
 *     PDI_Plugin* pdi_get_plugin(void);
 */
typedef PDI_Plugin* (*PDI_GetPluginFn)(void);

#define PDI_PLUGIN_ENTRY_SYMBOL "pdi_get_plugin"

/* Macro plugins use to define their entry point with correct linkage/export. */
#if defined(_WIN32)
  #define PDI_EXPORT __declspec(dllexport)
#else
  #define PDI_EXPORT __attribute__((visibility("default")))
#endif

#define PDI_DEFINE_PLUGIN(pluginStructPtr)                \
    PDI_EXPORT PDI_Plugin* pdi_get_plugin(void) {         \
        return (pluginStructPtr);                         \
    }

#ifdef __cplusplus
}
#endif

#endif /* PDI_PLUGIN_H */
