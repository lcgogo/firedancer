// QUIC encoders

/* TODO replace FD_QUIC_PARSE_FAIL with FD_QUIC_ENCODE_FAIL */

/* TODO add platform optimized versions of these
   e.g. 32 bit unaligned fetch w/ byte swap on intel */
#define FD_TEMPL_ENCODE_IMPL_uchar(p,val) (                            \
    ( (p)[0] = (uchar)( (val) ) ) )
#define FD_TEMPL_ENCODE_IMPL_ushort(p,val) (                           \
    ( (p)[0] = (uchar)( (ushort)(val) >> (ushort)0x08 ) ),             \
    ( (p)[1] = (uchar)( (ushort)(val) >> (ushort)0x00 ) ) )
#define FD_TEMPL_ENCODE_IMPL_uint(p,val) (                             \
    ( (p)[0] = (uchar)(   (uint)(val) >>   (uint)0x18 ) ),             \
    ( (p)[1] = (uchar)(   (uint)(val) >>   (uint)0x10 ) ),             \
    ( (p)[2] = (uchar)(   (uint)(val) >>   (uint)0x08 ) ),             \
    ( (p)[3] = (uchar)(   (uint)(val) >>   (uint)0x00 ) ) )
#define FD_TEMPL_ENCODE_IMPL_ulong(p,val) (                            \
    ( (p)[0] = (uchar)(  (ulong)(val) >>  (ulong)0x38 ) ),             \
    ( (p)[1] = (uchar)(  (ulong)(val) >>  (ulong)0x30 ) ),             \
    ( (p)[2] = (uchar)(  (ulong)(val) >>  (ulong)0x28 ) ),             \
    ( (p)[3] = (uchar)(  (ulong)(val) >>  (ulong)0x20 ) ),             \
    ( (p)[4] = (uchar)(  (ulong)(val) >>  (ulong)0x18 ) ),             \
    ( (p)[5] = (uchar)(  (ulong)(val) >>  (ulong)0x10 ) ),             \
    ( (p)[6] = (uchar)(  (ulong)(val) >>  (ulong)0x08 ) ),             \
    ( (p)[7] = (uchar)(  (ulong)(val) >>  (ulong)0x00 ) ) )

/* encodes the given type, "returns" the number of bytes encoded */
#define FD_TEMPL_ENCODE(TYPE,VAR,p) ( ( FD_TEMPL_ENCODE_IMPL_##TYPE((p),(VAR)) ), sizeof(fd_quic_##TYPE) )

/* returns bytes encoded

   frame is not const, as it may be mutated, for example to store offsets
   to particular bytes in the encoded data */
#define FD_TEMPL_DEF_STRUCT_BEGIN(NAME)                                \
  ulong fd_quic_encode_##NAME( uchar *              buf,               \
                               ulong                sz,                \
                               fd_quic_##NAME##_t * frame ) {          \
    (void)frame;                                                       \
    uchar *  orig_buf = buf;                                           \
    uchar *  buf_end  = buf + sz;                                      \
    ulong    tmp_len  = 0;          (void)tmp_len;                     \
    uchar *  type_ptr = NULL;       (void)type_ptr;

/* encodes TYPE into output */
#define FD_TEMPL_MBR_FRAME_TYPE(NAME,ID_LO,ID_HI)                      \
    if( buf >= buf_end ) return FD_QUIC_PARSE_FAIL;                    \
    buf[0] = ID_LO;                                                    \
    type_ptr = buf++;

/* encodes frame->NAME into the type field */
#define FD_TEMPL_MBR_FRAME_TYPE_FLAG(NAME,MASK)                        \
    if( type_ptr ) {                                                   \
      type_ptr[0] = (uchar)( ( type_ptr[0] & ~(uint)(MASK) )           \
                           | ( frame->NAME &  (uint)(MASK) ) );        \
    }


