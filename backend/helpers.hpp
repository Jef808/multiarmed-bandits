#ifndef HELPERS_H_
#define HELPERS_H_

#include <iostream>
#include <stdexcept>

inline static void on_error(std::error_code &error) {
  std::cerr << "Error: " << error.message() << std::endl;
}

inline static void on_error(std::exception &error) {
  std::cerr << "Error: " << error.what() << std::endl;
}

#endif // HELPERS_H_
