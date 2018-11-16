#define _GNU_SOURCE
#include "../common/hostfs.h"
#include <openenclave/internal/calls.h>
#include <openenclave/internal/fsinternal.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/hostfsargs.h"
#include "hostbatch.h"

#define BATCH_SIZE 4096

typedef oe_hostfs_args_t args_t;

static oe_host_batch_t* _batch;
static pthread_spinlock_t _lock;

static void _atexit_handler()
{
    pthread_spin_lock(&_lock);
    oe_host_batch_delete(_batch);
    _batch = NULL;
    pthread_spin_unlock(&_lock);
}

static oe_host_batch_t* _get_host_batch(void)
{
    if (_batch == NULL)
    {
        pthread_spin_lock(&_lock);

        if (_batch == NULL)
        {
            _batch = oe_host_batch_new(BATCH_SIZE);
            atexit(_atexit_handler);
        }

        pthread_spin_unlock(&_lock);
    }

    return _batch;
}

typedef struct _file
{
    FILE base;
    void* host_file;
} file_t;

typedef struct _dir
{
    DIR base;
    void* host_dir;
} dir_t;

OE_INLINE bool _valid_file(file_t* file)
{
    return file && file->base.magic == OE_FILE_MAGIC;
}

static int32_t _f_fclose(FILE* base)
{
    int32_t ret = -1;
    file_t* file = (file_t*)base;
    oe_host_batch_t* batch = _get_host_batch();
    args_t* args = NULL;

    if (!_valid_file(file) || !batch)
        goto done;

    /* Input */
    {
        if (!(args = oe_host_batch_calloc(batch, sizeof(args_t))))
            goto done;

        args->u.fclose.ret = -1;
        args->op = OE_HOSTFS_OP_FCLOSE;
        args->u.fclose.file = file->host_file;
    }

    /* Call */
    {
        if (oe_ocall(OE_OCALL_HOSTFS, (uint64_t)args, NULL) != OE_OK)
            goto done;
    }

    /* Output */
    {
        if ((ret = args->u.fclose.ret) != 0)
            goto done;
    }

    free(file);

done:

    if (args)
        oe_host_batch_free(batch);

    return ret;
}

static size_t _f_fread(void* ptr, size_t size, size_t nmemb, FILE* base)
{
    size_t ret = 0;
    file_t* file = (file_t*)base;
    oe_host_batch_t* batch = _get_host_batch();
    args_t* args = NULL;

    if (!ptr || !_valid_file(file) || !batch)
        goto done;

    /* Input */
    {
        size_t n = size + nmemb;

        if (!(args = oe_host_batch_calloc(batch, sizeof(args_t) + n)))
            goto done;

        memset(ptr, 0, n);
        args->u.fread.ret = -1;
        args->op = OE_HOSTFS_OP_FREAD;
        args->u.fread.size = size;
        args->u.fread.nmemb = nmemb;
        args->u.fread.file = file->host_file;
        args->u.fread.ptr = args->buf;
    }

    /* Call */
    {
        if (oe_ocall(OE_OCALL_HOSTFS, (uint64_t)args, NULL) != OE_OK)
            goto done;
    }

    /* Output */
    {
        if ((ret = args->u.fread.ret) > 0)
            memcpy(ptr, args->buf, ret);
    }

done:

    if (args)
        oe_host_batch_free(batch);

    return ret;
}

static size_t _f_fwrite(const void* ptr, size_t size, size_t nmemb, FILE* base)
{
    size_t ret = 0;
    file_t* file = (file_t*)base;
    oe_host_batch_t* batch = _get_host_batch();
    args_t* args = NULL;

    if (!ptr || !_valid_file(file) || !batch)
        goto done;

    /* Input */
    {
        size_t n = size + nmemb;

        if (!(args = oe_host_batch_calloc(batch, sizeof(args_t) + n)))
            goto done;

        args->u.fwrite.ret = -1;
        args->op = OE_HOSTFS_OP_FWRITE;
        args->u.fwrite.size = size;
        args->u.fwrite.nmemb = nmemb;
        args->u.fwrite.file = file->host_file;
        args->u.fwrite.ptr = args->buf;
        memcpy(args->buf, ptr, n);
    }

    /* Call */
    {
        if (oe_ocall(OE_OCALL_HOSTFS, (uint64_t)args, NULL) != OE_OK)
            goto done;
    }

    /* Output */
    {
        ret = args->u.fwrite.ret;
    }

done:

    if (args)
        oe_host_batch_free(batch);

    return ret;
}

