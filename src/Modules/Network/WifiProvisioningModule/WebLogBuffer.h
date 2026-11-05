#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

// Fixed-size recent-log ring buffer. Callers serialize append/read operations.
class WebLogBuffer {
public:
    static constexpr size_t LineSize = 224U;
    static constexpr size_t Capacity = 8U;

    struct ReadResult {
        uint32_t cursor;
        uint32_t lost;
    };

    void append(const char* line)
    {
        if (!line) return;
        const size_t length = strnlen(line, LineSize - 2U);
        memcpy(lines_[head_], line, length);
        lines_[head_][length] = '\n';
        lines_[head_][length + 1U] = '\0';
        head_ = (head_ + 1U) % Capacity;
        if (count_ < Capacity) ++count_;
        ++sequence_;
    }

    ReadResult read(uint32_t cursor, char* output, size_t capacity) const
    {
        if (!output || capacity == 0U) return {cursor, 0U};
        uint32_t available = sequence_ - cursor;
        uint32_t lost = 0U;
        if (available > count_) {
            lost = available - count_;
            available = count_;
            cursor = sequence_ - available;
        }

        size_t bytes = 0U;
        while (available > 0U) {
            const size_t index = (head_ + Capacity - available) % Capacity;
            const size_t length = strlen(lines_[index]);
            if (length >= capacity - bytes) break;
            memcpy(output + bytes, lines_[index], length);
            bytes += length;
            ++cursor;
            --available;
        }
        output[bytes] = '\0';
        return {cursor, lost};
    }

private:
    char lines_[Capacity][LineSize]{};
    size_t head_ = 0U;
    uint32_t count_ = 0U;
    uint32_t sequence_ = 0U;
};
