#include "cmsis_access.h"

#define RX_DMA_BufferSize 200
#define READ_BIT(REG, BIT)    ((REG) & (BIT)
#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

static const uint8_t DMA_OFFSET_TABLE[] = {
    0x010UL,
    0x028UL,
    0x040UL,
    0x058UL,
    0x070UL,
    0x088UL,
    0x0A0UL,
    0x0B8UL,
};
uint8_t gRxDMABuffer[RX_DMA_BufferSize];

typedef struct Readline {
    DMA_TypeDef *DMAChannel;
    uint32_t DMARxChannelNumber;
    uint32_t delimeterLen;
    uint8_t *delimeter;
    uint8_t (*callback)(uint8_t *, uint32_t);
} Readline;

static void processByteStream(Readline *self, uint8_t *start, uint32_t size);
void usartRxRoutine();