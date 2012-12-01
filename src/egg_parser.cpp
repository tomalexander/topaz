/*
 * Copyright (c) 2012 Tom Alexander
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
#include "egg_parser.h"
#include "util.h"
#include <stdio.h>
#include <iostream>

namespace topaz
{
    void DEBUG_print_tags(const string & content, int indentation = 0);
    
    /** 
     * Load a .egg text file containing a model
     *
     * @param file_path Path in PhysFS
     *
     * @return pointer to root panda_node if model loaded, NULL if failed
     */
    panda_node* load_model(const string & file_path)
    {
        long file_data_size;
        char* file_data = read_fully(file_path.c_str(), file_data_size);
        if (file_data == NULL)
            return NULL;
        
        string contents(file_data);

        panda_node* top_node = new panda_node("","",contents);
        fix_coordinate_system(top_node);
        top_node->parse();

        
        delete [] file_data;
        return top_node;
    }

    
    void DEBUG_print_tags(const string & content, int indentation)
    {
        string indent(indentation*4, ' ');
        string tag = "", name = "", subcontent = "";
        size_t index = 0;
        while (panda_node::get_tag(content, tag, name, subcontent, index))
        {
            printf("%sFound tag <%s>: %s | %s\n", indent.c_str(), tag.c_str(), name.c_str(), subcontent.c_str());
            DEBUG_print_tags(subcontent, indentation+1);
        }
    }
}
