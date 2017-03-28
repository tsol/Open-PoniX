#
# OpenSSL/ocsp/Makefile
#

DIR=	ocsp
TOP=	../..
CC=	cc
INCLUDES= -I.. -I$(TOP) -I../../include
CFLAG=-g
INSTALL_PREFIX=
OPENSSLDIR=     /usr/local/ssl
INSTALLTOP=/usr/local/ssl
MAKEDEPPROG=	makedepend
MAKEDEPEND=	$(TOP)/util/domd $(TOP) -MD $(MAKEDEPPROG)
MAKEFILE=	Makefile
AR=		ar r

CFLAGS= $(INCLUDES) $(CFLAG)

GENERAL=Makefile README
TEST=
APPS=

LIB=$(TOP)/libcrypto.a
LIBSRC= ocsp_asn.c ocsp_ext.c ocsp_ht.c ocsp_lib.c ocsp_cl.c \
	ocsp_srv.c ocsp_prn.c ocsp_vfy.c ocsp_err.c

LIBOBJ= ocsp_asn.o ocsp_ext.o ocsp_ht.o ocsp_lib.o ocsp_cl.o \
	ocsp_srv.o ocsp_prn.o ocsp_vfy.o ocsp_err.o

SRC= $(LIBSRC)

EXHEADER= ocsp.h
HEADER=	$(EXHEADER)

ALL=    $(GENERAL) $(SRC) $(HEADER)

top:
	(cd ../..; $(MAKE) DIRS=crypto SDIRS=$(DIR) sub_all)

all:	lib

lib:	$(LIBOBJ)
	$(AR) $(LIB) $(LIBOBJ)
	$(RANLIB) $(LIB) || echo Never mind.
	@touch lib

files:
	$(PERL) $(TOP)/util/files.pl Makefile >> $(TOP)/MINFO

links:
	@$(PERL) $(TOP)/util/mklink.pl ../../include/openssl $(EXHEADER)
	@$(PERL) $(TOP)/util/mklink.pl ../../test $(TEST)
	@$(PERL) $(TOP)/util/mklink.pl ../../apps $(APPS)

install:
	@headerlist="$(EXHEADER)"; for i in $$headerlist ; \
	do  \
	(cp $$i $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i; \
	chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i ); \
	done;

tags:
	ctags $(SRC)

tests:

lint:
	lint -DLINT $(INCLUDES) $(SRC)>fluff

depend:
	$(MAKEDEPEND) -- $(CFLAG) $(INCLUDES) $(DEPFLAG) -- $(LIBSRC)

dclean:
	$(PERL) -pe 'if (/^# DO NOT DELETE THIS LINE/) {print; exit(0);}' $(MAKEFILE) >Makefile.new
	mv -f Makefile.new $(MAKEFILE)

clean:
	rm -f *.o *.obj lib tags core .pure .nfs* *.old *.bak fluff

# DO NOT DELETE THIS LINE -- make depend depends on it.

