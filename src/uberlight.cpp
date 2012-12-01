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
#include "uberlight.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace topaz
{
    uberlight::uberlight() : position(0.0f, 0.0f, 5.0f),
                             target(0.0f, 0.0f, 0.0f),
                             up(0.0f, 1.0f, 0.0)
    {
        se_width = 5.0f;
        se_height = 5.0f;
        se_width_edge = 1.0f;
        se_height_edge = 1.0f;
        se_roundness = 0.5f;
        surface_roughness = 1.0f;
        ds_near = 0.1f;
        ds_far = 5.0f;
        ds_near_edge = 0.0f;
        ds_far_edge = 0.1f;
        color_r = 1.0f;
        color_g = 1.0f;
        color_b = 1.0f;
        weight_r = 0.5f;
        weight_g = 0.5f;
        weight_b = 0.5f;
        surface_weight_r = 1.0f;
        surface_weight_g = 1.0f;
        surface_weight_b = 1.0f;
        barn_shaping = true;
        ambient_clamping = true;
    }

    uberlight::~uberlight()
    {

    }

    void uberlight::populate_uniforms(const glm::mat4 & M, const glm::mat4 & V, const glm::mat4 & P, camera* C, gl_program* program)
    {
        glUniformMatrix4fv(program->uniform_locations["ModelMatrix"], 1, GL_FALSE, glm::value_ptr(M));
        CHECK_GL_ERROR("Filling model matrix");
        glUniformMatrix4fv(program->uniform_locations["ViewMatrix"], 1, GL_FALSE, glm::value_ptr(V));
        CHECK_GL_ERROR("Filling view matrix");
        glUniformMatrix4fv(program->uniform_locations["ProjectionMatrix"], 1, GL_FALSE, glm::value_ptr(P));
        CHECK_GL_ERROR("filling projection matrix");
        glUniform1i(program->uniform_locations["s_tex"],0);
        CHECK_GL_ERROR("filling s_tex");

        glUniform3fv(program->uniform_locations["WCLightPos"], 1, glm::value_ptr(position));
        glm::vec3 camera_position = C->get_position();
        glUniform4fv(program->uniform_locations["ViewPosition"], 1, glm::value_ptr(glm::vec4(camera_position, 1.0f)));
        CHECK_GL_ERROR("Positions");

        glm::mat4 mc_to_wc = V*M;
        glUniformMatrix4fv(program->uniform_locations["MCtoWC"], 1, GL_FALSE, glm::value_ptr(mc_to_wc));
        glm::mat4 mc_to_wc_it = glm::transpose(glm::inverse(mc_to_wc));
        glUniformMatrix4fv(program->uniform_locations["MCtoWCit"], 1, GL_FALSE, glm::value_ptr(mc_to_wc_it));
        glm::mat4 wc_to_lc = to_matrix();
        glUniformMatrix4fv(program->uniform_locations["WCtoLC"], 1, GL_FALSE, glm::value_ptr(wc_to_lc));
        glm::mat4 wc_to_lc_it = glm::transpose(glm::inverse(wc_to_lc));
        glUniformMatrix4fv(program->uniform_locations["WCtoLCit"], 1, GL_FALSE, glm::value_ptr(wc_to_lc_it));
        CHECK_GL_ERROR("Loading matricies");

        glUniform1f(program->uniform_locations["SeWidth"], se_width);
        glUniform1f(program->uniform_locations["SeHeight"], se_height);
        glUniform1f(program->uniform_locations["SeWidthEdge"], se_width_edge);
        glUniform1f(program->uniform_locations["SeHeightEdge"], se_height_edge);
        glUniform1f(program->uniform_locations["SeRoundness"], se_roundness);
        glUniform1i(program->uniform_locations["BarnShaping"], barn_shaping ? 1 : 0);

        glUniform3fv(program->uniform_locations["LightColor"], 1, colors);
        glUniform3fv(program->uniform_locations["LightWeights"], 1, weights);
        
        glUniform3fv(program->uniform_locations["SurfaceWeights"], 1, surface_weights);
        glUniform1f(program->uniform_locations["SurfaceRoughness"], surface_roughness);
        glUniform1i(program->uniform_locations["AmbientClamping"], ambient_clamping ? 1 : 0);
        glUniform1f(program->uniform_locations["DsNear"], ds_near);
        glUniform1f(program->uniform_locations["DsFar"], ds_far);
        glUniform1f(program->uniform_locations["DsNearEdge"], ds_near_edge);
        glUniform1f(program->uniform_locations["DsFarEdge"], ds_far_edge);
    }

    glm::mat4 uberlight::to_matrix()
    {
        return glm::lookAt(position, target, up);
    }
}
