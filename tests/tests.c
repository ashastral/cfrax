#include <stdio.h>

#include "verify.h"

extern int test_vector();
extern int test_model();
extern int test_canvas();

int main(int argc, char *argv[])
{
    verify_call(test_vector());
    verify_call(test_model());
    verify_call(test_canvas());
    verify_return();
}
