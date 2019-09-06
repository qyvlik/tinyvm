#include "tinyvm.h"

#include <cassert>
#include <iostream>

namespace qyvlik {

class CallInfo {
public:
    CallInfo(int args, int stackSize):
        dataStack(stackSize),
        stackIndex(-1),
        argv(args),
        argvIndex(-1),
        savedtPc(0),
        prevCall(nullptr)
    {}

    ~CallInfo() {
        this->prevCall = nullptr;
    }

    std::vector<int> dataStack;
    int stackIndex;
    std::vector<int> argv;
    int argvIndex;
    int savedtPc;

    CallInfo* prevCall;
};

TinyVM::TinyVM():
    programCounter(0),
    executeTimes(0),
    threadState(THREAD_STATE::NORMAL),
    vmState(VM_STATE::RUNNING),
    callIndex(-1),
    callStack(256),
    next_prepend_callinfo(nullptr)
{
    callStack[(this->callIndex+=1)] = new CallInfo(0, 16);
}

TinyVM::~TinyVM()
{
    // please use vm->interrup(); and wait is not running
    assert(!isRunning());

    this->threadState = THREAD_STATE::INTERRUPT;

    int index = callIndex;
    while(index -- > 0) {
        delete callStack[index];
    }
}

void TinyVM::interrup()
{
    this->threadState = THREAD_STATE::INTERRUPT;
}

void TinyVM::exec()
{
    execInternal();
    if (this->next_prepend_callinfo != nullptr) {
        delete this->next_prepend_callinfo;
        this->next_prepend_callinfo = nullptr;
    }
}

void TinyVM::add(int op, int num)
{
    this->codes.push_back(OpCode{op, num});
}

void TinyVM::execInternal()
{
    while(true) {
        this->executeTimes += 1;
        int pc = this->programCounter;
        this->programCounter += 1;
        const OpCode& op_code = this->codes[pc];
        const auto op = op_code.op & this->threadState;

        CallInfo* currentStack = callStack[this->callIndex];

//     std::cout << "stack index:" << currentStack->stackIndex << std::endl;
//        std::cout << ">>>>>>> pc:" << pc << std::endl;

        switch (op) {
        case OP_CODE::NONE :
//            std::cout << "none" << std::endl;
            break;

        case OP_CODE::PRINT:
            std::cout << "vm>" << op_code.num << std::endl;
            break;

        case OP_CODE::POP_PRINT:

            std::cout << "vm>"
                      << currentStack->dataStack[currentStack->stackIndex]
                      << std::endl;

            currentStack->stackIndex -= 1;
            break;

        case OP_CODE::PUSH:
//             std::cout << "push" << std::endl;

            currentStack->stackIndex += 1;
            currentStack->dataStack[currentStack->stackIndex] = op_code.num;

            break;
        case OP_CODE::POP:

//            std::cout << "pop" << std::endl;

            currentStack->stackIndex -= 1;
            break;

        case OP_CODE::ADD: {

            auto second = currentStack->dataStack[currentStack->stackIndex];
            auto first = currentStack->dataStack[(currentStack->stackIndex -= 1)];

//            std::cout << first << " add " << second << std::endl;

            currentStack->dataStack[currentStack->stackIndex] = first + second;
        }
            break;
        case OP_CODE::SUB: {
            auto second = currentStack->dataStack[currentStack->stackIndex];
            auto first = currentStack->dataStack[(currentStack->stackIndex -= 1)];

//            std::cout << first << " sub " << second << " = " << first - second << std::endl;

            currentStack->dataStack[currentStack->stackIndex] = first - second;
        }
            break;

        case OP_CODE::MUL: {
            auto second = currentStack->dataStack[currentStack->stackIndex];
            auto first = currentStack->dataStack[(currentStack->stackIndex -= 1)];

//            std::cout << first << " mul " << second << std::endl;

            currentStack->dataStack[currentStack->stackIndex] = first * second;
        }
            break;

        case OP_CODE::DIV: {
            auto second = currentStack->dataStack[currentStack->stackIndex];
            auto first = currentStack->dataStack[(currentStack->stackIndex -= 1)];

//            std::cout << first << " div " << second << std::endl;

            currentStack->dataStack[currentStack->stackIndex] = first / second;
        }
            break;

        case OP_CODE::MOD: {
            auto second = currentStack->dataStack[currentStack->stackIndex];
            auto first = currentStack->dataStack[(currentStack->stackIndex -= 1)];

//            std::cout << first << " mod " << second << std::endl;

            currentStack->dataStack[currentStack->stackIndex] = first % second;
        }
            break;

        case OP_CODE::GT: {
            auto second = currentStack->dataStack[currentStack->stackIndex];
            auto first = currentStack->dataStack[(currentStack->stackIndex -= 1)];

//            std::cout << first << " gt " << second << std::endl;

            currentStack->dataStack[currentStack->stackIndex] = first > second;
        }
            break;
        case OP_CODE::LT: {
            auto second = currentStack->dataStack[currentStack->stackIndex];
            auto first = currentStack->dataStack[(currentStack->stackIndex -= 1)];

//            std::cout << first << " lt " << second << ", result: " << (first < second) << std::endl;

            currentStack->dataStack[currentStack->stackIndex] = first < second;
        }
            break;

        case OP_CODE::GE: {
            auto second = currentStack->dataStack[currentStack->stackIndex];
            auto first = currentStack->dataStack[(currentStack->stackIndex -= 1)];

//            std::cout << first << " GE " << second << std::endl;

            currentStack->dataStack[currentStack->stackIndex] = first > second;
        }
            break;

        case OP_CODE::LE: {
            auto second = currentStack->dataStack[currentStack->stackIndex];
            auto first = currentStack->dataStack[(currentStack->stackIndex -= 1)];

//            std::cout << first << " le " << second << ", result: " << (first < second) << std::endl;

            currentStack->dataStack[currentStack->stackIndex] = first <= second;
        }
            break;
        case OP_CODE::EQ: {
            auto second = currentStack->dataStack[currentStack->stackIndex];
            auto first = currentStack->dataStack[(currentStack->stackIndex -= 1)];

//            std::cout << first << " eq " << second << std::endl;

            currentStack->dataStack[currentStack->stackIndex] = first == second;
        }
            break;

        case OP_CODE::JUMP : {

//            std::cout << "jump " <<  op_code.num << std::endl;

            this->programCounter = op_code.num;
            break;
        }

        case OP_CODE::JF : {
            auto result = currentStack->dataStack[currentStack->stackIndex];    // 1 or 0 -> true or false
            currentStack->stackIndex -=1;
            int jne_args[2] = {op_code.num, this->programCounter};      // 减少 if 判断
            this->programCounter = jne_args[result];
//             std::cout << "jne pc:"
//                       << this->programCounter
//                       << ", reslut: "
//                       << result
//                       << std::endl;
            break;
        }

        case OP_CODE::ARGS: {
            auto args = op_code.num;
            next_prepend_callinfo = new CallInfo(args, 32);         // new call info
            next_prepend_callinfo->prevCall = currentStack;

//            currentStack->nextCall = new CallInfo(args, 32);        // new call info
//            currentStack->nextCall->prevCall = currentStack;        // set prev call

//             std::cout << "args" << args << std::endl;

        }
            break;

        case OP_CODE::PUSH_A: {
//            assert(currentStack->nextCall != nullptr);
//            currentStack->nextCall->argv[(currentStack->nextCall->argvIndex +=1)]
//                    = currentStack->dataStack[currentStack->stackIndex] ;
//            currentStack->stackIndex -= 1;

            assert(next_prepend_callinfo != nullptr);
            next_prepend_callinfo->argv[(next_prepend_callinfo->argvIndex +=1)]
                     = currentStack->dataStack[currentStack->stackIndex];
            currentStack->stackIndex -= 1;

//            std::cout << "pusha"  << std::endl;

        }
            break;

        case OP_CODE::CALL: {
            currentStack->savedtPc = pc;                     // 保存 pc

            callStack[(this->callIndex+=1)] = next_prepend_callinfo;

            this->programCounter = op_code.num;

//            std::cout << "call " << op_code.num  << std::endl;
        }
            break;

        case OP_CODE::RET: {
            CallInfo* prevCall = currentStack->prevCall;

            prevCall->dataStack[(prevCall->stackIndex+=1)]
                    = currentStack->dataStack[currentStack->stackIndex];

            callStack[this->callIndex] = nullptr;
            this->callIndex -= 1;

            this->programCounter =  prevCall->savedtPc + 1;                    // 恢复 pc 并 + 1

//            std::cout << "ret " << prevCall->dataStack[prevCall->stackIndex]
//                      << ", prevPC:" << prevCall->currentPc << std::endl;

            if (this->next_prepend_callinfo == currentStack) {
                // std::cout << "next_prepend_callinfo == currentStack" << std::endl;
                this->next_prepend_callinfo = nullptr;
            }

            delete currentStack;
        }
            break;

        case OP_CODE::COPY_A : {
            currentStack->stackIndex += 1;
            currentStack->dataStack[currentStack->stackIndex]
                    = currentStack->argv[op_code.num];

//            std::cout << "COPY_A "
//                      << currentStack->argv[op_code.num]
//                      << " push into data-stack"
//                      << std::endl;

        }
            break;

        case OP_CODE::STOP :
            std::cout << "vm>Byte!" << std::endl;

            this->vmState = VM_STATE::TERMINATED;

            return;

        default:
            std::cerr << "vm error>unknow op:" << op << std::endl;

            return;
        }
    }
}

int TinyVM::getExecuteTimes() const
{
    return executeTimes;
}

bool TinyVM::isRunning() const
{
    return this->vmState == VM_STATE::RUNNING;
}

} // namespace qyvlik
