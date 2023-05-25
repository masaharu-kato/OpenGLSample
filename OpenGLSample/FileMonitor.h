#pragma once
#include <iostream>
#include <sys/stat.h>

class FileMonitor {
public:
    const char* filename;
    struct _stat c_stat;

    FileMonitor(const char* filename) : filename(filename) {
        _stat(filename, &c_stat);
    }

    bool isUpdated() {
        auto last_file_time = c_stat.st_mtime;
        //std::cout << "last_file_time: " << last_file_time << "\n";
        _stat(filename, &c_stat);
        return c_stat.st_mtime != last_file_time;
    }
};
