/******************************************************************************* 
 * Filename      : ZFileDecompressor.cpp
 * Header File(s):
 * Description   :
 * Authors(s)    :
 * Date Created  :
 * Date Modified :
 * Modifier(s)   :
 *******************************************************************************/
/************************************
* Included Headers 
************************************/
#include "ZFileDecompressor.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdio>
#include <cstdlib>

/************************************
* Namespaces
************************************/
using namespace std;

/************************************
* Local Variables
************************************/

/*******************************************************************************
* Constructor  : (Default)
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
ZFileDecompressor::ZFileDecompressor ( string path )
{ 
    ifstream file; 
    file.open(path.c_str(), ios::in | ios::binary | ios::ate);
    streampos size;
    unsigned char * _f;
    if (file.is_open())
    {
        size = file.tellg();
        _f = new unsigned char[size];
        file.seekg (0, ios::beg);
        file.read ((char*)_f, size);
        file.close();

        for ( unsigned int i = 0; i < size; i++ )
        {
            f.push_back(_f[i]);
        }

        delete[] _f;

        data.resize(10000,0);

        parse_header();
    }
    else 
    {
        cout << "Unable to open file";  
    } 
}
/*******************************************************************************
* Constructor  : (Copy)
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
ZFileDecompressor::ZFileDecompressor ( ZFileDecompressor& obj ) { ; }
/*******************************************************************************
* Deconstructor: 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
ZFileDecompressor::~ZFileDecompressor ( void )
{ 
    f.clear();
    data.clear();
    tab_prefix.clear();
    tab_suffix.clear();
    stack.clear();
}
/*******************************************************************************
* Constructor  : (Assignment)
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
ZFileDecompressor& ZFileDecompressor::operator=( const ZFileDecompressor& obj ) 
{
    if (this != &obj) // prevent self-assignment
    {
        ;
    }
    return *this;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void ZFileDecompressor::log ( void )
{
    ;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/        
void ZFileDecompressor::resetbuf ( void )
{
    unsigned int pos = bit_pos >> 3;
    for ( unsigned int i = pos; i < end; i++ )
    {
        data[i-pos] = data[i];
    }
    //data.resize(end-pos);
    end -= pos;
    bit_pos = 0;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/        
void ZFileDecompressor::fill ( void )
{
    unsigned int amt = data.size()-1-end;
    vector<unsigned char> binstring;

    for ( unsigned int i = 0; i < amt; i++ )
    {
        int tmp = file_read();

        if ( tmp < 0 )
        {
            binstring.resize(0);
            break;
        }
        binstring.push_back((unsigned char)tmp);
    }
    got = binstring.size();

    if ( got > 0 )
    {
        for ( unsigned int i = end; i < got; i++ )
        {
            data[i] = binstring[i-end];
        }
        end += got;
    }
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/        
void ZFileDecompressor::buffer_data ( void )
{
    if ( end < EXTRA )
    {
        fill();
    }

    unsigned int bit_in;
    if ( got > 0 )
    {
        bit_in =  ((end - end % n_bits) << 3);
    }
    else
    {
        bit_in = (end << 3) - (n_bits - 1);
    }

    while ( bit_pos < bit_in )
    {
        if ( free_ent > maxcode )
        {
            unsigned int n_bytes = n_bits << 3;
            unsigned int byte_pos = (bit_pos - 1) + n_bytes;
            bit_pos = byte_pos - (byte_pos % n_bytes);
            n_bits += 1;
            if ( n_bits == maxbits )
            {
                maxcode = maxmaxcode;
            }
            else
            {
                maxcode = (1 << n_bits) - 1;
            }
            bitmask = (1 << n_bits) - 1;
            resetbuf();  
            return;
        }

        //read next code
        unsigned int pos = bit_pos >> 3;
        unsigned int code = ((data[pos] | data[pos+1] << 8 | data[pos+2] << 16) >> (bit_pos & 0x7)) & bitmask;
        bit_pos += n_bits;

        //check for first iteration
        if (oldcode == -1)
        {
            if (code > 255)
            {
                cout << "Corrupted Input: iteration" << endl;
                exit(-1);
            }
            oldcode = code;
            finchar = (unsigned char)code;
            buf[off] = finchar;
            off += 1;
            num -= 1;
            continue;
        }

        //check for CLEAR code
        if ((code == TBL_CLEAR) && (block_mode))
        {
            for ( unsigned int i = 0; i < 256; i++ )
            {
                tab_prefix[i] = 0;
            }
            free_ent = TBL_FIRST - 1;
            unsigned int n_bytes = n_bits << 3;
            bit_pos = (bit_pos - 1) + n_bytes - ((bit_pos - 1 + n_bytes) % n_bytes);
            n_bits = INIT_BITS;
            maxcode = (1 << n_bits) - 1;
            bitmask = maxcode;
            resetbuf();
            return;
        }

        unsigned char incode = code;
        stackp = stack.size();

        //handle KwK case
        if (code >= free_ent)
        {
            if (code > free_ent)
            {
                 cout << "Corrupted Input (kwk): code=" << code << endl;
                exit(-1);
            }
            stackp -= 1;
            stack[stackp] = finchar;
            code = oldcode;
        }

        //generate output in reverse order
        while (code >= 256)
        {
            stackp -= 1;
            stack[stackp] = tab_suffix[code];
            code = tab_prefix[code];
        }
        finchar = tab_suffix[code];
        buf[off] = finchar;
        off += 1;
        num -= 1;

        //put them out in forward order
        unsigned int s_size = stack.size() - stackp;
        unsigned int amt = min(num, s_size);
        if (amt > 0)
        {
            for ( unsigned int i = off; i< off+amt; i++ )
            {
                buf[i] = stack[i+stackp-off];
            }
            off += amt;
            num -= amt;
            stackp += amt;
        }

        //generate new entry in table
        if (free_ent < maxmaxcode)
        {
            tab_prefix[free_ent] = oldcode;
            tab_suffix[free_ent] = finchar;
            free_ent += 1;
        }

        oldcode = incode;

        if (num == 0)
        {
            buffer_full = true;
            return;
        }
    }
    resetbuf();
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/        
vector<unsigned char> ZFileDecompressor::read ( int _num )
{
    buf.clear();
    buf.resize(_num);
    off = 0;
    num = _num;

    if ( eof )
        return buf;

    unsigned int s_size = stack.size() - stackp;

    if ( s_size > 0 )
    {
        unsigned int amt = min(num,s_size);

        for ( unsigned int i = 0; i < amt; i++ )
        {
            buf[i] = stack[i+stackp];
        }
        off += amt;
        num -= amt;
        stackp += amt;
    }

    if ( num == 0 )
        return buf;

    buffer_data();

    if ( buffer_full != true )
    {
        while ( got > 0 and buffer_full != false )
        {
            buffer_data();
        }
    }

    if ( got == 0 )
    {
        eof = true;
    }

    return buf;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/        
void ZFileDecompressor::parse_header ( void )
{
    unsigned short magic = ((unsigned short)file_read() << 8) | (unsigned char)file_read();
    if ( magic != LZW_MAGIC )
    {
        cout << "ERROR1" << endl;
        return;
    }
    unsigned char header = (unsigned char)file_read();
    block_mode = (header & HDR_BLOCK_MODE) > 0;
    maxbits = header & HDR_MAXBITS;
    if ( maxbits > MAX_BITS )
    {
        cout << "ERROR2" << endl;
        return;        
    }
    if ( (header & HDR_EXTENDED) > 0 )
    {
        cout << "ERROR3" << endl;
        return;        
    }    
    if ( (header & HDR_FREE) > 0 )
    {
        cout << "ERROR3" << endl;
        return;        
    }     

    maxmaxcode = 1 << maxbits;
    n_bits = INIT_BITS;
    maxcode = (1 << n_bits) - 1;
    bitmask = maxcode;
    oldcode = -1;
    finchar = 0;
    if (block_mode)
        free_ent = TBL_FIRST;
    else
        free_ent = 256;

    tab_prefix.resize(1 << maxbits);
    tab_suffix.resize(1 << maxbits);
    stack.resize(1 << maxbits);
    stackp = (1 << maxbits);

    for ( unsigned int i = 0; i < 256; i++ )
    {
        tab_suffix[i] = (unsigned char)i;
    }
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/        
int ZFileDecompressor::file_read ( void )
{
    static unsigned int index = 0;

    if ( index < f.size() )
        return f[index++];
    return -1;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
