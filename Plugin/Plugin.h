typedef struct {
    const char* name;
    const char* version;
    int (*init)(PDI_Context* ctx);
    int (*exec)(PDI_Context* ctx, void* data);
    void (*shutdown)(PDI_Context* ctx);
} PDI_Plugin;

// Every plugin exposes this symbol
PDI_Plugin* pdi_get_plugin(void);
