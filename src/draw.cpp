#include "draw.h"

#include <XPLMDisplay.h>
#include <XPLMGraphics.h>
#include <XPLMUtilities.h>

constexpr auto SURFACE_WIDTH = 500;
constexpr auto SURFACE_HEIGHT = 500;
constexpr auto SURFACE_X_OFFSET = 25;
constexpr auto SURFACE_Y_OFFSET = 450;

constexpr PanelDims dims = {SURFACE_WIDTH, SURFACE_HEIGHT, SURFACE_X_OFFSET, SURFACE_Y_OFFSET};

Draw::Draw() {
    GLenum err = glewInit();

    if (err != GLEW_OK) {
        XPLMDebugString("[cairoxp]: Failed to initialize glew\n");
        std::abort();
    }

    cairo_mt = std::make_unique<CairoMt>([this](cairo_t *cr) { cairo_draw_start(cr); },
        [this](cairo_t *cr) { cairo_draw_loop(cr); }, [this](cairo_t *cr) { cairo_draw_stop(cr); },
        dims);
}

// Important: cairo_draw_* functions are called from the render thread.
// X-Plane SDK functions CAN NOT be called on threads other than the main one
// Do not call X-Plane SDK functions from these (3) functions.
void Draw::cairo_draw_start([[maybe_unused]] cairo_t *cr) {}

void Draw::cairo_draw_loop(cairo_t *cr) {
    // Clear surface
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
    cairo_rectangle(cr, 50, 50, 400, 400);
    cairo_fill(cr);

    cairo_set_source_rgb(cr, 0.0, 1.0, 0.0);
    cairo_rectangle(cr, 50, 50, 400, 400);
    cairo_stroke(cr);

    cairo_select_font_face(cr, "serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size(cr, 32.0);
    cairo_set_source_rgb(cr, 0.0, 0.0, 1.0);
    cairo_move_to(cr, 100, 100);
    cairo_show_text(cr, "Hello, world");
}

void Draw::cairo_draw_stop([[maybe_unused]] cairo_t *cr) {}
