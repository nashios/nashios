#include <assert.h>
#include <mqueue.h>

extern "C"
{
    int mq_getattr(mqd_t mqdes, struct mq_attr *attr) { assert(false); }

    int mq_setattr(mqd_t mqdes, const struct mq_attr *__restrict__ newattr, struct mq_attr *__restrict__ oldattr)
    {
        assert(false);
    }

    int mq_unlink(const char *name) { assert(false); }

    mqd_t mq_open(const char *name, int flags, ...) { assert(false); }
}
