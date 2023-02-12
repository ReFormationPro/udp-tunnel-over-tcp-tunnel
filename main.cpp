#include "main.h"

#include <iostream>

#include "utils.h"

int main(int argc, char **argv)
{
    int tcp_fd = createListenPort("0.0.0.0", 1337, TT_TCP);
    handleListener(tcp_fd);
    std::cout << "Goodbye\n";
    return 0;
}