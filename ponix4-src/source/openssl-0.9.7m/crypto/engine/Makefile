#
# OpenSSL/crypto/engine/Makefile
#

DIR=	engine
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

GENERAL=Makefile
TEST= enginetest.c
APPS=

LIB=$(TOP)/libcrypto.a
LIBSRC= eng_err.c eng_lib.c eng_list.c eng_init.c eng_ctrl.c \
	eng_table.c eng_pkey.c eng_fat.c eng_all.c \
	tb_rsa.c tb_dsa.c tb_dh.c tb_rand.c tb_cipher.c tb_digest.c \
	eng_openssl.c eng_dyn.c eng_cnf.c \
	hw_atalla.c hw_cswift.c hw_ncipher.c hw_nuron.c hw_ubsec.c \
	hw_cryptodev.c hw_aep.c hw_sureware.c hw_4758_cca.c
LIBOBJ= eng_err.o eng_lib.o eng_list.o eng_init.o eng_ctrl.o \
	eng_table.o eng_pkey.o eng_fat.o eng_all.o \
	tb_rsa.o tb_dsa.o tb_dh.o tb_rand.o tb_cipher.o tb_digest.o \
	eng_openssl.o eng_dyn.o eng_cnf.o \
	hw_atalla.o hw_cswift.o hw_ncipher.o hw_nuron.o hw_ubsec.o \
	hw_cryptodev.o hw_aep.o hw_sureware.o hw_4758_cca.o

SRC= $(LIBSRC)

EXHEADER= engine.h
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

errors:
	$(PERL) $(TOP)/util/mkerr.pl -conf hw.ec \
		-nostatic -staticloader -write hw_*.c

tests:

lint:
	lint -DLINT $(INCLUDES) $(SRC)>fluff

depend:
	$(MAKEDEPEND) -- $(CFLAG) $(INCLUDES) $(DEPFLAG) -- $(PROGS) $(LIBSRC)

dclean:
	$(PERL) -pe 'if (/^# DO NOT DELETE THIS LINE/) {print; exit(0);}' $(MAKEFILE) >Makefile.new
	mv -f Makefile.new $(MAKEFILE)

