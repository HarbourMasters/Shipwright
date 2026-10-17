package com.harbourmasters.shipofharkinian;

import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.OpenableColumns;
import android.widget.Toast;

import org.libsdl.app.SDLActivity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Locale;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/** SDL host activity with app-private asset staging and the Android Storage Access Framework ROM picker. */
public final class ShipActivity extends SDLActivity {
    private static final int ROM_PICKER_REQUEST = 0x534f;
    private static final String ASSET_PREFS = "ship_asset_staging";
    private static final String ASSET_VERSION_KEY = "version_code";

    private static native void nativeOnRomPickerResult(String path);
    private static native void nativeOnMenuToggle();

    private final ExecutorService fileIoExecutor = Executors.newSingleThreadExecutor(runnable -> {
        Thread thread = new Thread(runnable, "SoHAndroidFileIO");
        thread.setDaemon(true);
        return thread;
    });
    private boolean runtimeAssetsReady;
    private boolean activityStarted;
    private Boolean pendingWindowFocus;
    private volatile boolean romPickerPending;
    private ProgressDialog progressDialog;

    @Override
    protected String[] getLibraries() {
        // SDL2_net is a shared CMake dependency and must be loaded before libmain.so.
        return new String[] { "SDL2", "SDL2_net", "main" };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (SDLActivity.mBrokenLibraries) {
            return;
        }

        // Keep Ship's data/config path aligned with the directory used for staged assets and ROM imports.
        SDLActivity.nativeSetenv("SHIP_HOME", getAppFilesDirectory().getAbsolutePath());
        // Android has no writable /tmp for app processes; ROM extraction uses std::filesystem temp paths.
        SDLActivity.nativeSetenv("TMPDIR", getCacheDir().getAbsolutePath());

        try {
            if (areRuntimeAssetsStaged()) {
                runtimeAssetsReady = true;
                return;
            }
        } catch (IOException | PackageManager.NameNotFoundException exception) {
            showAssetStagingFailure(exception);
            return;
        }

        showProgress("Preparing Ship of Harkinian", "Copying game data. This may take a few minutes the first time.");
        fileIoExecutor.execute(() -> {
            Exception failure = null;
            try {
                stageRuntimeAssets();
            } catch (IOException | PackageManager.NameNotFoundException exception) {
                failure = exception;
            }
            Exception result = failure;
            runOnUiThread(() -> finishAssetStaging(result));
        });
    }

    @Override
    protected void onStart() {
        activityStarted = true;
        if (runtimeAssetsReady || SDLActivity.mBrokenLibraries) {
            resumeSdlFromStart();
        } else {
            // Call SDLActivity.onStart so Android's Activity lifecycle is respected, but keep
            // mHasFocus false so SDL's native-state gate cannot start before staging completes.
            SDLActivity.mHasFocus = false;
            super.onStart();
        }
    }

    private void resumeSdlFromStart() {
        Boolean focus = pendingWindowFocus;
        if (focus != null) {
            SDLActivity.mHasFocus = focus;
        }
        super.onStart();
        if (focus != null) {
            pendingWindowFocus = null;
            super.onWindowFocusChanged(focus);
        }
    }

    private void resumeNativeAfterAssetStaging() {
        Boolean focus = pendingWindowFocus;
        if (focus != null) {
            SDLActivity.mHasFocus = focus;
        }
        // onStart already ran; only release SDL's native-thread gate here.
        resumeNativeThread();
        if (focus != null) {
            pendingWindowFocus = null;
            super.onWindowFocusChanged(focus);
        }
    }

    @Override
    protected void onResume() {
        // The base class requests native resume here; while staging is incomplete, mHasFocus stays
        // false and SDL's native-state gate prevents premature execution of SDL_main.
        super.onResume();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        if (runtimeAssetsReady || SDLActivity.mBrokenLibraries) {
            super.onWindowFocusChanged(hasFocus);
        } else {
            pendingWindowFocus = hasFocus;
        }
    }

