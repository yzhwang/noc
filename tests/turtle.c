/* noc turtle test code.
 *
 * Copyright (c) 2015 Guillaume Chereau <guillaume@noctua-software.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/*
 * This file contains a few examples of what we can do with the noc_turtle
 * library.  Some examples are inspired from the game Blowfish Rescue for
 * which I created this library.
 */

#include "noc_turtle.h"
#include "font.h"

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// A few user flags that can be set in each turtle to change OpenGL rendering
// mode.  Since noc_tt does not dictate the rendering code, it is up to you
// to define what flags you might need.
enum {
    FLAG_STENCIL_WRITE  = 1 << 0,   // Write on the stencil buffer.
    FLAG_STENCIL_FILTER = 1 << 1,   // Only render where the stencil is set.
    FLAG_EFFECT_LIGHT   = 1 << 2,   // Additive rendering.
};

// Include noc_turtle.h with NOC_TURTLE_DEFINE_NAMES so that we don't have
// to prefix all the macros with NOCTT.
#define NOC_TURTLE_DEFINE_NAMES
#include "noc_turtle.h"

// Defines a custom directive to render text.
#define TEXT(msg, ...) TRANSFORM(__VA_ARGS__) { \
    noctt_vec3_t pos = noctt_get_pos(ctx); \
    font_draw_text(pos.x, pos.y, msg); \
}

// ##### Some simple demos code #######

static void spiral_node(noctt_turtle_t *ctx)
{
    START
    SQUARE();
    TR(HUE, PM(0, 5));
    YIELD();
    if (BRAND(0.01)) {
        TR(FLIP, 0);
        SPAWN(spiral_node, R, -90);
    }
    SPAWN(spiral_node, X, 0.4, R, 3, X, 0.4, S, 0.99);
    END
}

static void spiral(noctt_turtle_t *ctx)
{
    START
    TR(HSL, 1, 0, 1, 0.5);
    CALL(spiral_node);
    CALL(spiral_node, FLIP, 90);
    END
}

static void demo1(noctt_turtle_t *ctx)
{
    START
    TEXT("Press any key to see other demos", X, -0.47, Y, 0.47);
    SQUARE(S, 0.9, LIGHT, 0.2);
    SQUARE(S, 0.9, G, -1, LIGHT, 0.1);
    TR(SN, LIGHT, 1);

    TRANSFORM_SPAWN(X, -0.25, 0.25, S, 0.5) {
        FOR(64, G, -2, LIGHT, -0.02) {
            RSQUARE(60, S, 0.5);
            YIELD(4);
        }
    }

    SQUARE(S, 0.1);
    SQUARE(S, 0.1, X, 2);
    SQUARE(S, 0.1, X, 4, R, 45, LIGHT, -0.5);
    SQUARE(S, 0.1, X, 6, R, 45, LIGHT, -0.5, SAT, 1, HUE, 180);
    CIRCLE(S, 0.1, Y, 2);
    TRIANGLE(S, 0.1, Y, 4);

    CALL(spiral, Y, -0.5, S, 0.02);
    END
}

// Defined in tree.c
void tree_rule(noctt_turtle_t *ctx);
// Defined in modern.c
void modern_rule(noctt_turtle_t *ctx);

static void shapes_rule(noctt_turtle_t *ctx)
{
    const noctt_vec3_t poly[] = {
        {-0.5, -0.5}, {0, -0.5}, {0.5, 0.5}, {-0.5, 0.5}};
    START
    TR(LIGHT, 1, S, 1.0 / 3, SN);
    SQUARE(             X, -1,  1, S, 0.5);
    TEXT("square",      X, -1,  1, X, -0.25, -0.35);
    CIRCLE(             X,  0,  1, S, 0.5);
    TEXT("circle",      X,  0,  1, X, -0.25, -0.35);
    RSQUARE(80,         X,  1,  1, S, 0.5);
    TEXT("rsquare",     X,  1,  1, X, -0.25, -0.35);
    TRIANGLE(           X, -1,  0, S, 0.5);
    TEXT("triangle",    X, -1,  0, X, -0.25, -0.35);
    STAR(5, 0.3, 0,           X, 0, 0, S, 0.5);
    TEXT("star(5, 0.3, 0)",   X, 0, 0, X, -0.25, -0.35);
    STAR(8, 0.2, 0.9,         X, 1, 0, S, 0.5);
    TEXT("star(8, 0.2, 0.9)", X, 1, 0, X, -0.25, -0.35);
    POLY(4, poly,       X, -1, -1, S, 0.5);
    TEXT("poly",        X, -1, -1, X, -0.25, -0.35);
    END
}

