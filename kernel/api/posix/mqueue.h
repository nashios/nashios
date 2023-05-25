#pragma once

struct mq_attr
{
    long long mq_flags;
    long long mq_maxmsg;
    long long mq_msgsize;
    long long mq_curmsgs;
    long long __reserved[4];
};
