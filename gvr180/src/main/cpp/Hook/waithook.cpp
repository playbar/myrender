
#include <Base/MojingLog.h>
#include "waithook.h"

bool phrase_dev_num(char *devno, int *pmajor, int *pminor)
{
    *pmajor = 0;
    *pminor = 0;
    if (devno != NULL) {
        char *second = strstr(devno, ":");
        if (second != NULL) {
            *pmajor = strtoul(devno + 0, NULL, 16);
            *pminor = strtoul(second + 1, NULL, 16);
            return true;
        }
    }
    return false;
}


bool phrase_proc_base_addr(char *addr, void **pbase_addr, void **pend_addr) {
    char *split = strchr(addr, '-');
    if (split != NULL) {
        if (pbase_addr != NULL) {
            *pbase_addr = (void *) strtoul(addr, NULL, 16);
        }
        if (pend_addr != NULL) {
            *pend_addr = (void *) strtoul(split + 1, NULL, 16);
        }
        return true;
    }
    return false;
}


void* getimagebase(const char *target) {
    FILE *fd = fopen("/proc/self/maps", "r");
    if (fd != NULL) {
        char buff[2048 + 1];
        while (fgets(buff, 2048, fd) != NULL) {
            const char *sep = "\t \r\n";
            char *line = NULL;
            char *addr = strtok_r(buff, sep, &line);
            if (!addr) {
                continue;
            }

            char *flags = strtok_r(NULL, sep, &line);
            if (!flags || flags[0] != 'r' || flags[3] == 's') {
                //
                /*
                    1. mem section cound NOT be read, without 'r' flag.
                    2. read from base addr of /dev/mail module would crash.
                       i dont know how to handle it, just skip it.

                       1f5573000-1f58f7000 rw-s 1f5573000 00:0c 6287 /dev/mali0

                 */
                continue;
            }
            strtok_r(NULL, sep, &line);  // offsets
            char *dev = strtok_r(NULL, sep, &line);  // dev number.
            int major = 0, minor = 0;
            if (!phrase_dev_num(dev, &major, &minor) || major == 0) {
                /*
                    if dev major number equal to 0, mean the module must NOT be
                    a shared or executable object loaded from disk.
                    e.g:
                    lookup symbol from [vdso] would crash.
                    7f7b48a000-7f7b48c000 r-xp 00000000 00:00 0  [vdso]
                 */
                continue;
            }

            strtok_r(NULL, sep, &line);  // node

            char *filename = strtok_r(NULL, sep, &line); //module name
            if (!filename) {
                continue;
            }

            if (*filename == '\0') {
                continue;
            }
//            LOGD("filename: %s ,target:%s",filename,target);
//            std::string module_name(filename);

            if (strstr(filename, target) != NULL) {
                void *base_addr = NULL;
                void *end_addr = NULL;
                phrase_proc_base_addr(addr, &base_addr, &end_addr);
                void * image_base = base_addr;
                LOGI("get Module : %s,%p", filename, image_base);
                fclose(fd);
                return image_base;
            }
        }

    }
    fclose(fd);
    return (void *)-1;

}

void waitLoading(const char *file)
{
    LOGE("begin to wait");
    char tempSave[128];
    strcpy(tempSave, file);
    LOGE("target:%s", tempSave);
    long image = (long)getimagebase(tempSave);
    while (image == -1) {
        LOGE("not found");
        usleep(100000);
        image = (long)getimagebase(tempSave);
    }
    LOGE("waiting end");
}

void waitLoadingHook(const char *file)
{
    LOGE("begin to wait");
    const char *pchgvr = "libgvr.so";
    const char *pchunity = "libunity.so";
    bool bgvr = false;
    bool bunity = false;

    long gvrimage = (long)getimagebase(pchgvr);
    long unityimage = (long)getimagebase(pchunity);
    while (gvrimage == -1 || unityimage == -1) {
        LOGE("not found");
        usleep(100000);
        unityimage = (long)getimagebase(pchunity);
        if( !bunity && unityimage != -1)
        {
            bunity = true;
        }
        gvrimage = (long)getimagebase(pchgvr);
        if( !bgvr && gvrimage != -1 )
        {
            bgvr = true;
        }

    }
    LOGE("waiting end");
}


//void starthookthread()
//{
//    pthread_t   pthread_id;
//    pthread_create(&pthread_id, NULL, waitLoading, NULL);
//}