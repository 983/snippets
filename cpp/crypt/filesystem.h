#pragma once

int is_directory(const char *path);
int is_file(const char *path);

void list_files_recursively(
    const char *directory,
    void (*path_callback)(const char *path, void *user_data),
    void *user_data
);
