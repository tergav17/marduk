/*
 * nabu_fdc.c
 *
 * This component gives a high-level emulation of the NABU floppy disk
 * drive controller card. The primary component of this card is an FD1797
 * IC, which is directly mapped into the first 4 addresses of the card. The
 * last address in the cards memory space is used for device identification,
 * drive density configuration, and drive selection.
 *
 * The emulated FD1797 is very primitive, and does not take full track data
 * nor timing into account. This could change later...
 *
 * The emulated core has a private address space starting at 0x0 and running
 * through 0xF. This can be mapped to whatever slot it needs to be in the Z80
 * I/O address space
 */
 
#include "nabu_fdc.h"
 
/*
 * Initializes a NABU FDC card. Any existing open disks should be closed
 * before attemting to init the FDC again
 *
 * fd = Pointer to FDC context object
 */
void fdc_init(fdc_context_t *fd)
{
	int i;
	
	// Reset all disk drives
	for (i = 0; i < FDC_NUM_DISKS; i++) {
		fd->disk[i].source = NULL;
		fd->disk[i].data = NULL;
		fd->disk[i].size = 0;
	}
	
	// Reset registers
	fd->command = fd->status = fd->track = fd->sector = 0;
}

/*
 * Attaches a floppy disk image to a virtual drive. The contents of the image
 * will be read and buffered in disk.data[]. This data will be acted on, and
 * only written back when the disk image it detached from the drive. Only
 * disk images of certain sizes can be attached, as each one has different
 * track and sector geometry.
 *
 * fd = Pointer to FDC context object
 * fname = File name
 * unit = Unit number
 * Returns 0 if successful
 *		   1 if file cannout be read
 *		   2 if disk image is invalid
 *         3 if invalid unit
 */
char fdc_attach(fdc_context_t *fd, char *fname, int unit)
{
	FILE *img;
	fdc_disk_t *disk;
	int size;
	
	// Unit sanity check
	if (unit >= FDC_NUM_DISKS)
		return 3;
	
	disk = &fd->disk[unit];
	
	// Open image
	img = fopen(fname, "rb");
	if (!img)
		return 1;
	
	// Detect image type
	fseek(img, 0, SEEK_END);
	size = ftell(img);
	if (size == 204800) {
		// 1024x5x40 image
		disk->size = size;
		disk->sec_size = 1024;
		disk->sec_count = 5;
		disk->track_count = 40;
	} else
		return 2;
	fseek(img, 0, SEEK_SET);
	
	// Buffer image
	// The current strategy is to buffer the entire disk image
	// If you are trying to run this on a RAM-constrained system, then this
	// may need to be changed
	disk->data = (uint8_t *) malloc(sizeof(uint8_t) * disk->size);
	if (fread(disk->data, 1, disk->size, img) < disk->size) {
		// Return error if cannot read
		free(disk->data);
		fclose(img);
		return 1;
	}
	fclose(img);
	
	// Copy source file name for writeback
	disk->source = (char *) malloc(sizeof(char) * (strlen(fname) + 1));
	strcpy(disk->source, fname);
	
	return 0;
	
}

/*
 * Executes an FDC command
 *
 * fd = Pointer to FDC context object
 * data = obvious
 */
void fdc_command(fdc_context_t *fd, uint8_t data)
{
	
}

/*
 * Returns FDC status
 *
 * fd = Pointer to FDC context object
 */
uint8_t fdc_status(fdc_context_t *fd)
{
	
}

/*
 * Data write
 *
 * fd = Pointer to FDC context object
 * data = obvious
 */
void fdc_dwrite(fdc_context_t *fd, uint8_t data)
{
	
}

/*
 * Data read
 *
 * fd = Pointer to FDC context object
 */
uint8_t fdc_dread(fdc_context_t *fd)
{
	
}

/*
 * Density write
 *
 * fd = Pointer to FDC context object
 * data = obvious
 */
void fdc_wden(fdc_context_t *fd, uint8_t data)
{
	
}

/*
 * Writes a byte to the virtual FDC address space
 *
 * fd = Pointer to FDC context object
 * address = I/O address
 * data = I/O data
 */
void fdc_write(fdc_context_t *fd, uint8_t address, uint8_t data)
{
	switch (address) {
		case 0x0:
			fdc_command(fd, data);
			break;
			
		case 0x1:
			fd->track = data;
			break;
			
		case 0x2:
			fd->sector = data;
			break;
			
		case 0x3:
			fdc_dwrite(fd, data);
			break;
			
		case 0xF:
			fdc_wden(fd, data);
			break;
			
		default:
			break;
	}
}

/*
 * Reads a byte from the virtual FDC address space
 *
 * fd = Pointer to FDC context object
 * address = I/O address
 * Returns data
 */
void fdc_write(fdc_context_t *fd, uint8_t address)
{
	switch (address) {
		case 0x0:
			return fdc_status(fd);
			
		case 0x1:
			return fd->track;
			
		case 0x2:
			return fd->sector;
			break;
			
		case 0x3:
			return fdc_dread(fd);
			
		case 0xF:
			return 0x10;
			
		default:
			return 0xFF;
	}
}