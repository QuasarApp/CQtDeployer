//#
//# Copyright (C) 2020-2024 QuasarApp.
//# Distributed under the GPLv3 software license, see the accompanying
//# Everyone is permitted to copy and distribute verbatim copies
//# of this license document, but changing it is not allowed.
//#


#ifndef TEST_H
#define TEST_H

class Test
{
public:
    Test() = default;
    virtual ~Test() = default;
    virtual void test() = 0;
};

#endif // TEST_H
