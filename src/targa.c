#include "targa.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

targa_t *targa_open (const char *filename) {
	int data_size, nskip;
	targa_t *ret = (targa_t*)malloc(sizeof(targa_t));
	ret->file = fopen(filename, "r");
	fread(&ret->header, 1, sizeof(targa_header_t), ret->file);
	data_size = ret->header.width * ret->header.height * ret->header.pixel_depth / 8;
	ret->data = (uint8_t*) malloc(data_size);

	nskip = ret->header.id_length + ret->header.entry_size * ret->header.length / 8;
	// skip color map file
	fseek(ret->file, nskip, SEEK_CUR);
	fread(ret->data, 1, data_size, ret->file);

	return ret;
}

void targa_free (targa_t *image) {
	if (image == NULL) return;
	if (image->file > 0 ) {
		fclose(image->file);
		image->file = 0;
	}
	if (image->data != NULL) {
		free(image->data);
		image->data = 0;
	}
	free(image);
}

/*
 * Check if the image is black and white
 */
int targa_is_gray (targa_t* image) {
	return image->header.type == 3 && image->header.pixel_depth == 8;
}

targa_t *targa_copy (targa_t *image) {
	int data_size;
	targa_t *ret = (targa_t*)malloc(sizeof(targa_t));
	memcpy(ret, image, sizeof(targa_t));
	data_size = image->header.width * image->header.height * image->header.pixel_depth / 8;
 	ret->data = (uint8_t*) malloc(data_size);
	memcpy(ret->data, image->data, data_size);
	return ret;
}

int targa_get_index (targa_t *image, int x, int y, int color) {
	uint8_t depth;
	uint16_t width, height;

	depth = image->header.pixel_depth / 8;
	width = image->header.width;
	height = image->header.height;

	return depth * y * width + x * depth + color;
}

uint8_t targa_get_color (targa_t *image, int x, int y, int color) {
	int index;
	index = targa_get_index (image, x, y, color);
	return image->data[index];
}

void targa_set_color (targa_t *image, int x, int y, int color, uint8_t value) {
	int index;
	index = targa_get_index (image, x, y, color);
	image->data[index] = value;
}

void targa_write (targa_t *image, const char *filename) {
	int data_size;
	int width, height, depth;
	FILE* f = fopen(filename, "w");
	if (f == NULL) {
		perror("fopen");
		return;
	}

	depth = image->header.pixel_depth / 8;
	width = image->header.width;
	height = image->header.height;
	image->file = f;
	data_size = width * height * depth;

	fwrite(&image->header, sizeof(targa_header_t), 1, f);
	fwrite(image->data, data_size, 1, f);
	// fclose(f);
}

/*
 * rotate image using TARGA_ROTATE_90 | TARGA_ROTATE_180 | TARGA_ROTATE_270
 *
 */
targa_t *targa_rotate (targa_t *image, int deg) {
	targa_t *ret;
	uint16_t tmp, x, y, width, height;
	uint8_t red, blue, green;

	ret = targa_copy (image);

	switch (deg) {
	case TARGA_ROTATE_90:
		width = image->header.height;
		height = image->header.width;
		ret->header.width = width;
		ret->header.height = height;
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				red = targa_get_color(image, y, x, TARGA_RED);
				blue = targa_get_color(image, y, x, TARGA_BLUE);
				green = targa_get_color(image, y, x, TARGA_GREEN);
				targa_set_color (ret, width - 1 - x, y, TARGA_RED, red);
				targa_set_color (ret, width - 1 - x, y, TARGA_BLUE, blue);
				targa_set_color (ret, width - 1 - x, y, TARGA_GREEN, green);
			}
		}
		break;
	case TARGA_ROTATE_180:
		width = image->header.width;
		height = image->header.height;
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				red = targa_get_color(image, x, y, TARGA_RED);
				blue = targa_get_color(image, x, y, TARGA_BLUE);
				green = targa_get_color(image, x, y, TARGA_GREEN);
				targa_set_color (ret, x, height - y - 1, TARGA_RED, red);
				targa_set_color (ret, x, height - y - 1, TARGA_BLUE, blue);
				targa_set_color (ret, x, height - y - 1, TARGA_GREEN, green);
			}
		}
		break;
	case TARGA_ROTATE_270:
		width = image->header.height;
		height = image->header.width;
		ret->header.width = width;
		ret->header.height = height;
		for (y = 0; y < height; y++) {
			for (x = 0; x < width; x++) {
				red = targa_get_color(image, y, x, TARGA_RED);
				blue = targa_get_color(image, y, x, TARGA_BLUE);
				green = targa_get_color(image, y, x, TARGA_GREEN);
				targa_set_color (ret, x, height - y - 1, TARGA_RED, red);
				targa_set_color (ret, x, height - y - 1, TARGA_BLUE, blue);
				targa_set_color (ret, x, height - y - 1, TARGA_GREEN, green);
			}
		}
		break;
	}

	return ret;
}

