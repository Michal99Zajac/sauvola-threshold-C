int read_pgm_header(const char *file_name, int *num_rows, int *num_cols,
                    int *max_color);

int read_pgm_data(unsigned char *image, const char *file_name,
                  int header_length, int num_rows, int num_cols, int max_val);

int write_pgm_image(const char *file_name, unsigned char *image_data,
                    int num_rows, int num_cols, int max_val);
