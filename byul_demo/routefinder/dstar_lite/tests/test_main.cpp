//test_coord.cpp

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include <locale.h>
#include <iostream>

int main(int argc, char** argv) {
    // setlocale(LC_ALL, "ko_KR.UTF-8");  // 💥 별이아빠님 핵심

    std::cout << "🌟 UTF-8 로케일로 테스트를 시작합니다!\n";

    doctest::Context context;

    context.applyCommandLine(argc, argv);

    int res = context.run();  // 테스트 실행

    if (context.shouldExit()) {
        return res;  // early return if test-only mode
    }

    // 여기서 테스트 이후 추가 로직 가능

    return res;
}
