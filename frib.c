#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Iterative Fibonacci
unsigned long long fibonacci_iterative(int n) {
    if (n <= 1) {
        return n;
    }
    
    unsigned long long prev = 0, curr = 1, next;
    for (int i = 2; i <= n; i++) {
        next = prev + curr;
        prev = curr;
        curr = next;
    }
    
    return curr;
}

// Recursive Fibonacci (not recommended for large n)
unsigned long long fibonacci_recursive(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci_recursive(n - 1) + fibonacci_recursive(n - 2);
    }
}

// Memoization Fibonacci
unsigned long long fibonacci_memoization(int n, unsigned long long* memo) {
    if (n <= 1) {
        return n;
    }
    
    if (memo[n] == 0) {
        memo[n] = fibonacci_memoization(n - 1, memo) + fibonacci_memoization(n - 2, memo);
    }
    
    return memo[n];
}

// Memoized Iterative Fibonacci
unsigned long long fibonacci_memoized_iterative(int n) {
    if (n <= 1) {
        return n;
    }

    unsigned long long* memo = (unsigned long long*)malloc((n + 1) * sizeof(unsigned long long));
    if (memo == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    memo[0] = 0;
    memo[1] = 1;

    for (int i = 2; i <= n; i++) {
        memo[i] = memo[i - 1] + memo[i - 2];
    }

    unsigned long long result = memo[n];
    free(memo);
    return result;
}

// clang -o frib frib.c && ./frib
int frib() {
    int n = 50; // You can change this value to test different Fibonacci numbers
    
    clock_t start, end;
    double cpu_time_used;

    // Iterative Fibonacci
    printf("Starting Iterative Fibonacci for n=%d...\n", n);
    start = clock();
    unsigned long long result1 = fibonacci_iterative(n);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Iterative Fibonacci for n=%d: %llu\n", n, result1);
    printf("Time taken (seconds): %f\n", cpu_time_used);


    // Memoized Iterative Fibonacci
    printf("Starting Memoized Iterative Fibonacci for n=%d...\n", n);
    start = clock();
    unsigned long long result4 = fibonacci_memoized_iterative(n);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Memoized Iterative Fibonacci for n=%d: %llu\n", n, result4);
    printf("Time taken (seconds): %f\n", cpu_time_used);


    // Memoization Fibonacci
    unsigned long long* memo = (unsigned long long*)malloc((n + 1) * sizeof(unsigned long long));
    for (int i = 0; i <= n; i++) {
        memo[i] = 0;
    }
    printf("Starting Memoization Fibonacci for n=%d...\n", n);
    start = clock();
    unsigned long long result3 = fibonacci_memoization(n, memo);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Memoization Fibonacci for n=%d: %llu\n", n, result3);
    printf("Time taken (seconds): %f\n", cpu_time_used);

    free(memo);

   // Recursive Fibonacci
    printf("Starting Recursive Fibonacci for n=%d...\n", n);
    start = clock();
    unsigned long long result2 = fibonacci_recursive(n);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Recursive Fibonacci for n=%d: %llu\n", n, result2);
    printf("Time taken (seconds): %f\n", cpu_time_used);



    return 0;
}
