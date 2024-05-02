#include <sal.h>
#include <cassert>

void test_version() {
    assert(strlen(sl_get_version()) != 0);
}

int main(void) {
    test_version();
    return 0;
}