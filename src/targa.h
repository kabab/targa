/* 
 * Specs from :
 * http://www.dca.fee.unicamp.br/~martino/disciplinas/ea978/tgaffs.pdf
 */

#ifndef _TARGA_H_
#define _TARGA_H_

#include <stdint.h>
#include <stdio.h>

#define	TARGA_RED			2
#define TARGA_GREEN		1
#define TARGA_BLUE		0
#define TARGA_GRAY		0

#define TARGA_ROTATE_90		0
#define TARGA_ROTATE_180	1
#define TARGA_ROTATE_270	2

/*
 * Prevent the compiler from padding the attribute
 */
typedef struct  __attribute__((__packed__)) targa_header_t_ {
	uint8_t id_length;
	uint8_t color_map_type;
	uint8_t type;

	uint16_t first_entry_index;
	uint16_t length;
	uint8_t entry_size;

	uint16_t x_origin;
	uint16_t y_origin;
	uint16_t width;
	uint16_t height;
	uint8_t pixel_depth;
	uint8_t image_desc;

}	targa_header_t;


typedef struct _targa_t {
	FILE *file;
	struct targa_header_t_ header;
	uint8_t *data;
} targa_t;

targa_t *targa_open(const char* filename);
void targa_free (targa_t *image);
int targa_is_gray (targa_t* image);
targa_t *targa_copy (targa_t *image);
int targa_get_index (targa_t *image, int x, int y, int color);
uint8_t targa_get_color (targa_t *image, int x, int y, int color);
void targa_set_color (targa_t *image, int x, int y, int color, uint8_t value);
void targa_write (targa_t *image, const char *filename);
targa_t *targa_rotate (targa_t *image, int deg);
targa_t *targa_gray (targa_t *image);
uint8_t *targa_histogram (targa_t *image, int color);
targa_t *targa_flip_h (targa_t *image);

#endif
