#include "filesystem.h"

#include <windows.h>

#define MAX_PATH_LENGTH 32768

#define REGULAR_FILE_FLAGS (FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE)

int is_directory(const char *path){
    return (GetFileAttributes(path) & FILE_ATTRIBUTE_DIRECTORY) ? 1 : 0;
}

int is_file(const char *path){
    return (GetFileAttributes(path) & REGULAR_FILE_FLAGS) ? 1 : 0;
}

void list_files_recursively(
    const char *directory,
    void (*path_callback)(const char *path, void *user_data),
    void *user_data
){
    WIN32_FIND_DATA file;
    file.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    
    char directory_star[MAX_PATH_LENGTH];
    snprintf(directory_star, sizeof(directory_star), "%s\\*", directory);
    
    HANDLE hFind = FindFirstFileEx(
        directory_star,
        FindExInfoBasic,
        &file,
        FindExSearchNameMatch,
        NULL,
        FIND_FIRST_EX_LARGE_FETCH);
    
    if (hFind != INVALID_HANDLE_VALUE){
        do  {
            // skip funny directories
            if (0 == strcmp(file.cFileName, ".")) continue;
            if (0 == strcmp(file.cFileName, "..")) continue;
            
            // join directory and path
            char path[MAX_PATH_LENGTH];
            snprintf(path, sizeof(path), "%s/%s", directory, file.cFileName);
            
            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
                list_files_recursively(path, path_callback, user_data);
            }
            
            if (file.dwFileAttributes & REGULAR_FILE_FLAGS){
                //uint64_t filesize = file.nFileSizeHigh;
                //filesize = (filesize << 32) | file.nFileSizeLow;
                path_callback(path, user_data);
            }
            
        } while(FindNextFile(hFind, &file));
    }
    
    FindClose(hFind);
}
