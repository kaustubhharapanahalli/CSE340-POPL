/*
 * Copyright (C) Rida Bazzi, 2017
 *
 * Do not share this file with anyone
 */
#ifndef INPUTBUF_H_
#define INPUTBUF_H_

#include <string>
#include <vector>

class InputBuffer {
 public:
  void GetChar(char&);
  char UngetChar(char);
  std::string UngetString(std::string);
  bool EndOfInput();

 private:
  std::vector<char> input_buffer;
};

#endif  // INPUTBUF_H_