static int64_t _f_ftell(FILE* base)
{
    int64_t ret = -1;
    file_t* file = (file_t*)base;
    oe_host_batch_t* batch = _get_host_batch();
    args_t* args = NULL;

    if (!_valid_file(file) || !batch)
        goto done;

    /* Input */
    {
        if (!(args = oe_host_batch_calloc(batch, sizeof(args_t))))
            goto done;

        args->u.ftell.ret = -1;
        args->op = OE_HOSTFS_OP_FTELL;
        args->u.ftell.file = file->host_file;
    }

    /* Call */
    {
        if (oe_ocall(OE_OCALL_HOSTFS, (uint64_t)args, NULL) != OE_OK)
            goto done;
    }

    /* Output */
    {
        ret = args->u.ftell.ret;
    }

done:

    if (args)
        oe_host_batch_free(batch);

    return ret;
}

static int32_t _f_fseek(FILE* base, int64_t offset, int whence)
{
    int64_t ret = -1;
    file_t* file = (file_t*)base;
    oe_host_batch_t* batch = _get_host_batch();
    args_t* args = NULL;

    if (!_valid_file(file) || !batch)
        goto done;

    /* Input */
    {
        if (!(args = oe_host_batch_calloc(batch, sizeof(args_t))))
            goto done;

        args->u.fseek.ret = -1;
        args->op = OE_HOSTFS_OP_FSEEK;
        args->u.fseek.file = file->host_file;
        args->u.fseek.offset = offset;
        args->u.fseek.whence = whence;
    }

    /* Call */
    {
        if (oe_ocall(OE_OCALL_HOSTFS, (uint64_t)args, NULL) != OE_OK)
            goto done;
    }

    /* Output */
    {
        ret = args->u.fseek.ret;
    }

done:

    if (args)
        oe_host_batch_free(batch);

    return ret;
}

static int32_t _f_fflush(FILE* base)
{
    int64_t ret = -1;
    file_t* file = (file_t*)base;
    oe_host_batch_t* batch = _get_host_batch();
    args_t* args = NULL;

    if (!_valid_file(file) || !batch)
        goto done;

    /* Input */
    {
        if (!(args = oe_host_batch_calloc(batch, sizeof(args_t))))
            goto done;

        args->u.fflush.ret = -1;
        args->op = OE_HOSTFS_OP_FFLUSH;
        args->u.fflush.file = file->host_file;
    }

    /* Call */
    {
        if (oe_ocall(OE_OCALL_HOSTFS, (uint64_t)args, NULL) != OE_OK)
            goto done;
    }

    /* Output */
    {
        ret = args->u.fflush.ret;
    }

done:

    if (args)
        oe_host_batch_free(batch);

    return ret;
}

static int32_t _f_ferror(FILE* base)
{
    int64_t ret = -1;
    file_t* file = (file_t*)base;
    oe_host_batch_t* batch = _get_host_batch();
    args_t* args = NULL;

    if (!_valid_file(file) || !batch)
        goto done;

    /* Input */
    {
        if (!(args = oe_host_batch_calloc(batch, sizeof(args_t))))
            goto done;

        args->u.ferror.ret = -1;
        args->op = OE_HOSTFS_OP_FERROR;
        args->u.ferror.file = file->host_file;
    }

    /* Call */
    {
        if (oe_ocall(OE_OCALL_HOSTFS, (uint64_t)args, NULL) != OE_OK)
            goto done;
    }

    /* Output */
    {
        ret = args->u.ferror.ret;
    }

done:

    if (args)
        oe_host_batch_free(batch);

    return ret;
}

