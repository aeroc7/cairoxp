#ifndef GL_PBO_H_
#define GL_PBO_H_

#include <GL/glew.h>

#include <mutex>

class PboHdlr {
public:
    PboHdlr(GLsizei pwidth, GLsizei pheight);
    void *get_back_buffer();
    void finish_back_buffer();
    void bind_front_buffer();
    ~PboHdlr();

private:
    GLsizei width{}, height{};
    void *cur_buffer_handle{nullptr};
    GLuint pbo_bufs[2];

    struct {
        GLuint back_buf_index;
        GLuint front_buf_index;
        bool back_buffer_ready;
        bool back_buffer_done;
    } mt;

    std::mutex mut;
};

#endif /* GL_PBO_H_ */