import org.gradle.api.tasks.Sync

plugins {
    id("com.android.application")
}

val requestedAbis = providers.gradleProperty("androidAbis")
    .orElse("arm64-v8a,armeabi-v7a")
    .get()
    .split(',')
    .map { it.trim() }
    .filter { it.isNotEmpty() }

val ymlAssetsDirectory = rootProject.file(providers.gradleProperty("sohYmlAssetsDir")
    .orElse("../soh/assets/yml")
    .get())
val portArchive = rootProject.file(providers.gradleProperty("sohOtrPath")
    .orElse("../soh.o2r")
    .get())
val optionalControllerDatabase = rootProject.file("../gamecontrollerdb.txt")
val stagedRuntimeAssets = layout.buildDirectory.dir("generated/soh-runtime-assets")

val stageSohAssets = tasks.register<Sync>("stageSohAssets") {
    from(ymlAssetsDirectory) {
        into("assets")
    }
    from(portArchive)
    if (optionalControllerDatabase.isFile) {
        from(optionalControllerDatabase)
    }
    into(stagedRuntimeAssets)

    // The Android build cannot run the host soh-o2r-packer under the NDK.
    // Generate soh.o2r with the host build first; fail here with an actionable message.
    doFirst {
        if (!ymlAssetsDirectory.isDirectory) {
            throw GradleException("SOH YAML assets not found: ${ymlAssetsDirectory.absolutePath}")
        }
        if (!portArchive.isFile || portArchive.length() == 0L) {
            throw GradleException(
                "soh.o2r is missing or empty: ${portArchive.absolutePath}. " +
                    "Build it with the host CMake target GenerateSohOtr before assembling the Android app."
            )
        }
    }
    outputs.upToDateWhen { ymlAssetsDirectory.isDirectory && portArchive.isFile && portArchive.length() > 0L }
}

android {
    namespace = "com.harbourmasters.shipofharkinian"
    compileSdk = 35
    ndkVersion = "27.2.12479018"

    defaultConfig {
        applicationId = "com.harbourmasters.shipofharkinian"
        minSdk = 24
        targetSdk = 35
        versionCode = 1
        versionName = "9.3.0"

        ndk {
            abiFilters += requestedAbis
        }

        externalNativeBuild {
            cmake {
                targets += listOf("soh", "SDL2_net")
                arguments += listOf(
                    "-DANDROID_STL=c++_shared",
                    "-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON",
                    "-DSUPPRESS_WARNINGS=ON"
                )
            }
        }
    }

    buildTypes {
        debug {
            isJniDebuggable = true
        }
        release {
            isMinifyEnabled = false
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }

    externalNativeBuild {
        cmake {
            path = file("../../CMakeLists.txt")
            version = "3.31.6"
        }
    }

    sourceSets.getByName("main").assets.srcDir(stagedRuntimeAssets)
    sourceSets.getByName("main").assets.srcDir(rootProject.file("licenses"))

    androidResources {
        // The port archive is already compressed; avoid recompressing it into the APK.
        noCompress += "o2r"
    }

    packaging {
        jniLibs {
            useLegacyPackaging = false
        }
    }
}

tasks.named("preBuild").configure {
    dependsOn(stageSohAssets)
}
tasks.configureEach {
    if (name.startsWith("merge") && name.endsWith("Assets")) {
        dependsOn(stageSohAssets)
    }
}
