#include <arm/intergrator-cp/drivers/qemu-PL110.h>
#include <types.h>
#include <beryllium/drivers/framebuffer.h>
uint16_t * volatile qemu_pl_fb;
pl110_mmio_t	*plio;

void qemu_pl110_write_index(uint32_t index,uint8_t r,uint8_t g,uint8_t b)
{
	qemu_pl_fb[index] = b << (5 + 6) | g << 5 | r ;
}

void qemu_pl110_write(int x, int y,uint8_t r,uint8_t g,uint8_t b)
{
	uint32_t place = (y*640)+x;
	qemu_pl110_write_index(place,r,g,b);
}

void qemu_pl110_clear()
{
	for (int x = 0; x < (640 * 480) - 10; ++x)
		qemu_pl110_write_index(x,0x0,0x0,0x0 + (x % 640 * 480));
}
void qemu_pl110_test()
{

}
extern video_device;
void qemu_pl110_start(void)
{
	plio = (pl110_mmio_t*)PL110_IOBASE;
	plio->tim0 = 0x3f1f3f9c;
	plio->tim1 = 0x080b61df;
	plio->upbase = 0x200000;
	plio->control = 0x1829;
	qemu_pl_fb = (uint16_t*)0x200000;
	qemu_pl110_clear();
	video_device = 1;
}