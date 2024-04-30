
#ifndef _H_CONTEXT_GETTER
#define _H_CONTEXT_GETTER

/**
 * Sets the context variables `m`, `bits`, and `c`
 * 0 to 20 correspond to the settings suggested to use from HElib
 * 
 * The remaing settings are used for testing
 * 
 * s =  0  =>  m =  16384, bits =  119, c = 2
 * s =  1  =>  m =  32768, bits =  358, c = 6
 * s =  2  =>  m =  32768, bits =  299, c = 3
 * s =  3  =>  m =  32768, bits =  239, c = 2
 * s =  4  =>  m =  65536, bits =  725, c = 8
 * s =  5  =>  m =  65536, bits =  717, c = 6
 * s =  6  =>  m =  65536, bits =  669, c = 4
 * s =  7  =>  m =  65536, bits =  613, c = 3
 * s =  8  =>  m =  65536, bits =  558, c = 2
 * s =  9  =>  m = 131072, bits = 1445, c = 8
 * s = 10  =>  m = 131072, bits = 1435, c = 6
 * s = 11  =>  m = 131072, bits = 1387, c = 5
 * s = 12  =>  m = 131072, bits = 1329, c = 4
 * s = 13  =>  m = 131072, bits = 1255, c = 3
 * s = 14  =>  m = 131072, bits = 1098, c = 2
 * s = 15  =>  m = 262144, bits = 2940, c = 8
 * s = 16  =>  m = 262144, bits = 2870, c = 6
 * s = 17  =>  m = 262144, bits = 2763, c = 5
 * s = 18  =>  m = 262144, bits = 2646, c = 4
 * s = 19  =>  m = 262144, bits = 2511, c = 3
 * s = 20  =>  m = 262144, bits = 2234, c = 2
 */
#define GET_CKKS_CONTEXT(s) \
    {                       \
        switch (s)          \
        {                   \
        case 0:             \
            m = 16384;      \
            bits = 119;     \
            c = 2;          \
            break;          \
        case 1:             \
            m = 32768;      \
            bits = 358;     \
            c = 6;          \
            break;          \
        case 2:             \
            m = 32768;      \
            bits = 299;     \
            c = 3;          \
            break;          \
        case 3:             \
            m = 32768;      \
            bits = 239;     \
            c = 2;          \
            break;          \
        case 4:             \
            m = 65536;      \
            bits = 725;     \
            c = 8;          \
            break;          \
        case 5:             \
            m = 65536;      \
            bits = 717;     \
            c = 6;          \
            break;          \
        case 6:             \
            m = 65536;      \
            bits = 669;     \
            c = 4;          \
            break;          \
        case 7:             \
            m = 65536;      \
            bits = 613;     \
            c = 3;          \
            break;          \
        case 8:             \
            m = 65536;      \
            bits = 558;     \
            c = 2;          \
            break;          \
        case 9:             \
            m = 131072;     \
            bits = 1445;    \
            c = 8;          \
            break;          \
        case 10:            \
            m = 131072;     \
            bits = 1435;    \
            c = 6;          \
            break;          \
        case 11:            \
            m = 131072;     \
            bits = 1387;    \
            c = 5;          \
            break;          \
        case 12:            \
            m = 131072;     \
            bits = 1329;    \
            c = 4;          \
            break;          \
        case 13:            \
            m = 131072;     \
            bits = 1255;    \
            c = 3;          \
            break;          \
        case 14:            \
            m = 131072;     \
            bits = 1098;    \
            c = 2;          \
            break;          \
        case 15:            \
            m = 262144;     \
            bits = 2940;    \
            c = 8;          \
            break;          \
        case 16:            \
            m = 262144;     \
            bits = 2870;    \
            c = 6;          \
            break;          \
        case 17:            \
            m = 262144;     \
            bits = 2763;    \
            c = 5;          \
            break;          \
        case 18:            \
            m = 262144;     \
            bits = 2646;    \
            c = 4;          \
            break;          \
        case 19:            \
            m = 262144;     \
            bits = 2511;    \
            c = 3;          \
            break;          \
        case 20:            \
            m = 262144;     \
            bits = 2234;    \
            c = 2;          \
            break;          \
        case 21:            \
            m = 262144;     \
            bits = 1800;    \
            c = 2;          \
            break;          \
        case 30:            \
            m = 2 * 262144; \
            bits = 2234;    \
            c = 2;          \
            break;          \
        case 31:            \
            m = 2 * 262144; \
            bits = 1800;    \
            c = 2;          \
            break;          \
        case 32:            \
            m = 4 * 262144; \
            bits = 2234;    \
            c = 2;          \
            break;          \
        case 33:            \
            m = 4 * 262144; \
            bits = 1800;    \
            c = 2;          \
            break;          \
        case 34:            \
            m = 8 * 262144; \
            bits = 2234;    \
            c = 2;          \
            break;          \
        case 35:            \
            m = 8 * 262144; \
            bits = 1800;    \
            c = 2;          \
            break;          \
                            \
        case 41:            \
            m = 32768;      \
            bits = 1860;    \
            c = 2;          \
            break;          \
        case 42:            \
            m = 65536;      \
            bits = 1860;    \
            c = 2;          \
            break;          \
        case 43:            \
            m = 131072;     \
            bits = 1860;    \
            c = 2;          \
            break;          \
        case 44:            \
            m = 262144;     \
            bits = 1860;    \
            c = 2;          \
            break;          \
        case 45:            \
            m = 2 * 262144; \
            bits = 1860;    \
            c = 2;          \
            break;          \
        case 46:            \
            m = 4 * 262144; \
            bits = 1860;    \
            c = 2;          \
            break;          \
        case 47:            \
            m = 8 * 262144; \
            bits = 1860;    \
            c = 2;          \
            break;          \
                            \
        default:            \
            m = 16384;      \
            bits = 119;     \
            c = 2;          \
            break;          \
        }                   \
    }

#endif
