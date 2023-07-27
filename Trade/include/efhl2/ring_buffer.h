#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>

//notes: this class can only be used in single proceduer 
//       and single consumer mode with non lock,only support linux.
//       the max buffer size limited 4G bytes.
const static unsigned long long MAX_BUFFER_SIZE = (unsigned long long)1<<32;

static int highestonebit(int a)
{
    int i = a;

    i |= (i>>1);
    i |= (i>>2);
    i |= (i>>4);
    i |= (i>>8);
    i |= (i>>16);

    return i - (i>>1);
}

class ring_buffer
{
public:
    ring_buffer()
    {
        m_size   = 0;
        m_inited = false;
        m_r_pos  = 0;
        m_w_pos  = 0;
        m_vptr   = 0;
        m_errno  = -1;
    }

    ~ring_buffer()
    {
        if( m_inited )
        {
            munmap(m_vptr,m_size<<1);
        }
    }

    //inited the cache,default pages=1,max page is 1048576,and pages must be satisfied with 2^n.
    int init(int pages=1)
    {
        if ( m_inited )
        {
            return 0;
        }

        pages = highestonebit(pages);
        assert( (( pages > 0 && pages <= 1048576 ) && ( 0 == (pages & (pages-1)) ) ) );

        unsigned long long size = pages * 4096;
        m_vptr = _init_(size);

        return m_errno;
    }
    
    //get the first address to write and the bytes can be writed.
    void*  get_write_addr(unsigned long long& bytes)
    {
        bytes =  (m_size - (m_w_pos - m_r_pos));
        return (char*)m_vptr + (m_w_pos & (m_size -1));
    }

    //get the first address to read and the bytes can be readed
    void*  get_read_addr(unsigned long long& bytes)
    {
        bytes = (m_w_pos - m_r_pos);
        return (char*)m_vptr + (m_r_pos & (m_size - 1));
    }
 
    //move bytes number byte to the next address to write
    void   move_to_next_write_addr(const unsigned long long& bytes)
    {
        m_w_pos += bytes;
    }

    //move bytes number byte to the next address to read
    void   move_to_next_read_addr(const unsigned long long& bytes)
    {
        m_r_pos += bytes;
    }

    //realloc the cache,default cache is 2*current size
    bool realloc()
    {
        if( m_size == MAX_BUFFER_SIZE )
        {
            m_errno = -7;
            return false;
        }

        unsigned long long o_size = m_size;
        unsigned long long n_size = m_size<<1;
        void* ptr = _init_(n_size);
        if( NULL == ptr )
        {
            return false;
        }

        memcpy((char*)ptr,(char*)m_vptr + (m_r_pos & (o_size -1)),m_w_pos - m_r_pos);
        m_w_pos -= m_r_pos;
        m_r_pos  = 0;

        void* tmp_ptr = m_vptr;
        m_vptr   = ptr;
        munmap(tmp_ptr,o_size<<1);

        return true;
    }

    //get the operate error no
    const int& get_errno()
    {
        return m_errno;
    }

    //get the cache capacity
    const unsigned long long& get_capacity()
    {
        return m_size;
    }

    //reset the cache
    void   reset()
    {
        m_r_pos = 0;
        m_w_pos = 0;
    }

    bool empty()
    {
        return m_r_pos==m_w_pos;
    }

private:

    void* _init_(unsigned long long& size)
    {
        char tmppath[] = "/dev/shm/ring_buffer_XXXXXX";
        int tmpfd,status;
        void* ptr = NULL;
        //void* tmpptr = NULL;
    
        tmpfd = mkstemp(tmppath);
        if( -1 == tmpfd )
        {
            m_errno = -1;
            return NULL;
        }

        status = unlink(tmppath);
        if( -1 == status )
        {
            m_errno = -2;
            goto end_step;
        }

        status = ftruncate(tmpfd,size);
        if( -1 == status )
        {
            m_errno = -3;
            goto end_step;
        }

        ptr = mmap(NULL,size<<1,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);
        if( MAP_FAILED == ptr )
        {
            m_errno = -4;
            goto end_step;
        }

        void* tmpPtr;
        tmpPtr = mmap(ptr,size,PROT_READ|PROT_WRITE,MAP_FIXED|MAP_SHARED,tmpfd,0);
        if( MAP_FAILED == tmpPtr )
        {
            m_errno = -5;
            goto end_mmap;
        }

        tmpPtr = mmap((void*)((unsigned char*)ptr + size) ,size,PROT_READ|PROT_WRITE,MAP_FIXED|MAP_SHARED,tmpfd,0);
        if ( MAP_FAILED == tmpPtr )
        {
            m_errno = -6;
            goto end_mmap;
        }

        memset(ptr,0,size<<1);

        m_inited = true;
        m_errno  = 0;
        m_size   = size;

        goto end_step;

    end_mmap:
        munmap(ptr,m_size<<1);
        ptr = NULL;

    end_step:
        close(tmpfd);

        return ptr;
    }
    
private:
    //cache first address
    void*                        m_vptr;
    
    //current read pointer
    volatile unsigned long long  m_r_pos;

    //current write pointer
    volatile unsigned long long  m_w_pos;

    //cache size
    unsigned long long           m_size;

    //inited fals
    bool                         m_inited;

    //error no
    int                          m_errno;
};

#endif