    @Override
    protected void onStop() {
        activityStarted = false;
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        fileIoExecutor.shutdownNow();
        dismissProgress();
        if (romPickerPending) {
            romPickerPending = false;
            try {
                nativeOnRomPickerResult(null);
            } catch (UnsatisfiedLinkError ignored) {
                // SDL may not have loaded libmain.so if the activity is being closed during startup.
            }
        }
        super.onDestroy();
    }

    private void finishAssetStaging(Exception failure) {
        if (isFinishing() || isDestroyed()) {
            return;
        }
        dismissProgress();
        if (failure != null) {
            showAssetStagingFailure(failure);
            return;
        }

        runtimeAssetsReady = true;
        if (activityStarted) {
            // Start the native thread only if the activity is still in the started state. If Android
            // stopped us during the initial copy, the next real onStart callback will start it instead.
            resumeNativeAfterAssetStaging();
        } else if (pendingWindowFocus != null) {
            SDLActivity.mHasFocus = pendingWindowFocus;
        }
    }

    private void showAssetStagingFailure(Exception exception) {
        dismissProgress();
        new AlertDialog.Builder(this)
                .setTitle("Could not prepare game data")
                .setMessage(exception.getMessage() == null ? exception.toString() : exception.getMessage())
                .setCancelable(false)
                .setPositiveButton("Exit", (dialog, which) -> finish())
                .show();
    }

    private void showProgress(String title, String message) {
        dismissProgress();
        progressDialog = new ProgressDialog(this);
        progressDialog.setTitle(title);
        progressDialog.setMessage(message);
        progressDialog.setIndeterminate(true);
        progressDialog.setCancelable(false);
        progressDialog.show();
    }

    private void dismissProgress() {
        if (progressDialog != null && progressDialog.isShowing()) {
            progressDialog.dismiss();
        }
        progressDialog = null;
    }

