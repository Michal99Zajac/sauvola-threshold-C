int read_ppm_header(const char *filename, int *num_rows, int *num_cols,
                    int *max_color);

int write_ppm_data(const char *file_name, unsigned char *red_channel,
                   unsigned char *green_channel, unsigned char *blue_channel,
                   int num_rows, int num_cols, int max_color);

int read_ppm_data(unsigned char *red_channel, unsigned char *green_channel,
                  unsigned char *blue_channel, const char *filename,
                  int header_length, int num_rows, int num_cols,
                  int max_color);

int write_ppm_image(const char *file_name, unsigned char *red_channel,
                    unsigned char *green_channel, unsigned char *blue_channel,
                    int num_rows, int num_cols, int max_color);

void ppm_sauvola_flow(const char *input_file_name,
                      const char *output_file_name);

double ppm_sauvola_flow_with_integral_image(const char *input_file_name,
                                            const char *output_file_name);
