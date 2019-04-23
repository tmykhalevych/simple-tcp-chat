#include "dynamic_buff.h"
#include <cstring>
#include <sstream>

namespace infra {
    dynamic_buffer::dynamic_buffer()
        : _beg_ptr(nullptr)
        , _size(0)
        , _dynamic(false)
    {
        LOG_SCOPE
        LOG_MSG("Create dynamic buffer object")
        memset(_small_buff, 0, dynamic_buffer::_small_buff_size);
    }

    dynamic_buffer::~dynamic_buffer() {
        LOG_SCOPE
        LOG_MSG("Deleting dynamic buffer object")
        if (_dynamic) {
            free(_beg_ptr);
        }
    }

    void dynamic_buffer::realloc(int32_t new_size) {
        LOG_SCOPE
        if (new_size >= dynamic_buffer::_small_buff_size) {
            if (_dynamic) {
                _beg_ptr = (char*)::realloc(_beg_ptr, new_size);
            }
            else {
                _dynamic = true;
                _beg_ptr = (char*)malloc(new_size);
                memset(_small_buff, 0, dynamic_buffer::_small_buff_size);
            }
        }
        else {
            if (_dynamic) {
                free(_beg_ptr);
            }
            _dynamic = false;
            _beg_ptr = _small_buff;
            memset(_small_buff, 0, dynamic_buffer::_small_buff_size);
        }
        _size = new_size;

        std::ostringstream oss;
        oss << "Realloc new buffer: state - " << (_dynamic?"DYNAMIC":"STATIC") << ", size - " << _size;
        LOG_MSG(oss.str())
    }
}
