/*
 * Copyright (C) 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include "bibledit.h"
#include "library/bibledit.h"
#include <string.h>
#include <jni.h>


jstring Java_org_bibledit_android_MainActivity_GetVersionNumber (JNIEnv* env, jobject obj)
{
    return (*env)->NewStringUTF(env, bibledit_get_version_number ());
}


jstring Java_org_bibledit_android_MainActivity_GetNetworkPort (JNIEnv* env, jobject obj)
{
    return (*env)->NewStringUTF(env, bibledit_get_network_port ());
}


void Java_org_bibledit_android_MainActivity_InitializeLibrary (JNIEnv* env, jobject obj, jstring package, jstring webroot)
{
    const char * native_package = (*env)->GetStringUTFChars(env, package, 0);
    const char * native_webroot = (*env)->GetStringUTFChars(env, webroot, 0);
    bibledit_initialize_library (native_package, native_webroot);
}


void Java_org_bibledit_android_MainActivity_SetTouchEnabled (JNIEnv* env, jobject obj, jboolean enabled)
{
    // This fails to work on Android 6.
    bibledit_set_touch_enabled ((enabled == JNI_TRUE));
    // Set it always to true.
    bibledit_set_touch_enabled (true);
}


void Java_org_bibledit_android_MainActivity_SetQuitAtMidnight (JNIEnv* env, jobject obj)
{
    bibledit_set_quit_at_midnight ();
}


void Java_org_bibledit_android_MainActivity_SetTimezoneHoursOffsetUtc (JNIEnv* env, jobject obj, int hours)
{
    bibledit_set_timezone_hours_offset_utc (hours);
}


void Java_org_bibledit_android_MainActivity_StartLibrary (JNIEnv* env, jobject obj)
{
    bibledit_start_library ();
}


bool Java_org_bibledit_android_MainActivity_IsRunning (JNIEnv* env, jobject obj)
{
    return bibledit_is_running ();
}


bool Java_org_bibledit_android_MainActivity_IsSynchronizing (JNIEnv* env, jobject obj)
{
    return bibledit_is_synchronizing ();
}


void Java_org_bibledit_android_MainActivity_StopLibrary (JNIEnv* env, jobject obj)
{
    bibledit_stop_library ();
}


void Java_org_bibledit_android_MainActivity_ShutdownLibrary (JNIEnv* env, jobject obj)
{
    bibledit_shutdown_library ();
}


void Java_org_bibledit_android_MainActivity_Log (JNIEnv* env, jobject obj, jstring message)
{
    const char * native_message = (*env)->GetStringUTFChars(env, message, 0);
    bibledit_log (native_message);
    (*env)->ReleaseStringUTFChars(env, message, native_message);
}


jstring Java_org_bibledit_android_MainActivity_GetLastPage (JNIEnv* env, jobject obj)
{
    return (*env)->NewStringUTF(env, bibledit_get_last_page ());
}