static void stencil_rule(noctt_turtle_t *ctx)
{
    START
    TEXT("Show how to use FLAG to set the stencil operations",
         X, -0.48, Y, 0.47);
    TR(SN, S, 0.5, LIGHT, 0.5);
    TRANSFORM(FLAG, FLAG_STENCIL_WRITE) {
        SQUARE();
        CIRCLE(X, 0.5, 0.5, S, 0.5);
    }
    TRANSFORM(FLAG, FLAG_STENCIL_FILTER) {
        CIRCLE(X, 0.5, LIGHT, 0.5);
    }
    END
}

static void colors_rule(noctt_turtle_t *ctx)
{
    START
    TR(SN, X, -0.5, -0.5, S, 1.0 / 64, SAT, 0.5);
    FOR(64, Y, 1, HUE, 360.0 / 64) {
        FOR(64, X, 1) {
            SQUARE(LIGHT, 1, (float)ctx->i / (ctx->n - 1));
        }
    }
    END
}

// ###### Rules to render the tree demo ######

static void moon(noctt_turtle_t *ctx)
{
    START
    CIRCLE(LIGHT, -0.5, G, 2);
    FOR(32, S, 0.95, LIGHT, 0.01) {
        CIRCLE();
        YIELD();
    }
    END
}

static void part(noctt_turtle_t *ctx)
{
    START
    RSQUARE(0, SX, 0.2, HUE, PM(0, 15));
    RSQUARE(0, SX, 0.2, LIGHT, -0.4, G, 2, Z, -0.5);
    ctx->vars[0] += 1;
    if (ctx->vars[0] == 15) {
        TR(S, PM(1, 0.4));
        CIRCLE(HUE, PM(0, 45));
        CIRCLE(HUE, PM(0, 45), LIGHT, -0.4, G, 2, Z, -0.5);
        KILL();
    }
    if (BRAND(0.3)) {
        SPAWN(part, R, PM(0, 90), Y, 0.5);
    }
    YIELD(4);
    CALL(part, Y, 0.45, R, PM(0, 45), Y, 0.45, S, 0.9);
    END
}

void tree_rule(noctt_turtle_t *ctx)
{
    START
    TR(HSL, 180, 0.5, 0.5);
    SQUARE(LIGHT, 0.1, SAT, -0.5, Z, -1);
    TEXT("Example of using depth buffer for border effects",
         X, -0.48, Y, 0.47);
    SPAWN(moon, X, 0.3, 0.3, SN, S, 0.2);
    TR(Y, -0.5, SN, S, 0.1);
    SPAWN(part);
    END
}

// Rules for the 'modern' demo.

static void modern_branch(noctt_turtle_t *ctx)
{
    START
    SQUARE(SY, 0.2);
    TR(LIGHT, -0.01);
    YIELD();
    if (BRAND(0.1)) {
        TR(FLIP, 0);
        SPAWN(modern_branch, R, -90);
    }
    if (BRAND(0.01)) {
        CIRCLE(S, 2);
    }
    // We could use a CALL, but JUMP is more efficient since it tell the
    // library that we won't do any operation after the call, and so we
    // can directly reuse the turtle.
    JUMP(modern_branch, X, 0.4, R, PM(0, 1), X, 0.4, S, 0.99);
    END
}

void modern_rule(noctt_turtle_t *ctx)
{
    START
    TR(SN, S, 0.2, LIGHT, 1);
    CIRCLE();
    FOR(4) {
        SPAWN(modern_branch, R, PM(0, 180), S, 0.1, X, 1);
    }
    END
}



