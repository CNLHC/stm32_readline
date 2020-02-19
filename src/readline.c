#include "readline.h"
#include <stdint.h>
#include "cmsis_access.h"

static void processByteStream(Readline *self, uint8_t *start, uint32_t size) {
    static uint32_t curState;
    static uint32_t curIdx;
    static uint8_t gBuf[RX_DMA_BufferSize];
    for (uint32_t i = 0; i < size; i++) {
        uint8_t ch = start[i];
        gBuf[curIdx] = ch;

        if (ch == self->delimeter[curState]) {
            curState++;
        } else {
            curState = 0;
        }
        if (curState == self->delimeterLen) {
            self->callback(gBuf, curIdx + 1);
            curState = 0;
            curIdx = 0;
            continue;
        } else {
            curIdx++;
        }
    }
}

inline static uint32_t getDMADataLength(DMA_TypeDef *DMAx, uint32_t Stream) {
    register uint32_t dma_base_addr = (uint32_t)DMAx;
    return (READ_BIT(((DMA_Stream_TypeDef *)(dma_base_addr + DMA_OFFSET_TABLE[Stream]))->NDTR, DMA_SxNDT));
}

void ReadLineCallback(Readline *self) {
    static uint32_t gCachedPos;
    uint32_t curPos;
    curPos = RX_DMA_BufferSize - getDMADataLength(self->DMAChannel, self->DMARxChannelNumber);
    if (curPos != gCachedPos) {
        if (curPos > gCachedPos) {
            // normal mode
            processByteStream(self, &gRxDMABuffer[gCachedPos], curPos - gCachedPos);
        } else {
            // overflow mode
            processByteStream(self, &gRxDMABuffer[gCachedPos], RX_DMA_BufferSize - gCachedPos);
            if (curPos > 0) {
                processByteStream(self, &gRxDMABuffer[0], curPos);
            }
        }
    }
    gCachedPos = curPos;
    if (gCachedPos == RX_DMA_BufferSize) {
        gCachedPos = 0;
    }
}
