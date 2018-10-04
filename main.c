#include "HelloLibrary.hpp"
#include <dlfcn.h>
#include <stdio.h>
typedef void (*CALL)(const char*);

int main(void) {
    void* handle = dlopen("./libHelloLibrary.so", RTLD_LAZY);
    if (!handle) {
       printf("Could not open the library\n");
       return 1;
    }

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