clean:
	rm -f *.o */*.o *.obj lib tags core .pure .nfs* *.old *.bak fluff

# DO NOT DELETE THIS LINE -- make depend depends on it.

eng_all.o: ../../include/openssl/asn1.h ../../include/openssl/bio.h
eng_all.o: ../../include/openssl/bn.h ../../include/openssl/crypto.h
eng_all.o: ../../include/openssl/dsa.h ../../include/openssl/e_os2.h
eng_all.o: ../../include/openssl/engine.h ../../include/openssl/err.h
eng_all.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
eng_all.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
eng_all.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
eng_all.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
eng_all.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
eng_all.o: eng_all.c eng_int.h
eng_cnf.o: ../../e_os.h ../../include/openssl/asn1.h
eng_cnf.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
eng_cnf.o: ../../include/openssl/buffer.h ../../include/openssl/conf.h
eng_cnf.o: ../../include/openssl/crypto.h ../../include/openssl/dsa.h
eng_cnf.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
eng_cnf.o: ../../include/openssl/err.h ../../include/openssl/lhash.h
eng_cnf.o: ../../include/openssl/opensslconf.h ../../include/openssl/opensslv.h
eng_cnf.o: ../../include/openssl/ossl_typ.h ../../include/openssl/rand.h
eng_cnf.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
eng_cnf.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
eng_cnf.o: ../../include/openssl/ui.h ../cryptlib.h eng_cnf.c
eng_ctrl.o: ../../e_os.h ../../include/openssl/asn1.h
eng_ctrl.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
eng_ctrl.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
eng_ctrl.o: ../../include/openssl/dsa.h ../../include/openssl/e_os2.h
eng_ctrl.o: ../../include/openssl/engine.h ../../include/openssl/err.h
eng_ctrl.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
eng_ctrl.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
eng_ctrl.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
eng_ctrl.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
eng_ctrl.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
eng_ctrl.o: ../cryptlib.h eng_ctrl.c eng_int.h
eng_dyn.o: ../../e_os.h ../../include/openssl/asn1.h
eng_dyn.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
eng_dyn.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
eng_dyn.o: ../../include/openssl/dsa.h ../../include/openssl/dso.h
eng_dyn.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
eng_dyn.o: ../../include/openssl/err.h ../../include/openssl/lhash.h
eng_dyn.o: ../../include/openssl/opensslconf.h ../../include/openssl/opensslv.h
eng_dyn.o: ../../include/openssl/ossl_typ.h ../../include/openssl/rand.h
eng_dyn.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
eng_dyn.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
eng_dyn.o: ../../include/openssl/ui.h ../cryptlib.h eng_dyn.c eng_int.h
eng_err.o: ../../include/openssl/asn1.h ../../include/openssl/bio.h
eng_err.o: ../../include/openssl/bn.h ../../include/openssl/crypto.h
eng_err.o: ../../include/openssl/dsa.h ../../include/openssl/e_os2.h
eng_err.o: ../../include/openssl/engine.h ../../include/openssl/err.h
eng_err.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
eng_err.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
eng_err.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
eng_err.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
eng_err.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
eng_err.o: eng_err.c
eng_fat.o: ../../e_os.h ../../include/openssl/asn1.h
eng_fat.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
eng_fat.o: ../../include/openssl/buffer.h ../../include/openssl/conf.h
eng_fat.o: ../../include/openssl/crypto.h ../../include/openssl/dsa.h
eng_fat.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
eng_fat.o: ../../include/openssl/err.h ../../include/openssl/lhash.h
eng_fat.o: ../../include/openssl/opensslconf.h ../../include/openssl/opensslv.h
eng_fat.o: ../../include/openssl/ossl_typ.h ../../include/openssl/rand.h
eng_fat.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
eng_fat.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
eng_fat.o: ../../include/openssl/ui.h ../cryptlib.h eng_fat.c eng_int.h
eng_init.o: ../../e_os.h ../../include/openssl/asn1.h
eng_init.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
eng_init.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
eng_init.o: ../../include/openssl/dsa.h ../../include/openssl/e_os2.h
eng_init.o: ../../include/openssl/engine.h ../../include/openssl/err.h
eng_init.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
eng_init.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
eng_init.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
eng_init.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
eng_init.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
eng_init.o: ../cryptlib.h eng_init.c eng_int.h
eng_lib.o: ../../e_os.h ../../include/openssl/asn1.h
eng_lib.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
eng_lib.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
eng_lib.o: ../../include/openssl/dsa.h ../../include/openssl/e_os2.h
eng_lib.o: ../../include/openssl/engine.h ../../include/openssl/err.h
eng_lib.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
eng_lib.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
eng_lib.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
eng_lib.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
eng_lib.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
eng_lib.o: ../cryptlib.h eng_int.h eng_lib.c
eng_list.o: ../../e_os.h ../../include/openssl/asn1.h
eng_list.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
eng_list.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
eng_list.o: ../../include/openssl/dsa.h ../../include/openssl/e_os2.h
eng_list.o: ../../include/openssl/engine.h ../../include/openssl/err.h
eng_list.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
eng_list.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
eng_list.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
eng_list.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
eng_list.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
eng_list.o: ../cryptlib.h eng_int.h eng_list.c
eng_openssl.o: ../../e_os.h ../../include/openssl/aes.h
eng_openssl.o: ../../include/openssl/asn1.h ../../include/openssl/bio.h
eng_openssl.o: ../../include/openssl/bn.h ../../include/openssl/buffer.h
eng_openssl.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
eng_openssl.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
eng_openssl.o: ../../include/openssl/dso.h ../../include/openssl/e_os2.h
eng_openssl.o: ../../include/openssl/engine.h ../../include/openssl/err.h
eng_openssl.o: ../../include/openssl/evp.h ../../include/openssl/lhash.h
eng_openssl.o: ../../include/openssl/md5.h ../../include/openssl/obj_mac.h
eng_openssl.o: ../../include/openssl/objects.h
eng_openssl.o: ../../include/openssl/opensslconf.h
eng_openssl.o: ../../include/openssl/opensslv.h
eng_openssl.o: ../../include/openssl/ossl_typ.h ../../include/openssl/pem.h
eng_openssl.o: ../../include/openssl/pem2.h ../../include/openssl/pkcs7.h
eng_openssl.o: ../../include/openssl/rand.h ../../include/openssl/rc5.h
eng_openssl.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
eng_openssl.o: ../../include/openssl/sha.h ../../include/openssl/stack.h
eng_openssl.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
eng_openssl.o: ../../include/openssl/ui_compat.h ../../include/openssl/x509.h
eng_openssl.o: ../../include/openssl/x509_vfy.h ../cryptlib.h eng_openssl.c
eng_pkey.o: ../../e_os.h ../../include/openssl/asn1.h
eng_pkey.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
eng_pkey.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
eng_pkey.o: ../../include/openssl/dsa.h ../../include/openssl/e_os2.h
eng_pkey.o: ../../include/openssl/engine.h ../../include/openssl/err.h
eng_pkey.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
eng_pkey.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
eng_pkey.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
eng_pkey.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
eng_pkey.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
eng_pkey.o: ../cryptlib.h eng_int.h eng_pkey.c
eng_table.o: ../../include/openssl/aes.h ../../include/openssl/asn1.h
eng_table.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
eng_table.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
eng_table.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
eng_table.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
eng_table.o: ../../include/openssl/err.h ../../include/openssl/evp.h
eng_table.o: ../../include/openssl/lhash.h ../../include/openssl/md5.h
eng_table.o: ../../include/openssl/obj_mac.h ../../include/openssl/objects.h
eng_table.o: ../../include/openssl/opensslconf.h
eng_table.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
eng_table.o: ../../include/openssl/rand.h ../../include/openssl/rc5.h
eng_table.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
eng_table.o: ../../include/openssl/sha.h ../../include/openssl/stack.h
eng_table.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
eng_table.o: ../../include/openssl/ui_compat.h eng_int.h eng_table.c
hw_4758_cca.o: ../../e_os.h ../../include/openssl/aes.h
hw_4758_cca.o: ../../include/openssl/asn1.h ../../include/openssl/bio.h
hw_4758_cca.o: ../../include/openssl/bn.h ../../include/openssl/buffer.h
hw_4758_cca.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
hw_4758_cca.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
hw_4758_cca.o: ../../include/openssl/dso.h ../../include/openssl/e_os2.h
hw_4758_cca.o: ../../include/openssl/engine.h ../../include/openssl/err.h
hw_4758_cca.o: ../../include/openssl/evp.h ../../include/openssl/lhash.h
hw_4758_cca.o: ../../include/openssl/md5.h ../../include/openssl/obj_mac.h
hw_4758_cca.o: ../../include/openssl/objects.h
hw_4758_cca.o: ../../include/openssl/opensslconf.h
hw_4758_cca.o: ../../include/openssl/opensslv.h
hw_4758_cca.o: ../../include/openssl/ossl_typ.h ../../include/openssl/pkcs7.h
hw_4758_cca.o: ../../include/openssl/rand.h ../../include/openssl/rc5.h
hw_4758_cca.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
hw_4758_cca.o: ../../include/openssl/sha.h ../../include/openssl/stack.h
hw_4758_cca.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
hw_4758_cca.o: ../../include/openssl/ui_compat.h ../../include/openssl/x509.h
hw_4758_cca.o: ../../include/openssl/x509_vfy.h ../cryptlib.h hw_4758_cca.c
hw_4758_cca.o: hw_4758_cca_err.c hw_4758_cca_err.h vendor_defns/hw_4758_cca.h
hw_aep.o: ../../include/openssl/asn1.h ../../include/openssl/bio.h
hw_aep.o: ../../include/openssl/bn.h ../../include/openssl/buffer.h
hw_aep.o: ../../include/openssl/crypto.h ../../include/openssl/dsa.h
hw_aep.o: ../../include/openssl/dso.h ../../include/openssl/e_os2.h
hw_aep.o: ../../include/openssl/engine.h ../../include/openssl/err.h
hw_aep.o: ../../include/openssl/lhash.h ../../include/openssl/opensslconf.h
hw_aep.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
hw_aep.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
hw_aep.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
hw_aep.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h hw_aep.c
hw_aep.o: hw_aep_err.c hw_aep_err.h vendor_defns/aep.h
hw_atalla.o: ../../e_os.h ../../include/openssl/asn1.h
hw_atalla.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
hw_atalla.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
hw_atalla.o: ../../include/openssl/dsa.h ../../include/openssl/dso.h
hw_atalla.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
hw_atalla.o: ../../include/openssl/err.h ../../include/openssl/lhash.h
hw_atalla.o: ../../include/openssl/opensslconf.h
hw_atalla.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
hw_atalla.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
hw_atalla.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
hw_atalla.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
hw_atalla.o: ../cryptlib.h hw_atalla.c hw_atalla_err.c hw_atalla_err.h
hw_atalla.o: vendor_defns/atalla.h
hw_cryptodev.o: ../../include/openssl/aes.h ../../include/openssl/asn1.h
hw_cryptodev.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
hw_cryptodev.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
hw_cryptodev.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
hw_cryptodev.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
hw_cryptodev.o: ../../include/openssl/err.h ../../include/openssl/evp.h
hw_cryptodev.o: ../../include/openssl/lhash.h ../../include/openssl/md5.h
hw_cryptodev.o: ../../include/openssl/obj_mac.h ../../include/openssl/objects.h
hw_cryptodev.o: ../../include/openssl/opensslconf.h
hw_cryptodev.o: ../../include/openssl/opensslv.h
hw_cryptodev.o: ../../include/openssl/ossl_typ.h ../../include/openssl/rand.h
hw_cryptodev.o: ../../include/openssl/rc5.h ../../include/openssl/rsa.h
hw_cryptodev.o: ../../include/openssl/safestack.h ../../include/openssl/sha.h
hw_cryptodev.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
hw_cryptodev.o: ../../include/openssl/ui.h ../../include/openssl/ui_compat.h
hw_cryptodev.o: hw_cryptodev.c
hw_cswift.o: ../../e_os.h ../../include/openssl/asn1.h
hw_cswift.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
hw_cswift.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
hw_cswift.o: ../../include/openssl/dsa.h ../../include/openssl/dso.h
hw_cswift.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
hw_cswift.o: ../../include/openssl/err.h ../../include/openssl/lhash.h
hw_cswift.o: ../../include/openssl/opensslconf.h
hw_cswift.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
hw_cswift.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
hw_cswift.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
hw_cswift.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
hw_cswift.o: ../cryptlib.h hw_cswift.c hw_cswift_err.c hw_cswift_err.h
hw_cswift.o: vendor_defns/cswift.h
hw_ncipher.o: ../../e_os.h ../../include/openssl/aes.h
hw_ncipher.o: ../../include/openssl/asn1.h ../../include/openssl/bio.h
hw_ncipher.o: ../../include/openssl/bn.h ../../include/openssl/buffer.h
hw_ncipher.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
hw_ncipher.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
hw_ncipher.o: ../../include/openssl/dso.h ../../include/openssl/e_os2.h
hw_ncipher.o: ../../include/openssl/engine.h ../../include/openssl/err.h
hw_ncipher.o: ../../include/openssl/evp.h ../../include/openssl/lhash.h
hw_ncipher.o: ../../include/openssl/md5.h ../../include/openssl/obj_mac.h
hw_ncipher.o: ../../include/openssl/objects.h
hw_ncipher.o: ../../include/openssl/opensslconf.h
hw_ncipher.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
hw_ncipher.o: ../../include/openssl/pem.h ../../include/openssl/pem2.h
hw_ncipher.o: ../../include/openssl/pkcs7.h ../../include/openssl/rand.h
hw_ncipher.o: ../../include/openssl/rc5.h ../../include/openssl/rsa.h
hw_ncipher.o: ../../include/openssl/safestack.h ../../include/openssl/sha.h
hw_ncipher.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
hw_ncipher.o: ../../include/openssl/ui.h ../../include/openssl/ui_compat.h
hw_ncipher.o: ../../include/openssl/x509.h ../../include/openssl/x509_vfy.h
hw_ncipher.o: ../cryptlib.h hw_ncipher.c hw_ncipher_err.c hw_ncipher_err.h
hw_ncipher.o: vendor_defns/hwcryptohook.h
hw_nuron.o: ../../e_os.h ../../include/openssl/asn1.h
hw_nuron.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
hw_nuron.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
hw_nuron.o: ../../include/openssl/dsa.h ../../include/openssl/dso.h
hw_nuron.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
hw_nuron.o: ../../include/openssl/err.h ../../include/openssl/lhash.h
hw_nuron.o: ../../include/openssl/opensslconf.h
hw_nuron.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
hw_nuron.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
hw_nuron.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
hw_nuron.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
hw_nuron.o: ../cryptlib.h hw_nuron.c hw_nuron_err.c hw_nuron_err.h
hw_sureware.o: ../../e_os.h ../../include/openssl/aes.h
hw_sureware.o: ../../include/openssl/asn1.h ../../include/openssl/bio.h
hw_sureware.o: ../../include/openssl/bn.h ../../include/openssl/buffer.h
hw_sureware.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
hw_sureware.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
hw_sureware.o: ../../include/openssl/dso.h ../../include/openssl/e_os2.h
hw_sureware.o: ../../include/openssl/engine.h ../../include/openssl/err.h
hw_sureware.o: ../../include/openssl/evp.h ../../include/openssl/lhash.h
hw_sureware.o: ../../include/openssl/md5.h ../../include/openssl/obj_mac.h
hw_sureware.o: ../../include/openssl/objects.h
hw_sureware.o: ../../include/openssl/opensslconf.h
hw_sureware.o: ../../include/openssl/opensslv.h
hw_sureware.o: ../../include/openssl/ossl_typ.h ../../include/openssl/pem.h
hw_sureware.o: ../../include/openssl/pem2.h ../../include/openssl/pkcs7.h
hw_sureware.o: ../../include/openssl/rand.h ../../include/openssl/rc5.h
hw_sureware.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
hw_sureware.o: ../../include/openssl/sha.h ../../include/openssl/stack.h
hw_sureware.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
hw_sureware.o: ../../include/openssl/ui_compat.h ../../include/openssl/x509.h
hw_sureware.o: ../../include/openssl/x509_vfy.h ../cryptlib.h eng_int.h
hw_sureware.o: engine.h hw_sureware.c hw_sureware_err.c hw_sureware_err.h
hw_sureware.o: vendor_defns/sureware.h
hw_ubsec.o: ../../e_os.h ../../include/openssl/asn1.h
hw_ubsec.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
hw_ubsec.o: ../../include/openssl/buffer.h ../../include/openssl/crypto.h
hw_ubsec.o: ../../include/openssl/dsa.h ../../include/openssl/dso.h
hw_ubsec.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
hw_ubsec.o: ../../include/openssl/err.h ../../include/openssl/lhash.h
hw_ubsec.o: ../../include/openssl/opensslconf.h
hw_ubsec.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
hw_ubsec.o: ../../include/openssl/rand.h ../../include/openssl/rsa.h
hw_ubsec.o: ../../include/openssl/safestack.h ../../include/openssl/stack.h
hw_ubsec.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
hw_ubsec.o: ../cryptlib.h hw_ubsec.c hw_ubsec_err.c hw_ubsec_err.h
hw_ubsec.o: vendor_defns/hw_ubsec.h
tb_cipher.o: ../../include/openssl/aes.h ../../include/openssl/asn1.h
tb_cipher.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
tb_cipher.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
tb_cipher.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
tb_cipher.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
tb_cipher.o: ../../include/openssl/err.h ../../include/openssl/evp.h
tb_cipher.o: ../../include/openssl/lhash.h ../../include/openssl/md5.h
tb_cipher.o: ../../include/openssl/obj_mac.h ../../include/openssl/objects.h
tb_cipher.o: ../../include/openssl/opensslconf.h
tb_cipher.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
tb_cipher.o: ../../include/openssl/rand.h ../../include/openssl/rc5.h
tb_cipher.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
tb_cipher.o: ../../include/openssl/sha.h ../../include/openssl/stack.h
tb_cipher.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
tb_cipher.o: ../../include/openssl/ui_compat.h eng_int.h tb_cipher.c
tb_dh.o: ../../include/openssl/aes.h ../../include/openssl/asn1.h
tb_dh.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
tb_dh.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
tb_dh.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
tb_dh.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
tb_dh.o: ../../include/openssl/err.h ../../include/openssl/evp.h
tb_dh.o: ../../include/openssl/lhash.h ../../include/openssl/md5.h
tb_dh.o: ../../include/openssl/obj_mac.h ../../include/openssl/objects.h
tb_dh.o: ../../include/openssl/opensslconf.h ../../include/openssl/opensslv.h
tb_dh.o: ../../include/openssl/ossl_typ.h ../../include/openssl/rand.h
tb_dh.o: ../../include/openssl/rc5.h ../../include/openssl/rsa.h
tb_dh.o: ../../include/openssl/safestack.h ../../include/openssl/sha.h
tb_dh.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
tb_dh.o: ../../include/openssl/ui.h ../../include/openssl/ui_compat.h eng_int.h
tb_dh.o: tb_dh.c
tb_digest.o: ../../include/openssl/aes.h ../../include/openssl/asn1.h
tb_digest.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
tb_digest.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
tb_digest.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
tb_digest.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
tb_digest.o: ../../include/openssl/err.h ../../include/openssl/evp.h
tb_digest.o: ../../include/openssl/lhash.h ../../include/openssl/md5.h
tb_digest.o: ../../include/openssl/obj_mac.h ../../include/openssl/objects.h
tb_digest.o: ../../include/openssl/opensslconf.h
tb_digest.o: ../../include/openssl/opensslv.h ../../include/openssl/ossl_typ.h
tb_digest.o: ../../include/openssl/rand.h ../../include/openssl/rc5.h
tb_digest.o: ../../include/openssl/rsa.h ../../include/openssl/safestack.h
tb_digest.o: ../../include/openssl/sha.h ../../include/openssl/stack.h
tb_digest.o: ../../include/openssl/symhacks.h ../../include/openssl/ui.h
tb_digest.o: ../../include/openssl/ui_compat.h eng_int.h tb_digest.c
tb_dsa.o: ../../include/openssl/aes.h ../../include/openssl/asn1.h
tb_dsa.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
tb_dsa.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
tb_dsa.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
tb_dsa.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
tb_dsa.o: ../../include/openssl/err.h ../../include/openssl/evp.h
tb_dsa.o: ../../include/openssl/lhash.h ../../include/openssl/md5.h
tb_dsa.o: ../../include/openssl/obj_mac.h ../../include/openssl/objects.h
tb_dsa.o: ../../include/openssl/opensslconf.h ../../include/openssl/opensslv.h
tb_dsa.o: ../../include/openssl/ossl_typ.h ../../include/openssl/rand.h
tb_dsa.o: ../../include/openssl/rc5.h ../../include/openssl/rsa.h
tb_dsa.o: ../../include/openssl/safestack.h ../../include/openssl/sha.h
tb_dsa.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
tb_dsa.o: ../../include/openssl/ui.h ../../include/openssl/ui_compat.h
tb_dsa.o: eng_int.h tb_dsa.c
tb_rand.o: ../../include/openssl/aes.h ../../include/openssl/asn1.h
tb_rand.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
tb_rand.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
tb_rand.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
tb_rand.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
tb_rand.o: ../../include/openssl/err.h ../../include/openssl/evp.h
tb_rand.o: ../../include/openssl/lhash.h ../../include/openssl/md5.h
tb_rand.o: ../../include/openssl/obj_mac.h ../../include/openssl/objects.h
tb_rand.o: ../../include/openssl/opensslconf.h ../../include/openssl/opensslv.h
tb_rand.o: ../../include/openssl/ossl_typ.h ../../include/openssl/rand.h
tb_rand.o: ../../include/openssl/rc5.h ../../include/openssl/rsa.h
tb_rand.o: ../../include/openssl/safestack.h ../../include/openssl/sha.h
tb_rand.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
tb_rand.o: ../../include/openssl/ui.h ../../include/openssl/ui_compat.h
tb_rand.o: eng_int.h tb_rand.c
tb_rsa.o: ../../include/openssl/aes.h ../../include/openssl/asn1.h
tb_rsa.o: ../../include/openssl/bio.h ../../include/openssl/bn.h
tb_rsa.o: ../../include/openssl/crypto.h ../../include/openssl/des.h
tb_rsa.o: ../../include/openssl/des_old.h ../../include/openssl/dsa.h
tb_rsa.o: ../../include/openssl/e_os2.h ../../include/openssl/engine.h
tb_rsa.o: ../../include/openssl/err.h ../../include/openssl/evp.h
tb_rsa.o: ../../include/openssl/lhash.h ../../include/openssl/md5.h
tb_rsa.o: ../../include/openssl/obj_mac.h ../../include/openssl/objects.h
tb_rsa.o: ../../include/openssl/opensslconf.h ../../include/openssl/opensslv.h
tb_rsa.o: ../../include/openssl/ossl_typ.h ../../include/openssl/rand.h
tb_rsa.o: ../../include/openssl/rc5.h ../../include/openssl/rsa.h
tb_rsa.o: ../../include/openssl/safestack.h ../../include/openssl/sha.h
tb_rsa.o: ../../include/openssl/stack.h ../../include/openssl/symhacks.h
tb_rsa.o: ../../include/openssl/ui.h ../../include/openssl/ui_compat.h
tb_rsa.o: eng_int.h tb_rsa.c