    /** Called from the native extractor thread; the picker itself must be launched on Android's UI thread. */
    public void openRomPicker() {
        if (romPickerPending) {
            return;
        }
        // Publish the pending request before posting to the UI thread so onDestroy can always
        // release the native extractor if the activity closes before the picker launch runs.
        romPickerPending = true;
        runOnUiThread(() -> {
            if (!romPickerPending) {
                return;
            }
            if (isFinishing() || isDestroyed()) {
                completeRomPicker(null);
                return;
            }
            Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
            intent.addCategory(Intent.CATEGORY_OPENABLE);
            intent.setType("*/*");
            intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION | Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION);
            try {
                startActivityForResult(intent, ROM_PICKER_REQUEST);
            } catch (RuntimeException exception) {
                Toast.makeText(this, "No document picker is available: " + exception.getMessage(),
                               Toast.LENGTH_LONG).show();
                completeRomPicker(null);
            }
        });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode != ROM_PICKER_REQUEST) {
            return;
        }

        if (resultCode != RESULT_OK || data == null || data.getData() == null) {
            completeRomPicker(null);
            return;
        }

        Uri uri = data.getData();
        int readPermission = data.getFlags() & Intent.FLAG_GRANT_READ_URI_PERMISSION;
        boolean persistablePermission = false;
        if (readPermission != 0 && (data.getFlags() & Intent.FLAG_GRANT_PERSISTABLE_URI_PERMISSION) != 0) {
            try {
                getContentResolver().takePersistableUriPermission(uri, readPermission);
                persistablePermission = true;
            } catch (SecurityException ignored) {
                // The temporary grant is still sufficient while this import is in progress.
            }
        }
        final boolean releasePermission = persistablePermission;

        showProgress("Importing ROM", "Copying the selected file into app storage.");
        fileIoExecutor.execute(() -> {
            String selectedPath = null;
            Exception failure = null;
            try {
                selectedPath = copySelectedRom(uri);
            } catch (IOException | RuntimeException exception) {
                failure = exception;
            } finally {
                if (releasePermission) {
                    try {
                        getContentResolver().releasePersistableUriPermission(uri, readPermission);
                    } catch (SecurityException ignored) {
                        // A provider may have revoked the grant after the import completed.
                    }
                }
            }
            final String resultPath = selectedPath;
            final Exception resultFailure = failure;
            runOnUiThread(() -> {
                if (resultFailure != null) {
                    Toast.makeText(this, "Could not read the selected ROM: " + resultFailure.getMessage(),
                                   Toast.LENGTH_LONG).show();
                }
                completeRomPicker(resultPath);
            });
        });
    }

    private void completeRomPicker(String path) {
        if (!romPickerPending) {
            return;
        }
        dismissProgress();
        romPickerPending = false;
        nativeOnRomPickerResult(path);
    }

    @Override
    public void onBackPressed() {
        // Let Back cancel asset preparation; once the game is ready it toggles the in-game menu.
        if (!runtimeAssetsReady) {
            super.onBackPressed();
            return;
        }
        try {
            nativeOnMenuToggle();
        } catch (UnsatisfiedLinkError error) {
            super.onBackPressed();
        }
    }

    private File getAppFilesDirectory() {
        File appDirectory = getExternalFilesDir(null);
        return appDirectory == null ? getFilesDir() : appDirectory;
    }

    private long getInstalledVersionCode() throws PackageManager.NameNotFoundException {
        PackageInfo packageInfo = getPackageManager().getPackageInfo(getPackageName(), 0);
        return Build.VERSION.SDK_INT >= Build.VERSION_CODES.P ? packageInfo.getLongVersionCode() : packageInfo.versionCode;
    }

    private boolean areRuntimeAssetsStaged() throws IOException, PackageManager.NameNotFoundException {
        File appDirectory = getAppFilesDirectory();
        File assetDirectory = new File(appDirectory, "assets");
        File portArchive = new File(appDirectory, "soh.o2r");
        SharedPreferences preferences = getSharedPreferences(ASSET_PREFS, MODE_PRIVATE);
        return preferences.getLong(ASSET_VERSION_KEY, -1L) == getInstalledVersionCode()
                && new File(assetDirectory, "config.yml").isFile()
                && portArchive.isFile()
                && portArchive.length() > 0L;
    }

    private void stageRuntimeAssets() throws IOException, PackageManager.NameNotFoundException {
        File appDirectory = getAppFilesDirectory();
        File assetDirectory = new File(appDirectory, "assets");
        File portArchive = new File(appDirectory, "soh.o2r");
        SharedPreferences preferences = getSharedPreferences(ASSET_PREFS, MODE_PRIVATE);
        long versionCode = getInstalledVersionCode();

        copyAssetTree(getAssets(), "assets", assetDirectory);
        copyAssetFile(getAssets(), "soh.o2r", portArchive, true);
        if (assetExists(getAssets(), "gamecontrollerdb.txt")) {
            copyAssetFile(getAssets(), "gamecontrollerdb.txt", new File(appDirectory, "gamecontrollerdb.txt"), false);
        }
        preferences.edit().putLong(ASSET_VERSION_KEY, versionCode).apply();
    }

    private static boolean assetExists(AssetManager assets, String name) throws IOException {
        String[] rootAssets = assets.list("");
        if (rootAssets == null) {
            return false;
        }
        for (String child : rootAssets) {
            if (name.equals(child)) {
                return true;
            }
        }
        return false;
    }

    private static void copyAssetTree(AssetManager assets, String assetPath, File destination) throws IOException {
        String[] children = assets.list(assetPath);
        if (children != null && children.length > 0) {
            if (!destination.isDirectory() && !destination.mkdirs() && !destination.isDirectory()) {
                throw new IOException("Could not create " + destination.getAbsolutePath());
            }
            for (String child : children) {
                copyAssetTree(assets, assetPath + "/" + child, new File(destination, child));
            }
            return;
        }

        copyAssetFile(assets, assetPath, destination, false);
    }

    private static void copyAssetFile(AssetManager assets, String assetPath, File destination, boolean sync)
            throws IOException {
        File parent = destination.getParentFile();
        if (parent != null && !parent.isDirectory() && !parent.mkdirs() && !parent.isDirectory()) {
            throw new IOException("Could not create " + parent.getAbsolutePath());
        }

        File temporary = new File(destination.getAbsolutePath() + ".tmp");
        try (InputStream input = assets.open(assetPath);
             FileOutputStream output = new FileOutputStream(temporary, false)) {
            byte[] buffer = new byte[64 * 1024];
            int count;
            while ((count = input.read(buffer)) != -1) {
                output.write(buffer, 0, count);
            }
            if (sync) {
                output.getFD().sync();
            }
        } catch (IOException exception) {
            // Never leave a truncated OTR archive or asset that could be mistaken for a complete copy.
            //noinspection ResultOfMethodCallIgnored
            temporary.delete();
            throw exception;
        }

        if (destination.exists() && !destination.delete()) {
            //noinspection ResultOfMethodCallIgnored
            temporary.delete();
            throw new IOException("Could not replace " + destination.getAbsolutePath());
        }
        if (!temporary.renameTo(destination)) {
            //noinspection ResultOfMethodCallIgnored
            temporary.delete();
            throw new IOException("Could not finish copying " + destination.getAbsolutePath());
        }
    }

    private String copySelectedRom(Uri uri) throws IOException {
        String displayName = "selected-rom.z64";
        try (Cursor cursor = getContentResolver().query(uri, new String[] { OpenableColumns.DISPLAY_NAME },
                                                       null, null, null)) {
            if (cursor != null && cursor.moveToFirst()) {
                int nameColumn = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);
                if (nameColumn >= 0) {
                    String candidate = cursor.getString(nameColumn);
                    if (candidate != null && !candidate.trim().isEmpty()) {
                        displayName = candidate;
                    }
                }
            }
        }

        // Keep the SAF display name from becoming a path, and make unsupported/missing extensions pass through
        // the existing native ROM validation as a .z64 candidate.
        displayName = new File(displayName).getName().replaceAll("[^A-Za-z0-9._-]", "_");
        String lowerName = displayName.toLowerCase(Locale.ROOT);
        if (!lowerName.endsWith(".z64") && !lowerName.endsWith(".n64") && !lowerName.endsWith(".v64")) {
            displayName += ".z64";
        }

        // Keep the ROM in the app-private root: the existing extractor scans this directory on later launches.
        File destination = new File(getAppFilesDirectory(), displayName);
        copyUriToFile(uri, destination);
        return destination.getAbsolutePath();
    }

    private void copyUriToFile(Uri uri, File destination) throws IOException {
        File temporary = new File(destination.getAbsolutePath() + ".tmp");
        try (InputStream input = getContentResolver().openInputStream(uri);
             FileOutputStream output = new FileOutputStream(temporary, false)) {
            if (input == null) {
                throw new IOException("The selected document could not be opened");
            }
            byte[] buffer = new byte[64 * 1024];
            int count;
            while ((count = input.read(buffer)) != -1) {
                output.write(buffer, 0, count);
            }
            output.getFD().sync();
        } catch (IOException exception) {
            //noinspection ResultOfMethodCallIgnored
            temporary.delete();
            throw exception;
        }

        if (destination.exists() && !destination.delete()) {
            //noinspection ResultOfMethodCallIgnored
            temporary.delete();
            throw new IOException("Could not replace the copied ROM");
        }
        if (!temporary.renameTo(destination)) {
            //noinspection ResultOfMethodCallIgnored
            temporary.delete();
            throw new IOException("Could not finish copying the selected ROM");
        }
    }
}
