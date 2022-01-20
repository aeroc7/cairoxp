#include "cairo_mt.h"

#include <XPLMGraphics.h>

#include <chrono>
#include <cstring>

constexpr auto RENDER_FPS_TARGET = 30;
constexpr auto RENDER_FPS_TARGET_MS = (1000.0 / RENDER_FPS_TARGET);

class CairoSufaceHandler {
public:
    CairoSufaceHandler(int w, int h) {
        surface = cairo_image_surface_create(SURFACE_FORMAT, w, h);
        cr = cairo_create(surface);
    }

    cairo_t *get() const noexcept { return cr; }
    cairo_surface_t *get_surface() const noexcept { return surface; }
    operator cairo_t *() const noexcept { return get(); }

    ~CairoSufaceHandler() {
        cairo_destroy(cr);
        cairo_surface_destroy(surface);

        cr = nullptr;
        surface = nullptr;
    }

private:
    static constexpr auto SURFACE_FORMAT = CAIRO_FORMAT_ARGB32;
    cairo_t *cr{nullptr};
    cairo_surface_t *surface{nullptr};
};

CairoMt::CairoMt(CairoMtCb fstart, CairoMtCb floop, CairoMtCb fstop, PanelDims dims)
    : start(fstart),
      loop(floop),
      stop(fstop),
      surf_width(dims.width),
      surf_height(dims.height),
      pan_x_offset(dims.x),
      pan_y_offset(dims.y) {
    pbo = std::make_unique<PboHdlr>(surf_width, surf_height);
    render_thread = std::thread(&CairoMt::render_loop, this);
    XPLMRegisterDrawCallback(xplane_draw_callback, xplm_Phase_Gauges, 0, this);

    // Generate texture to blit into X-Plane
    glGenTextures(1, &gl_tex_id);
    glBindTexture(GL_TEXTURE_2D, gl_tex_id);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA8, surf_width, surf_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void CairoMt::render_loop() {
    using namespace std::chrono;
    CairoSufaceHandler surf{surf_width, surf_height};

    start(surf);
    while (!quit_thread) {
        auto t1 = steady_clock::now();

        // Clear surface
        cairo_set_source_rgb(surf, 0, 0, 0);
        cairo_paint(surf);

        loop(surf);
        auto t2 = steady_clock::now();

        const auto render_time_millis = duration_cast<milliseconds>(t2 - t1).count();
        // Subtract render time from sleep time to maintain constant frame rate.
        // If render time is longer than sleep time, then sleep for just sleep time
        const auto sleep_time =
            (static_cast<double>(render_time_millis) > RENDER_FPS_TARGET_MS
                    ? 0
                    : (RENDER_FPS_TARGET_MS - static_cast<double>(render_time_millis)));
        std::this_thread::sleep_for(milliseconds(static_cast<long>(sleep_time)));

        const auto stride = cairo_image_surface_get_stride(surf.get_surface());
        void *surf_data = cairo_image_surface_get_data(surf.get_surface());
        void *pbo_buf = pbo->get_back_buffer();

        if (surf_data && pbo_buf) {
            std::memcpy(pbo_buf, surf_data, static_cast<std::size_t>(stride * surf_height));
            pbo->finish_back_buffer();
        }
    }
    stop(surf);
}
int CairoMt::xplane_draw_callback(XPLMDrawingPhase, int, void *ref) {
    const CairoMt *inst = reinterpret_cast<CairoMt *>(ref);

    XPLMSetGraphicsState(0, 1, 0, 0, 1, 0, 0);
    glBindTexture(GL_TEXTURE_2D, inst->gl_tex_id);
    inst->pbo->bind_front_buffer();

    const int x1 = inst->pan_x_offset;
    const int y1 = inst->pan_y_offset;
    const int x2 = x1 + inst->surf_width;
    const int y2 = y1 + inst->surf_height;

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex2i(x1, y1);
    glTexCoord2f(0, 0);
    glVertex2i(x1, y2);
    glTexCoord2f(1, 0);
    glVertex2i(x2, y2);
    glTexCoord2f(1, 1);
    glVertex2i(x2, y1);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);

    return 1;
}

CairoMt::~CairoMt() {
    XPLMUnregisterDrawCallback(xplane_draw_callback, xplm_Phase_Gauges, 0, this);
    quit_thread = true;
    render_thread.join();
    pbo.reset();
}