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
#ifndef DEF_H_
#define DEF_H_

/*
 * PHYSICS_METHOD controls how rigid body dynamics are handled.
 * 
 * 0 Explicit Euler Numeric Integration
 * 1 Newton Math
 */
#define PHYSICS_METHOD 0

#define PRINT_VELOCITY 0
#define CLIP_Y_TO_0 1
#define CLIP_BOX 1
#define CLIP_BOX_SIZE 25

#define DRAW_COLLISION_SOLIDS 1
// Set LIMIT_FRAMES to -1 for unlimited fps
#define LIMIT_FRAMES 60

//Debug Flags
#define PRINT_ID_ALLOCATION (1 << 0)
#define PRINT_SHADERS (1 << 1)

#define ACTIVE_LEVEL 0
#define debug_level(PRINT_LEVEL, ...) if (PRINT_LEVEL <= ACTIVE_LEVEL) printf(__VA_ARGS__)
//#define ACTIVE_FLAGS (PRINT_ID_ALLOCATION | PRINT_SHADERS)
#define ACTIVE_FLAGS (0)
#define CHECK_FLAGS(PRINT_FLAGS) ((ACTIVE_FLAGS & PRINT_FLAGS) == PRINT_FLAGS)
#define CHECK_FLAGS_ANY(PRINT_FLAGS) (ACTIVE_FLAGS & PRINT_FLAGS != 0)
#define debug_flags(PRINT_FLAGS, ...) if (CHECK_FLAGS(PRINT_FLAGS)) printf(__VA_ARGS__)
#define debug_flags_any(PRINT_FLAGS, ...) if (CHECK_FLAGS_ANY(PRINT_FLAGS)) printf(__VA_ARGS__)

#endif
