// make && ./fs -f here
#define FUSE_USE_VERSION 30

#include <fuse.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <vector>
#include <string>

#define UNUSED(x) ((void)x)

typedef std::vector<char> Bytes;
typedef std::string String;

struct File {
    String name;
    bool is_dir;

    Bytes data;
    std::vector<File*> files;

    File(const String &name, bool is_dir):
        name(name),
        is_dir(is_dir)
    {}

    File* find_non_recursive(const String &name){
        for (File *f : files){
            if (f->name == name) return f;
        }
        
        return NULL;
    }

    void add(File *file){
        files.push_back(file);
    }
};

File* find_recursive(File *directory, const String &path){
    if (!directory || path.empty()) return directory;
    
    size_t i = path.find('/');

    if (i == String::npos) return directory->find_non_recursive(path);

    String dirname = path.substr(0, i);
    String rest = path.substr(i + 1);

    directory = directory->find_non_recursive(dirname);

    return find_recursive(directory, rest);
}

File* find(File *directory, const String &path){
    if (path.empty()) return NULL;

    return find_recursive(directory, path.substr(1));
}

File *root;

static int do_getattr(const char *path, struct stat *st){
    memset(st, 0, sizeof(*st));
    st->st_uid = getuid();
    st->st_gid = getgid();

    File *f = find(root, path);

    printf("do_getattr %s: %p\n", path, (void*)f);

    if (!f) return -1;

    if (f->is_dir){
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2;
    }else{
        st->st_mode = S_IFREG | 0644;
        st->st_nlink = 1;
        st->st_size = f->data.size();
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
    printf("do_readdir %s\n", path);
    
    UNUSED(offset);
    UNUSED(fi);
    
    filler(buffer, ".", NULL, 0);
    filler(buffer, "..", NULL, 0);

    File *directory = find(root, path);

    if (directory){
        for (File *file : directory->files){
            filler(buffer, file->name.c_str(), NULL, 0);
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
    printf("do_read %s\n", path);
    
    UNUSED(fi);

    File *f = find(root, path);

    if (!f || offset < 0 || (size_t)offset > f->data.size()) return -1;

    size_t remaining = f->data.size() - offset;

    if (size > remaining) size = remaining;

    memcpy(buffer, f->data.data() + offset, size);
    
    return size;
}

File* make_text_file(const String &name, const String &text){
    File *file = new File(name, false);

    file->data = Bytes(text.data(), text.data() + text.size());

    return file;
}

int main(int argc, char *argv[]){
    struct fuse_operations operations;
    memset(&operations, 0, sizeof(operations));
    
    operations.getattr = do_getattr;
    operations.readdir = do_readdir;
    operations.read = do_read;
    
    root = new File("/", true);

    root->add(make_text_file("foo.txt", "Hello, World!\n"));

    File *dir = new File("bar", true);

    root->add(dir);

    dir->add(make_text_file("bar.txt", "Content of file bar.txt"));
    
    return fuse_main(argc, argv, &operations, NULL);
}
