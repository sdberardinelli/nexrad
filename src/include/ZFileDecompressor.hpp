/******************************************************************************
* Filename      : ZFileDecompressor.hpp
* Source File(s): ZFileDecompressor.cpp
* Description   :
* Authors(s)    :
* Date Created  :
* Date Modified :
* Modifier(s)   :
*******************************************************************************/
#ifndef ZFILEDECOMPRESSOR_H
#define ZFILEDECOMPRESSOR_H

/*******************************************************************************
*  INCLUDES
********************************************************************************/
#include <string>
#include <vector>

/*******************************************************************************
*  DEFINES
********************************************************************************/

/*******************************************************************************
*  MACROS
********************************************************************************/

/*******************************************************************************
*  DATA TYPES
********************************************************************************/

/*******************************************************************************
*  EXTERNALS
********************************************************************************/

/*******************************************************************************
*  CLASS DEFINITIONS
********************************************************************************/
class ZFileDecompressor
{
    public: 
        /* constructors */
        ZFileDecompressor ( std::string ); /* default */
        ZFileDecompressor ( ZFileDecompressor& ); /* copy */
        ZFileDecompressor& operator= ( const ZFileDecompressor& ); /* assign */
        ~ZFileDecompressor ( void );
        /* functions */
        void log ( void );
        void resetbuf ( void );
        void fill ( void );
        void buffer_data ( void );
        std::vector<unsigned char> read ( int );
        void parse_header ( void );
        int file_read ( void );

    private:
        const unsigned int LZW_MAGIC = 0x1f9d;
        const unsigned int MAX_BITS = 16;
        const unsigned int INIT_BITS = 9;
        const unsigned int HDR_MAXBITS = 0x1f;
        const unsigned int HDR_EXTENDED = 0x20;
        const unsigned int HDR_FREE = 0x40;
        const unsigned int HDR_BLOCK_MODE = 0x80;

        const unsigned int TBL_CLEAR = 0x100;
        const unsigned int TBL_FIRST = TBL_CLEAR + 1;

        const unsigned int EXTRA = 64;

        std::vector<unsigned char> f;
        std::vector<unsigned char> data;
        std::vector<unsigned char> buf;
        std::vector<unsigned int> tab_prefix;
        std::vector<unsigned char> tab_suffix;
        std::vector<unsigned char> stack;        

        /* SELF */
        unsigned int bit_pos = 0;
        unsigned int end = 0;
        unsigned int got = 0;
                bool eof = false;
                bool debug = false;
                bool buffer_full = false;

       /* PARSE_HEADER */
        unsigned int stackp;
                bool block_mode;     
        unsigned int maxbits;    
        unsigned int maxmaxcode;
        unsigned int n_bits;
        unsigned int maxcode;
        unsigned int bitmask;
                 int oldcode;
        unsigned int finchar;
        unsigned int free_ent;

        /* READ */
        unsigned int off;    
        unsigned int num;
        unsigned int len_stack;

        /* RESETBUF */
};
#endif
