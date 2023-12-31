#include <stdio.h>
#include <stdlib.h>

#include <llvm-c/Core.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>

int llvm_fib_iter()
{
    // Initialize LLVM

    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();

    LLVMContextRef context = LLVMContextCreate();
    LLVMModuleRef module = LLVMModuleCreateWithNameInContext("fib_module", context);

    // Create Function Prototype: int fib(int)
    LLVMTypeRef param_types[] = {LLVMInt32TypeInContext(context)};
    LLVMTypeRef return_type = LLVMInt32TypeInContext(context);
    LLVMTypeRef fib_func_type = LLVMFunctionType(return_type, param_types, 1, 0);
    LLVMValueRef fib_func = LLVMAddFunction(module, "fib", fib_func_type);

    // Create Basic Blocks
    LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(context, fib_func, "entry");
    LLVMBasicBlockRef loop_header = LLVMAppendBasicBlockInContext(context, fib_func, "loop_header");
    LLVMBasicBlockRef iterative_case = LLVMAppendBasicBlockInContext(context, fib_func, "iterative_case");
    LLVMBasicBlockRef exit = LLVMAppendBasicBlockInContext(context, fib_func, "exit"); // New exit block

    // Function Arguments
    LLVMValueRef n = LLVMGetParam(fib_func, 0);

    // Builder
    LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);
    LLVMPositionBuilderAtEnd(builder, entry);

    // Initialize variables
    LLVMValueRef fib_1 = LLVMBuildAlloca(builder, LLVMInt32TypeInContext(context), "fib_1");
    LLVMBuildStore(builder, LLVMConstInt(LLVMInt32TypeInContext(context), 1, 0), fib_1);
    LLVMValueRef fib_2 = LLVMBuildAlloca(builder, LLVMInt32TypeInContext(context), "fib_2");
    LLVMBuildStore(builder, LLVMConstInt(LLVMInt32TypeInContext(context), 1, 0), fib_2);

    // Loop Setup
    LLVMValueRef i = LLVMBuildAlloca(builder, LLVMInt32TypeInContext(context), "i");
    LLVMBuildStore(builder, LLVMConstInt(LLVMInt32TypeInContext(context), 2, 0), i); // Start from 2

    // Branch to loop_header
    LLVMBuildBr(builder, loop_header);

    // Loop Header
    LLVMPositionBuilderAtEnd(builder, loop_header);
    LLVMValueRef loop_condition = LLVMBuildICmp(builder, LLVMIntSLT, LLVMBuildLoad2(builder, LLVMInt32TypeInContext(context), i, "i"), n, "loop_cond");
    LLVMBuildCondBr(builder, loop_condition, iterative_case, exit); // Branch to exit block when loop condition is false

    // Iterative Case Block
    LLVMPositionBuilderAtEnd(builder, iterative_case);

    // Calculate Fibonacci for the current iteration
    LLVMValueRef fib_current = LLVMBuildAdd(builder, LLVMBuildLoad2(builder, LLVMInt32TypeInContext(context), fib_1, ""), LLVMBuildLoad2(builder, LLVMInt32TypeInContext(context), fib_2, ""), "fib_current");

    // Update fib_1 and fib_2 for the next iteration
    LLVMBuildStore(builder, LLVMBuildLoad2(builder, LLVMInt32TypeInContext(context), fib_2, ""), fib_1);
    LLVMBuildStore(builder, fib_current, fib_2);

    // Update the loop counter
    LLVMValueRef current_i = LLVMBuildLoad2(builder, LLVMInt32TypeInContext(context), i, "current_i");
    LLVMValueRef next_i = LLVMBuildAdd(builder, current_i, LLVMConstInt(LLVMInt32TypeInContext(context), 1, 0), "next_i");
    LLVMBuildStore(builder, next_i, i);

    // Branch back to the loop header
    LLVMBuildBr(builder, loop_header);

    // Return the result
    LLVMPositionBuilderAtEnd(builder, exit); // Position builder at the end of exit block
    LLVMBuildRet(builder, LLVMBuildLoad2(builder, LLVMInt32TypeInContext(context), fib_2, ""));

    // Verify and Compile
    char *error = NULL;
    LLVMVerifyModule(module, LLVMAbortProcessAction, &error);

    LLVMDumpModule(module);

    LLVMLinkInMCJIT();
    LLVMInitializeNativeTarget();

    // Create execution engine
    LLVMExecutionEngineRef engine;
    if (LLVMCreateExecutionEngineForModule(&engine, module, &error) != 0)
    {
        fprintf(stderr, "Failed to create execution engine\n");
        abort();
    }

    // Run the function
    int fib_n = 30; // Change this to the number you want to compute the Fibonacci of
    LLVMGenericValueRef args[] = {LLVMCreateGenericValueOfInt(LLVMInt32Type(), fib_n, 0)};
    LLVMGenericValueRef result = LLVMRunFunction(engine, fib_func, 1, args);

    // Print the result
    printf("fib(%d) = %lld\n", fib_n, LLVMGenericValueToInt(result, 0));

    // Clean up ExecutionEngine
    if (LLVMRemoveModule(engine, module, &module, &error) != 0)
    {
        fprintf(stderr, "Failed to remove module from execution engine\n");
        abort();
    }
    LLVMDisposeExecutionEngine(engine);

    // Cleanup
    LLVMDisposeModule(module);
    LLVMDisposeBuilder(builder);
    LLVMContextDispose(context);

    return 0;
}