targa_t *targa_gray (targa_t *image) {
	targa_t *ret;
	uint16_t x, y, width, height;
	uint8_t	red, blue, green, gray;

	ret = (targa_t*) malloc(sizeof(targa_t));
	memcpy(&ret->header, &image->header, sizeof(targa_header_t));
	// http://www.gamers.org/dEngine/quake3/TGA.txt
	ret->header.type = 3; // Uncompressed, black and white images
	ret->header.pixel_depth = 8;
	width = ret->header.width;
	height = ret->header.height;

	ret->data  = (uint8_t*) malloc(width * height);

	for (x = 0; x < width; x++) {
			for (y = 0; y < height; y++) {
				red = targa_get_color(image, x, y, TARGA_RED);
				blue = targa_get_color(image, x, y, TARGA_BLUE);
				green = targa_get_color(image, x, y, TARGA_GREEN);
				// https://en.wikipedia.org/wiki/YIQ
				gray = red * 0.299 + green * 0.587 + blue * 0.114;
				targa_set_color(ret, x, y, TARGA_GRAY, gray);
			}
	}

	return ret;
}

uint8_t *targa_histogram (targa_t *image, int color) {
	uint8_t *ans, level;
	uint16_t x, y, width, height;
	ans = (uint8_t*) malloc(256);
	width = image->header.width;
	height = image->header.height;
	memset(ans, 0, 256);
	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			level = targa_get_color(image, x, y, color);
			ans[level]++;
		}
	}
	return ans;
}

targa_t *targa_flip_h (targa_t *image) {
	targa_t *ret;
	uint8_t gray, red, blue, green;
	uint16_t width, height, x, y;
	ret = targa_copy(image);
	width = image->header.width;
	height = image->header.height;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			if (targa_is_gray(image)) {
				gray = targa_get_color(image, x, y, TARGA_GRAY);
				targa_set_color (ret, width - x - 1, y, TARGA_GRAY, gray);
			} else {
				red = targa_get_color(image, x, y, TARGA_RED);
				blue = targa_get_color(image, x, y, TARGA_BLUE);
				green = targa_get_color(image, x, y, TARGA_GREEN);
				targa_set_color (ret, width - x - 1, y, TARGA_RED, red);
				targa_set_color (ret, width - x - 1, y, TARGA_BLUE, blue);
				targa_set_color (ret, width - x - 1, y, TARGA_GREEN, green);
			}
		}
	}
	return ret;
}

int test (int argc, char *argv[]) {
	targa_t *image;
	targa_t *image90, *image180, *image270, *image_gray;
	int i, j;

	image = targa_open(argv[1]);
	printf("Open %s\n", argv[1]);
	printf("%u %u\n", image->header.width, image->header.height);
	printf("%u\n", image->header.pixel_depth);
	printf("%u\n", image->header.color_map_type);
	printf("%u\n", image->header.id_length);
	printf("%u\n", image->header.type);
	// printf("%u\n", targa_get_color(image, atoi(argv[2]), atoi(argv[3]), TARGA_GRAY));

	/*
	image90 = targa_rotate (image, TARGA_ROTATE_90);
	image180 = targa_rotate (image, TARGA_ROTATE_180);
	image270 = targa_rotate (image, TARGA_ROTATE_270);

	targa_write(image90, "hello90.tga");
	targa_write(image180, "hello180.tga");
	targa_write(image270, "hello270.tga");
	*/

	image_gray = targa_gray(image);
	targa_write(image_gray, "hello_gray.tga");
	targa_free(image_gray);
	targa_free(image);


	/*
	targa_free(image90);
	targa_free(image180);
	targa_free(image270);
	*/

	return 0;
}
