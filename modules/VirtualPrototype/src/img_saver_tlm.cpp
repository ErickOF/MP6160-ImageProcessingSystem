#ifndef IMG_SAVER_TLM_CPP
#define IMG_SAVER_TLM_CPP

#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include "img_saver_tlm.hpp"
#include "address_map.hpp"

#include "important_defines.hpp"

//For pre-loading images
#include "include/stb_image.h"
#include "include/stb_image_write.h"

void img_saver_tlm::do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address)
{
  dbgimgtarmodprint(true, "Nothing to do here");
}

void img_saver_tlm::do_when_write_transaction(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  if (address < IMG_SAVER_ID_SIZE)
  {
    unsigned char *write_ptr = (unsigned char *)&local_id;

    memcpy(write_ptr, data, data_length);
  }
  else
  {
    int channels, pixel_count;
    unsigned char *img_ptr;

    switch(local_id) {
    case 0:
      channels = 3;
      pixel_count = IMAG_COLS * IMAG_ROWS * channels;
      img_ptr = new unsigned char[pixel_count];

      memcpy(img_ptr, img_input_ptr, pixel_count);

      for (int i = 0; i < IMAG_COLS * IMAG_ROWS * channels; i += channels) {
        // Swap the R (index i) and B (index i+2) channels
        std::swap(img_ptr[i], img_ptr[i + 2]);
      }

      stbi_write_png("output_image_0.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);

      dbgimgtarmodprint(true, "Saving original image");
      break;
    case 1:
      channels = 1;
      pixel_count = IMAG_COLS * IMAG_ROWS * channels;
      img_ptr = new unsigned char[pixel_count];

      memcpy(img_ptr, img_inprocess_a_ptr, pixel_count);
      stbi_write_png("output_image_1.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);

      dbgimgtarmodprint(true, "Saving results from grayscale conversion image");
      break;
    case 2:
      channels = 1;
      pixel_count = IMAG_COLS * IMAG_ROWS * channels;
      img_ptr = new unsigned char[pixel_count];

      memcpy(img_ptr, img_inprocess_d_ptr, pixel_count);
      stbi_write_png("output_image_2.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);

      dbgimgtarmodprint(true, "Saving results from filtering image");
      break;
    case 3:
      unsigned char *read_ptr;
      short int *value_ptr;

      channels = 1;
      pixel_count = IMAG_COLS * IMAG_ROWS * channels;
      img_ptr = new unsigned char[pixel_count];
      read_ptr = new unsigned char[pixel_count * sizeof(short int)];

      memcpy(read_ptr, img_inprocess_b_ptr, pixel_count * sizeof(short int));
      value_ptr = (short int *)read_ptr;

      for (int i = 0; i < IMAG_ROWS; i++)
      {
        for (int j = 0; j < IMAG_COLS; j++)
        {
          if ((*(value_ptr + ((i * IMAG_COLS) + j)) > 255) || ((*(value_ptr + ((i * IMAG_COLS) + j)) < -255)))
          {
            *(img_ptr + ((i * IMAG_COLS) + j)) = 255;
          }
          else if (*(value_ptr + ((i * IMAG_COLS) + j)) < 0)
          {
            *(img_ptr + ((i * IMAG_COLS) + j)) = (unsigned char)(-*(value_ptr + ((i * IMAG_COLS) + j)));
          }
          else
          {
            *(img_ptr + ((i * IMAG_COLS) + j)) = (unsigned char)(*(value_ptr + ((i * IMAG_COLS) + j)));
          }
        }
      }

      stbi_write_png("output_image_3_A.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);

      memcpy(read_ptr, img_inprocess_c_ptr, pixel_count * sizeof(short int));
      value_ptr = (short int *)read_ptr;

      for (int i = 0; i < IMAG_ROWS; i++)
      {
        for (int j = 0; j < IMAG_COLS; j++)
        {
          if ((*(value_ptr + ((i * IMAG_COLS) + j)) > 255) || ((*(value_ptr + ((i * IMAG_COLS) + j)) < -255)))
          {
            *(img_ptr + ((i * IMAG_COLS) + j)) = 255;
          }
          else if (*(value_ptr + ((i * IMAG_COLS) + j)) < 0)
          {
            *(img_ptr + ((i * IMAG_COLS) + j)) = (unsigned char)(-*(value_ptr + ((i * IMAG_COLS) + j)));
          }
          else
          {
            *(img_ptr + ((i * IMAG_COLS) + j)) = (unsigned char)(*(value_ptr + ((i * IMAG_COLS) + j)));
          }
        }
      }

      delete[] read_ptr;

      stbi_write_png("output_image_3_B.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);

      dbgimgtarmodprint(true, "Saving results from gradient computation images");
      break;
    case 4:
      channels = 1;
      pixel_count = IMAG_COLS * IMAG_ROWS * channels;
      img_ptr = new unsigned char[pixel_count];

      memcpy(img_ptr, img_inprocess_a_ptr, pixel_count);
      stbi_write_png("output_image_4.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);

      dbgimgtarmodprint(true, "Saving results from magnitude unification image");
      break;
    case 5:
      channels = 1;
      pixel_count = IMAG_COLS * IMAG_ROWS * channels;
      img_ptr = new unsigned char[pixel_count];

      memcpy(img_ptr, img_output_ptr, pixel_count);
      stbi_write_png("output_image_5.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);

      dbgimgtarmodprint(true, "Saving results from memory in transmiter image");
      break;
    case 6:
      channels = 1;
      pixel_count = IMAG_COLS * IMAG_ROWS * channels;
      img_ptr = new unsigned char[pixel_count];

      memcpy(img_ptr, img_output_dec_ptr, pixel_count);
      stbi_write_png("output_image_6.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);

      dbgimgtarmodprint(true, "Saving results from memory in transmiter image");
      break;
    default:
      channels = 3;
      pixel_count = IMAG_COLS * IMAG_ROWS * channels;
      img_ptr = new unsigned char[pixel_count];

      memcpy(img_ptr, img_input_ptr, pixel_count);
      stbi_write_png("output_image_0.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);

      dbgimgtarmodprint(true, "WARNING: The image was not valid %0d, the original image be saved", local_id);
      break;
    }

    delete[] img_ptr;
  }
}

#endif // IMG_SAVER_TLM_CPP