static int32_t _f_feof(FILE* base)
{
    int64_t ret = -1;
    file_t* file = (file_t*)base;
    oe_host_batch_t* batch = _get_host_batch();
    args_t* args = NULL;

    if (!_valid_file(file) || !batch)
        goto done;

    /* Input */
    {
        if (!(args = oe_host_batch_calloc(batch, sizeof(args_t))))
            goto done;

        args->u.feof.ret = -1;
        args->op = OE_HOSTFS_OP_FEOF;
        args->u.feof.file = file->host_file;
    }

    /* Call */
    {
        if (oe_ocall(OE_OCALL_HOSTFS, (uint64_t)args, NULL) != OE_OK)
            goto done;
    }

    /* Output */
    {
        ret = args->u.feof.ret;
    }

done:

    if (args)
        oe_host_batch_free(batch);

    return ret;
}

static int32_t _f_clearerr(FILE* base)
{
    int64_t ret = -1;
    file_t* file = (file_t*)base;
    oe_host_batch_t* batch = _get_host_batch();
    args_t* args = NULL;

    if (!_valid_file(file) || !batch)
        goto done;

    /* Input */
    {
        if (!(args = oe_host_batch_calloc(batch, sizeof(args_t))))
            goto done;

        args->u.clearerr.ret = -1;
        args->op = OE_HOSTFS_OP_CLEARERR;
        args->u.clearerr.file = file->host_file;
    }

    /* Call */
    {
        if (oe_ocall(OE_OCALL_HOSTFS, (uint64_t)args, NULL) != OE_OK)
            goto done;
    }

    /* Output */
    {
        ret = args->u.clearerr.ret;
    }

done:

    if (args)
        oe_host_batch_free(batch);

    return ret;
}

static FILE* _fs_fopen(
    oe_fs_t* fs,
    const char* path,
    const char* mode,
    const void* args_)
{
    FILE* ret = NULL;
    file_t* file = NULL;
    oe_host_batch_t* batch = _get_host_batch();
    args_t* args = NULL;

    if (!path || !mode || !batch)
        goto done;

    /* Input */
    {
        if (!(args = oe_host_batch_calloc(batch, sizeof(args_t))))
            goto done;

        args->op = OE_HOSTFS_OP_FOPEN;
        args->u.fopen.ret = NULL;
        strlcpy(args->u.fopen.path, path, sizeof(args->u.fopen.path));
        strlcpy(args->u.fopen.mode, mode, sizeof(args->u.fopen.mode));
    }

    /* Call */
    {
        if (oe_ocall(OE_OCALL_HOSTFS, (uint64_t)args, NULL) != OE_OK)
            goto done;

        if (args->u.fopen.ret == NULL)
            goto done;
    }

    /* Output */
    {
        if (!(file = calloc(1, sizeof(file_t))))
            goto done;

        file->base.magic = OE_FILE_MAGIC;
        file->base.f_fclose = _f_fclose;
        file->base.f_fread = _f_fread;
        file->base.f_fwrite = _f_fwrite;
        file->base.f_ftell = _f_ftell;
        file->base.f_fseek = _f_fseek;
        file->base.f_fflush = _f_fflush;
        file->base.f_ferror = _f_ferror;
        file->base.f_feof = _f_feof;
        file->base.f_clearerr = _f_clearerr;
        file->host_file = args->u.fopen.ret;
    }

    ret = &file->base;
    file = NULL;

done:

    if (file)
        free(file);

    if (args)
        oe_host_batch_free(batch);

    return ret;
}

