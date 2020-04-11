/*
 *  Copyright (C) 2007-2015 Lonelycoder AB
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  This program is also available under a commercial proprietary license.
 *  For more information, contact andreas@lonelycoder.com
 */
#include <stdio.h>
#include "main.h"
#include "htsmsg/htsmsg_store.h"
#include "settings.h"
#include "video_settings.h"
#include "misc/str.h"


struct video_settings video_settings;


void
video_settings_init(void)
{
  prop_t *s;

  s = settings_add_dir(NULL, _p("Video playback"), "video", NULL,
		       _p("Video acceleration and display behaviour"),
		       "settings:video");

#if ENABLE_VDPAU
  setting_create(SETTING_BOOL, s, SETTINGS_INITIAL_UPDATE,
                 SETTING_TITLE(_p("Enable VDPAU")),
                 SETTING_VALUE(1),
                 SETTING_WRITE_BOOL(&video_settings.vdpau),
                 SETTING_STORE("videoplayback", "vdpau"),
                 NULL);

  setting_create(SETTING_MULTIOPT, s, SETTINGS_INITIAL_UPDATE,
                 SETTING_TITLE(_p("Preferred VDPAU deinterlacer method")),
                 SETTING_STORE("videoplayback", "vdpau_deinterlace"),
                 SETTING_WRITE_INT(&video_settings.vdpau_deinterlace),
                 SETTING_OPTION("2", _p("Temporal/Spatial")),
                 SETTING_OPTION("1", _p("Temporal")),
                 SETTING_OPTION("0", _p("Off")),
                 NULL);

  setting_create(SETTING_MULTIOPT, s, SETTINGS_INITIAL_UPDATE,
                 SETTING_TITLE(_p("Maximum resolution for deinterlacer")),
                 SETTING_STORE("videoplayback",
                                "vdpau_deinterlace_resolution_limit"),
                 SETTING_WRITE_INT(&video_settings.
                                   vdpau_deinterlace_resolution_limit),
                 SETTING_OPTION     ("0", _p("No limit")),
                 SETTING_OPTION_CSTR("576",  "576"),
                 SETTING_OPTION_CSTR("720",  "720"),
                 SETTING_OPTION_CSTR("1080", "1080"),
                 NULL);
#endif

#if defined(__APPLE__) || defined(__ANDROID__)
  setting_create(SETTING_BOOL, s, SETTINGS_INITIAL_UPDATE,
                 SETTING_TITLE(_p("Hardware accelerated decoding")),
                 SETTING_STORE("videoplayback", "videoaccel2"),
                 SETTING_VALUE(1),
                 SETTING_WRITE_BOOL(&video_settings.video_accel),
                 NULL);
#endif

  video_settings.vzoom_setting =
    setting_create(SETTING_INT, s, SETTINGS_INITIAL_UPDATE,
                   SETTING_TITLE(_p("Video zoom")),
                   SETTING_UNIT_CSTR("%"),
                   SETTING_RANGE(50, 200),
                   SETTING_VALUE(100),
                   SETTING_STORE("videoplayback", "vzoom"),
                   SETTING_VALUE_ORIGIN("global"),
                   NULL);

  video_settings.pan_horizontal_setting =
    setting_create(SETTING_INT, s, SETTINGS_INITIAL_UPDATE,
                   SETTING_TITLE(_p("Horizontal pan")),
                   SETTING_UNIT_CSTR("%"),
                   SETTING_RANGE(-100, 100),
                   SETTING_VALUE(0),
                   SETTING_STORE("videoplayback", "horizontalpan"),
                   SETTING_VALUE_ORIGIN("global"),
                   NULL);

  video_settings.pan_vertical_setting =
    setting_create(SETTING_INT, s, SETTINGS_INITIAL_UPDATE,
                   SETTING_TITLE(_p("Vertical pan")),
                   SETTING_UNIT_CSTR("%"),
                   SETTING_RANGE(-100, 100),
                   SETTING_VALUE(0),
                   SETTING_STORE("videoplayback", "verticalpan"),
                   SETTING_VALUE_ORIGIN("global"),
                   NULL);

  video_settings.scale_horizontal_setting =
    setting_create(SETTING_INT, s, SETTINGS_INITIAL_UPDATE,
                   SETTING_TITLE(_p("Horizontal scale")),
                   SETTING_UNIT_CSTR("%"),
                   SETTING_RANGE(10, 300),
                   SETTING_VALUE(100),
                   SETTING_STORE("videoplayback", "horizontalscale"),
                   SETTING_VALUE_ORIGIN("global"),
                   NULL);

  video_settings.scale_vertical_setting =
    setting_create(SETTING_INT, s, SETTINGS_INITIAL_UPDATE,
                   SETTING_TITLE(_p("Vertical scale")),
                   SETTING_UNIT_CSTR("%"),
                   SETTING_RANGE(10, 300),
                   SETTING_VALUE(100),
                   SETTING_STORE("videoplayback", "verticalscale"),
                   SETTING_VALUE_ORIGIN("global"),
                   NULL);

  video_settings.stretch_horizontal_setting =
    setting_create(SETTING_BOOL, s, SETTINGS_INITIAL_UPDATE,
                   SETTING_TITLE(_p("Stretch video to widescreen")),
                   SETTING_STORE("videoplayback", "stretch_horizontal"),
                   SETTING_VALUE_ORIGIN("global"),
                   NULL);

  video_settings.stretch_fullscreen_setting =
    setting_create(SETTING_BOOL, s, SETTINGS_INITIAL_UPDATE,
                   SETTING_TITLE(_p("Stretch video to fullscreen")),
                   SETTING_STORE("videoplayback", "stretch_fullscreen"),
                   SETTING_VALUE_ORIGIN("global"),
                   NULL);

  video_settings.vinterpolate_setting =
    setting_create(SETTING_BOOL, s, SETTINGS_INITIAL_UPDATE,
                   SETTING_TITLE(_p("Video frame interpolation")),
                   SETTING_STORE("videoplayback", "vinterpolate"),
                   SETTING_VALUE_ORIGIN("global"),
                   SETTING_VALUE(1),
                   NULL);

  setting_create(SETTING_MULTIOPT, s, SETTINGS_INITIAL_UPDATE,
                 SETTING_TITLE(_p("Resume video playback")),
                 SETTING_WRITE_INT(&video_settings.resume_mode),
                 SETTING_STORE("videoplayback", "resumemode"),
                 SETTING_OPTION("2", _p("Ask")),
                 SETTING_OPTION("1", _p("Always")),
                 SETTING_OPTION("0", _p("Never")),
                 NULL);

  setting_create(SETTING_INT, s, SETTINGS_INITIAL_UPDATE,
                 SETTING_TITLE(_p("Count video as played when reaching")),
                 SETTING_VALUE(90),
                 SETTING_RANGE(1, 100),
                 SETTING_UNIT_CSTR("%"),
                 SETTING_WRITE_INT(&video_settings.played_threshold),
                 SETTING_STORE("videoplayback", "played_threshold"),
                 NULL);

  setting_create(SETTING_BOOL, s, SETTINGS_INITIAL_UPDATE,
                 SETTING_TITLE(_p("Automatically play next video in list")),
                 SETTING_STORE("videoplayback", "continuous_playback"),
                 SETTING_WRITE_BOOL(&video_settings.continuous_playback),
                 NULL);

  setting_create(SETTING_MULTIOPT, s, SETTINGS_INITIAL_UPDATE,
                 SETTING_TITLE(_p("Up / Down during video playback controls")),
                 SETTING_WRITE_INT(&video_settings.dpad_up_down_mode),
                 SETTING_STORE("videoplayback", "dpad_up_down_mode"),
                 SETTING_OPTION("0", _p("Master volume")),
                 SETTING_OPTION("1", _p("Per-file volume")),
                 NULL);



  setting_create(SETTING_BOOL, s, SETTINGS_INITIAL_UPDATE,
                 SETTING_TITLE(_p("Show clock during playback")),
                 SETTING_STORE("videoplayback", "show_clock"),
                 SETTING_WRITE_PROP(prop_create(prop_create(prop_get_global(),
                                                            "clock"),
                                                "showDuringVideo")),
                 NULL);

  setting_create(SETTING_INT, s, SETTINGS_INITIAL_UPDATE,
                 SETTING_TITLE(_p("Step when seeking backward")),
                 SETTING_VALUE(15),
                 SETTING_RANGE(3, 60),
                 SETTING_UNIT_CSTR("s"),
                 SETTING_WRITE_INT(&video_settings.seek_back_step),
                 SETTING_STORE("videoplayback", "seekbackstep"),
                 NULL);

  setting_create(SETTING_INT, s, SETTINGS_INITIAL_UPDATE,
                 SETTING_TITLE(_p("Step when seeking forward")),
                 SETTING_VALUE(30),
                 SETTING_RANGE(3, 60),
                 SETTING_UNIT_CSTR("s"),
                 SETTING_WRITE_INT(&video_settings.seek_fwd_step),
                 SETTING_STORE("videoplayback", "seekfwdstep"),
                 NULL);

  setting_create(SETTING_INT, s, SETTINGS_INITIAL_UPDATE,
                 SETTING_TITLE(_p("Video buffer size")),
                 SETTING_VALUE(48),
                 SETTING_RANGE(16, gconf.max_video_buffer_size ?: 768),
                 SETTING_UNIT_CSTR("MB"),
                 SETTING_STORE("videoplayback", "videobuffersize"),
                 SETTING_WRITE_INT(&video_settings.video_buffer_size),
                 NULL);
}
