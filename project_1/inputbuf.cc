/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#include "./inputbuf.h"

#include <cstdio>
#include <iostream>
#include <istream>
#include <string>
#include <vector>

bool InputBuffer::EndOfInput() {
  if (!input_buffer.empty())
    return false;
  else
    return std::cin.eof();
}

char InputBuffer::UngetChar(char c) {
  if (c != EOF) input_buffer.push_back(c);
  return c;
}

void InputBuffer::GetChar(char& c) {
  if (!input_buffer.empty()) {
    c = input_buffer.back();
    input_buffer.pop_back();
  } else {
    std::cin.get(c);
  }
}

std::string InputBuffer::UngetString(std::string s) {
  for (int i = 0; i < static_cast<int>(s.size()); i++)
    input_buffer.push_back(s[s.size() - i - 1]);
  return s;
}
