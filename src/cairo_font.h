#ifndef CAIRO_FONT_H_
#define CAIRO_FONT_H_

#include <cairo/cairo-ft.h>

#include <string>
#include <vector>

class CairoFontLoader final {
public:
    using font_type = FT_Byte;
    CairoFontLoader(const std::vector<font_type> &fdata);
    CairoFontLoader(const std::string &filename);
    CairoFontLoader(const CairoFontLoader &fl) = delete;
    CairoFontLoader(CairoFontLoader &&fl) = delete;
    CairoFontLoader &operator=(const CairoFontLoader &fl) = delete;
    CairoFontLoader &operator=(CairoFontLoader &&fl) = delete;
    explicit operator cairo_font_face_t *() const noexcept;
    cairo_font_face_t *font_face() const noexcept;
    ~CairoFontLoader();

private:
    void font_loader_create();
    void set_font_data(const std::vector<font_type> &fdata);
    cairo_font_face_t *font{nullptr};
    FT_Library ft_lib;
    FT_Face ft_face;
    std::vector<font_type> font_data;
};

#endif /* CAIRO_FONT_H_ */