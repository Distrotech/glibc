#define MODULE_libc		1
#define MODULE_libpthread	2
#define MODULE_rtld		3
#define MODULE_libdl		4
#define MODULE_libm		5
#define MODULE_iconv_programs	6
#define MODULE_iconvdata	7
#define MODULE_lddlibc4		8
#define MODULE_locale_programs	9
#define MODULE_memusagestat	10
#define MODULE_libutil		12
#define MODULE_libBrokenLocale	13
#define MODULE_libmemusage	15
#define MODULE_libresolv	16
#define MODULE_libnss_db	17
#define MODULE_libnss_files	18
#define	MODULE_libnss_dns	19
#define MODULE_libnss_compat	20
#define MODULE_libnss_hesiod	21
#define MODULE_libnss_nis	22
#define MODULE_libnss_nisplus	23
#define MODULE_libanl		24
#define MODULE_librt		25
#define MODULE_libSegFault	26
#define MODULE_libthread_db	27
#define MODULE_libcidn		28
#define MODULE_libcrypt		29
#define MODULE_libnsl		30
#define MODULE_libpcprofile	31
#define MODULE_librpcsvc	32
#define MODULE_nscd		33
#define MODULE_ldconfig		34

/* Added to the core library modules, i.e. libc, libpthread, libnss_*, etc.  */
#define MODULE_libs		1000

/* Catch-all for test modules and other binaries.  */
#define MODULE_nonlib		98
#define MODULE_extramodules	99

#define IS_IN_MODULE(mod) (IN_MODULE == MODULE_##mod \
			   || IN_MODULE == MODULE_##mod + MODULE_libs)

#define IS_IN_MODULE_LIB (IN_MODULE > MODULE_libs)

#if IS_IN_MODULE (libc)
# define MODULE_NAME libc
#elif IS_IN_MODULE (libpthread)
# define MODULE_NAME libpthread
#elif IS_IN_MODULE (rtld)
# define MODULE_NAME rtld
#elif IS_IN_MODULE (libm)
# define MODULE_NAME libm
#elif IS_IN_MODULE (libanl)
# define MODULE_NAME libanl
#elif IS_IN_MODULE (librt)
# define MODULE_NAME librt
#endif