// ########### Rules for rendering the city ######################

// Render a square from bottom up.
static void square_up(noctt_turtle_t *ctx)
{
    float k;
    const float time = 0.1 * sqrtf(ctx->scale[1]);
    const int nb = time * 50;
    START
    FOR(nb) {
        k = ctx->i / (ctx->n - 1.0);
        SQUARE(Y, -0.5, S, 1, k, Y, 0.5);
        YIELD(1);
    }
    SQUARE();
    END
}

static void cloud(noctt_turtle_t *ctx)
{
    START
    TR(A, -0.5, LIGHT, 1);
    TR(SY, 1.0 / 3, Y, -1, X, -0.5, SN, X, 0.5);
    TR(LIGHT, 1);
    FOR(4, X, 2.0 / 3) {
        CIRCLE(S, FRAND(0.75, 1));
    }
    TR(Y, 0.5, X, 1.0 / 3);
    FOR(3, X, 2.0 / 3) {
        CIRCLE(S, FRAND(0.75, 1));
    }
    TR(Y, 0.25, X, 2.0 / 3);
    FOR(1, X, 2.0 / 3) {
        CIRCLE(S, FRAND(0.75, 1));
    }
    END
}

static void city_noise(noctt_turtle_t *ctx)
{
    START
    TR(FLAG, FLAG_EFFECT_LIGHT, SAT, -1, LIGHT, 1, 0.5);
    FOR(1000) {
        TR(X, PM(0, 0.5), PM(0, 0.5));
        TR(S, PM(0.02, 0.02), SN, R, FRAND(0, 360));
        SQUARE(LIGHT, PM(0, 0.04));
    }
    END
}

static void sky(noctt_turtle_t *ctx)
{
    const float sx = 0.02;
    const float dx = 0.9;
    int i;

    START
    SQUARE();
    TR(SN);
    TR(Y, -0.3);
    // Render the sky lines.
    FOR(3, Y, 0.3) {
        TR(X, -0.5, S, sx, 0.2, LIGHT, 0.2);
        for (i = 0; i < 3; i++) ctx->vars[i] = PM(0, 0.5);
        FOR(1.1 / sx / dx, X, dx, R, PM(0, 0.1), SY, PM(1, 0.01))
        {
            SQUARE();
            for (i = 0; i < 3; i++)
                SQUARE(Y, ctx->vars[i], S, 1.2, 0.02, LIGHT, -0.05);
        }
    }
    END
}

static void antenna(noctt_turtle_t *ctx)
{
    START
    CALL(square_up, S, 0.02, FRAND(1.5, 2) / 6, Y, 0.5);
    END
}

static void tower(noctt_turtle_t *ctx)
{
    const int n = 4;
    START
    CALL(square_up);

    // Sides
    if (BRAND(0.5)) {
        FOR(n) {
            SPAWN(square_up, Y, (float)ctx->i / n - 0.4, S, 1.1, 0.1);
        }
    }

    if (BRAND(0.5)) SPAWN(antenna, X, PM(0, 0.5), 0.5, S, 3, 0.5);

    // Top
    TRANSFORM(Y, 0.5, S, 0.9, 0.02, Y, 0.5) {
        ctx->vars[0] = FRAND(0, 3);
        FOR(ctx->vars[0], Y, 1, S, 0.9, 1)
            CALL(square_up);
    }

    END
}

static void building(noctt_turtle_t *ctx)
{
    START
    CALL(square_up);

    // Top
    TRANSFORM(Y, 0.5, S, 0.9, 0.05, Y, 0.5) {
        FOR(FRAND(0, 3), Y, 1, S, 0.9, 1) {
            CALL(square_up);
        }
    }

    if (BRAND(0.5)) SPAWN(antenna, X, PM(0, 0.5), 0.5);

    // Chimneys
    if (BRAND(0.5)) {
        TRANSFORM_SPAWN(X, 0, 0.5, S, 0.1, 0.5, Y, 0.5) {
            FOR(3, X, 1.5, -0.2) {
                CALL(square_up);
                CALL(square_up, Y, 0.4, S, 1.2, 0.2, Y, 0.5);
            }
        }
    }
    END
}