int llvm_frib_rec()
{
    // Initialize LLVM

    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();

    LLVMContextRef context = LLVMContextCreate();
    LLVMModuleRef module = LLVMModuleCreateWithNameInContext("fib_module", context);

    // Create Function Prototype: int fib(int)
    LLVMTypeRef param_types[] = {LLVMInt32TypeInContext(context)};
    LLVMTypeRef return_type = LLVMInt32TypeInContext(context);
    LLVMTypeRef fib_func_type = LLVMFunctionType(return_type, param_types, 1, 0);
    LLVMValueRef fib_func = LLVMAddFunction(module, "fib", fib_func_type);

    // Create Basic Blocks
    LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(context, fib_func, "entry");
    LLVMBasicBlockRef recursive_case = LLVMAppendBasicBlockInContext(context, fib_func, "recursive_case");
    LLVMBasicBlockRef n_1_case = LLVMAppendBasicBlockInContext(context, fib_func, "n_1_case");

    // Function Arguments
    LLVMValueRef n = LLVMGetParam(fib_func, 0);

    // Builder
    LLVMBuilderRef builder = LLVMCreateBuilderInContext(context);
    LLVMPositionBuilderAtEnd(builder, entry);

    // n_1_case Logic
    LLVMValueRef n_1_case_cond = LLVMBuildICmp(builder, LLVMIntSLE, n, LLVMConstInt(LLVMInt32TypeInContext(context), 1, 0), "n_1_case_cond");
    LLVMBuildCondBr(builder, n_1_case_cond, n_1_case, recursive_case);

    // n_1_case Block
    LLVMPositionBuilderAtEnd(builder, n_1_case);
    LLVMBuildRet(builder, n);

    // Recursive Case Block
    LLVMPositionBuilderAtEnd(builder, recursive_case);
    LLVMValueRef n_minus_one = LLVMBuildSub(builder, n, LLVMConstInt(LLVMInt32TypeInContext(context), 1, 0), "n_minus_one");
    LLVMValueRef n_minus_two = LLVMBuildSub(builder, n, LLVMConstInt(LLVMInt32TypeInContext(context), 2, 0), "n_minus_two");

    // Now call the function recursively
    LLVMValueRef args_n_minus_one[] = {n_minus_one};
    LLVMValueRef call_fib_n_minus_one = LLVMBuildCall2(builder, fib_func_type, fib_func, args_n_minus_one, 1, "call_fib_n_minus_one");

    LLVMValueRef args_n_minus_two[] = {n_minus_two};
    LLVMValueRef call_fib_n_minus_two = LLVMBuildCall2(builder, fib_func_type, fib_func, args_n_minus_two, 1, "call_fib_n_minus_two");

    // Compute the sum of the two recursive calls
    LLVMValueRef sum = LLVMBuildAdd(builder, call_fib_n_minus_one, call_fib_n_minus_two, "sum");

    // Return the sum
    LLVMBuildRet(builder, sum);

    // Verify and Compile
    char *error = NULL;
    LLVMVerifyModule(module, LLVMAbortProcessAction, &error);

    LLVMDumpModule(module);

    LLVMLinkInMCJIT();
    LLVMInitializeNativeTarget();

    // Create execution engine
    LLVMExecutionEngineRef engine;
    if (LLVMCreateExecutionEngineForModule(&engine, module, &error) != 0)
    {
        fprintf(stderr, "Failed to create execution engine\n");
        abort();
    }

    // Run the function
    int fib_n = 30; // Change this to the number you want to compute the Fibonacci of
    LLVMGenericValueRef args[] = {LLVMCreateGenericValueOfInt(LLVMInt32Type(), fib_n, 0)};
    LLVMGenericValueRef result = LLVMRunFunction(engine, fib_func, 1, args);

    // Print the result
    printf("fib(%d) = %lld\n", fib_n, LLVMGenericValueToInt(result, 0));

    // Clean up ExecutionEngine
    // char *error = NULL;
    if (LLVMRemoveModule(engine, module, &module, &error) != 0)
    {
        fprintf(stderr, "Failed to remove module from execution engine\n");
        abort();
    }
    LLVMDisposeExecutionEngine(engine);

    // cleanup
    LLVMDisposeModule(module);
    LLVMDisposeBuilder(builder);
    LLVMContextDispose(context);

    return 0;
}

//  clang `llvm-config --cflags --libs --ldflags --system-libs` -fcolor-diagnostics -fansi-escape-codes -g main.c -o main
int main()
{
    llvm_fib_iter();
    llvm_frib_rec();
}