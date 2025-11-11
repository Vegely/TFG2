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
#ifndef STRING_FORMATTER_H
#define STRING_FORMATTER_H

#include <stdint.h>

#define MAXIMUM_LENGTH 8192
#define ACK  0x06
#define NACK 0x15
#define START_BYTE 0x02

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Compute CRC8 of given data buffer.
 * IN: data Pointer to input bytes
 * IN: len  Number of bytes
 * OUT: Computed CRC8
 */
uint8_t crc8(const uint8_t *data, uint16_t len);

/**
 * Build a message frame with header, length, payload, and CRC.
 * IN: payload Pointer to input data
 * IN: Length of payload (must be <= MAXIMUM_LENGTH)
 * IN: Output buffer for the formatted frame
 * OUT: Total frame length on success, -1 on error
 */
// Format of a string: HEADER | Length  | Payload | Checksum
int build_message(const uint8_t *payload, uint16_t length, uint8_t *frame);

/**
 * Verify integrity of a received message frame.
 * IN: frame      Pointer to received frame buffer
 * IN: total_len  Total number of bytes in the frame
 * OUT: 1 if valid, 0 if invalid
 */
// Format of a string: HEADER | Length  | Payload | Checksum
int verify_message(const uint8_t *frame, uint16_t total_len);

#ifdef __cplusplus
}
#endif

#endif /* STRING_FORMATTER_H */




/* [] END OF FILE */
