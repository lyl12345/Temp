#pragma once

#include <glib-object.h>
#include <gtkwave.h>

G_BEGIN_DECLS

#define GW_TYPE_VCD_LOADER (gw_vcd_loader_get_type())
G_DECLARE_FINAL_TYPE(GwVcdLoader, gw_vcd_loader, GW, VCD_LOADER, GwLoader)

GwLoader *gw_vcd_loader_new(void);

void gw_vcd_loader_set_vlist_prepack(GwVcdLoader *self, gboolean vlist_prepack);
gboolean gw_vcd_loader_is_vlist_prepack(GwVcdLoader *self);
void gw_vcd_loader_set_vlist_compression_level(GwVcdLoader *self, gint level);
gint gw_vcd_loader_get_vlist_compression_level(GwVcdLoader *self);
void gw_vcd_loader_set_warning_filesize(GwVcdLoader *self, guint warning_filesize);
guint gw_vcd_loader_get_warning_filesize(GwVcdLoader *self);

//直接构造Vcd波形相关接口
void gw_vcd_loader_begin(GwVcdLoader* self);
void gw_vcd_loader_set_timescale(GwVcdLoader* self, GwTime time_scale, GwTimeDimension dimension);
void gw_vcd_loader_scope(GwVcdLoader* self, GwTreeKind kind, const gchar* name);
void gw_vcd_loader_var(GwVcdLoader* self, unsigned int type, int size, const gchar* name);
void gw_vcd_parse_upscope(GwVcdLoader* self);
void gw_vcd_parse_enddefinitions(GwVcdLoader* self);
void gw_vcd_add_time(GwVcdLoader* self, GwTime time);
void gw_vcd_add_signal_action(GwVcdLoader* self, const gchar* name, char action);
GwDumpFile* gw_vcd_loader_end(GwVcdLoader* self);

G_END_DECLS
