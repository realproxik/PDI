#include "core.h"
#include "PDI/pdi_log.h"

#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void*       handle;   /* dlopen handle, needed to dlclose later */
    PDI_Plugin* plugin;
} PDI_LoadedPlugin;

struct PDI_Context {
    PDI_LoadedPlugin plugins[PDI_MAX_PLUGINS];
    int               plugin_count;
};

PDI_Context* pdi_init(void) {
    PDI_Context* ctx = (PDI_Context*)calloc(1, sizeof(PDI_Context));
    if (!ctx) {
        PDI_LOG_ERROR("failed to allocate context");
        return NULL;
    }
    PDI_LOG_INFO("context initialized");
    return ctx;
}

int pdi_load_plugin(PDI_Context* ctx, const char* path) {
    if (!ctx || !path) return PDI_ERR_BAD_ARG;

    if (ctx->plugin_count >= PDI_MAX_PLUGINS) {
        PDI_LOG_ERROR("plugin limit reached (%d)", PDI_MAX_PLUGINS);
        return PDI_ERR_EXEC_FAILED;
    }

    void* handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        PDI_LOG_ERROR("dlopen failed for '%s': %s", path, dlerror());
        return PDI_ERR_NOT_FOUND;
    }

    dlerror(); /* clear any existing error */
    PDI_GetPluginFn get_plugin =
        (PDI_GetPluginFn)dlsym(handle, PDI_PLUGIN_ENTRY_SYMBOL);

    const char* dlsym_err = dlerror();
    if (dlsym_err || !get_plugin) {
        PDI_LOG_ERROR("symbol '%s' not found in '%s': %s",
                      PDI_PLUGIN_ENTRY_SYMBOL, path,
                      dlsym_err ? dlsym_err : "unknown");
        dlclose(handle);
        return PDI_ERR_NOT_FOUND;
    }

    PDI_Plugin* plugin = get_plugin();
    if (!plugin) {
        PDI_LOG_ERROR("'%s' returned NULL plugin", path);
        dlclose(handle);
        return PDI_ERR_INIT_FAILED;
    }

    if (plugin->abi_version != PDI_ABI_VERSION) {
        PDI_LOG_ERROR("'%s' ABI mismatch (plugin=%d host=%d)",
                      plugin->name ? plugin->name : path,
                      plugin->abi_version, PDI_ABI_VERSION);
        dlclose(handle);
        return PDI_ERR_INIT_FAILED;
    }

    if (!plugin->init || !plugin->shutdown) {
        PDI_LOG_ERROR("'%s' missing required init/shutdown", plugin->name);
        dlclose(handle);
        return PDI_ERR_INIT_FAILED;
    }

    int rc = plugin->init(ctx);
    if (rc != PDI_OK) {
        PDI_LOG_ERROR("'%s' init() failed with code %d", plugin->name, rc);
        dlclose(handle);
        return PDI_ERR_INIT_FAILED;
    }

    ctx->plugins[ctx->plugin_count].handle = handle;
    ctx->plugins[ctx->plugin_count].plugin = plugin;
    ctx->plugin_count++;

    PDI_LOG_INFO("loaded plugin '%s' v%s", plugin->name, plugin->version);
    return PDI_OK;
}

PDI_Plugin* pdi_find_plugin(PDI_Context* ctx, const char* name) {
    if (!ctx || !name) return NULL;
    for (int i = 0; i < ctx->plugin_count; i++) {
        if (strcmp(ctx->plugins[i].plugin->name, name) == 0) {
            return ctx->plugins[i].plugin;
        }
    }
    return NULL;
}

int pdi_call(PDI_Context* ctx, const char* plugin_name,
             const char* op, void* in, void* out) {
    PDI_Plugin* p = pdi_find_plugin(ctx, plugin_name);
    if (!p) {
        PDI_LOG_ERROR("plugin '%s' not found", plugin_name);
        return PDI_ERR_NOT_FOUND;
    }
    if (!p->exec) {
        PDI_LOG_ERROR("plugin '%s' has no exec()", plugin_name);
        return PDI_ERR_EXEC_FAILED;
    }
    return p->exec(ctx, op, in, out);
}

void pdi_shutdown(PDI_Context* ctx) {
    if (!ctx) return;

    /* Shut down in reverse load order */
    for (int i = ctx->plugin_count - 1; i >= 0; i--) {
        PDI_Plugin* p = ctx->plugins[i].plugin;
        PDI_LOG_INFO("shutting down plugin '%s'", p->name);
        p->shutdown(ctx);
        dlclose(ctx->plugins[i].handle);
    }

    free(ctx);
    PDI_LOG_INFO("context shutdown complete");
}