static void structure(noctt_turtle_t *ctx)
{
    START
    if (BRAND(0.5))
        CALL(tower, S, FRAND(1, 3), FRAND(5, 10), Y, 0.5);
    else
        CALL(building, S, FRAND(4, 10), FRAND(2, 4), Y, 0.5);
    END
}

void blowfish_city_rule(noctt_turtle_t *ctx)
{
    START
    TR(HSL, 1, 0, 0.3, 0.5);
    CALL(sky, Z, -0.5);
    CALL(city_noise);
    TR(SN);
    TR(HSL, 1, 180, 0.1, 0.1);
    SQUARE(X, -1);
    SQUARE(X, +1);
    TEXT("Background from the game Blowish Rescue", X, -0.48, Y, 0.47);

    FOR(4) {
        SPAWN(cloud, X, PM(0, 0.4), PM(0.25, 0.25), S, PM(0.1, 0.05), SN);
    }

    TR(Y, -0.05);
    SQUARE(Y, -0.7, S, 1, 0.4);
    TR(Y, -0.5);
    FOR(20) {
        SPAWN(structure, X, FRAND(-0.45, 0.45), S, 1.0 / 30);
        YIELD(1);
    }
    END
}

// ############# Rules for rendering some blowfish objects ##############

static void noise1(noctt_turtle_t *ctx)
{
    START
    TR(SAT, -1, LIGHT, 1, 0.5, FLAG, FLAG_EFFECT_LIGHT);
    FOR(100) {
        SQUARE(X, PM(0, 0.5), PM(0, 0.5),
               SN, S, 0.2, S, PM(1, 1),
               R, FRAND(0, 360),
               LIGHT, PM(0, ctx->vars[0]));
    }
    END
}

static void block(noctt_turtle_t *ctx)
{
    START
    TR(HSL, 1, PM(200, 25), 0.5, 0.5);

    RSQUARE(64);
    RSQUARE(64, G, -8, FLAG, FLAG_STENCIL_WRITE);
    TR(FLAG, FLAG_STENCIL_FILTER);
    FOR(2, R, 90) {
        SQUARE(R, 45, S, 1.5, 0.2, LIGHT, 0.2);
    }
    CALL(noise1, VAR, 0, 0.05);
    END
}

static void saw(noctt_turtle_t *ctx)
{
    START
    TR(HSL, 1, 0, 0, 0.5);
    STAR(8, 0.2, -0.9, LIGHT, -0.25);
    STAR(8, 0.15, -0.9, R, -4, G, -8, FLAG, FLAG_STENCIL_WRITE);
    CALL(noise1, FLAG, FLAG_EFFECT_LIGHT, 1, FLAG_STENCIL_FILTER, 1,
                 VAR, 0, 0.1);
    CIRCLE(S, 0.3, LIGHT, -1);
    END
}

static void bomb(noctt_turtle_t *ctx)
{
    const int n = 8;
    START
    TR(HSL, 1, 0, 0.5, 0.5);
    TR(S, 0.8);
    CIRCLE();
    CIRCLE(G, -1, FLAG, FLAG_STENCIL_WRITE);

    TRANSFORM(FLAG, FLAG_STENCIL_FILTER) {
        CALL(noise1, FLAG, FLAG_EFFECT_LIGHT, VAR, 0, 0.1);
    }

    TRANSFORM(LIGHT, 1) {
        FOR(n, R, 360 / n) {
            TR(X, 0.5, S, 0.2);
            TRIANGLE(LIGHT, -0.3);
            TRIANGLE(G, -1);
        }
        FOR(6, R, 360 / 6) {
            TR(R, 180 / 6, X, 0.3, S, 0.2);
            TRIANGLE(LIGHT, -0.3);
            TRIANGLE(G, -1);
        }
        TRANSFORM(S, 0.15) {
            CIRCLE(LIGHT, -0.3);
            CIRCLE(G, -0.75);
        }
    }
    END
}

