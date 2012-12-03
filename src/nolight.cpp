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
#include "nolight.h"
#include <glm/gtc/type_ptr.hpp>

namespace topaz
{
    void nolight::populate_uniforms(const glm::mat4 & M, const glm::mat4 & V, const glm::mat4 & P, camera* C, gl_program* program)
    {
        glUniformMatrix4fv(program->uniform_locations["ModelMatrix"], 1, GL_FALSE, glm::value_ptr(M));
        CHECK_GL_ERROR("Filling model matrix");
        glUniformMatrix4fv(program->uniform_locations["ViewMatrix"], 1, GL_FALSE, glm::value_ptr(V));
        CHECK_GL_ERROR("Filling view matrix");
        glUniformMatrix4fv(program->uniform_locations["ProjectionMatrix"], 1, GL_FALSE, glm::value_ptr(P));
        CHECK_GL_ERROR("filling projection matrix");
        glUniform1i(program->uniform_locations["s_tex"],0);
        CHECK_GL_ERROR("filling s_tex");
    }
}
