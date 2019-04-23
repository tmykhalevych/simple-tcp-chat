#pragma once
#include "config.h"
#include "logger.h"

namespace infra {
    class dynamic_buffer
        : private Loggable
    {
        LOG_MODULE("BUF")
    public:
        dynamic_buffer();
        ~dynamic_buffer();

        void realloc(int32_t new_size);

        char* begin() noexcept { return _beg_ptr; }
        int32_t size() const noexcept { return _size; }

    private:
        char* _beg_ptr;
        int32_t _size;
        static const int _small_buff_size = _DYNAMIC_BUFF_SMALLBUFF_SIZE_;
        char _small_buff[_small_buff_size];
        bool _dynamic;
    };
}
