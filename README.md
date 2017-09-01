Simple TARGA image library
--------------------------

API :

`targa_t *targa_open (const char *filename)`

`void targa_free (targa_t *image)`

`int targa_is_gray (targa_t* image)`

`targa_t *targa_copy (targa_t *image)`

`int targa_get_index (targa_t *image, int x, int y, int color)`

`uint8_t targa_get_color (targa_t *image, int x, int y, int color)`

`void targa_set_color (targa_t *image, int x, int y, int color, uint8_t value)`

`void targa_write (targa_t *image, const char *filename)`

`targa_t *targa_rotate (targa_t *image, int deg)`: rotate image using TARGA_ROTATE_90 | TARGA_ROTATE_180 | TARGA_ROTATE_270

`targa_t *targa_gray (targa_t *image)`

`uint8_t *targa_histogram (targa_t *image, int color)`

`targa_t *targa_flip_h (targa_t *image)`

