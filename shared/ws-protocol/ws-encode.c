#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
  unsigned char first_byte;
  unsigned char second_byte;
  uint16_t extended_payload_len;
  uint32_t maybe_mask;
  unsigned char *payload;
  size_t payload_len;
  size_t frame_len;
} ws_frame_t;

/* DEBUG FUNCTIONS */
void print_binary_byte(const unsigned char binary) {
  for (int i = 7; i >= 0; i--) {
    printf("%d", 1 & (binary >> i));
  }
  printf("\n");
}

void print_binary_bytes(const unsigned char *binary, int num_bytes) {
  if (!binary || num_bytes <= 0) {
    return;
  }
  for (int byte_index = 0; byte_index < num_bytes; byte_index++) {
    print_binary_byte(binary[byte_index]);
  }
  printf("\n");
}

/* HELPER FUNCTIONS */
void encode_payload(ws_frame_t *frame, const char *input_payload) {
  if (!input_payload || frame->payload_len <= 0) {
    fprintf(stderr, "payload is NULL or frame.payload_len <= 0");
    return;
  }

  frame->payload = malloc(frame->payload_len);
  if (!frame->payload) {
    fprintf(stderr, "unable to allocate frame->payload");
    return;
  }
  memcpy(frame->payload, input_payload, frame->payload_len);
}

void calculate_frame_len(ws_frame_t *frame) {
  frame->frame_len = 2; // first_byte + second_byte
  if (frame->extended_payload_len != 0) {
    frame->frame_len += 2;
  }
  if (frame->maybe_mask != 0) {
    frame->frame_len += 4;
  }
  frame->frame_len += frame->payload_len;
}

int assemble_frame(unsigned char *frame_buf, const ws_frame_t *frame) {
  if (!frame_buf || !frame) {
    fprintf(stderr, "frame_buf is NULL or frame is NULL");
    return -1;
  }
  size_t offset = 0;
  frame_buf[offset++] = frame->first_byte;
  frame_buf[offset++] = frame->second_byte;
  if (offset > frame->frame_len)
    return -1;

  if (frame->extended_payload_len) {
    frame_buf[offset++] = (frame->extended_payload_len >> 8) & 0xFF;
    frame_buf[offset++] = (frame->extended_payload_len >> 0) & 0xFF;
  }
  if (offset > frame->frame_len) {
    return -1;
  }
  if (frame->maybe_mask) {
    for (int i = 3; i >= 0; i--) {
      frame_buf[offset++] = (frame->maybe_mask >> 8 * i) & 0xFF;
    }
  }
  if (offset + frame->payload_len > frame->frame_len) {
    return -1;
  }
  for (size_t i = 0; i < frame->payload_len; i++) {
    frame_buf[offset++] = frame->payload[i];
  }
  if (offset != frame->frame_len) {
    return -1;
  }
  return 0;
}

int generate_mask(uint32_t *mask) {
  if (!mask) {
    return -1;
  }

  static int seeded = 0;
  if (!seeded) {
    srand(time(NULL));
    seeded = 1;
  }

  *mask = ((uint32_t)(rand() & 0xFF)) | ((uint32_t)(rand() & 0xFF) << 8) |
          ((uint32_t)(rand() & 0xFF) << 16) | ((uint32_t)(rand() & 0xFF) << 24);

  return 0;
}

int mask_unmask_payload(ws_frame_t *frame) {
  if (!frame->payload || frame->payload_len <= 0) {
    return -1;
  }

  for (size_t i = 0; i < frame->payload_len; i++) {
    unsigned char mask_byte = (frame->maybe_mask >> (8 * (3 - i % 4))) & 0xFF;
    frame->payload[i] ^= mask_byte;
  }
  return 0;
}

/* MAIN FUNCTION */
/* this implementation does not facilitate continuation frames or payloads
 * larger than 65,536 bytes */
unsigned char *ws_encode(const char *input_payload,
                         const unsigned int should_mask) {
  if (!input_payload) {
    return NULL;
  }

  ws_frame_t frame;

  // FIN=1, RSV1-3=0, opcode=0x1 (text)
  frame.first_byte = 0;
  frame.first_byte |= (1 << 7);
  frame.first_byte |= (1 << 0);

  frame.second_byte = 0;
  frame.extended_payload_len = 0;

  if (should_mask) {
    frame.second_byte |= (1 << 7);
  }

  frame.payload_len = strlen(input_payload);

  if (frame.payload_len <= 125) {
    // report the length in this byte
    frame.second_byte |= frame.payload_len;
  } else if (frame.payload_len <= 65536) {
    // report length as 126 and use the next 16 bits to report the actual
    // payload length
    frame.second_byte |= 126;
    frame.extended_payload_len |= frame.payload_len;
  } else {
    fprintf(stderr,
            "Unable to encode websocket frame because payload of length %lu is "
            "longer than 65,536 bytes\n",
            frame.payload_len);
    return NULL;
  }

  frame.maybe_mask = 0;
  if (should_mask) {
    if (generate_mask(&frame.maybe_mask) != 0) {
      return NULL;
    }
  }

  encode_payload(&frame, input_payload);
  if (!frame.payload) {
    return NULL;
  }

  if (frame.maybe_mask) {
    if (mask_unmask_payload(&frame) != 0) {
      return NULL;
    }
  }

  calculate_frame_len(&frame);

  unsigned char *frame_buf = malloc(frame.frame_len);
  if (!frame_buf) {
    fprintf(stderr, "unable to allocate frame_buf");
    free(frame.payload);
    return NULL;
  }
  if (assemble_frame(frame_buf, &frame) != 0) {
    fprintf(stderr, "Failed to assemble frame");
    free(frame_buf);
    free(frame.payload);
    return NULL;
  }

  // printf("frame:\n");
  // print_binary_bytes(frame_buf, frame.frame_len);

  free(frame.payload);
  return frame_buf;
}
