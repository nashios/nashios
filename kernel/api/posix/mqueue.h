#pragma once

struct mq_attr
{
    long mq_flags;
    long mq_maxmsg;
    long mq_msgsize;
    long mq_curmsgs;
    long __reserved[4];
};
