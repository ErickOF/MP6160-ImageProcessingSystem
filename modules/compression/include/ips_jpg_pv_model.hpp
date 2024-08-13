#include <systemc.h>

/**
 * @brief jpg_output module. Federico Cruz
 * It takes the image and compresses it into jpeg format
 * It is done in 4 parts:
 * 1. Divides the image in 8x8 pixel blocks; for 8-bit grayscale images the a level shift is done by substracting 128 from each pixel.
 * 2. Discrete Cosine Transform (DCT) of the 8x8 image.
 * 3. Each transformed 8x8 block is divided by a quantization value for each block entry.
 * 4. Each quantized 8x8 block is reordered by a Zig-Zag sequence into a array of size 64.
 * *5. Entropy compression by variable length encoding (huffman). Used to maximize compression. Not implemented here.
 */
#define PI 3.1415926535897932384626433832795
#define BLOCK_ROWS 8
#define BLOCK_COLS 8

SC_MODULE(jpg_output)
{

  //-----------Internal variables-------------------
  // const int Block_rows = 8;
  // const int Block_cols = 8;
  double *image;
  int image_rows = 480;
  int image_cols = 640;
  signed char eob = 127; // end of block

  int quantificator[8][8] = {// quantization table
                             {16, 11, 10, 16, 24, 40, 51, 61},
                             {12, 12, 14, 19, 26, 58, 60, 55},
                             {14, 13, 16, 24, 40, 57, 69, 56},
                             {14, 17, 22, 29, 51, 87, 80, 62},
                             {18, 22, 37, 56, 68, 109, 103, 77},
                             {24, 35, 55, 64, 81, 104, 113, 92},
                             {49, 64, 78, 87, 103, 121, 120, 101},
                             {72, 92, 95, 98, 112, 100, 103, 99}};

  int zigzag_index[64] = {// zigzag table
                          0, 1, 5, 6, 14, 15, 27, 28,
                          2, 4, 7, 13, 16, 26, 29, 42,
                          3, 8, 12, 17, 25, 30, 41, 43,
                          9, 11, 18, 24, 31, 40, 44, 53,
                          10, 19, 23, 32, 39, 45, 52, 54,
                          20, 22, 33, 38, 46, 51, 55, 60,
                          21, 34, 37, 47, 50, 56, 59, 61,
                          35, 36, 48, 49, 57, 58, 62, 63};

  // Constructor for compressor
  SC_HAS_PROCESS(jpg_output);
  jpg_output(sc_module_name jpg_compressor, int im_rows = 480, int im_cols = 640) : sc_module(jpg_compressor)
  {
    if (im_rows % BLOCK_ROWS == 0)
    {
      image_rows = im_rows;
    }
    else
    {
      image_rows = (im_rows / BLOCK_ROWS + 1) * BLOCK_ROWS;
    }

    if (im_cols % BLOCK_COLS == 0)
    {
      image_cols = im_cols;
    }
    else
    {
      image_cols = (im_cols / BLOCK_COLS + 1) * BLOCK_COLS;
    }

    image = new double[image_rows * image_cols];

    // initialize the image matrix to avoid nan
    for (int i = 0; i < (image_rows * image_cols); i++)
    {
      image[i] = 0;
    }
  } // End of Constructor

  //------------Code Starts Here-------------------------
  void input_pixel(int pixel_value, int row, int col)
  {
    double *i_row = &image[row * image_cols];
    i_row[col] = double(pixel_value);
  }

  void output_pixel(int *Pixel, int row, int col)
  {
    double *i_row = &image[row * image_cols];
    *Pixel = int(i_row[col]);
  }

  void output_byte(signed char *element, int index)
  {
    element[index] = image[index];
  }

  void jpeg_compression(int *output_size)
  {
    // Level shift
    for (int i = 0; i < (image_rows * image_cols); i++)
    {
      image[i] = image[i] - 128;
    }

    int number_of_blocks = image_rows * image_cols / (BLOCK_ROWS * BLOCK_COLS);

#ifndef USING_TLM_TB_EN
    int block_output[number_of_blocks][BLOCK_ROWS * BLOCK_COLS] = {0};
    int block_output_size[number_of_blocks] = {0};
#else

    int **block_output = new int *[number_of_blocks];
    int *block_output_size = new int[number_of_blocks];

    for (int i = 0; i < number_of_blocks; i++)
    {
      block_output[i] = new int[BLOCK_ROWS * BLOCK_COLS];
    }

    for (int i = 0; i < number_of_blocks; i++)
    {
      block_output_size[i] = 0;

      for (int j = 0; j < BLOCK_ROWS * BLOCK_COLS; j++)
      {
        block_output[i][j] = 0;
      }
    }
#endif // USING_TLM_TB_EN
    int block_counter = 0;
    *output_size = 0;

    for (int row = 0; row < image_rows; row += BLOCK_ROWS)
    {
      for (int col = 0; col < image_cols; col += BLOCK_COLS)
      {
        // Divided the image in 8×8 blocks
        dct(row, col);
        quantization(row, col);

        zigzag(row, col, &block_output_size[block_counter], block_output[block_counter]);
        *output_size += block_output_size[block_counter] + 1;
        block_counter++;
      }
    }

    int output_counter = 0;

    for (int block_index = 0; block_index < number_of_blocks; block_index++)
    {
      for (int out_index = 0; out_index < block_output_size[block_index]; out_index++)
      {
        image[output_counter] = block_output[block_index][out_index];
        output_counter++;
      }

      image[output_counter] = eob;
      output_counter++;
    }
#ifdef USING_TLM_TB_EN
    for (int i = 0; i < number_of_blocks; i++)
    {
      delete[] block_output[i];
    }

    delete[] block_output;
    delete[] block_output_size;
#endif // USING_TLM_TB_EN
  }

  void dct(int row_offset, int col_offset)
  {
    double cos_table[BLOCK_ROWS][BLOCK_COLS];

    // make the cosine table
    for (int row = 0; row < BLOCK_ROWS; row++)
    {
      for (int col = 0; col < BLOCK_COLS; col++)
      {
        cos_table[row][col] = cos((((2 * row) + 1) * col * PI) / 16);
      }
    }

    double temp = 0.0;

    for (int row = row_offset; row < row_offset + BLOCK_ROWS; row++)
    {
      double *i_row = &image[row * image_cols];

      for (int col = col_offset; col < col_offset + BLOCK_COLS; col++)
      {
        // i_row[col] = cos_table[row-row_offset][col-col_offset];
        temp = 0.0;

        for (int x = 0; x < 8; x++)
        {
          double *x_row = &image[(x + row_offset) * image_cols];

          for (int y = 0; y < 8; y++)
          {
            temp += x_row[y + col_offset] * cos_table[x][row - row_offset] * cos_table[y][col - col_offset];
          }
        }

        if ((row - row_offset == 0) && (col - col_offset == 0))
        {
          temp /= 8.0;
        }
        else if (((row - row_offset == 0) && (col - col_offset != 0)) || ((row - row_offset != 0) && (col - col_offset == 0)))
        {
          temp /= (4.0 * sqrt(2.0));
        }
        else
        {
          temp /= 4.0;
        }

        i_row[col] = temp;
      }
    }
  }

  void quantization(int row_offset, int col_offset)
  {
    for (int row = row_offset; row < row_offset + BLOCK_ROWS; row++)
    {
      double *i_row = &image[row * image_cols];

      for (int col = col_offset; col < col_offset + BLOCK_COLS; col++)
      {
        i_row[col] = round(i_row[col] / quantificator[row - row_offset][col - col_offset]);
      }
    }
  }

  void zigzag(int row_offset, int col_offset, int *block_output_size, int *block_output)
  {
    int index_last_non_zero_value = 0;

    // index to last non-zero in a block zigzag array
    for (int row = row_offset; row < row_offset + BLOCK_ROWS; row++)
    {
      double *i_row = &image[row * image_cols];

      for (int col = col_offset; col < col_offset + BLOCK_COLS; col++)
      {
        int temp_index = zigzag_index[(row - row_offset) * 8 + (col - col_offset)];
        block_output[temp_index] = i_row[col];

        if (i_row[col] != 0 && temp_index > index_last_non_zero_value)
        {
          index_last_non_zero_value = temp_index + 1;
        }
      }
    }

    *block_output_size = index_last_non_zero_value;
  }
};
