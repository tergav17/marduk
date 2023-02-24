#ifndef NABU_FDC_H
#define NABU_FDC_H

/* includes */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* defines */

// This should only really be 2
#define FDC_NUM_DISKS 2

/* structs / types */

typedef struct {
	// Disk data in sectors
	char *source;
	uint8_t *data;
	int size;
	
	// Disk geometry
	int sec_size;
	int sec_count;
	int track_count;
	
	// Emulated drive head position
	int track;
} fdc_disk_t;

typedef struct {

	// Drives A and B
	fdc_disk_t disk[FDC_NUM_DISKS];
	
	// Registers
	uint8_t command;
	uint8_t status;
	uint8_t track;
	uint8_t sector;

} fdc_context_t;

/* prototypes */
void fdc_init(fdc_context_t *fd);
char fdc_attach(fdc_context_t *fd, char *fname, int unit);
void fdc_write(fdc_context_t *fd, uint8_t address, uint8_t data);
uint8_t fdc_read(fdc_context_t *fd, uint8_t address);

#endif