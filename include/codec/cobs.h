

#ifndef MU_COBS_H
#define MU_COBS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

struct mu_cobs_if
{
  size_t (*encode)(const void *data, size_t length, uint8_t *buffer);
  size_t (*decode)(const uint8_t *buffer, size_t length, void *data);
};

extern struct mu_cobs_if muCobs;

#ifdef __cplusplus
}
#endif

#endif //MU_COBS_H
