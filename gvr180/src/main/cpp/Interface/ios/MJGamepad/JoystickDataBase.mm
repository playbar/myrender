#include "JoystickDataBase.h"

void JoystickDataBase::ResetData(char* bytes)
{
    buffer = bytes;
    nPosition = 0;
}

int32_t JoystickDataBase::ReadInt(int32_t nBits, bool bSignBit) {
    int32_t result = 0;
    if ((nBits <= 0) || (nBits > 16)) {
        //__android_log_print(ANDROID_LOG_ERROR, TAG, "Invalid number of bits to unpack: %d", nBits);
        return result;
    }

    int32_t start_byte = nPosition / 8;
    int32_t bit_length = nBits + nPosition - (start_byte << 3);
    int32_t byte_length = (bit_length + 7) / 8;
    int32_t tail_bits = (byte_length << 3) - bit_length;
    for (int32_t i = start_byte; i < start_byte + byte_length; i++) {
        result = (result << 8) | (0xFF & buffer[i]);
    }
    result = (result >> tail_bits) & (-1 + (1 << nBits));
    nPosition += nBits;

    if ((!(bSignBit)) || (((result & (1 << nBits)) - 1) == 0) || ((result >> (nBits - 1)) == 0)) {
        return result;
    } else {
        return (result | -1 << nBits);
    }
}
