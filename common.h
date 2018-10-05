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
#ifndef LIBCE_DYNAMIC_H
#define LIBCE_DYNAMIC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#if defined(_MSC_VER)
#if defined(BUILD_LIBRARY)
#define SHARED_API   __declspec(dllexport)
#else
#define SHARED_API   __declspec(dllimport)
#endif
#else
#define SHARED_API   __attribute__ ((visibility("default")))
#endif

#if defined(__cplusplus)
extern "C" {
#endif

enum Error {
    Ok = 0,
    SessionCreate = 100,
    SessionBadStructure,
    LastPublicError
};

struct Version {
    int major;
    int minor;
    int build;
};

/**
 * Session for controlling and parametrizing work
 */
struct Session {
    /** Library or algorithm version */
    struct Version version;
    /** Unique userspace session identification sequence */
    char *id;
    /** Internal session data*/
    void *payment;
    /** Lats execution session error*/
    int last_error;

    volatile void *busy;
};

struct IntValue {
    int maximum;
    int minimum;
    bool enable;
};

struct SingleIntValue {
    int value;
    bool enable;
};

struct DoubleValue {
    double maximum;
    double minimum;
    bool enable;
};

/**
 * Readed all file bytes. Function allocate needed bytes for put content
 * @param file_path path to file read
 * @param content pointer to bytes stream
 * @param content_size
 * @return true - file read and allocate memory, false - something went wrong
 */
bool read_file_content(const char *file_path, uint8_t **content, uint64_t *content_size);

/**
 * Free allocated function read_file_content memory
 * @param content pointer to memory for free
 * @return true - memory free
 */
bool free_file_content(uint8_t **content);

typedef bool (*CREATE_SESSION)(struct Session *, const char *);
typedef bool (*DESTROY_SESSION)(struct Session *);
typedef bool (*CHECK_SESSION)(struct Session *, const uint8_t *, uint64_t);

/**
 * Execute function sequence for file stream
 * @param fd opened file stream
 * @param configuration path to configuration file
 * @param create_session function pointer for create session
 * @param destroy_session function pointer for destroy session
 * @param check_session function pointer to check session
 * @return executing status code
 */
int read_from_fd(FILE *fd,
                 const char *configuration,
                 CREATE_SESSION create_session,
                 DESTROY_SESSION destroy_session,
                 CHECK_SESSION check_session
);

#if defined(__cplusplus)
}
#endif

#endif //LIBCE_DYNAMIC_H
