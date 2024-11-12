ifneq (,$(wildcard $(OPT)/lib/libs2nbignum.a))
FD_HAS_S2NBIGNUM:=1
CFLAGS+=-DFD_HAS_S2NBIGNUM=1
S2NBIGNUM_LIBS:=$(OPT)/lib/libs2nbignum.a
else
$(warning "s2n-bignum not installed, skipping")
endif