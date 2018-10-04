#include "HelloLibrary.h"
#include <dlfcn.h>
#include <stdio.h>
#include "common.h"

#define READ_BUFFER 1024
#define wav "./EE45AEAAD1A31B1B1A45F4B38C98BE62893E590A47C1166061B0B1C52163531C.wav"
typedef void (*CALL)(const char*);
typedef bool (*BKK)(struct Session *session, const char *path_to_config);
typedef bool (*BKKCheck)(struct Session *session, const uint8_t * content, uint64_t content_size);

int main(void)
{
    libHello();
    loadlibcv();
    return 0;
}

int libHello()
{
 void* handle = dlopen("./libHelloLibrary.so", RTLD_LAZY);
    if (!handle)
      return 1;
    CALL hello = (CALL)(dlsym(handle,"SayHello"));
    if (!hello) {
        printf("Could not find symbol SayHello");
        dlclose(handle);
        return 1;
    }
    hello("Roman");
    dlclose(handle);
    return 0;
}

int loadlibcv()
{
    void* handle = dlopen("./libcv.so", RTLD_LAZY);
    if (!handle)
        return 1;
    BKK load = (BKK)(dlsym(handle, "v_create_session"));
    if (!load)
    {
        return 2;
    }
    struct Session sess;
    sess.id="my_session";
    if (!load(&sess, "./cv_config.json")){
        printf("error create");
        return 3;
    }
    uint8_t array[40000];
    printf("create session succesfully!\n");
    uint8_t **content;
    uint8_t * direct;
    FILE *fd = fopen("./7.wav", "rb");
    uint64_t *content_size;
    if (fd != NULL) {
        fseek(fd, 0L, SEEK_END);
        uint64_t size = ((size_t) ftell(fd) );
        printf("%d\n", size);
        content_size = & size;
        printf("sucess open\n");
        rewind(fd);
        uint8_t * pointer =(uint8_t *) calloc(1, (*content_size));
        direct = pointer;
        content = & pointer;
        printf("%d\n", pointer);
        fread((*content), (*content_size), 1, fd);
        fclose(fd);
        printf("sucess\n");
    }
    BKKCheck v_check = (BKKCheck)(dlsym(handle,"v_check"));
    if (!v_check) {
        printf("Could not find symbol v_check");
        dlclose(handle);
        return 1;
    }
    void * pointSession = &sess;
    printf("size = %d ",*content_size );
    if (!v_check(pointSession, direct, *content_size)){
        printf("FAILED!");
    }
    printf("error code= %d", sess.last_error);
    return 0;
}


bool read_file_content(const char *file_path, uint8_t **content, uint64_t *content_size)
{
    FILE *fd = fopen(file_path, "rb");
    if (fd != NULL) {
        fseek(fd, 0L, SEEK_END);
        (*content_size) = (size_t) ftell(fd);
        rewind(fd);
        (*content) = (uint8_t *) calloc(1, (*content_size));
        fread((*content), (*content_size), 1, fd);
        fclose(fd);
        return true;
    }
    return false;
}

bool free_file_content(uint8_t **content)
{
    if (content && (*content)) {
        free((*content));
        (*content) = NULL;
    }
    return true;
}

int read_from_fd(FILE *fd,
             const char *configuration,
             CREATE_SESSION create_session,
             DESTROY_SESSION destroy_session,
             CHECK_SESSION check_session) {
    struct Session session;
    int ret = 1;
    uint32_t frame_size = 0;
    uint8_t *frame_content = NULL;

    if (configuration == NULL)
        return -1;
    if (!(*create_session)(&session, configuration)) {
        return -2;
    }
    if (fread(&frame_size, 1, sizeof(uint32_t), fd) == sizeof(uint32_t) && frame_size > 0) {

        frame_content = (uint8_t *) calloc(1, frame_size);
        if (frame_size < READ_BUFFER) {
            if (fread(frame_content, 1, frame_size, fd) != frame_size) {
                ret = -4;
            }
        } else {
            size_t read = 0;
            size_t already_read = 0;

            while ((read = fread(frame_content + already_read, 1, READ_BUFFER, fd)) > 0) {
                already_read += read;
                if (already_read >= frame_size)
                    break;
            }
            if (already_read != frame_size)
                return -4;
        }
        if (ret > 0) {
            (*check_session)(&session, frame_content, frame_size);
            ret = session.last_error;
        }
        free(frame_content);
    } else {
        ret = -3;
    }
    (*destroy_session)(&session);
    return ret;
}

