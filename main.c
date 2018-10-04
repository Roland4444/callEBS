#include "HelloLibrary.h"
#include <dlfcn.h>
#include <stdio.h>
#include "common.h"
typedef void (*CALL)(const char*);
typedef bool (*BKK)(struct Session *session, const char *path_to_config);
typedef void (*BKKCheck)(struct Session *session, const uint8_t * content, uint64_t content_size);

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
    printf("create succesfully");
    return 0;
}

