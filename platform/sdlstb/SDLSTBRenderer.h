/*
 * This file is part of NumptyPhysics <http://thp.io/2015/numptyphysics/>
 * Coyright (c) 2014 Thomas Perl <m@thp.io>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#ifndef NUMPTYPHYSICS_SDLSTBRENDERER_H
#define NUMPTYPHYSICS_SDLSTBRENDERER_H

#include <map>
#include <string>

#include "Renderer.h"
#include "GLRenderer.h"

#include <SDL.h>

class SDLSTBRenderer : public GLRenderer {
public:
    SDLSTBRenderer(Vec2 world_size);
    ~SDLSTBRenderer();

    virtual NP::Texture load(const char *filename, bool cache);

    virtual NP::Font load(const char *filename, int size);

    virtual void metrics(const NP::Font &font, const char *text, int *width, int *height);
    virtual NP::Texture text(const NP::Font &font, const char *text, int rgb);

    virtual void swap();

private:
    SDL_Surface *m_surface;
    std::map<std::string, NP::Texture> m_texture_cache;
};

#endif /* NUMPTYPHYSICS_SDLSTBRENDERER_H */
