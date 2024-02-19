#ifndef VARS_H
#define VARS_H

#define PROG_NAME "rus2utf"
#define ARG_ENCODE_LONG "--encode"
#define ARG_ENCODE_SHORT "-e"
#define ARG_OUTPUTFILE "-of"
#define ARG_HELP "--help"
#define MAXLEN_FILENAME 25
#define ARGC_OF_SPECIFIED 6
#define ENCODING_TABLE_HALF 128

#define S_KOI8 "koi8-r"
#define S_CP1251 "cp-1251"
#define S_ISO8859 "iso-8859-5"
#define H_KOI8 0x001
#define H_CP1251 0x010
#define H_ISO8859 0x100


extern int encoding;
extern unsigned short koi8r_to_unicode[ENCODING_TABLE_HALF];
extern unsigned short cp1251_to_unicode[ENCODING_TABLE_HALF];
extern unsigned short iso88595_to_unicode[ENCODING_TABLE_HALF];

#endif // VARS_H
