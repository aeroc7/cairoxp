#include "gl_pbo.h"

PboHdlr::PboHdlr(GLsizei pwidth, GLsizei pheight) : width(pwidth), height(pheight) {
    glGenBuffers(2, pbo_bufs);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_bufs[0]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 4, nullptr, GL_DYNAMIC_DRAW);

    cur_buffer_handle = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    mt.back_buf_index = 0;
    mt.front_buf_index = 1;
    mt.back_buffer_ready = true;
    mt.back_buffer_done = false;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_bufs[1]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 4, nullptr, GL_DYNAMIC_DRAW);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void *PboHdlr::get_back_buffer() {
    std::lock_guard<std::mutex> lock(mut);
    if (mt.back_buffer_ready) {
        return cur_buffer_handle;
    }
    return nullptr;
}

void PboHdlr::finish_back_buffer() {
    std::lock_guard<std::mutex> lock(mut);
    mt.back_buffer_done = true;
    mt.back_buffer_ready = false;
}

void PboHdlr::bind_front_buffer() {
    bool us_back_buf_done;
    bool us_back_buf_ready;
    GLuint us_back_index;
    GLuint us_front_index;

    {
        std::lock_guard<std::mutex> lock(mut);
        us_back_buf_done = mt.back_buffer_done;
        us_back_buf_ready = mt.back_buffer_ready;
        us_back_index = mt.back_buf_index;
        us_front_index = mt.front_buf_index;
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_bufs[us_front_index]);

    if (us_back_buf_done) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_bufs[us_back_index]);
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

        us_back_index = (us_back_index + 1) % 2;
        us_front_index = (us_front_index + 1) % 2;

        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbo_bufs[us_back_index]);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, width * height * 4, nullptr, GL_DYNAMIC_DRAW);
        cur_buffer_handle = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

        us_back_buf_done = false;
        us_back_buf_ready = true;

        {
            std::lock_guard<std::mutex> lock(mut);
            mt.back_buffer_done = us_back_buf_done;
            mt.back_buffer_ready = us_back_buf_ready;
            mt.back_buf_index = us_back_index;
            mt.front_buf_index = us_front_index;
        }
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

PboHdlr::~PboHdlr() {
    glDeleteBuffers(2, pbo_bufs);
    cur_buffer_handle = nullptr;
}