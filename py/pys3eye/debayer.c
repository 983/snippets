
void DebayerGray(int frame_width, int frame_height, const uint8_t* inBayer, uint8_t* outBuffer)
{
    // PSMove output is in the following Bayer format (GRBG):
    //
    // G R G R G R
    // B G B G B G
    // G R G R G R
    // B G B G B G
    //
    // This is the normal Bayer pattern shifted left one place.
    
    int             source_stride   = frame_width;
    const uint8_t*  source_row      = inBayer;                      // Start at first bayer pixel
    int             dest_stride     = frame_width;
    uint8_t*        dest_row        = outBuffer + dest_stride + 1;  // We start outputting at the second pixel of the second row's G component
    uint32_t R,G,B;
    
    // Fill rows 1 to height-1 of the destination buffer. First and last row are filled separately (they are copied from the second row and second-to-last rows respectively)
    for (int y = 0; y < frame_height-1; source_row += source_stride, dest_row += dest_stride, ++y)
    {
        const uint8_t* source       = source_row;
        const uint8_t* source_end   = source + (source_stride-2);                               // -2 to deal with the fact that we're starting at the second pixel of the row and should end at the second-to-last pixel of the row (first and last are filled separately)
        uint8_t* dest               = dest_row;
        
        // Row starting with Green
        if (y % 2 == 0)
        {
            // Fill first pixel (green)
            B = (source[source_stride] + source[source_stride + 2] + 1) >> 1;
            G = source[source_stride + 1];
            R = (source[1] + source[source_stride * 2 + 1] + 1) >> 1;
            *dest = (uint8_t)((R*77 + G*151 + B*28)>>8);
            
            source++;
            dest++;
            
            // Fill remaining pixel
            for (; source <= source_end - 2; source += 2, dest += 2)
            {
                // Blue pixel
                B = source[source_stride + 1];
                G = (source[1] + source[source_stride] + source[source_stride + 2] + source[source_stride * 2 + 1] + 2) >> 2;
                R = (source[0] + source[2] + source[source_stride * 2] + source[source_stride * 2 + 2] + 2) >> 2;
                dest[0] = (uint8_t)((R*77 + G*151 + B*28)>>8);

                //  Green pixel
                B = (source[source_stride + 1] + source[source_stride + 3] + 1) >> 1;
                G = source[source_stride + 2];
                R = (source[2] + source[source_stride * 2 + 2] + 1) >> 1;
                dest[1] = (uint8_t)((R*77 + G*151 + B*28)>>8);

            }
        }
        else
        {
            for (; source <= source_end - 2; source += 2, dest += 2)
            {
                // Red pixel
                B = (source[0] + source[2] + source[source_stride * 2] + source[source_stride * 2 + 2] + 2) >> 2;;
                G = (source[1] + source[source_stride] + source[source_stride + 2] + source[source_stride * 2 + 1] + 2) >> 2;;
                R = source[source_stride + 1];
                dest[0] = (uint8_t)((R*77 + G*151 + B*28)>>8);

                // Green pixel
                B = (source[2] + source[source_stride * 2 + 2] + 1) >> 1;
                G = source[source_stride + 2];
                R = (source[source_stride + 1] + source[source_stride + 3] + 1) >> 1;
                dest[1] = (uint8_t)((R*77 + G*151 + B*28)>>8);
            }
        }
        
        if (source < source_end)
        {
            B = source[source_stride + 1];
            G = (source[1] + source[source_stride] + source[source_stride + 2] + source[source_stride * 2 + 1] + 2) >> 2;
            R = (source[0] + source[2] + source[source_stride * 2] + source[source_stride * 2 + 2] + 2) >> 2;;
            dest[0] = (uint8_t)((R*77 + G*151 + B*28)>>8);

            source++;
            dest++;
        }
        
        // Fill first pixel of row (copy second pixel)
        uint8_t* first_pixel    = dest_row-1;
        first_pixel[0]          = dest_row[0];
        
        // Fill last pixel of row (copy second-to-last pixel). Note: dest row starts at the *second* pixel of the row, so dest_row + (width-2) * num_output_channels puts us at the last pixel of the row
        uint8_t* last_pixel             = dest_row + (frame_width - 2);
        uint8_t* second_to_last_pixel   = last_pixel - 1;
        last_pixel[0]                   = second_to_last_pixel[0];
    }
    
    // Fill first & last row
    for (int i = 0; i < dest_stride; i++)
    {
        outBuffer[i]                                    = outBuffer[i + dest_stride];
        outBuffer[i + (frame_height - 1)*dest_stride]   = outBuffer[i + (frame_height - 2)*dest_stride];
    }
}

