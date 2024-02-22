#include "bsp_tp.h"
#include "tp_config.h"
#include <stdio.h>

static const uint16_t s_tp_value_360[] = {
    0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 10, 11,
    12, 13, 13, 14, 15, 16, 16, 17, 18, 19, 19, 20, 21, 21, 22, 23,
    24, 24, 25, 26, 27, 27, 28, 29, 30, 30, 31, 32, 33, 33, 34, 35,
    36, 36, 37, 38, 39, 39, 40, 41, 41, 42, 43, 44, 44, 45, 46, 47,
    47, 48, 49, 50, 50, 51, 52, 53, 53, 54, 55, 56, 56, 57, 58, 59,
    59, 60, 61, 61, 62, 63, 64, 64, 65, 66, 67, 67, 68, 69, 70, 70,
    71, 72, 73, 73, 74, 75, 76, 76, 77, 78, 79, 79, 80, 81, 81, 82,
    83, 84, 84, 85, 86, 87, 87, 88, 89, 90, 90, 91, 92, 93, 93, 94,
    95, 96, 96, 97, 98, 99, 99, 100, 101, 101, 102, 103, 104, 104, 105, 106,
    107, 107, 108, 109, 110, 110, 111, 112, 113, 113, 114, 115, 116, 116, 117, 118,
    119, 119, 120, 121, 121, 122, 123, 124, 124, 125, 126, 127, 127, 128, 129, 130,
    130, 131, 132, 133, 133, 134, 135, 136, 136, 137, 138, 139, 139, 140, 141, 141,
    142, 143, 144, 144, 145, 146, 147, 147, 148, 149, 150, 150, 151, 152, 153, 153,
    154, 155, 156, 156, 157, 158, 159, 159, 160, 161, 161, 162, 163, 164, 164, 165,
    166, 167, 167, 168, 169, 170, 170, 171, 172, 173, 173, 174, 175, 176, 176, 177,
    178, 179, 179, 180, 181, 181, 182, 183, 184, 184, 185, 186, 187, 187, 188, 189,
    190, 190, 191, 192, 193, 193, 194, 195, 196, 196, 197, 198, 199, 199, 200, 201,
    201, 202, 203, 204, 204, 205, 206, 207, 207, 208, 209, 210, 210, 211, 212, 213,
    213, 214, 215, 216, 216, 217, 218, 219, 219, 220, 221, 221, 222, 223, 224, 224,
    225, 226, 227, 227, 228, 229, 230, 230, 231, 232, 233, 233, 234, 235, 236, 236,
    237, 238, 239, 239, 240, 241, 241, 242, 243, 244, 244, 245, 246, 247, 247, 248,
    249, 250, 250, 251, 252, 253, 253, 254, 255, 256, 256, 257, 258, 259, 259, 260,
    261, 261, 262, 263, 264, 264, 265, 266, 267, 267, 268, 269, 270, 270, 271, 272,
    273, 273, 274, 275, 276, 276, 277, 278, 279, 279, 280, 281, 281, 282, 283, 284,
    284, 285, 286, 287, 287, 288, 289, 290, 290, 291, 292, 293, 293, 294, 295, 296,
    296, 297, 298, 299, 299, 300, 301, 301, 302, 303, 304, 304, 305, 306, 307, 307,
    308, 309, 310, 310, 311, 312, 313, 313, 314, 315, 316, 316, 317, 318, 319, 319,
    320, 321, 321, 322, 323, 324, 324, 325, 326, 327, 327, 328, 329, 330, 330, 331,
    332, 333, 333, 334, 335, 336, 336, 337, 338, 339, 339, 340, 341, 341, 342, 343,
    344, 344, 345, 346, 347, 347, 348, 349, 350, 350, 351, 352, 353, 353, 354, 355,
    356, 356, 357, 358, 359, 359,
};

void tp_set_sleep_mode(bool sleep_enable)
{
    if (sleep_enable)
    {
        tp_write(0xFD, 0x01);
    }
    else
    {
        tp_write(0xA5, 0x03);
    }
}

bool tp_get_data(int16_t *p_x, int16_t *p_y)
{
    uint8_t read_data[5];

    tp_read(0x5c, read_data, 5);

    if (read_data[0] == 128 || (read_data[0] == 0 && read_data[1] == 0 \
       && read_data[2] == 0 && read_data[3] == 0 && read_data[4] == 0))
    {
        /**
         * the finger leave the touchpad
         */
        return false;
    }
    else
    {
        /**
         * when interrupt generates, set the mcu not to sleep until finger leave the touchpad
         * or the mcu may lost the interrupts
         */
        uint16_t raw_x = (((read_data[1] & 0x0f)) << 8) | read_data[2];
        uint16_t raw_y = (((read_data[3] & 0x0f)) << 8) | read_data[4];

        // raw x and y are reported from 70 to 2010
        *p_x = s_tp_value_360[(raw_x - 70) >> 2];
        *p_y = s_tp_value_360[(raw_y - 70) >> 2];

        return true;
    }
}

void tp_init(void)
{
    tp_config_init((void *)0);
}