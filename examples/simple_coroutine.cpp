#include "corolib/Awaitable.h"
#include <iostream>

corolib::Awaitable foo()
{
    std::cout << "start foo\n";
    co_return;
}

int main()
{
    auto task = foo();
    task.resume();
    return 0;
}


