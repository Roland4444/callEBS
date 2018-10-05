/**
 * Copyright (c) 2017, rtlabs
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *     * Neither the name of "libce" nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   20.09.2017 Andrey Khlebnikov <andrey.khlebnikov@rtlabs.ru>
 */
#ifndef LIBCE_LIBCV_H
#define LIBCE_LIBCV_H

#include "common.h"

#if defined(__cplusplus)
extern "C" {
#endif

enum CheckVoiceError {
    CheckVoiceReadConfiguration = LastPublicError,
    CheckVoiceIllegalContent,
    CheckVoiceIllegalFormat,
    CheckVoiceUnpackFailed,
    CheckVoiceSignalNoiseFailed, /* 106 */
    CheckVoiceMaximumChannels,
    CheckVoiceSampleRateFailed,/*108*/
    CheckVoiceDepthFailed, /*109*/
    CheckVoiceGreatRecordLength, /* 110*/
    CheckVoiceGreatRecordDuration,
    CheckTelephonyBorderFailed,
    CheckVoiceUnknownFailed,
    CheckVoiceOverloadedFailed
};

/**
 * Sample configuration file
 *
 * maximum - maximized parameter value
 * minimum - minimal parameter value
 * enable  - true or false. Enable or disable check this parameter
 *
 {
  "signal_noise": {
    "maximum": 100,
    "minimum": 20,
    "enable": true
  },
  "duration": {
    "maximum": 35,
    "minimum": 5,
    "enable": true
  },
  "simple_rate": {
    "maximum": 96000,
    "minimum": 8000,
    "enable": true
  },
  "channels": {
    "maximum": 2,
    "minimum": 1,
    "enable": true
  },
  "length": {
    "maximum": 1500,
    "minimum": 100,
    "enable": true
  },
  "depth": {
    "maximum": 32,
    "minimum": 8,
    "enable": true
  },
  "frequency": {
    "maximum": 3000.0,
    "minimum": 20.0,
    "enable": true
  }
 }
 */

/* signal-to-noise ratio parameter */
#define SignalNoisePropertyName "signal_noise"
/* duration parameter*/
#define DurationPropertyName    "duration"
/* sample rate parameter */
#define SampleRatePropertyName  "sample_rate"
/* channels parameter*/
#define ChannelsPropertyName    "channels"
/* length (in KB) parameter*/
#define LengthPropertyName      "length"
/* depth parameter*/
#define DepthPropertyName       "depth"
/* frequency - for compute the SNR*/
#define FrequencyPropertyName   "frequency"
/* telephony border - to prevent phone records*/
#define TelephonyBorderName   "telephony_border"

bool SHARED_API v_enable_property(struct Session *session, const char *key, bool value);

/**
 * Create session for check voice on properties. The session stores the parameters necessary for check.
 * Thus, you can simultaneously work with different sessions thread safe.
 *
 * struct Session session;
 *
 * session.id = "MyUniqueId";
 * if (v_create_session(&session, "/home/user/cv_configuration.json")) {
 *     ...
 *     v_destroy_session(&session);
 * }
 * @param session - pointer to session, where put internal session environment
 * @param configuration_file - configuration file for create session
 * @return true - session create, false - session create failed, error code put to session->last_error
 * 102 - can't read configuration file
 *
 * @attention If you want to reload the configuration, call first v_destroy_session otherwise you will get memory leak
 */
bool SHARED_API v_create_session(struct Session *session, const char *configuration_file);

/**
 * Destroy created function v_create_session session
 * @param session - pointer to session
 * @return true - session destroyed success, false - session can't be destroyed. Error code put to session->last_error
 * 101 - bad session structure. Putted session pointer not created v_create_session function
 */
bool SHARED_API v_destroy_session(struct Session *session);

/**
 * Check voice record for session
 * @param session - pointer to session, created in function v_create_session
 * @param content - pointer to raw uncompressed and simplified format(RIFF) voice record
 * @param content_size - data size
 * @return true - voice record corresponds to the properties on session, false - bad voice record. Error code put to session->last_error
 * 101 - bad session structure. Putted session pointer not created v_create_session function
 * 103 - illegal content. content was null or content_size = 0
 * 104 - bad record format. Only RIFF. Simple, uncompressed, wav record.
 * 105 - bad record structure. Is there damaged?
 * 106 - SNR(signal-to-noise ratio) parameter does not match the limit values
 * 107 - bad channels count
 * 108 - simple rate parameter does not match the limit values
 * 109 - depth parameter does not match the limit values
 * 110 - record size (in bytes) does not match the limit values
 * 111 - duration great the limit values
 */
bool SHARED_API v_check(struct Session *session, const uint8_t *content, uint64_t content_size);

#ifndef DISABLE_DEBUG_OUTPUT

#define OutputProcessingSoundName "processing_sound_name"

bool SHARED_API v_dbg_set_property(struct Session *session, const char *key, const char *value);

#endif

#if defined(__cplusplus)
}
#endif

#endif //LIBCE_LIBCV_H
