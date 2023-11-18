
#include <assert.h>
#include "shl/streams.hpp"
#include "shl/defer.hpp"

bool read_entire_file(const char *path, memory_stream *out, error *err)
{
    assert(path != nullptr);
    assert(out != nullptr);

    file_stream fstream{};
    defer { close(&fstream); };

    if (!open(&fstream, path, MODE_READ, false, false, err))
        return false;

    if (!read_entire_file(&fstream, out, err))
        return false;
    
    return true;
}

bool read_entire_file(file_stream *stream, memory_stream *out, error *err)
{
    assert(stream != nullptr);
    assert(out != nullptr);

    u64 sz = calculate_file_size(stream);

    if (sz == 0)
        return true;

    if (!open(out, sz, true, true, err))
        return false;

    if (read_entire_file(stream, out->data, sz, err) == 0)
        return false;

    return true;
}

bool read_entire_file(const char *path, string *out, error *err)
{
    assert(path != nullptr);
    assert(out != nullptr);

    file_stream fstream{};
    defer { close(&fstream); };

    if (!open(&fstream, path, MODE_READ, false, false, err))
        return false;

    if (!read_entire_file(&fstream, out, err))
        return false;
    
    return true;
}

bool read_entire_file(file_stream *stream, string *out, error *err)
{
    assert(stream != nullptr);
    assert(out != nullptr);

    u64 sz = calculate_file_size(stream);

    if (sz == 0)
        return true;

    init(out, sz);

    if (read_entire_file(stream, out->data, sz, err) == 0)
        return false;

    return true;
}
