/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2022
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "execute.h"
#include "tasks.h"

using namespace std;


int main (int argc, char* argv[])
{
    int task;

    if (argc < 2)
    {
        cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);
    

    switch (task) {
        case 1: // Task 1
		parse_and_generate_AST();
            break;

        case 2: // Task 2
		parse_and_type_check();
            break;

        case 3: instNode* code;
		code = parse_and_generate_statement_list();
		cout << "1234567890" << "testing" << "testing" << endl;
		execute_inst_list(code);
		cout << "1234567890" << "testing" << "testing" << endl;
            break;

        default:
            cout << "Error: unrecognized task number " << task << "\n";
            break;
    }
    return 0;
}

