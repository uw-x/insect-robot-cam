JPEG Encoder for Cortex-M Series

It requires small SRAM. Because it proccesses an image data by each MCU(16x16 block) like data stream.


Spec:
*Image format: "8bit Grayscale" and "YUV422(CbYCrY format)" 1-dimensional array
*FPU mode: "Floating point" and "Fixed point"


Requires:
*Strage of target image data & JPEG data size.
*2Kbytes SRAM.

I guess it can run on other 32-bits MCU(Micro Controller Unit).
