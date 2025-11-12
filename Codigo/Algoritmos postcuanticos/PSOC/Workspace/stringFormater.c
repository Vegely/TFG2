/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "stringFormater.h"
#include <string.h>

uint8_t crc8(const uint8_t *data, uint16_t len)
{
    uint8_t crc = 0x00;
    while (len--) {
        uint8_t extract = *data++;
        for (uint8_t i = 8; i; i--) {
            uint8_t sum = (crc ^ extract) & 0x01;
            crc >>= 1;
            if (sum)
                crc ^= 0x8C;
            extract >>= 1;
        }
    }
    return crc;
}

int build_message(const uint8_t *payload, uint16_t length, uint8_t *frame)
{
    if (payload == NULL || frame == NULL)
        return -1;

    if (length > MAXIMUM_LENGTH)
        return -1; // too long

    frame[0] = START_BYTE;
    frame[1] = (length >> 8) & 0xFF;
    frame[2] = length & 0xFF;

    memcpy(&frame[3], payload, length);

    uint8_t crc = crc8(payload, length);
    frame[3 + length] = crc;

    return (int)(length + 4); // total frame length
}

int verify_message(const uint8_t *frame, uint16_t total_len)
{
    if (frame == NULL)
        return 0;

    if (total_len < 4)
        return 0;

    if (frame[0] != START_BYTE)
        return 0;

    uint16_t length = ((uint16_t)frame[1] << 8) | frame[2];
    if ((length + 4) != total_len)
        return 0;

    uint8_t crc_calc = crc8(&frame[3], length);
    uint8_t crc_recv = frame[3 + length];

    return (crc_calc == crc_recv);
}

/* [] END OF FILE */
