#include "person.pb.h"
#include <iostream>
#include <string>
#include <bitset>

int main()
{
    test::Person p1;
    p1.set_id(100);
    p1.set_name("aaa");

    std::string data;
    p1.SerializeToString(&data);

    std::cout << "size: " << data.size() << std::endl;

    for (auto& c: data) {
        std::cout << std::bitset<8>(c) << " ";
    }
    std::cout << std::endl;

    return 0;
}