#pragma once
#define HEARBEAT 1000000
#include <iostream>

void assert_action(int iter_num) {
  std::cerr << "Itreation: " << iter_num
            << ", Store-Load Reordering Happened, r1 == 0 && r2 == 0"
            << std::endl;
  std::abort();
}