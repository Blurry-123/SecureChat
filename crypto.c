#include "crypto.h"
#include <string.h>

void xor_crypt(
    char *data,
    int length,
    const char *key)
{
    int key_length = (int)strlen(key);

    if (key_length == 0)
        return;

    for (int i = 0; i < length; i++)
    {
        data[i] ^= key[i % key_length];
    }
}