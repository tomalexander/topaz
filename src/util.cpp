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
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include "util.h"
#include "physfs.h"
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics/Image.hpp>
#include <unordered_map>

using std::unordered_map;

namespace topaz
{
    extern unordered_map<string, GLuint> textures;
    /** 
     * Checks to see if a file exists
     *
     * @param path Path to the file in the real filesystem
     *
     * @return true if file exists, false if it does not
     */
    bool path_exists(const char* path)
    {
        struct stat st;
        return (stat(path,&st) == 0);
    }
    
    /** 
     * Read the entire contents of a file into memory, and null terminate it
     *
     * @param path path to the file using PhysFS
     * @param file_size size of file, passed by reference
     *
     * @return pointer to location in memory containing file, null if
     * failed. Must delete memory or you will get a memory leak!
     */
    char* read_fully_string(const char* path, long & file_size)
    {
        //Check for file
        if (PHYSFS_exists(path) == 0)
        {
            fprintf(stderr, "File %s does not exist!\n", path);
            return NULL;
        }

        //Open file
        PHYSFS_File* file_ptr = PHYSFS_openRead(path);
        if (file_ptr == NULL)
        {
            fprintf(stderr, "Failed to open file %s\n", path);
            return NULL;
        }

        file_size = PHYSFS_fileLength(file_ptr);
        char* ret = new char[file_size+1];

        PHYSFS_read(file_ptr, ret, 1, file_size);

        PHYSFS_close(file_ptr);
        ret[file_size] = '\0';
        return ret;
    }

    /** 
     * Read the entire contents of a file into memory
     *
     * @param path path to the file using PhysFS
     * @param file_size size of file, passed by reference
     *
     * @return pointer to location in memory containing file, null if
     * failed. Must delete memory or you will get a memory leak!
     */
    char* read_fully(const char* path, long & file_size)
    {
        //Check for file
        if (PHYSFS_exists(path) == 0)
        {
            fprintf(stderr, "File %s does not exist!\n", path);
            return NULL;
        }

        //Open file
        PHYSFS_File* file_ptr = PHYSFS_openRead(path);
        if (file_ptr == NULL)
        {
            fprintf(stderr, "Failed to open file %s\n", path);
            return NULL;
        }

        file_size = PHYSFS_fileLength(file_ptr);
        char* ret = new char[file_size];

        PHYSFS_read(file_ptr, ret, 1, file_size);

        PHYSFS_close(file_ptr);
        return ret;
    }



    /** 
     * Remove trailing and following white space
     *
     * @param input string to clean
     *
     * @return a new string with trailing and following whitespace
     * removed
     */
    string trim_whitespace(const string & input)
    {
        size_t start = 0;
        size_t end = input.size();

        while (start < end && (input[start] == ' ' || input[start] == '\t' || input[start] == '\n' || input[start] == '\r')) ++start;
        if (start == end)
            return "";
        while (end > 0 && (input[end-1] == ' ' || input[end-1] == '\t' || input[end-1] == '\n' || input[end-1] == '\r')) --end;
        return string(input, start, end-start);
    }

    /** 
     * Remove all carriage return characters from the string in-place
     *
     * @param input the string to be cleaned
     *
     * @return reference to string passed in to this function
     */
    string& to_unix_line_endings_ip(string & input)
    {
        size_t carriage_return_position = input.find('\r');
        while (carriage_return_position != string::npos)
        {
            input.erase(carriage_return_position,1);
            carriage_return_position = input.find('\r', carriage_return_position);
        }
        return input;
    }

    /** 
     * Load a texture file from disk using the virtual file system and load it into OpenGL
     *
     * @param name name of texture file
     * @param pixel_format OpenGL pixel format
     *
     * @return GLuint to the texture file in OpenGL
     */
    GLuint load_texture(const string & name, int pixel_format)
    {
        if (textures.find(name) != textures.end())
        {//Texture already loaded
            return textures[name];
        }

        long file_size;
        sf::Image img;
        char* file_data = read_fully(("textures/"+name).c_str(), file_size);
        if (!img.loadFromMemory(file_data, file_size))
        {
            std::cerr << "Failed to load texture: " << name << std::endl;
        }
        img.flipVertically();
        GLuint ret;
        glGenTextures(1, &ret);
        CHECK_GL_ERROR("Gen Texture");
        glBindTexture(GL_TEXTURE_2D, ret);
        CHECK_GL_ERROR("Bind Texture");
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        CHECK_GL_ERROR("Tex Parameters");
        glTexImage2D(GL_TEXTURE_2D, 0, pixel_format, img.getSize().x, img.getSize().y, 0, pixel_format, GL_UNSIGNED_BYTE, img.getPixelsPtr());
        CHECK_GL_ERROR("Copying Image Data");

        delete[] file_data;
        textures[name] = ret;
        add_cleanup_function([ret]() {glDeleteTextures(1, &ret);});
        return ret;
    }

    /** 
     * Checks to see if haystack ends with needle
     *
     * @param needle string to look for
     * @param haystack string to search through
     *
     * @return true if haystack ends with needle, false if not
     */
    bool ends_with(const string & needle, const string & haystack)
    {
        if (needle.size() > haystack.size())
            return false;

        for (size_t x = 0; x < needle.size(); ++x)
        {
            if (needle[needle.size()-1-x] != haystack[haystack.size()-1-x])
            {
                return false;
            }
        }
        return true;
    }

    /** 
     * Return the path that is one directory above dir
     *
     * @param dir The path to start from
     *
     * @return The path to the folder above dir
     */
    string get_up_one_dir(string dir)
    {
        static string sep(PHYSFS_getDirSeparator());
        if (ends_with(sep, dir))
            dir = string(dir, 0, dir.size()-sep.size());
        while (!ends_with(sep, dir))
            dir.erase(dir.size()-1);
        return dir;
    }
}
