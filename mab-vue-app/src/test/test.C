#include "test.H"
#include <iostream>

Test::Test() { std::cout << "Test constructed" << std::endl; }

Test::~Test() { std::cout << "Test destructed" << std::endl; }

void Test::sayHello() { std::cout << "Hello!" << std::endl; }

// #include "config.h"
#ifndef HAVE_EMSCRIPTEN
#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(Test_ex) {
  emscripten::class_<Test>("Test").constructor().function("sayHello",
                                                          &Test::sayHello);
}
#endif