void DebayerRGB(int frame_width, int frame_height, const uint8_t* inBayer, uint8_t* outBuffer, int inBGR)
{
    // PSMove output is in the following Bayer format (GRBG):
    //
    // G R G R G R
    // B G B G B G
    // G R G R G R
    // B G B G B G
    //
    // This is the normal Bayer pattern shifted left one place.

    int             num_output_channels     = 3;
    int             source_stride           = frame_width;
    const uint8_t*  source_row              = inBayer;                                              // Start at first bayer pixel
    int             dest_stride             = frame_width * num_output_channels;
    uint8_t*        dest_row                = outBuffer + dest_stride + num_output_channels + 1;    // We start outputting at the second pixel of the second row's G component
    int             swap_br                 = inBGR ? 1 : -1;

    // Fill rows 1 to height-1 of the destination buffer. First and last row are filled separately (they are copied from the second row and second-to-last rows respectively)
    for (int y = 0; y < frame_height-1; source_row += source_stride, dest_row += dest_stride, ++y)
    {
        const uint8_t* source       = source_row;
        const uint8_t* source_end   = source + (source_stride-2);                               // -2 to deal with the fact that we're starting at the second pixel of the row and should end at the second-to-last pixel of the row (first and last are filled separately)
        uint8_t* dest               = dest_row;     

        // Row starting with Green
        if (y % 2 == 0)
        {
            // Fill first pixel (green)
            dest[-1*swap_br]    = (source[source_stride] + source[source_stride + 2] + 1) >> 1;
            dest[0]             = source[source_stride + 1];
            dest[1*swap_br]     = (source[1] + source[source_stride * 2 + 1] + 1) >> 1;     

            source++;
            dest += num_output_channels;

            // Fill remaining pixel
            for (; source <= source_end - 2; source += 2, dest += num_output_channels * 2)
            {
                // Blue pixel
                uint8_t* cur_pixel  = dest;
                cur_pixel[-1*swap_br]   = source[source_stride + 1];
                cur_pixel[0]            = (source[1] + source[source_stride] + source[source_stride + 2] + source[source_stride * 2 + 1] + 2) >> 2;
                cur_pixel[1*swap_br]    = (source[0] + source[2] + source[source_stride * 2] + source[source_stride * 2 + 2] + 2) >> 2;             

                //  Green pixel
                uint8_t* next_pixel     = cur_pixel+num_output_channels;
                next_pixel[-1*swap_br]  = (source[source_stride + 1] + source[source_stride + 3] + 1) >> 1;                 
                next_pixel[0]           = source[source_stride + 2];
                next_pixel[1*swap_br]   = (source[2] + source[source_stride * 2 + 2] + 1) >> 1;
            }
        }
        else
        {
            for (; source <= source_end - 2; source += 2, dest += num_output_channels * 2)
            {
                // Red pixel
                uint8_t* cur_pixel  = dest;
                cur_pixel[-1*swap_br]   = (source[0] + source[2] + source[source_stride * 2] + source[source_stride * 2 + 2] + 2) >> 2;;
                cur_pixel[0]            = (source[1] + source[source_stride] + source[source_stride + 2] + source[source_stride * 2 + 1] + 2) >> 2;;
                cur_pixel[1*swap_br]    = source[source_stride + 1];

                // Green pixel
                uint8_t* next_pixel     = cur_pixel+num_output_channels;
                next_pixel[-1*swap_br]  = (source[2] + source[source_stride * 2 + 2] + 1) >> 1;
                next_pixel[0]           = source[source_stride + 2];
                next_pixel[1*swap_br]   = (source[source_stride + 1] + source[source_stride + 3] + 1) >> 1;
            }
        }

        if (source < source_end)
        {
            dest[-1*swap_br]    = source[source_stride + 1];
            dest[0]             = (source[1] + source[source_stride] + source[source_stride + 2] + source[source_stride * 2 + 1] + 2) >> 2;         
            dest[1*swap_br]     = (source[0] + source[2] + source[source_stride * 2] + source[source_stride * 2 + 2] + 2) >> 2;;            

            source++;
            dest += num_output_channels;
        }

        // Fill first pixel of row (copy second pixel)
        uint8_t* first_pixel        = dest_row-num_output_channels;
        first_pixel[-1*swap_br]     = dest_row[-1*swap_br];
        first_pixel[0]              = dest_row[0];
        first_pixel[1*swap_br]      = dest_row[1*swap_br];
    
        // Fill last pixel of row (copy second-to-last pixel). Note: dest row starts at the *second* pixel of the row, so dest_row + (width-2) * num_output_channels puts us at the last pixel of the row
        uint8_t* last_pixel             = dest_row + (frame_width - 2)*num_output_channels;
        uint8_t* second_to_last_pixel   = last_pixel - num_output_channels;
        
        last_pixel[-1*swap_br]          = second_to_last_pixel[-1*swap_br];
        last_pixel[0]                   = second_to_last_pixel[0];
        last_pixel[1*swap_br]           = second_to_last_pixel[1*swap_br];
    }

    // Fill first & last row
    for (int i = 0; i < dest_stride; i++)
    {
        outBuffer[i]                                    = outBuffer[i + dest_stride];
        outBuffer[i + (frame_height - 1)*dest_stride]   = outBuffer[i + (frame_height - 2)*dest_stride];
    }
}
