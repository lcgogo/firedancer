#define _GNU_SOURCE  /* memmem */
#include "fd_x509_mock.h"
#include <string.h>  /* memmem */

static uchar const
fd_x509_mock_tpl[ FD_X509_MOCK_CERT_SZ ] = {
  /* Certificate SEQUENCE (3 elem) */
  0x30, 0x81, 0xf6,

    /* tbsCertificate TBSCertificate SEQUENCE (8 elem) */
    0x30, 0x81, 0xa9,

      /* version [0] (1 elem)  */
      0xa0, 0x03,
        /* INTEGER  2 */
        0x02, 0x01, 0x02,

      /* serialNumber CertificateSerialNumber INTEGER (62 bit) */
      0x02, 0x08,
      0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,

      /* signature AlgorithmIdentifier SEQUENCE (1 elem) */
      0x30, 0x05,
        /* algorithm OBJECT IDENTIFIER 1.3.101.112 curveEd25519 (EdDSA 25519 signature algorithm) */
        0x06, 0x03, 0x2b, 0x65, 0x70,

      /* issuer Name SEQUENCE (1 elem) */
      0x30, 0x16,
        /* RelativeDistinguishedName SET (1 elem) */
        0x31, 0x14,
          /* AttributeTypeAndValue SEQUENCE (2 elem) */
          0x30, 0x12,
            /* type AttributeType OBJECT IDENTIFIER 2.5.4.3 commonName (X.520 DN component) */
            0x06, 0x03, 0x55, 0x04, 0x03,
            /* value AttributeValue [?] UTF8String Solana node */
            0x0c, 0x0b, 0x53, 0x6f, 0x6c, 0x61, 0x6e, 0x61, 0x20, 0x6e, 0x6f, 0x64, 0x65,

      /* validity Validity SEQUENCE (2 elem) */
      0x30, 0x20,
        /* notBefore Time UTCTime 1970-01-01 00:00:00 UTC */
        0x17, 0x0d, 0x37, 0x30, 0x30, 0x31, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x5a,
        /* notAfter Time GeneralizedTime 4096-01-01 00:00:00 UTC */
        0x18, 0x0f, 0x34, 0x30, 0x39, 0x36, 0x30, 0x31, 0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x5a,

      /* subject Name SEQUENCE (0 elem) */
      0x30, 0x00,

      /* subjectPublicKeyInfo SubjectPublicKeyInfo SEQUENCE (2 elem) */
      0x30, 0x2a,
        /* algorithm AlgorithmIdentifier SEQUENCE (1 elem) */
        0x30, 0x05,
          /* algorithm OBJECT IDENTIFIER 1.3.101.112 curveEd25519 (EdDSA 25519 signature algorithm) */
          0x06, 0x03, 0x2b, 0x65, 0x70,
        /* subjectPublicKey BIT STRING (256 bit) */
        0x03, 0x21, 0x00,
        #define FD_X509_MOCK_PUBKEY_OFF (0x64)
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,

      /* extensions [3] (1 elem) */
      0xa3, 0x29,
        /* Extensions SEQUENCE (2 elem) */
        0x30, 0x27,
          /* Extension SEQUENCE (3 elem) */
          0x30, 0x17,
            /* extnID OBJECT IDENTIFIER 2.5.29.17 subjectAltName (X.509 extension) */
            0x06, 0x03, 0x55, 0x1d, 0x11,
            /* critical BOOLEAN true */
            0x01, 0x01, 0xff,
            /* extnValue OCTET STRING (13 byte) encapsulating */
            0x04, 0x0d,
              /* SEQUENCE (1 elem) */
              0x30, 0x0b,
              /* [2] (9 byte) localhost */
              0x82, 0x09,
              0x6c, 0x6f, 0x63, 0x61, 0x6c, 0x68, 0x6f, 0x73, 0x74,

          /* Extension SEQUENCE (3 elem) */
          0x30, 0x0c,
            /* extnID OBJECT IDENTIFIER 2.5.29.19 basicConstraints (X.509 extension) */
            0x06, 0x03, 0x55, 0x1d, 0x13,
            /* critical BOOLEAN true */
            0x01, 0x01, 0xff,
            /* extnValue OCTET STRING (2 byte) encapsulating */
            0x04, 0x02,
              /* SEQUENCE (0 elem) */
              0x30, 0x00,

    /* signatureAlgorithm AlgorithmIdentifier SEQUENCE (1 elem) */
    0x30, 0x05,
      /* algorithm OBJECT IDENTIFIER 1.3.101.112 curveEd25519 (EdDSA 25519 signature algorithm) */
      0x06, 0x03, 0x2b, 0x65, 0x70,
      /* signature BIT STRING (512 bit) */
      0x03, 0x41, 0x00,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

/* Agave v1.18 uses a different template.  It is less
   determistic due to a variable length serial number, so we match the
   prefix before the public key. */

static uchar const
fd_x509_mock_v1_prefix[] = {
  0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70, 0x30,
  0x16, 0x31, 0x14, 0x30, 0x12, 0x06, 0x03, 0x55,
  0x04, 0x03, 0x0c, 0x0b, 0x53, 0x6f, 0x6c, 0x61,
  0x6e, 0x61, 0x20, 0x6e, 0x6f, 0x64, 0x65, 0x30,
  0x20, 0x17, 0x0d, 0x37, 0x35, 0x30, 0x31, 0x30,
  0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x5a,
  0x18, 0x0f, 0x34, 0x30, 0x39, 0x36, 0x30, 0x31,
  0x30, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
  0x5a, 0x30, 0x16, 0x31, 0x14, 0x30, 0x12, 0x06,
  0x03, 0x55, 0x04, 0x03, 0x0c, 0x0b, 0x53, 0x6f,
  0x6c, 0x61, 0x6e, 0x61, 0x20, 0x6e, 0x6f, 0x64,
  0x65, 0x30, 0x2a, 0x30, 0x05, 0x06, 0x03, 0x2b,
  0x65, 0x70, 0x03, 0x21, 0x00
};

void
fd_x509_mock_cert( uchar buf[ static FD_X509_MOCK_CERT_SZ ],
                   uchar public_key[ static 32 ] ) {
  fd_memcpy( buf, fd_x509_mock_tpl, FD_X509_MOCK_CERT_SZ );
  fd_memcpy( buf+FD_X509_MOCK_PUBKEY_OFF, public_key, 32UL );
}

static uchar const *
fd_x509_mock_pubkey_v1( uchar const * cert,
                        ulong         cert_sz ) {
  uchar const * end   = cert + cert_sz;
  uchar const * match = memmem( cert, cert_sz, fd_x509_mock_v1_prefix, sizeof(fd_x509_mock_v1_prefix) );
  if( !match ) return NULL;
  uchar const * pubkey = match + sizeof(fd_x509_mock_v1_prefix);
  if( FD_UNLIKELY( pubkey + 32 > end ) ) return NULL;
  return pubkey;
}

static uchar const *
fd_x509_mock_pubkey_v2( uchar const * cert,
                        ulong         cert_sz ) {
  if( cert_sz != FD_X509_MOCK_CERT_SZ ) return NULL;

  ulong off = 0UL;
  int match0 = (0==memcmp( cert+off, fd_x509_mock_tpl+off, FD_X509_MOCK_PUBKEY_OFF ) );
  off += FD_X509_MOCK_PUBKEY_OFF;
  off += 32UL;
  int match1 = (0==memcmp( cert+off, fd_x509_mock_tpl+off, FD_X509_MOCK_CERT_SZ-off ) );

  if( (!match0) | (!match1) ) return NULL;

  return cert + FD_X509_MOCK_PUBKEY_OFF;
}

uchar const *
fd_x509_mock_pubkey( uchar const * cert,
                     ulong         cert_sz ) {
  uchar const * match;
  match = fd_x509_mock_pubkey_v1( cert, cert_sz );
  if( match ) return match;
  match = fd_x509_mock_pubkey_v2( cert, cert_sz );
  if( match ) return match;
  return NULL;
}
