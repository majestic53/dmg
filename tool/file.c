/*
 * DMG
 * Copyright (C) 2023 David Jolly
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <file.h>

int file_load(file_t *const file)
{
    FILE *fp;
    long length;
    if (!(fp = fopen(file->path, "rb")))
    {
        fprintf(stderr, "File does not exist -- %s\n", file->path);
        return EXIT_FAILURE;
    }
    fseek(fp, 0, SEEK_END);
    length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (length <= 0)
    {
        fprintf(stderr, "Invalid file length -- %s\n", file->path);
        fclose(fp);
        return EXIT_FAILURE;
    }
    if (!(file->data.buffer = calloc(length, sizeof (uint8_t))))
    {
        fprintf(stderr, "Failed to allocate read buffer\n");
        fclose(fp);
        return EXIT_FAILURE;
    }
    file->data.length = length;
    if (fread(file->data.buffer, sizeof (uint8_t), file->data.length, fp) != file->data.length)
    {
        fprintf(stderr, "Failed to read file -- %s\n", file->path);
        fclose(fp);
        return EXIT_FAILURE;
    }
    fclose(fp);
    return EXIT_SUCCESS;
}

int file_save(const file_t *const file)
{
    FILE *fp;
    if (!(fp = fopen(file->path, "wb")))
    {
        fprintf(stderr, "File does not exist -- %s\n", file->path);
        return EXIT_FAILURE;
    }
    if (fwrite(file->data.buffer, sizeof (uint8_t), file->data.length, fp) != file->data.length)
    {
        fprintf(stderr, "Failed to write file -- %s\n", file->path);
        fclose(fp);
        return EXIT_FAILURE;
    }
    fclose(fp);
    return EXIT_SUCCESS;
}
