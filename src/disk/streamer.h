#ifndef DISKSTREAMR_H
#define DISKSTREAMR_H

#include "disk.h"

struct disk_stream {
    int pos;
    struct disk* disk;
};

struct disk_stream* diskstream_new(int disk_id);
int diskstream_seek(struct disk_stream* stream, int pos);
int diskstream_read(struct disk_stream* stream, void* out, int total);
void diskstream_close(struct disk_stream* stream);

#endif