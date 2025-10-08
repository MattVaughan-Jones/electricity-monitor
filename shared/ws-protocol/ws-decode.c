#include "ws.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* HELPER FUNCTIONS */
int set_first_byte(ws_frame_t *frame, const unsigned char *input) {
  if (!frame || !input) {
    return -1;
  }

  frame->first_byte = input[0];
  return 0;
}

int set_second_byte(ws_frame_t *frame, const unsigned char *input) {
  if (!frame || !input) {
    return -1;
  }

  frame->second_byte = input[1];
  return 0;
}

int set_len(ws_frame_t *frame, const unsigned char *input, size_t input_len) {
  if (!frame || !input || input_len < 2) {
    return -1;
  }

  frame->payload_len =
      frame->second_byte &
      0x7F; // use only the last 7 bits (first bit is mask flag)
  frame->extended_payload_len = 0;

  if (frame->payload_len == WS_MAX_PAYLOAD_126) {
    if (input_len < 4) {
      fprintf(stderr, "WebSocket frame decode error: Cannot read "
                      "extended_payload_len from payload shorter than 4 bytes");
      return -1;
    }
    frame->extended_payload_len |= input[2];
    frame->extended_payload_len |= input[3];
  } else if (frame->payload_len > WS_MAX_PAYLOAD_126) {
    fprintf(stderr, "WebSocket frame decode error: Received a websocket "
                    "payload larger than 65536 bytes");
    return -1;
  }

  return 0;
}

int set_mask(ws_frame_t *frame, const unsigned char *input) {
  if (!frame || !input) {
    return -1;
  }

  int offset = 0;
  if (frame->extended_payload_len) {
    offset = WS_EXTENDED_LEN_2;
  }

  // Mask read in network byte order
  frame->maybe_mask |= (input[2 + offset] << 3 * 8);
  frame->maybe_mask |= (input[3 + offset] << 2 * 8);
  frame->maybe_mask |= (input[4 + offset] << 1 * 8);
  frame->maybe_mask |= (input[5 + offset]);

  return 0;
}

int check_input_len(ws_frame_t *frame, const unsigned char *input,
                    size_t input_len) {
  if (!frame || !input) {
    return -1;
  }

  size_t calculated_input_length = WS_FRAME_HEADER_SIZE_2;
  if ((frame->second_byte >> 7) & 0x1) {
    calculated_input_length += WS_MASK_SIZE_4;
  }
  if (frame->extended_payload_len) {
    calculated_input_length += WS_EXTENDED_LEN_2;
    calculated_input_length += frame->extended_payload_len;
  } else {
    calculated_input_length += frame->payload_len;
  }

  if (calculated_input_length != input_len) {
    fprintf(stderr,
            "WebSocket frame decode error: Calculated input length of %luB "
            "does not match what was received "
            "(%luB)\n",
            calculated_input_length, input_len);
    return -1;
  }

  return 0;
}

int set_payload(ws_frame_t *frame, const unsigned char *input) {
  if (!frame || !input) {
    return -1;
  }

  size_t input_position = 2; // not masked and payload_len <=125
  if ((frame->second_byte >> 7) & 0x1)
    input_position += WS_MASK_SIZE_4;
  if (frame->extended_payload_len)
    input_position += WS_EXTENDED_LEN_2;

  size_t actual_payload_len = frame->extended_payload_len
                                  ? frame->extended_payload_len
                                  : frame->payload_len;

  frame->payload = malloc(actual_payload_len);
  if (frame->payload == NULL) {
    fprintf(stderr,
            "WebSocket frame decode error: Failed to allocate frame->payload");
    return -1;
  }

  size_t payload_position = 0;
  while (payload_position < actual_payload_len) {
    frame->payload[payload_position] = input[input_position];

    input_position++;
    payload_position++;
  }

  return 0;
}

int parse_input(ws_frame_t *frame, const unsigned char *input,
                size_t input_len) {
  if (!frame || !input || input_len < WS_FRAME_HEADER_SIZE_2) {
    return -1;
  }

  if (set_first_byte(frame, input) != 0) {
    return -1;
  }
  // printf("parsed first byte: ");
  // print_binary_byte(frame->first_byte);

  if (set_second_byte(frame, input) != 0) {
    return -1;
  }
  // printf("parsed second byte: ");
  // print_binary_byte(frame->second_byte);

  const int is_masked = (frame->second_byte >> 7) & 0x1;
  // printf("mask: %d\n", is_masked);

  if (set_len(frame, input, input_len) != 0) {
    return -1;
  }
  // printf("payload_len: %lu\n", frame->payload_len);
  // printf("extended_payload_len: %u\n", frame->extended_payload_len);

  if (is_masked) {
    if (set_mask(frame, input) != 0) {
      return -1;
    }
    // printf("parsed mask:\n");
    // print_binary_bytes((const unsigned char *)&frame->maybe_mask,
    // WS_MASK_SIZE_4);
  }

  if (check_input_len(frame, input, input_len) != 0) {
    return -1;
  }

  if (set_payload(frame, input) != 0) {
    return -1;
  }
  // printf("parsed payload:\n");
  // if (frame->extended_payload_len) {
  //   print_binary_bytes(frame->payload, frame->extended_payload_len);
  // } else {
  //   print_binary_bytes(frame->payload, frame->payload_len);
  // }

  if (is_masked) {
    if (mask_unmask_payload(frame) != 0) {
      return -1;
    }
  }

  return 0;
}

int check_fin(ws_frame_t *frame) {
  if (!frame) {
    return -1;
  }

  // Only support FIN = 1; No continuation frames.
  if (!((frame->first_byte >> 7) & 1)) {
    fprintf(stderr,
            "WebSocket frame decode error: Continuation frames not supported");
    return -1;
  }
  return 0;
}

int check_opcode(ws_frame_t *frame) {
  if (!frame) {
    return -1;
  }

  int opcode = frame->first_byte & 0xF; // opcode is the last 4 bits

  // Only support text opcode
  if (!(opcode == 0x1)) { // opcode = 1 corresponds to text payload
    fprintf(stderr,
            "WebSocket frame decode error: Only text payloads supported");
    return -1;
  }
  return 0;
}

char *ws_decode(const unsigned char *input, size_t input_len) {
  if (!input) {
    return NULL;
  }

  ws_frame_t frame = {0};
  if (parse_input(&frame, input, input_len) != 0) {
    return NULL;
  }

  if (check_fin(&frame) != 0) {
    return NULL;
  }

  if (check_opcode(&frame) != 0) {
    return NULL;
  }

  int message_len = frame.extended_payload_len ? frame.extended_payload_len
                                               : frame.payload_len;
  char *message = malloc(message_len + 1);
  if (!message) {
    fprintf(stderr, "WebSocket frame decode error: Unable to allocate message");
    return NULL;
  }
  memcpy(message, (const char *)frame.payload, message_len);
  message[message_len] = '\0';

  if (frame.payload) {
    free(frame.payload);
  }

  return message;
}