ocsp_asn.o: ../../include/openssl/aes.h ../../include/openssl/asn1.h
ocsp_asn.o: ../../include/openssl/asn1t.h ../../include/openssl/bio.h
ocsp_asn.o: ../../include/openssl/bn.h ../../include/openssl/buffer.h
ocsp_asn.o: ../../include/openssl/conf.h ../../include/openssl/crypto.h
ocsp_asn.o: ../../include/openssl/des.h ../../include/openssl/des_old.h
ocsp_asn.o: ../../include/openssl/dsa.h ../../include/openssl/e_os2.h
ocsp_asn.o: ../../include/openssl/evp.h ../../include/openssl/lhash.h
ocsp_asn.o: ../../include/openssl/md5.h ../../include/openssl/obj_mac.h
ocsp_asn.o: ../../include/openssl/objects.h ../../include/openssl/ocsp.h
ocsp_asn.o: ../../include/openssl/opensslconf.h
ocsp_asn.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
ocsp_asn.o: ../../include/openssl/pkcs7.h ../../include/openssl/rc5.h
ocsp_asn.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
ocsp_asn.o: ../../include/openssl/sha.h ../../include/openssl/stack.h
ocsp_asn.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
ocsp_asn.o: ../../include/openssl/ui_compat.h ../../include/openssl/x509.h
ocsp_asn.o: ../../include/openssl/x509_vfy.h ../../include/openssl/x509v3.h
ocsp_asn.o: ocsp_asn.c
ocsp_cl.o: ../../e_os.h ../../include/openssl/aes.h
ocsp_cl.o: ../../include/openssl/asn1.h ../../include/openssl/bio.h
ocsp_cl.o: ../../include/openssl/bn.h ../../include/openssl/buffer.h
ocsp_cl.o: ../../include/openssl/conf.h ../../include/openssl/crypto.h
ocsp_cl.o: ../../include/openssl/des.h ../../include/openssl/des_old.h
ocsp_cl.o: ../../include/openssl/dsa.h ../../include/openssl/e_os2.h
ocsp_cl.o: ../../include/openssl/err.h ../../include/openssl/evp.h
ocsp_cl.o: ../../include/openssl/lhash.h ../../include/openssl/md5.h
ocsp_cl.o: ../../include/openssl/obj_mac.h ../../include/openssl/objects.h
ocsp_cl.o: ../../include/openssl/ocsp.h ../../include/openssl/opensslconf.h
ocsp_cl.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
ocsp_cl.o: ../../include/openssl/pem.h ../../include/openssl/pem2.h
ocsp_cl.o: ../../include/openssl/pkcs7.h ../../include/openssl/rand.h
ocsp_cl.o: ../../include/openssl/rc5.h ../../include/openssl/rsa.h
ocsp_cl.o: ../../include/openssl/safestack.h ../../include/openssl/sha.h
ocsp_cl.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
ocsp_cl.o: ../../include/openssl/ui.h ../../include/openssl/ui_compat.h
ocsp_cl.o: ../../include/openssl/x509.h ../../include/openssl/x509_vfy.h
ocsp_cl.o: ../../include/openssl/x509v3.h ../cryptlib.h ocsp_cl.c
ocsp_err.o: ../../include/openssl/aes.h ../../include/openssl/asn1.h
ocsp_err.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
ocsp_err.o: ../../include/openssl/buffer.h ../../include/openssl/conf.h
ocsp_err.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
ocsp_err.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
ocsp_err.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
ocsp_err.o: ../../include/openssl/evp.h ../../include/openssl/lhash.h
ocsp_err.o: ../../include/openssl/md5.h ../../include/openssl/obj_mac.h
ocsp_err.o: ../../include/openssl/objects.h ../../include/openssl/ocsp.h
ocsp_err.o: ../../include/openssl/opensslconf.h
ocsp_err.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
ocsp_err.o: ../../include/openssl/pkcs7.h ../../include/openssl/rc5.h
ocsp_err.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
ocsp_err.o: ../../include/openssl/sha.h ../../include/openssl/stack.h
ocsp_err.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
ocsp_err.o: ../../include/openssl/ui_compat.h ../../include/openssl/x509.h
ocsp_err.o: ../../include/openssl/x509_vfy.h ../../include/openssl/x509v3.h
ocsp_err.o: ocsp_err.c
ocsp_ext.o: ../../e_os.h ../../include/openssl/aes.h
ocsp_ext.o: ../../include/openssl/asn1.h ../../include/openssl/bio.h
ocsp_ext.o: ../../include/openssl/bn.h ../../include/openssl/buffer.h
ocsp_ext.o: ../../include/openssl/conf.h ../../include/openssl/crypto.h
ocsp_ext.o: ../../include/openssl/des.h ../../include/openssl/des_old.h
ocsp_ext.o: ../../include/openssl/dsa.h ../../include/openssl/e_os2.h
ocsp_ext.o: ../../include/openssl/err.h ../../include/openssl/evp.h
ocsp_ext.o: ../../include/openssl/lhash.h ../../include/openssl/md5.h
ocsp_ext.o: ../../include/openssl/obj_mac.h ../../include/openssl/objects.h
ocsp_ext.o: ../../include/openssl/ocsp.h ../../include/openssl/opensslconf.h
ocsp_ext.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
ocsp_ext.o: ../../include/openssl/pkcs7.h ../../include/openssl/rand.h
ocsp_ext.o: ../../include/openssl/rc5.h ../../include/openssl/rsa.h
ocsp_ext.o: ../../include/openssl/safestack.h ../../include/openssl/sha.h
ocsp_ext.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
ocsp_ext.o: ../../include/openssl/ui.h ../../include/openssl/ui_compat.h
ocsp_ext.o: ../../include/openssl/x509.h ../../include/openssl/x509_vfy.h
ocsp_ext.o: ../../include/openssl/x509v3.h ../cryptlib.h ocsp_ext.c
ocsp_ht.o: ../../include/openssl/aes.h ../../include/openssl/asn1.h
ocsp_ht.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
ocsp_ht.o: ../../include/openssl/buffer.h ../../include/openssl/conf.h
ocsp_ht.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
ocsp_ht.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
ocsp_ht.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
ocsp_ht.o: ../../include/openssl/evp.h ../../include/openssl/lhash.h
ocsp_ht.o: ../../include/openssl/md5.h ../../include/openssl/obj_mac.h
ocsp_ht.o: ../../include/openssl/objects.h ../../include/openssl/ocsp.h
ocsp_ht.o: ../../include/openssl/opensslconf.h ../../include/openssl/opensslv.h
ocsp_ht.o: ../../include/openssl/ossl_typ.h ../../include/openssl/pkcs7.h
ocsp_ht.o: ../../include/openssl/rc5.h ../../include/openssl/rsa.h
ocsp_ht.o: ../../include/openssl/safestack.h ../../include/openssl/sha.h
ocsp_ht.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
ocsp_ht.o: ../../include/openssl/ui.h ../../include/openssl/ui_compat.h
ocsp_ht.o: ../../include/openssl/x509.h ../../include/openssl/x509_vfy.h
ocsp_ht.o: ../../include/openssl/x509v3.h ocsp_ht.c
ocsp_lib.o: ../../e_os.h ../../include/openssl/aes.h
ocsp_lib.o: ../../include/openssl/asn1.h ../../include/openssl/bio.h
ocsp_lib.o: ../../include/openssl/bn.h ../../include/openssl/buffer.h
ocsp_lib.o: ../../include/openssl/conf.h ../../include/openssl/crypto.h
ocsp_lib.o: ../../include/openssl/des.h ../../include/openssl/des_old.h
ocsp_lib.o: ../../include/openssl/dsa.h ../../include/openssl/e_os2.h
ocsp_lib.o: ../../include/openssl/err.h ../../include/openssl/evp.h
ocsp_lib.o: ../../include/openssl/lhash.h ../../include/openssl/md5.h
ocsp_lib.o: ../../include/openssl/obj_mac.h ../../include/openssl/objects.h
ocsp_lib.o: ../../include/openssl/ocsp.h ../../include/openssl/opensslconf.h
ocsp_lib.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
ocsp_lib.o: ../../include/openssl/pem.h ../../include/openssl/pem2.h
ocsp_lib.o: ../../include/openssl/pkcs7.h ../../include/openssl/rand.h
ocsp_lib.o: ../../include/openssl/rc5.h ../../include/openssl/rsa.h
ocsp_lib.o: ../../include/openssl/safestack.h ../../include/openssl/sha.h
ocsp_lib.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
ocsp_lib.o: ../../include/openssl/ui.h ../../include/openssl/ui_compat.h
ocsp_lib.o: ../../include/openssl/x509.h ../../include/openssl/x509_vfy.h
ocsp_lib.o: ../../include/openssl/x509v3.h ../cryptlib.h ocsp_lib.c
ocsp_prn.o: ../../include/openssl/aes.h ../../include/openssl/asn1.h
ocsp_prn.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
ocsp_prn.o: ../../include/openssl/buffer.h ../../include/openssl/conf.h
ocsp_prn.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
ocsp_prn.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
ocsp_prn.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
ocsp_prn.o: ../../include/openssl/evp.h ../../include/openssl/lhash.h
ocsp_prn.o: ../../include/openssl/md5.h ../../include/openssl/obj_mac.h
ocsp_prn.o: ../../include/openssl/objects.h ../../include/openssl/ocsp.h
ocsp_prn.o: ../../include/openssl/opensslconf.h
ocsp_prn.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
ocsp_prn.o: ../../include/openssl/pem.h ../../include/openssl/pem2.h
ocsp_prn.o: ../../include/openssl/pkcs7.h ../../include/openssl/rc5.h
ocsp_prn.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
ocsp_prn.o: ../../include/openssl/sha.h ../../include/openssl/stack.h
ocsp_prn.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
ocsp_prn.o: ../../include/openssl/ui_compat.h ../../include/openssl/x509.h
ocsp_prn.o: ../../include/openssl/x509_vfy.h ../../include/openssl/x509v3.h
ocsp_prn.o: ocsp_prn.c
ocsp_srv.o: ../../e_os.h ../../include/openssl/aes.h
ocsp_srv.o: ../../include/openssl/asn1.h ../../include/openssl/bio.h
ocsp_srv.o: ../../include/openssl/bn.h ../../include/openssl/buffer.h
ocsp_srv.o: ../../include/openssl/conf.h ../../include/openssl/crypto.h
ocsp_srv.o: ../../include/openssl/des.h ../../include/openssl/des_old.h
ocsp_srv.o: ../../include/openssl/dsa.h ../../include/openssl/e_os2.h
ocsp_srv.o: ../../include/openssl/err.h ../../include/openssl/evp.h
ocsp_srv.o: ../../include/openssl/lhash.h ../../include/openssl/md5.h
ocsp_srv.o: ../../include/openssl/obj_mac.h ../../include/openssl/objects.h
ocsp_srv.o: ../../include/openssl/ocsp.h ../../include/openssl/opensslconf.h
ocsp_srv.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
ocsp_srv.o: ../../include/openssl/pem.h ../../include/openssl/pem2.h
ocsp_srv.o: ../../include/openssl/pkcs7.h ../../include/openssl/rand.h
ocsp_srv.o: ../../include/openssl/rc5.h ../../include/openssl/rsa.h
ocsp_srv.o: ../../include/openssl/safestack.h ../../include/openssl/sha.h
ocsp_srv.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
ocsp_srv.o: ../../include/openssl/ui.h ../../include/openssl/ui_compat.h
ocsp_srv.o: ../../include/openssl/x509.h ../../include/openssl/x509_vfy.h
ocsp_srv.o: ../../include/openssl/x509v3.h ../cryptlib.h ocsp_srv.c
ocsp_vfy.o: ../../include/openssl/aes.h ../../include/openssl/asn1.h
ocsp_vfy.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
ocsp_vfy.o: ../../include/openssl/buffer.h ../../include/openssl/conf.h
ocsp_vfy.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
ocsp_vfy.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
ocsp_vfy.o: ../../include/openssl/e_os2.h ../../include/openssl/err.h
ocsp_vfy.o: ../../include/openssl/evp.h ../../include/openssl/lhash.h
ocsp_vfy.o: ../../include/openssl/md5.h ../../include/openssl/obj_mac.h
ocsp_vfy.o: ../../include/openssl/objects.h ../../include/openssl/ocsp.h
ocsp_vfy.o: ../../include/openssl/opensslconf.h
ocsp_vfy.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
ocsp_vfy.o: ../../include/openssl/pkcs7.h ../../include/openssl/rc5.h
ocsp_vfy.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
ocsp_vfy.o: ../../include/openssl/sha.h ../../include/openssl/stack.h
ocsp_vfy.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
ocsp_vfy.o: ../../include/openssl/ui_compat.h ../../include/openssl/x509.h
ocsp_vfy.o: ../../include/openssl/x509_vfy.h ../../include/openssl/x509v3.h
ocsp_vfy.o: ocsp_vfy.c