/* encodes aligned bytes into output */
#define FD_TEMPL_MBR_ELEM(NAME,TYPE)                                   \
    if( FD_UNLIKELY( buf+sizeof(fd_quic_##TYPE) > buf_end ) )          \
      return FD_QUIC_PARSE_FAIL;                                       \
    buf += FD_TEMPL_ENCODE(TYPE,frame->NAME,buf);


/* encodes a packet number.  Assumes pkt_number_len == 3 (4 bytes)
   keeps the pointer to the start of the packet number field */
#define FD_TEMPL_MBR_ELEM_PKTNUM(NAME,TYPE)                            \
    if( FD_UNLIKELY( buf+4 > buf_end ) ) return FD_QUIC_ENCODE_FAIL;   \
    frame->NAME##_pnoff = (unsigned)( buf - orig_buf );                \
    FD_STORE( uint, buf, (uint)frame->NAME );                          \
    buf += 4;


/* encodes a VARINT
   always aligned
   most significant two bits represent the width of the int
   remaining bits are all data bits
   checks for capacity before writing */
#define FD_TEMPL_MBR_ELEM_VARINT(NAME,TYPE)                            \
    do {                                                               \
      tmp_len = FD_QUIC_ENCODE_VARINT_LEN(frame->NAME);                \
      if( tmp_len > ((ulong)(buf_end - buf) ) ) return FD_QUIC_PARSE_FAIL; \
      ulong tmp_sz = (ulong)( buf_end - buf );                         \
      FD_QUIC_ENCODE_VARINT(buf,tmp_sz,frame->NAME);                   \
    } while(0);


// encodes unaligned bits into buf
#define FD_TEMPL_MBR_BITS_BEGIN()                                      \
  do {                                                                 \
    uint bits = 0U;                                                    \
    if( FD_UNLIKELY( buf >= buf_end ) ) return FD_QUIC_ENCODE_FAIL;    \

#define FD_TEMPL_MBR_ELEM_BITS(NAME,TYPE,BITS)                         \
    bits <<= BITS;                                                     \
    bits |= frame->NAME;

#define FD_TEMPL_MBR_ELEM_BITS_TYPE(NAME,TYPE,BITS,CODE)               \
    frame->NAME = CODE;                                                \
    FD_TEMPL_MBR_ELEM_BITS(NAME,TYPE,BITS)

#define FD_TEMPL_MBR_BITS_END()                                        \
    buf[0] = (uchar)bits;                                              \
    buf++;                                                             \
  } while(0);


// VAR currently assumed to be aligned bytes
#define FD_TEMPL_MBR_ELEM_VAR(NAME,BITS_MIN,BITS_MAX,LEN_NAME)         \
    tmp_len = frame->LEN_NAME;                                         \
    if( FD_UNLIKELY( tmp_len*8 < BITS_MIN || tmp_len*8 > BITS_MAX ) ) {\
      FD_LOG_DEBUG(( "buffer overflow encoding variable length field." \
            "  field: " #NAME                                          \
            "  BITS_MIN: %lu"                                          \
            "  BITS_MAX: %lu"                                          \
            "  " #LEN_NAME ": %lu"                                     \
            "  tmp_len*8: %lu\n",                                      \
            (ulong)BITS_MIN,                                           \
            (ulong)BITS_MAX,                                           \
            (ulong)frame->LEN_NAME,                                    \
            (ulong)( tmp_len * 8 ) ));                                 \
      return FD_QUIC_PARSE_FAIL;                                       \
    }                                                                  \
    if( FD_UNLIKELY( (ulong)buf + tmp_len > (ulong)buf_end ) ) {       \
      return FD_QUIC_PARSE_FAIL;                                       \
    }                                                                  \
    fd_memcpy( buf, frame->NAME, tmp_len );                            \
    buf += tmp_len;


// VAR currently assumed to be aligned bytes
#define FD_TEMPL_MBR_ELEM_VAR_RAW(NAME,BITS_MIN,BITS_MAX,LEN_NAME)     \
    tmp_len = frame->LEN_NAME;                                         \
    if( FD_UNLIKELY( tmp_len*8 < BITS_MIN || tmp_len*8 > BITS_MAX ) ) {\
      FD_LOG_DEBUG(( "buffer overflow encoding variable length field." \
            "  field: " #NAME                                          \
            "  BITS_MIN: %lu"                                          \
            "  BITS_MAX: %lu"                                          \
            "  " #LEN_NAME ": %lu"                                     \
            "  tmp_len*8: %lu\n",                                      \
            (ulong)BITS_MIN,                                           \
            (ulong)BITS_MAX,                                           \
            (ulong)frame->LEN_NAME,                                    \
            (ulong)( tmp_len * 8 ) ));                                 \
      return FD_QUIC_PARSE_FAIL;                                       \
    }                                                                  \
    if( FD_UNLIKELY( (ulong)buf + tmp_len > (ulong)buf_end ) ) {       \
      return FD_QUIC_PARSE_FAIL;                                       \
    }                                                                  \
    fd_memcpy( buf, frame->NAME, tmp_len );                            \
    buf += tmp_len;

/* ARRAY is an array of elements, each of the same size,
   with length implied by the packet size
   caller has responsibility of ensuring the size of the array is not
   too large for the space in a packet */
#define FD_TEMPL_MBR_ELEM_ARRAY(NAME,TYPE,BYTES_MIN,BYTES_MAX)         \
    tmp_len = frame->NAME##_len;                                       \
    if( tmp_len * sizeof( fd_quic_##TYPE ) > BYTES_MAX ) {             \
      return FD_QUIC_ENCODE_FAIL;                                      \
    }                                                                  \
    for( ulong j=0; j<tmp_len; ++j ) {                                 \
      buf += FD_TEMPL_ENCODE(TYPE,frame->NAME[j],buf);                 \
    }

/* FIXED is an array of elements, each of the same size,
   with length constant */
#define FD_TEMPL_MBR_ELEM_FIXED(NAME,TYPE,BYTES)                       \
    if( FD_UNLIKELY( BYTES > buf_end-buf ||                            \
                     BYTES % sizeof(fd_quic_##TYPE) ) )                \
      return FD_QUIC_PARSE_FAIL;                                       \
    tmp_len = BYTES / sizeof(fd_quic_##TYPE);                          \
    for( ulong j=0; j<tmp_len; ++j ) {                                 \
      buf += FD_TEMPL_ENCODE(TYPE,frame->NAME[j],buf);                 \
    }

/* TODO remove abort() once tested */
#define FD_TEMPL_MBR_OPT(TYPE,NAME,MASK,...)                           \
    if( !type_ptr ) {                                                  \
      abort();                                                         \
    }                                                                  \
    if( frame->NAME##_opt ) {                                          \
      type_ptr[0] |= (uchar)(MASK);                                    \
      __VA_ARGS__                                                      \
    }

    /* TODO probably easier to split up relevant frames rather than
       code a generic optional in macros */


/* at end, return the number of bytes consumed */
#define FD_TEMPL_DEF_STRUCT_END(NAME)                                  \
    return (ulong)( buf-orig_buf );                                    \
  }

#include "fd_quic_dft.h"

