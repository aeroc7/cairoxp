#include "cairo_font.h"

#include <fstream>
#include <limits>
#include <stdexcept>

CairoFontLoader::CairoFontLoader(const std::vector<unsigned char> &fdata) {
    font_data = fdata;
    font_loader_create();
}

CairoFontLoader::CairoFontLoader(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary);

    auto get_file_size = [](std::ifstream &f) {
        f.ignore(std::numeric_limits<std::streamsize>::max());
        const std::streamsize len = f.gcount();
        f.clear();
        f.seekg(0, std::ios_base::beg);
        return len;
    };

    const auto buf_size = get_file_size(file);

    font_data.resize(buf_size);
    file.read(reinterpret_cast<char *>(font_data.data()), buf_size);

    // Important: verify stream read correctly
    if (file.gcount() == 0) {
        // No bytes read
        throw std::runtime_error("Failed to read data from file stream");
    }

    font_loader_create();
}

cairo_font_face_t *CairoFontLoader::font_face() const noexcept { return font; }

CairoFontLoader::operator cairo_font_face_t *() const noexcept { return font_face(); }

void CairoFontLoader::set_font_data(const std::vector<font_type> &fdata) { font_data = fdata; }

void CairoFontLoader::font_loader_create() {
    if (font_data.size() == 0) {
        throw std::runtime_error("No font data");
    }

    FT_Init_FreeType(&ft_lib);
    FT_New_Memory_Face(ft_lib, font_data.data(), font_data.size(), 0, &ft_face);

    font = cairo_ft_font_face_create_for_ft_face(ft_face, 0);
    // Check for font errors
}

CairoFontLoader::~CairoFontLoader() {
    cairo_font_face_destroy(font);
    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft_lib);
}
