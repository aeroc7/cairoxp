#ifndef CAIRO_MT_H_
#define CAIRO_MT_H_

#include <XPLMDisplay.h>
#include <cairo/cairo.h>

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>

#include "gl_pbo.h"

struct PanelDims {
    int width{};
    int height{};
    int x{}, y{};
};

class CairoMt {
public:
    using CairoMtCb = std::function<void(cairo_t *cr)>;
    CairoMt(CairoMtCb fstart, CairoMtCb floop, CairoMtCb fstop, PanelDims dims);
    void cairo_mt_draw();  // Main (xplane) thread
    ~CairoMt();

private:
    static int xplane_draw_callback(XPLMDrawingPhase, int, void *);
    CairoMtCb start;
    CairoMtCb loop;
    CairoMtCb stop;
    void render_loop();
    std::thread render_thread;
    std::atomic_bool quit_thread{false};
    int surf_width{}, surf_height{};
    std::mutex render_mutex;
    std::unique_ptr<PboHdlr> pbo;
    unsigned gl_tex_id{};
    unsigned pan_x_offset{}, pan_y_offset{};
};

#endif /* CAIRO_MT_H_ */