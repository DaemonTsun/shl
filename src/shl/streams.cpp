
#include <assert.h>
#include "shl/streams.hpp"

u64 read_entire_file(const char *path, memory_stream *out)
{
    assert(path != nullptr);
    assert(out != nullptr);

    file_stream fstream;
    open(&fstream, path, MODE_READ, false, false);
    u64 ret = read_entire_file(&fstream, out);
    close(&fstream);
    
    return ret;
}

u64 read_entire_file(file_stream *stream, memory_stream *out)
{
    assert(stream != nullptr);
    assert(out != nullptr);

    u64 sz = calculate_file_size(stream);
    open(out, sz, true, true);
    return read_entire_file(stream, out->data, sz);
}
