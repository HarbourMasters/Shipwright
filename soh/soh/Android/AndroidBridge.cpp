#include "soh/Android/AndroidBridge.h"

#ifdef __ANDROID__

#include "soh/Android/TouchControls.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_system.h>
#include <jni.h>

#include <condition_variable>
#include <mutex>
#include <string>
#include <utility>

extern "C" int SDLCALL SohAndroidSDL_Init(Uint32 flags) {
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
    const int result = SDL_Init(flags);
    if (result == 0 && (flags & SDL_INIT_VIDEO) != 0) {
        // libultraship's default SDL attributes are desktop GL. Android's GLSL
        // shaders require an ES 3.0 context, so set the profile immediately
        // after SDL_Init and before the backend creates its window.
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    }
    return result;
}

namespace {
std::mutex gRomPickerMutex;
std::condition_variable gRomPickerCondition;
bool gRomPickerComplete = false;
std::string gSelectedRomPath;
}

namespace SohAndroid {
bool SelectRom(std::string& path) {
    JNIEnv* env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
    jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());
    if (env == nullptr || activity == nullptr) {
        return false;
    }

    jclass activityClass = env->GetObjectClass(activity);
    if (activityClass == nullptr) {
        env->DeleteLocalRef(activity);
        return false;
    }
    jmethodID openPicker = env->GetMethodID(activityClass, "openRomPicker", "()V");
    if (openPicker == nullptr || env->ExceptionCheck()) {
        env->ExceptionClear();
        env->DeleteLocalRef(activityClass);
        env->DeleteLocalRef(activity);
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(gRomPickerMutex);
        gRomPickerComplete = false;
        gSelectedRomPath.clear();
    }

    env->CallVoidMethod(activity, openPicker);
    const bool callFailed = env->ExceptionCheck();
    if (callFailed) {
        env->ExceptionClear();
    }
    env->DeleteLocalRef(activityClass);
    env->DeleteLocalRef(activity);
    if (callFailed) {
        return false;
    }

    std::unique_lock<std::mutex> lock(gRomPickerMutex);
    gRomPickerCondition.wait(lock, [] { return gRomPickerComplete; });
    path = gSelectedRomPath;
    return !path.empty();
}
} // namespace SohAndroid

extern "C" JNIEXPORT void JNICALL
Java_com_harbourmasters_shipofharkinian_ShipActivity_nativeOnRomPickerResult(JNIEnv* env, jclass, jstring selectedPath) {
    std::string path;
    if (env != nullptr && selectedPath != nullptr) {
        const char* utfPath = env->GetStringUTFChars(selectedPath, nullptr);
        if (utfPath != nullptr) {
            path = utfPath;
            env->ReleaseStringUTFChars(selectedPath, utfPath);
        }
    }

    {
        std::lock_guard<std::mutex> lock(gRomPickerMutex);
        gSelectedRomPath = std::move(path);
        gRomPickerComplete = true;
    }
    gRomPickerCondition.notify_one();
}

extern "C" JNIEXPORT void JNICALL
Java_com_harbourmasters_shipofharkinian_ShipActivity_nativeOnMenuToggle(JNIEnv*, jclass) {
    SohAndroid::TouchControls::RequestMenuToggle();
}

#endif // __ANDROID__
