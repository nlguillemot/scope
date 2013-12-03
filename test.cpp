#include "scope.hpp"

#include <iostream>

struct HasScopeExitInDtor {
    ~HasScopeExitInDtor() {
        scope (exit) {
            std::cout << "scope (exit) in dtor success test\n";
        };
        try {
            std::cout << "scope (exit) in dtor failure test\n";
            throw 1;
        } catch (...) { }
    }
};

struct HasScopeSuccessInDtor {
    ~HasScopeSuccessInDtor() {
        std::cout << std::flush;
        scope (success) {
            std::cout << "error: scope (success) used in dtor\n";
        };
    }
};

int main()
{
    {
        const char* captureTest = nullptr;
        scope (exit) {
            std::cout << "scope (exit) success test\n";
            std::cout << "captureTest: " << captureTest;
        };
        scope (success) {
            std::cout << "scope (success) success test\n";
        };
        scope (failure) {
            std::cout << "scope (failure) success test\n";
        };
        captureTest = "ok\n";
    }
    try {
        HasScopeSuccessInDtor s;
        
        scope (exit) {
            std::cout << "scope (exit) failure test\n";
        };
        scope (success) {
            std::cout << "scope (success) failure test\n";
        };
        scope (failure) {
            std::cout << "scope (failure) failure test\n";
        };
        
        HasScopeExitInDtor e;
        
        throw 1;
    } catch (...) { }
}
