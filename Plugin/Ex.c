/*
 * Example plugin: demonstrates the minimum a plugin author must write.
 * Build as a shared library (see CMakeLists.txt) and load via
 * pdi_load_plugin(ctx, "libexample_plugin.so").
 */
#include "Plugin/Plugin.h"
#include "Plugin/pdi_log.h"

#include <string.h>

static int example_init(PDI_Context* ctx) {
    (void)ctx;
    PDI_LOG_INFO("example_plugin: init()");
    return PDI_OK;
}

static int example_exec(PDI_Context* ctx, const char* op, void* in, void* out) {
    (void)ctx;
    if (!op) return PDI_ERR_BAD_ARG;

    if (strcmp(op, "ping") == 0) {
        if (out) *(const char**)out = "pong";
        return PDI_OK;
    }

    if (strcmp(op, "add") == 0) {
        if (!in || !out) return PDI_ERR_BAD_ARG;
        int* pair = (int*)in;      /* pair[0] + pair[1] */
        int* result = (int*)out;
        *result = pair[0] + pair[1];
        return PDI_OK;
    }

    PDI_LOG_WARN("example_plugin: unknown op '%s'", op);
    return PDI_ERR_NOT_FOUND;
}

static void example_shutdown(PDI_Context* ctx) {
    (void)ctx;
    PDI_LOG_INFO("example_plugin: shutdown()");
}

static PDI_Plugin g_example_plugin = {
    .name         = "example_plugin",
    .version      = "1.0.0",
    .abi_version  = PDI_ABI_VERSION,
    .init         = example_init,
    .exec         = example_exec,
    .shutdown     = example_shutdown
};

/* Exports `pdi_get_plugin()` -- the one symbol the loader looks for. */
PDI_DEFINE_PLUGIN(&g_example_plugin)
