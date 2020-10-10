#include <iostream>
#include "linked_hashmap.hpp"

int main()
{
    std::cout << "Hello, from linked_hashmap!\n";
    sharper::linked_hashmap<int, int> map{{1,2},{2,3}};
    map.insert({ 1, 1 });
    std::vector<std::pair<int,int>> v = { { 11, 11 }, { 22, 22 }, { 33, 33 } };
    map.insert(v.begin(), v.end());
    for (const auto& [key,val] : map) {
        std::cout << key << ":" << val << std::endl;
    }

    auto map2 = map;
    auto map3 = std::move(map2);
}
