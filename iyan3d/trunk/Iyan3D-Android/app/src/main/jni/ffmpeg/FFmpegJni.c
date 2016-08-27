#include <jni.h>
#include "logjni.h"
#include "FFmpegJni.h"
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char **argv);

JNIEXPORT jint JNICALL Java_com_smackall_animator_FFmpeg_run(JNIEnv *env, jobject obj, jint argc, jobjectArray args)
{
    int i = 0;
    char **argv = NULL;
    jstring *strr = NULL;

    if (args != NULL)
    {
        argv = (char **) malloc(sizeof(char *) * argc);
        strr = (jstring *) malloc(sizeof(jstring) * argc);

        for (i = 0; i < argc; ++i)
        {
            strr[i] = (jstring)(*env)->GetObjectArrayElement(env, args, i);
            argv[i] = (char *)(*env)->GetStringUTFChars(env, strr[i], 0);
        }
    }

    int result = main(argc, argv);

    for (i = 0; i < argc; ++i)
    {
        (*env)->ReleaseStringUTFChars(env, strr[i], argv[i]);
    }
    free(argv);
    free(strr);

    return result;
}