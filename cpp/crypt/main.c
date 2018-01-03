#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "aes128ctr.h"
#include "sha256.h"
#include "filesystem.h"
#include "str.h"

struct encrypt_state {
    FILE *encrypted;
    struct aes128ctr aes[1];
    struct sha256 sha_all[1];
    uint8_t buffer[1024*1024];
    uint64_t offset;
    
    str index[1];
};

void encrypt_file(const char *path, void *user_data){
    struct encrypt_state *s = (struct encrypt_state*)user_data;
    
    if (0 == strcmp(path, "./encrypted")) return;
    
    FILE *fp = fopen(path, "rb");
    
    if (!fp){
        printf("Failed to open file %s\n", path);
        return;
    }
    
    struct sha256 sha_file[1];
    sha256_init(sha_file);
    
    uint64_t n_bytes = 0;
    
    while (1){
        size_t n = fread(s->buffer, 1, sizeof(s->buffer), fp);
        
        if (n <= 0) break;
        
        sha256_append(sha_file, s->buffer, n);
        aes128ctr_crypt(s->aes, s->buffer, n);
        sha256_append(s->sha_all, s->buffer, n);
        
        fwrite(s->buffer, 1, n, s->encrypted);
        
        n_bytes += n;
    }
    
    uint8_t hash[32];
    
    sha256_digest(sha_file, hash, sizeof(hash));
    
    str_append(s->index, path);                   str_append(s->index, "\n");
    str_append_u64(s->index, s->offset);          str_append(s->index, "\n");
    str_append_u64(s->index, n_bytes);            str_append(s->index, "\n");
    str_append_hex(s->index, hash, sizeof(hash)); str_append(s->index, "\n");
    str_append(s->index, "\n");
    
    /*
    fprintf(s->index, "%s\n", path);
    fprintf(s->index, "%I64u\n", s->offset);
    fprintf(s->index, "%I64u\n", n_bytes);
    for (int i = 0; i < 8; i++){
        fprintf(s->index, "%08x", sha_file->state[i]);
    }
    fprintf(s->index, "\n");
    fprintf(s->index, "\n");
    */
    fclose(fp);
    
    s->offset += n_bytes;
}

int main(){
    
    uint8_t key[3] = {1, 2, 3};
    uint8_t salt[4] = {4, 5, 6, 7};
    
    uint8_t derived_key[256];
    
    pbkdf2_sha256(
        derived_key, sizeof(derived_key), 
        key, sizeof(key),
        salt, sizeof(salt),
        4096);
    
    return 0;
}
/*
int main(){
    
    struct aes128ctr aes[1];
    aes128ctr_init(aes, key, sizeof(key), nonce, sizeof(nonce));
    
    struct sha256 sha[1];
    sha256_init(sha);
    
    FILE *fsrc = fopen("garbage/eax.pdf", "rb");
    FILE *fdst = fopen("encrypted", "wb");
    
    uint8_t buffer[64*1024];
    while (1){
        size_t n = fread(buffer, 1, sizeof(buffer), fsrc);
        
        if (n <= 0) break;
        
        aes128ctr_crypt(aes, buffer, n);
        sha256_append(sha, buffer, n);
        
        fwrite(buffer, 1, n, fdst);
    }
    
    uint8_t hash[32];
    sha256_digest(sha, hash, sizeof(hash));
    
    fwrite(hash, 1, sizeof(hash), fdst);
    
    fclose(fsrc);
    fclose(fdst);
    
    return 0;
}
*/
#include "aes128ctr.c"
#include "sha256.c"
#include "filesystem.c"
#include "str.c"

