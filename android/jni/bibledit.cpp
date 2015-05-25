/*
 * Copyright (C) 2009 The Android Open Source Project
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

const char * bibledit_get_version_number ()
{
    return "1.0.999";
}

const char * bibledit_get_network_port ()
{
    return "8080";
}


void bibledit_set_touch_enabled (bool enabled)
{
    if (enabled) {};
}

void bibledit_set_quit_at_midnight ()
{
    
}

void bibledit_set_timezone_hours_offset_utc (int hours)
{
    if (hours) {};
}

void bibledit_initialize_library (const char * package, const char * webroot)
{
    
}

void bibledit_start_library ()
{
    
}


bool bibledit_is_running ()
{
    return true;
}

void bibledit_stop_library ()
{
    
}

void bibledit_shutdown_library ()
{
    
}

void bibledit_log (const char * message)
{
    
}
