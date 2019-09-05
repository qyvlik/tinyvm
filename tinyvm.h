#ifndef TINYVM_H
#define TINYVM_H

#include <cstdint>
#include <vector>
#include <stack>

namespace qyvlik {

struct OpCode {
    int op;
    int num;
};

class CallInfo;


class TinyVM
{
public:
    enum OP_CODE {
        STOP = 0x0,
        NONE = 0x1,

        PRINT = 0x2,
        POP_PRINT = 0x3,
        PUSH = 0x4,
        POP = 0x5,

        ADD = 0x6,          // +
        SUB = 0x7,          // -
        MUL = 0x8,          // *
        DIV = 0x9,          // /
        MOD = 0x10,         // %

        JUMP = 0x11,
        JF = 0x12,          // jump false

        GT,                 // >
        LT,                 // <
        GE,                 // >=
        LE,                 // <=
        EQ,                 // ==

        ARGS,               // prepend call and declare args size
        PUSH_A,             // pop data-stack and push args
        CALL,               // call
        RET,                // return result

        COPY_A,             // COPY argv[num] and push into data-stack
    };
    enum THREAD_STATE {
        NORMAL = INT32_MAX,
        INTERRUPT = INT32_MIN
    };

    enum VM_STATE {
        RUNNING = INT32_MAX,
        TERMINATED = INT32_MIN,
    };

public:
    TinyVM();
    ~TinyVM();

    void add(int op, int num);

    void interrup();

    void exec();

    int getExecuteTimes() const;

    bool isRunning() const;

private:
    int programCounter;
    volatile int executeTimes;
    volatile int thread_state;
    volatile int vm_state;
    int callIndex;
    std::vector<CallInfo*> callStack;
    std::vector<OpCode> codes;
};


} // namespace qyvlik

#endif // TINYVM_H
