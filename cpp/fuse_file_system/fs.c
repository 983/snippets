// make && ./fs -f here
#define FUSE_USE_VERSION 30
#include <fuse.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#define UNUSED(x) ((void)x)
#define MAX_FILES 128

typedef struct File File;

struct File {
    char *name;
    size_t name_size;
    int is_dir;
    char *data;
    size_t size;
    File *files[MAX_FILES];
    size_t n_files;
};

void add(File *directory, File *file){
    assert(directory->n_files < MAX_FILES);
    directory->files[directory->n_files++] = file;
}

File* find_non_recursive(File *directory, const char *name, size_t name_size){
    for (size_t i = 0; i < directory->n_files; i++){
        File *file = directory->files[i];

        if (name_size == file->name_size && 0 == memcmp(name, file->name, name_size)) return file;
    }

    return NULL;
}

File* find_recursive(File *directory, const char *path){
    if (!directory || !path || *path == '\0') return directory;

    char *sep = strchr(path, '/');

    if (!sep) return find_non_recursive(directory, path, strlen(path));

    char *rest = sep + 1;
    size_t dirname_size = sep - path;

    directory = find_non_recursive(directory, path, dirname_size);

    return find_recursive(directory, rest);
}

File* find(File *directory, const char *path){
    if (path && *path == '/') path++;
    
    return find_recursive(directory, path);
}

File *root;

static int do_getattr(const char *path, struct stat *st){
    memset(st, 0, sizeof(*st));
    st->st_uid = getuid();
    st->st_gid = getgid();

    File *f = find(root, path);

    if (!f) return -1;

    if (f->is_dir){
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2;
    }else{
        st->st_mode = S_IFREG | 0644;
        st->st_nlink = 1;
        st->st_size = f->size;
    }

    return 0;
}

static int do_readdir(
    const char *path,
    void *buffer,
    fuse_fill_dir_t filler,
    off_t offset,
    struct fuse_file_info *fi
){
    UNUSED(offset);
    UNUSED(fi);
    
    filler(buffer, ".", NULL, 0);
    filler(buffer, "..", NULL, 0);

    File *directory = find(root, path);

    if (directory){
        for (size_t i = 0; i < directory->n_files; i++){
            File *file = directory->files[i];
            
            filler(buffer, file->name, NULL, 0);
        }
    }
    
    return 0;
}

static int do_read(
    const char *path,
    char *buffer,
    size_t size,
    off_t offset,
    struct fuse_file_info *fi
){
    UNUSED(fi);

    File *f = find(root, path);

    if (!f || offset < 0 || (size_t)offset > f->size) return -1;

    size_t remaining = f->size - offset;

    if (size > remaining) size = remaining;

    memcpy(buffer, f->data + offset, size);
    
    return size;
}

char* substr(const char *c, size_t n){
    char *s = calloc(n + 1, 1);
    memcpy(s, c, n);
    return s;
}

File* make_file(const char *name){
    File *file = calloc(1, sizeof(*file));

    file->name_size = strlen(name);
    file->name = substr(name, file->name_size);

    return file;
}

File* make_text_file(const char *name, const char *text){
    File *file = make_file(name);

    file->size = strlen(text);
    file->data = substr(text, file->size);
    
    return file;
}

File* make_dir(const char *name){
    File *file = make_file(name);

    file->is_dir = 1;
    
    return file;
}

int main(int argc, char *argv[]){
    struct fuse_operations operations;
    memset(&operations, 0, sizeof(operations));
    
    operations.getattr = do_getattr;
    operations.readdir = do_readdir;
    operations.read = do_read;

    root = make_dir("");

    add(root, make_text_file("foo.txt", "Hello, World!\n"));
    
    File *bar = make_dir("bar");

    add(root, bar);

    add(bar, make_text_file("bar.txt", "Content of file bar.txt"));
    
    return fuse_main(argc, argv, &operations, NULL);
}
