// pdi_core.h
PDI_Context* pdi_init(void);
int pdi_load_plugin(PDI_Context* ctx, const char* path);
int pdi_dispatch(PDI_Context* ctx, const char* event_name, void* payload);
void pdi_shutdown(PDI_Context* ctx);
