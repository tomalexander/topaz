/*
 * Copyright (c) 2012 Tom Alexander, Tate Larsen
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */
#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include "topaz.h"

using std::string;

namespace topaz
{
    bool path_exists(const char* path);
    char* read_fully(const char* path, long & file_size);
    char* read_fully_string(const char* path, long & file_size);
    bool get_tag(const string & input, string & tag, string & name, string & content, size_t & start_index);
    string trim_whitespace(const string & input);
    string& to_unix_line_endings_ip(string & input);
    GLuint load_texture(const string & name, int pixel_format = GL_RGBA);

    bool ends_with(const string & needle, const string & haystack);
    string get_up_one_dir(string dir);
}

#endif
