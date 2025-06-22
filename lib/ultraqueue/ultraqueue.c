// ultraqueue.c - Production-grade hybrid RAM+disk queue with thread safety

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
static CRITICAL_SECTION queue_lock;
#define INIT_QUEUE_LOCK() InitializeCriticalSection(&queue_lock)
#define LOCK_QUEUE() EnterCriticalSection(&queue_lock)
#define UNLOCK_QUEUE() LeaveCriticalSection(&queue_lock)
#define CLEANUP_QUEUE_LOCK() DeleteCriticalSection(&queue_lock)
#else
#include <pthread.h>
static pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
#define INIT_QUEUE_LOCK() // pthread mutex initialized statically
#define LOCK_QUEUE() pthread_mutex_lock(&queue_lock)
#define UNLOCK_QUEUE() pthread_mutex_unlock(&queue_lock)
#define CLEANUP_QUEUE_LOCK() pthread_mutex_destroy(&queue_lock)
#endif

#define MAX_LINE 4096
#define RAM_BUFFER_CAPACITY 10000

// RAM circular buffer for queue items (strings)
static char *ram_buffer[RAM_BUFFER_CAPACITY];
static int ram_start = 0, ram_end = 0;

// Check if RAM buffer is full
static int is_ram_full()
{
    return ((ram_end + 1) % RAM_BUFFER_CAPACITY) == ram_start;
}

// Check if RAM buffer is empty
static int is_ram_empty()
{
    return ram_start == ram_end;
}

// Push string item to RAM buffer
static int push_ram(const char *item)
{
    if (is_ram_full())
        return -1;
    char *copy = strdup(item);
    if (!copy)
        return -2; // malloc failure
    ram_buffer[ram_end] = copy;
    ram_end = (ram_end + 1) % RAM_BUFFER_CAPACITY;
    return 0;
}

// Pop string item from RAM buffer into out_buf (safe copy)
static int pop_ram(char *out_buf, size_t buf_size)
{
    if (is_ram_empty())
        return -1;
    strncpy(out_buf, ram_buffer[ram_start], buf_size - 1);
    out_buf[buf_size - 1] = '\0';
    free(ram_buffer[ram_start]);
    ram_buffer[ram_start] = NULL;
    ram_start = (ram_start + 1) % RAM_BUFFER_CAPACITY;
    return 0;
}

// Spill all RAM buffer contents to disk file (append mode), clear RAM buffer
static int spill_to_disk(const char *path)
{
    FILE *fp = fopen(path, "a");
    if (!fp)
        return -1;
    while (!is_ram_empty())
    {
        if (fputs(ram_buffer[ram_start], fp) == EOF)
        {
            fclose(fp);
            return -2;
        }
        if (fputc('\n', fp) == EOF)
        {
            fclose(fp);
            return -3;
        }
        free(ram_buffer[ram_start]);
        ram_buffer[ram_start] = NULL;
        ram_start = (ram_start + 1) % RAM_BUFFER_CAPACITY;
    }
    fclose(fp);
    ram_start = ram_end = 0; // reset circular buffer indices
    return 0;
}

// Push item to hybrid queue (RAM or spill to disk)
int ultraqueue_push(const char *path, const char *item)
{
    LOCK_QUEUE();
    int res = push_ram(item);
    if (res == -1)
    { // RAM full
        int spill_res = spill_to_disk(path);
        if (spill_res != 0)
        {
            UNLOCK_QUEUE();
            return spill_res; // Propagate error from spill_to_disk
        }
        res = push_ram(item); // retry push after spill
    }
    UNLOCK_QUEUE();
    return res;
}

// Pop item from hybrid queue (RAM preferred, fallback to disk)
int ultraqueue_pop(const char *path, char *out_buf, size_t buf_size)
{
    LOCK_QUEUE();
    int res = pop_ram(out_buf, buf_size);
    if (res == 0)
    {
        UNLOCK_QUEUE();
        return 0; // Success from RAM
    }

    // RAM empty, read from disk file
    FILE *fp = fopen(path, "r");
    if (!fp)
    {
        UNLOCK_QUEUE();
        return -1; // No file or error
    }

    FILE *temp = tmpfile();
    if (!temp)
    {
        fclose(fp);
        UNLOCK_QUEUE();
        return -2; // temp file creation error
    }

    int found = 0;
    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, fp))
    {
        if (!found)
        {
            // Return first line found as popped item
            strncpy(out_buf, line, buf_size - 1);
            out_buf[buf_size - 1] = '\0';
            // Remove trailing newline if present
            size_t len = strlen(out_buf);
            if (len > 0 && out_buf[len - 1] == '\n')
                out_buf[len - 1] = '\0';
            found = 1;
        }
        else
        {
            // Copy remaining lines to temp file
            fputs(line, temp);
        }
    }
    fclose(fp);

    // Rewrite disk file with remaining lines from temp
    fp = fopen(path, "w");
    if (!fp)
    {
        fclose(temp);
        UNLOCK_QUEUE();
        return -3;
    }
    rewind(temp);
    while (fgets(line, MAX_LINE, temp))
    {
        fputs(line, fp);
    }
    fclose(fp);
    fclose(temp);

    UNLOCK_QUEUE();
    return found ? 0 : -4; // Return success if found
}

// Get total length of queue (RAM + disk lines count)
int ultraqueue_len(const char *path)
{
    LOCK_QUEUE();
    int count = (ram_end + RAM_BUFFER_CAPACITY - ram_start) % RAM_BUFFER_CAPACITY;

    FILE *fp = fopen(path, "r");
    if (fp)
    {
        int ch;
        while ((ch = fgetc(fp)) != EOF)
        {
            if (ch == '\n')
                count++;
        }
        fclose(fp);
    }
    UNLOCK_QUEUE();
    return count;
}

// Initialize queue lock (call before any other functions)
void initialize_queue_lock()
{
    INIT_QUEUE_LOCK();
}

// Cleanup queue lock (call on program exit)
void cleanup_queue_lock()
{
#ifdef _WIN32
    CLEANUP_QUEUE_LOCK();
#else
    // On POSIX pthread_mutex_destroy is safe to call
    CLEANUP_QUEUE_LOCK();
#endif
}
