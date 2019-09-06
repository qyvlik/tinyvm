#ifndef TINYVM_TEST_H
#define TINYVM_TEST_H

#include <iostream>
#include <cstring>
#include <vector>
#include <cctype>
#include <thread>
#include <chrono>

#include <tinyvm.h>

using namespace std;
using namespace qyvlik;

void test_vm_interrup() {

    std::cout << "=======================test_vm_interrup================" << std::endl;

    TinyVM* vm = new TinyVM();

    vm->add(TinyVM::OP_CODE::PRINT, 8080);             // 0
    vm->add(TinyVM::OP_CODE::NONE, 0);                  // 1

    vm->add(TinyVM::OP_CODE::PUSH, 10);                 // 2
    vm->add(TinyVM::OP_CODE::PUSH, 20);                 // 3
    vm->add(TinyVM::OP_CODE::ADD, 0);                   // 4

    // vm->add(TinyVM::OP_CODE::POP_PRINT, 0);             // 5
    vm->add(TinyVM::OP_CODE::POP, 0);             // 5

    vm->add(TinyVM::OP_CODE::JUMP, 1);                  // 6

    vm->add(TinyVM::OP_CODE::STOP, 0);                  // 7

    thread vm_thread([=](){
        vm->exec();
    });

    vm_thread.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    vm->interrup();

    std::cout << "vm execute times: " << vm->getExecuteTimes() << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "vm execute times: " << vm->getExecuteTimes() << std::endl;

    delete vm;
}


/*
func f(a)
    if a <= 1 then
        return 1;
    end
    return f(a-1) + f(a-2);
end
print f(10);

# follow as op_code
00:     JUMP        22          // goto `print f(10)`

01:     METHOD      f           // method f
02:     PUSH        1           // `1` as second
03:     COPY_A      0           // `a` as first
04:     LE                      // <=
05:     JF          8           // if false
06:     PUSH        1           // push `1` for return
07:     RET                     //

08:     PUSH        1           // `1` as second
09:     COPY_A      0           // `a` as first
10:     SUB                     // -
11:     ARGS        1           // prepend call
12:     PUSH_A                  // pop data-stack and push args
13:     CALL        f           // call method `f(a-1)`

14:     PUSH        2           // `2` as second
15:     COPY_A      0           // `a` as first
16:     SUB                     // -
17:     ARGS        1           // prepend call
18:     PUSH_A                  // pop data-stack and push args
19:     CALL        f           // call method

20:     ADD                     // `f(a-1)` + `f(a-2)`
21:     RET                     // return
# end of method

22:     PUSH        10          // `10` as method args
23:     ARGS        1           // prepend call
24:     PUSH_A                  // pop data-stack and push args
25:     CALL        f           // call method

26:     POP_PRINT               // pop data-stack and print
27:     STOP
*/
void test_fibonacci() {
    std::cout << "=======================test_fibonacci================" << std::endl;
    TinyVM* vm = new TinyVM();

    /* 00 */ vm->add(TinyVM::OP_CODE::JUMP, 22);             // goto `print f(10)`

    /* 01 */ vm->add(TinyVM::OP_CODE::NONE, 0);              // method f
    /* 03 */ vm->add(TinyVM::OP_CODE::COPY_A, 0);            // `a` as first
    /* 02 */ vm->add(TinyVM::OP_CODE::PUSH, 1);              // `1` as second
    /* 04 */ vm->add(TinyVM::OP_CODE::LE, 0);                // <=
    /* 05 */ vm->add(TinyVM::OP_CODE::JF, 8);                // if false, goto 8,
    /* 06 */ vm->add(TinyVM::OP_CODE::PUSH, 1);              //  push `1` for return
    /* 07 */ vm->add(TinyVM::OP_CODE::RET, 0);

    /* 09 */ vm->add(TinyVM::OP_CODE::COPY_A, 0);            // `a` as first
    /* 08 */ vm->add(TinyVM::OP_CODE::PUSH, 1);              // `1` as second
    /* 10 */ vm->add(TinyVM::OP_CODE::SUB, 0);               // -
    /* 11 */ vm->add(TinyVM::OP_CODE::ARGS, 1);              // prepend call
    /* 12 */ vm->add(TinyVM::OP_CODE::PUSH_A, 0);            // pop data-stack and push args
    /* 13 */ vm->add(TinyVM::OP_CODE::CALL, 1);              // call method `f(a-1)`

    /* 15 */ vm->add(TinyVM::OP_CODE::COPY_A, 0);            // `a` as first
    /* 14 */ vm->add(TinyVM::OP_CODE::PUSH, 2);              // `2` as second
    /* 16 */ vm->add(TinyVM::OP_CODE::SUB, 0);               // a - 2
    /* 17 */ vm->add(TinyVM::OP_CODE::ARGS, 1);              // prepend call
    /* 18 */ vm->add(TinyVM::OP_CODE::PUSH_A, 0);            // pop data-stack and push args
    /* 19 */ vm->add(TinyVM::OP_CODE::CALL, 1);              // call method `f(a-2)`

    /* 20 */ vm->add(TinyVM::OP_CODE::ADD, 0);               // `f(a-1)` + `f(a-2)`
    /* 21 */ vm->add(TinyVM::OP_CODE::RET, 0);               // return

    int f_param_a = 30;

    /* 22 */ vm->add(TinyVM::OP_CODE::PUSH, f_param_a);      // `1` as method args
    /* 23 */ vm->add(TinyVM::OP_CODE::ARGS, 1);              // prepend call
    /* 24 */ vm->add(TinyVM::OP_CODE::PUSH_A, 0);            // pop data-stack and push args
    /* 25 */ vm->add(TinyVM::OP_CODE::CALL, 1);              // call method `f(10)`

    /* 26 */ vm->add(TinyVM::OP_CODE::POP_PRINT, 0);         // 7

    /* 27 */ vm->add(TinyVM::OP_CODE::STOP, 0);              // 7


    vm->exec();

    std::cout << "vm execute times: " << vm->getExecuteTimes() << std::endl;

    delete vm;
}

