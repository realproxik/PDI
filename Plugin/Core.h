#ifndef PDI_CORE_H
#define PDI_CORE_H

#include "plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PDI_MAX_PLUGINS 64

/* Create a new host context. Must be freed with pdi_shutdown(). */
PDI_Context* pdi_init(void);

/* Load a plugin shared library from disk (.so/.dll/.dylib), call its
 * init(), and register it. Returns PDI_OK on success. */
int pdi_load_plugin(PDI_Context* ctx, const char* path);

/* Look up an already-loaded plugin by name. Returns NULL if not found. */
PDI_Plugin* pdi_find_plugin(PDI_Context* ctx, const char* name);

/* Convenience: find plugin by name and call its exec(). */
int pdi_call(PDI_Context* ctx, const char* plugin_name,
             const char* op, void* in, void* out);

/* Call shutdown() on every loaded plugin, unload libraries, free context. */
void pdi_shutdown(PDI_Context* ctx);

#ifdef __cplusplus
}
#endif

#endif /* PDI_CORE_H */
