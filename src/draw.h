#ifndef DRAW_H_
#define DRAW_H_

#include <XPLMDisplay.h>
#include <cairo/cairo.h>

#include <memory>

#include "cairo_mt.h"

class Draw {
public:
    Draw();
    ~Draw() = default;

private:
    void cairo_draw_start(cairo_t *cr);
    void cairo_draw_loop(cairo_t *cr);
    void cairo_draw_stop(cairo_t *cr);

    std::unique_ptr<CairoMt> cairo_mt;
};

#endif /* DRAW_H_ */