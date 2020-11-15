#include "hello.hpp"
#include <iostream>

void hello::say_hello(){
    std::cout << "Hello World!\n";
}

int main(){
    hello::say_hello();
}