static void cannon(noctt_turtle_t *ctx)
{
    START
    TR(HSL, 1, 90, 0, 0.5);

    SQUARE(Z, -0.5, FLAG, FLAG_STENCIL_WRITE, X, -0.25, LIGHT, -1);
    TRANSFORM(FLAG, FLAG_STENCIL_FILTER) {
        CIRCLE(Z, -0.5);
        CIRCLE(G, -5);
    }

    RSQUARE(4, Z, -0.5, SX, 0.4, X, 0.2);
    RSQUARE(4, SX, 0.4, X, 0.2, G, -5);

    TRANSFORM(FLAG, FLAG_STENCIL_FILTER) {
        CIRCLE(S, 0.8, LIGHT, -0.5);
    }

    TRIANGLE(S, 0.4, LIGHT, 1);
    TRIANGLE(S, 0.4, LIGHT, 1, G, -5);
    TRIANGLE(X, -0.25, S, 0.2, LIGHT, 1);
    TRIANGLE(X, -0.25, S, 0.2, LIGHT, 1, G, -5);

    END
}

void blowfish_objs(noctt_turtle_t *ctx)
{
    START
    TEXT("Some objects from Blowish Rescue", X, -0.48, Y, 0.47);
    CALL(cannon, SN, S, 0.25);
    CALL(bomb, SN, S, 0.25, X, 1.5);
    CALL(block, SN, S, 0.25, X, -1.5);
    CALL(saw, SN, S, 0.25, X, -1.5, -1.25);
    END
}


#define NOC_TURTLE_UNDEF_NAMES
#include "noc_turtle.h"

// ####### End of the rules, start of rendering code ###########

typedef struct {
    GLuint prog;
    GLuint u_proj_l;
    GLuint u_color_l;
    GLuint a_pos_l;
} gl_prog_t;

static gl_prog_t gl_prog;


static void mat_ortho(float m[16], float left, float right, float bottom,
                      float top, float near, float far)
{
    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    float tz = -(far + near) / (far - near);
    float tmp[16] = {
        2 / (right - left), 0, 0, 0,
        0, 2 / (top - bottom), 0, 0,
        0, 0, -2 / (far - near), 0,
        tx, ty, tz, 1
    };
    memcpy(m, tmp, sizeof(tmp));
}

static void mat_scale(float m[16], float x, float y, float z)
{
    m[0] *= x;   m[4] *= y;   m[8]  *= z;
    m[1] *= x;   m[5] *= y;   m[9]  *= z;
    m[2] *= x;   m[6] *= y;   m[10] *= z;
    m[3] *= x;   m[7] *= y;   m[11] *= z;
}

static void init_opengl(int w, int h)
{
    const char *vshader_src =
        "uniform     mat4 u_proj;                       \n"
        "attribute   vec3 a_pos;                        \n"
        "void main()                                    \n"
        "{                                              \n"
        "   gl_Position = u_proj * vec4(a_pos, 1.0);    \n"
        "}                                              \n"
    ;

    const char *fshader_src =
        "uniform     vec4 u_color;                      \n"
        "void main()                                    \n"
        "{                                              \n"
        "   gl_FragColor = u_color;                     \n"
        "}                                              \n"
    ;

    float mat[16];
    GLuint vshader, fshader;

    vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vshader_src, NULL);
    glCompileShader(vshader);

    fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fshader_src, NULL);
    glCompileShader(fshader);

    gl_prog.prog = glCreateProgram();
    glAttachShader(gl_prog.prog, vshader);
    glAttachShader(gl_prog.prog, fshader);
    glLinkProgram(gl_prog.prog);

    gl_prog.u_proj_l = glGetUniformLocation(gl_prog.prog, "u_proj");
    gl_prog.u_color_l = glGetUniformLocation(gl_prog.prog, "u_color");
    gl_prog.a_pos_l = glGetAttribLocation(gl_prog.prog, "a_pos");
    glEnableVertexAttribArray(gl_prog.a_pos_l);

    glViewport(0, 0, w, h);
    glUseProgram(gl_prog.prog);
    mat_ortho(mat, -w / 2, +w / 2, -h / 2, h / 2, -1, +1);
    glUniformMatrix4fv(gl_prog.u_proj_l, 1, 0, mat);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

