#ifndef WEB_MAIN_H
#define WEB_MAIN_H

#ifdef __EMSCRIPTEN__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Called from JavaScript when OTR files have been written to the Emscripten FS.
 */
void web_otr_loaded(void);

/**
 * Returns 1 if OTR files are loaded and available, 0 otherwise.
 */
int web_get_otr_status(void);

/**
 * Triggers FS.syncfs() to persist IDBFS data (saves, config) to IndexedDB.
 */
void web_save_to_idb(void);

/**
 * Initializes IDBFS mounts at /soh/save and /soh/config and loads persisted data.
 */
void web_fs_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __EMSCRIPTEN__ */

#endif /* WEB_MAIN_H */