void test_fibonacci_with_thread() {
    std::cout << "=======================test_fibonacci_with_thread================" << std::endl;
    TinyVM* vm = new TinyVM();

    /* 00 */ vm->add(TinyVM::OP_CODE::JUMP, 22);             // goto `print f(10)`

    /* 01 */ vm->add(TinyVM::OP_CODE::NONE, 0);              // method f
    /* 03 */ vm->add(TinyVM::OP_CODE::COPY_A, 0);            // `a` as first
    /* 02 */ vm->add(TinyVM::OP_CODE::PUSH, 1);              // `1` as second
    /* 04 */ vm->add(TinyVM::OP_CODE::LE, 0);                // <=
    /* 05 */ vm->add(TinyVM::OP_CODE::JF, 8);                // if false, goto 8,
    /* 06 */ vm->add(TinyVM::OP_CODE::PUSH, 1);              //  push `1` for return
    /* 07 */ vm->add(TinyVM::OP_CODE::RET, 0);

    /* 09 */ vm->add(TinyVM::OP_CODE::COPY_A, 0);            // `a` as first
    /* 08 */ vm->add(TinyVM::OP_CODE::PUSH, 1);              // `1` as second
    /* 10 */ vm->add(TinyVM::OP_CODE::SUB, 0);               // -
    /* 11 */ vm->add(TinyVM::OP_CODE::ARGS, 1);              // prepend call
    /* 12 */ vm->add(TinyVM::OP_CODE::PUSH_A, 0);            // pop data-stack and push args
    /* 13 */ vm->add(TinyVM::OP_CODE::CALL, 1);              // call method `f(a-1)`

    /* 15 */ vm->add(TinyVM::OP_CODE::COPY_A, 0);            // `a` as first
    /* 14 */ vm->add(TinyVM::OP_CODE::PUSH, 2);              // `2` as second
    /* 16 */ vm->add(TinyVM::OP_CODE::SUB, 0);               // a - 2
    /* 17 */ vm->add(TinyVM::OP_CODE::ARGS, 1);              // prepend call
    /* 18 */ vm->add(TinyVM::OP_CODE::PUSH_A, 0);            // pop data-stack and push args
    /* 19 */ vm->add(TinyVM::OP_CODE::CALL, 1);              // call method `f(a-2)`

    /* 20 */ vm->add(TinyVM::OP_CODE::ADD, 0);               // `f(a-1)` + `f(a-2)`
    /* 21 */ vm->add(TinyVM::OP_CODE::RET, 0);               // return

    int f_param_a = 30;

    /* 22 */ vm->add(TinyVM::OP_CODE::PUSH, f_param_a);      // `1` as method args
    /* 23 */ vm->add(TinyVM::OP_CODE::ARGS, 1);              // prepend call
    /* 24 */ vm->add(TinyVM::OP_CODE::PUSH_A, 0);            // pop data-stack and push args
    /* 25 */ vm->add(TinyVM::OP_CODE::CALL, 1);              // call method `f(10)`

    /* 26 */ vm->add(TinyVM::OP_CODE::POP_PRINT, 0);         // 7

    /* 27 */ vm->add(TinyVM::OP_CODE::STOP, 0);              // 7

    thread vm_thread([=](){
        vm->exec();
    });

    vm_thread.detach();

    std::this_thread::sleep_for(std::chrono::milliseconds(1030));

    std::cout << "before interrup execute times:"
              << vm->getExecuteTimes()
              << std::endl;

    vm->interrup();

    std::cout << "after interrup execute times:"
              << vm->getExecuteTimes()
              << std::endl;

    while(vm->isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::cout << "stop execute times:"
              << vm->getExecuteTimes()
              << std::endl;

    delete vm;

    std::cout << "vm execute times: " << vm->getExecuteTimes() << std::endl;
}

#endif // TINYVM_TEST_H
