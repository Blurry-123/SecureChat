#include "crypto.h"
#include <string.h>
void xor_crypt(char *data, int length, const char *key)
{
    int keyLen = (int)strlen(key);
    if (keyLen <= 0)
        return;
    for (int index = 0; index < length; index++)
    {
        int keyIndex = index % keyLen;
        data[index] = data[index] ^ key[keyIndex];
    }
}

