/*
 * SPDX-FileCopyrightText: 2023 David Jolly <majestic53@gmail.com>
 * SPDX-License-Identifier: MIT
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
