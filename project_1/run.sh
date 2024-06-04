g++ -std=c++11 -Werror -Wunused-value -Wall -c parser.cc lexer.cc inputbuf.cc
g++ parser.o lexer.o inputbuf.o -o a.out