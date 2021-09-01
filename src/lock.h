#ifndef LOCK_H
#define LOCK_H

#include <stdbool.h>
#include <stdint.h>
typedef struct lock
{
    uint8_t data;
} Lock;

static inline bool lock_try_acquire(Lock *self)
{
    return __sync_bool_compare_and_swap(&self->data, false, true);
}

static inline void lock_acquire(Lock *self)
{
    while (!lock_try_acquire(self))
    {
        asm("pause");
    }
}

static inline void lock_release(Lock *self)
{
    self->data = false;
}

#endif