static void hsl_to_rgb(const float hsl[3], float rgb[3])
{
    float r = 0, g = 0, b = 0, c, x, m;
    const float h = hsl[0] / 60, s = hsl[1], l = hsl[2];
    c = (1 - fabs(2 * l - 1)) * s;
    x = c * (1 - fabs(fmod(h, 2) - 1));
    if      (h < 1) {r = c; g = x; b = 0;}
    else if (h < 2) {r = x; g = c; b = 0;}
    else if (h < 3) {r = 0; g = c; b = x;}
    else if (h < 4) {r = 0; g = x; b = c;}
    else if (h < 5) {r = x; g = 0; b = c;}
    else if (h < 6) {r = c; g = 0; b = x;}
    m = l - 0.5 * c;
    rgb[0] = r + m;
    rgb[1] = g + m;
    rgb[2] = b + m;
}

static void render_callback(int n, const noctt_vec3_t *poly, const float color[4],
                            unsigned int flags, void *user_data)
{
    float rgba[4];
    static unsigned int current_flags = 0;
    assert(sizeof(float) == sizeof(GL_FLOAT));
    if (current_flags != flags) {
        if (flags & (FLAG_STENCIL_WRITE | FLAG_STENCIL_FILTER)) {
            glEnable(GL_STENCIL_TEST);
            glStencilFunc(GL_LEQUAL, 0x1,
                          flags & FLAG_STENCIL_WRITE ? 0x0 : 0x1);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        } else {
            glDisable(GL_STENCIL_TEST);
        }

        if (flags & FLAG_EFFECT_LIGHT) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
        } else {
            glDisable(GL_BLEND);
        }
        current_flags = flags;
    }
    hsl_to_rgb(color, rgba);
    rgba[3] = color[3];
    glUniform4fv(gl_prog.u_color_l, 1, rgba);
    glVertexAttribPointer(gl_prog.a_pos_l, 3, GL_FLOAT, false, 0, (void*)poly);
    glDrawArrays(GL_TRIANGLE_FAN, 0, n);
}

static struct {
    const char          *name;
    noctt_rule_func_t   rule;
} RULES[] = {
    {"press key to see more",    demo1},
    {"blowfish objs", blowfish_objs},
    {"tree" , tree_rule},
    {"modern", modern_rule},
    {"shapes",  shapes_rule},
    {"stencil", stencil_rule},
    {"colors", colors_rule},
    {"blowfish", blowfish_city_rule},
};

static const nb_rules = sizeof(RULES) / sizeof(RULES[0]);
static int rule_index = 0;
static noctt_prog_t *prog = NULL;
static GLFWwindow *window;

static void start_demo(int index)
{
    float mat[16] = {1, 0, 0, 0,
                     0, 1, 0, 0,
                     0, 0, 1, 0,
                     0, 0, 0, 1};
    int w, h;
    if (prog) noctt_prog_delete(prog);
    glfwGetWindowSize(window, &w, &h);
    mat_scale(mat, w, h, 1);
    prog = noctt_prog_create(RULES[index].rule, 256, 0, mat, 1);
    prog->render_callback = render_callback;
    prog->render_callback_data = NULL;
    glfwSetWindowTitle(window, RULES[index].name);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void key_callback(GLFWwindow* window,
                         int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS) return;
    rule_index = (rule_index + 1) % nb_rules;
    start_demo(rule_index);
}

int main()
{
    int i;
    int r;
    int w = 640, h = 480;
    float proj_mat[16];

    glfwInit();

    glfwWindowHint(GLFW_SAMPLES, 2);
    window = glfwCreateWindow(w, h, "turtles", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glewInit();

    mat_ortho(proj_mat, -w / 2, +w / 2, -h / 2, h / 2, -1, +1);
    font_init(proj_mat);
    init_opengl(w, h);
    start_demo(0);

    while (!glfwWindowShouldClose(window)) {
        glUseProgram(gl_prog.prog);

        noctt_prog_iter(prog);
        font_flush();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    noctt_prog_delete(prog);
    glfwTerminate();
}
