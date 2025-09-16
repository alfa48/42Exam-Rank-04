// main.c para testar
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

int sandbox(void (*f)(void), unsigned int timeout, bool verbose);

void good(void) { }
void bad_exit(void) { _exit(42); }
void bad_sig(void) { int *p = NULL; *p = 1; }
void slow(void) { sleep(3); }

int main(void)
{
    sandbox(good, 1, true);
    sandbox(bad_exit, 1, true);
    sandbox(bad_sig, 1, true);
    sandbox(slow, 1, true);
}
