#include <stddef.h>
#include <stdint.h>

#ifndef WS_H
#define WS_H

/**
 * WebSocket Protocol Constants
 */
#define WS_FRAME_HEADER_SIZE_2 2   // Minimum frame header size
#define WS_MASK_SIZE_4 4           // Mask size in bytes
#define WS_EXTENDED_LEN_2 2        // Extended length field size
#define WS_MAX_PAYLOAD_126 126     // Maximum payload for single-byte length
#define WS_MAX_PAYLOAD_65536 65536 // Maximum supported payload size

/**
 * WebSocket frame buffer structure
 * Contains the encoded frame data and its length
 */
typedef struct {
  unsigned char *frame_buf; // Pointer to encoded frame data
  size_t len;               // Length of the frame data
} ws_frame_buf_t;

/**
 * WebSocket frame structure
 * Represents a parsed WebSocket frame
 */
typedef struct {
  unsigned char first_byte;      // FIN, RSV, and opcode
  unsigned char second_byte;     // MASK and payload length
  uint16_t extended_payload_len; // Extended payload length (if > 125)
  uint32_t maybe_mask;           // Masking key (if masked)
  unsigned char *payload;        // Pointer to payload data
  size_t payload_len;            // Payload length
  size_t frame_len;              // Total frame length
} ws_frame_t;

/**
 * Encode a string into a WebSocket frame
 * @param input_payload The string to encode
 * @param should_mask Whether to mask the payload (1 = mask, 0 = no mask)
 * @return Pointer to encoded frame buffer, or NULL on error
 * @note Caller must free the returned buffer using free()
 */
ws_frame_buf_t *ws_encode(char *input_payload, const unsigned int should_mask);

/**
 * Decode a WebSocket frame into a string
 * @param input Pointer to the frame data
 * @param input_len Length of the frame data
 * @return Pointer to decoded string, or NULL on error
 * @note Caller must free the returned string using free()
 */
char *ws_decode(const unsigned char *input, size_t input_len);

/**
 * Apply or remove masking from a WebSocket frame payload
 * @param frame Pointer to the frame structure
 * @return 0 on success, -1 on error
 */
int mask_unmask_payload(ws_frame_t *frame);

/**
 * Print a single byte in binary format
 * @param binary The byte to print
 */
void print_binary_byte(const unsigned char binary);

/**
 * Print multiple bytes in binary format
 * @param binary Pointer to the bytes to print
 * @param num_bytes Number of bytes to print
 */
void print_binary_bytes(const unsigned char *binary, int num_bytes);

#endif