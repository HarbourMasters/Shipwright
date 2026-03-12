#ifdef __EMSCRIPTEN__

#include <stdio.h>
#include <emscripten.h>
#include <emscripten/html5.h>

#include "web_main.h"

static int s_otr_loaded = 0;

extern "C" {

EMSCRIPTEN_KEEPALIVE
void web_otr_loaded(void) {
    s_otr_loaded = 1;
    printf("[Web] OTR files loaded into virtual filesystem.\n");
}

EMSCRIPTEN_KEEPALIVE
int web_get_otr_status(void) {
    return s_otr_loaded;
}

EMSCRIPTEN_KEEPALIVE
void web_save_to_idb(void) {
    emscripten_run_script(
        "if (typeof FS !== 'undefined' && FS.syncfs) {"
        "  FS.syncfs(false, function(err) {"
        "    if (err) console.error('[Web] FS.syncfs save failed:', err);"
        "    else console.log('[Web] FS.syncfs save complete.');"
        "  });"
        "}"
    );
}

EM_JS(void, web_mount_idbfs, (), {
    var dirs = ["/soh/save", "/soh/config"];
    for (var i = 0; i < dirs.length; i++) {
        try { FS.mkdir(dirs[i]); } catch (e) { /* may exist */ }
        FS.mount(IDBFS, {}, dirs[i]);
    }
    FS.syncfs(true, function(err) {
        if (err) console.error("[Web] IDBFS load failed:", err);
        else console.log("[Web] IDBFS loaded.");
    });
});

void web_fs_init(void) {
    // Ensure the /soh directory exists
    EM_ASM(
        try { FS.mkdir('/soh'); } catch(e) { /* exists */ }
    );
    web_mount_idbfs();
    printf("[Web] IDBFS mounts initialized for /soh/save, /soh/config.\n");
}

} // extern "C"

#endif /* __EMSCRIPTEN__ */