int32_t _d_readdir(DIR* base, struct dirent* entry, struct dirent** result)
{
    int32_t ret = -1;
    dir_t* dir = (dir_t*)base;
    oe_host_batch_t* batch = _get_host_batch();
    args_t* args = NULL;

    if (entry)
        memset(entry, 0, sizeof(struct dirent));

    if (result)
        *result = NULL;

    if (!dir || !dir->host_dir || !entry || !result || !batch)
        goto done;

    /* Input */
    {
        if (!(args = oe_host_batch_calloc(batch, sizeof(args_t))))
            goto done;

        args->u.readdir.ret = -1;
        args->op = OE_HOSTFS_OP_READDIR;
        args->u.readdir.dir = dir->host_dir;
    }

    /* Call */
    {
        if (oe_ocall(OE_OCALL_HOSTFS, (uint64_t)args, NULL) != OE_OK)
            goto done;
    }

    /* Output */
    {
        if ((ret = args->u.readdir.ret) == 0 && args->u.readdir.result)
        {
            entry->d_ino = args->u.readdir.entry.d_ino;
            entry->d_off = args->u.readdir.entry.d_off;
            entry->d_reclen = args->u.readdir.entry.d_reclen;
            entry->d_type = args->u.readdir.entry.d_type;
            strlcpy(
                entry->d_name,
                args->u.readdir.entry.d_name,
                sizeof(entry->d_name));

            *result = entry;
        }
        else
        {
            goto done;
        }
    }

done:

    if (args)
        oe_host_batch_free(batch);

    return ret;
}

int32_t _d_closedir(DIR* base)
{
    int32_t ret = -1;
    dir_t* dir = (dir_t*)base;
    oe_host_batch_t* batch = _get_host_batch();
    args_t* args = NULL;

    if (!dir || !dir->host_dir || !batch)
        goto done;

    /* Input */
    {
        if (!(args = oe_host_batch_calloc(batch, sizeof(args_t))))
            goto done;

        args->u.closedir.ret = -1;
        args->op = OE_HOSTFS_OP_CLOSEDIR;
        args->u.closedir.dir = dir->host_dir;
    }

    /* Call */
    {
        if (oe_ocall(OE_OCALL_HOSTFS, (uint64_t)args, NULL) != OE_OK)
            goto done;
    }

    /* Output */
    {
        if ((ret = args->u.closedir.ret) != 0)
            goto done;
    }

    free(dir);

    ret = 0;

done:

    if (args)
        oe_host_batch_free(batch);

    return ret;
}

static DIR* _fs_opendir(oe_fs_t* fs, const char* name, const void* args_)
{
    DIR* ret = NULL;
    oe_host_batch_t* batch = _get_host_batch();
    args_t* args = NULL;
    dir_t* dir = NULL;

    if (!fs || !name || !batch)
        goto done;

    /* Input */
    {
        if (!(args = oe_host_batch_calloc(batch, sizeof(args_t))))
            goto done;

        args->op = OE_HOSTFS_OP_OPENDIR;
        args->u.fopen.ret = NULL;
        strlcpy(args->u.opendir.name, name, sizeof(args->u.opendir.name));
    }

    /* Call */
    {
        if (oe_ocall(OE_OCALL_HOSTFS, (uint64_t)args, NULL) != OE_OK)
            goto done;

        if (args->u.opendir.ret == NULL)
            goto done;
    }

    /* Output */
    {
        if (!(dir = calloc(1, sizeof(dir_t))))
            goto done;

        dir->base.d_readdir = _d_readdir;
        dir->base.d_closedir = _d_closedir;
        dir->host_dir = args->u.opendir.ret;
    }

    ret = &dir->base;
    dir = NULL;

done:

    if (args)
        oe_host_batch_free(batch);

    if (dir)
        free(dir);

    return ret;
}

static int32_t _fs_release(oe_fs_t* fs)
{
    uint32_t ret = -1;

    if (!fs)
        goto done;

    ret = 0;

done:
    return ret;
}

oe_fs_t oe_hostfs = {
    .fs_fopen = _fs_fopen,
    .fs_opendir = _fs_opendir,
    .fs_release = _fs_release,
};
