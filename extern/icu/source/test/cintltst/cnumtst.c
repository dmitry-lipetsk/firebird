/********************************************************************
 * COPYRIGHT:
 * Copyright (c) 1997-2004, International Business Machines Corporation and
 * others. All Rights Reserved.
 ********************************************************************/
/********************************************************************************
*
* File CNUMTST.C
*
*     Madhu Katragadda              Creation
*
* Modification History:
*
*   Date        Name        Description
*   06/24/99    helena      Integrated Alan's NF enhancements and Java2 bug fixes
*   07/15/99    helena      Ported to HPUX 10/11 CC.
*********************************************************************************
*/

/* C API TEST FOR NUMBER FORMAT */

#include "unicode/utypes.h"

#if !UCONFIG_NO_FORMATTING

#include "unicode/uloc.h"
#include "unicode/unum.h"
#include "unicode/ustring.h"
#include "cintltst.h"
#include "cnumtst.h"
#include "cmemory.h"

#define LENGTH(arr) (sizeof(arr)/sizeof(arr[0]))

void addNumForTest(TestNode** root);

#define TESTCASE(x) addTest(root, &x, "tsformat/cnumtst/" #x)

void addNumForTest(TestNode** root)
{
    TESTCASE(TestNumberFormat);
    TESTCASE(TestSignificantDigits);
    TESTCASE(TestNumberFormatPadding);
    TESTCASE(TestInt64Format);
    TESTCASE(TestRBNFFormat);
}

/** copy src to dst with unicode-escapes for values < 0x20 and > 0x7e, null terminate if possible */
static int32_t ustrToAstr(const UChar* src, int32_t srcLength, char* dst, int32_t dstLength) {
    static const char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

    char *p = dst;
    const char *e = p + dstLength;
    if (srcLength < 0) {
        const UChar* s = src;
        while (*s) {
            ++s;
        }
        srcLength = (int32_t)(s - src);
    }
    while (p < e && --srcLength >= 0) {
        UChar c = *src++;
        if (c == 0xd || c == 0xa || c == 0x9 || (c>= 0x20 && c <= 0x7e)) {
            *p++ = (char) c & 0x7f;
        } else if (e - p >= 6) {
            *p++ = '\\';
            *p++ = 'u';
            *p++ = hex[(c >> 12) & 0xf];
            *p++ = hex[(c >> 8) & 0xf];
            *p++ = hex[(c >> 4) & 0xf];
            *p++ = hex[c & 0xf];
        } else {
            break;
        }
    }
    if (p < e) {
        *p = 0;
    }
    return (int32_t)(p - dst);
}

/* test Number Format API */
static void TestNumberFormat()
{
    UChar *result=NULL;
    UChar temp1[512];
    UChar temp2[512];

    UChar temp[5];

    UChar prefix[5];
    UChar suffix[5];
    UChar symbol[20];
    int32_t resultlength;
    int32_t resultlengthneeded;
    int32_t parsepos;
    double d1;
    int32_t l1;
    double d = -10456.37;
    double a = 1234.56, a1 = 1235.0;
    int32_t l = 100000000;
    UFieldPosition pos1;
    UFieldPosition pos2;
    int32_t numlocales;
    int32_t i;

    UNumberFormatAttribute attr;
    UNumberFormatSymbol symType = UNUM_DECIMAL_SEPARATOR_SYMBOL;
    int32_t newvalue;
    UErrorCode status=U_ZERO_ERROR;
    UNumberFormatStyle style= UNUM_DEFAULT;
    UNumberFormat *pattern;
    UNumberFormat *def, *fr, *cur_def, *cur_fr, *per_def, *per_fr,
                  *cur_frpattern, *myclone, *spellout_def;

    /* Testing unum_open() with various Numberformat styles and locales*/
    status = U_ZERO_ERROR;
    log_verbose("Testing  unum_open() with default style and locale\n");
    def=unum_open(style, NULL,0,NULL, NULL,&status);
    if(U_FAILURE(status))
        log_err("Error in creating NumberFormat default using unum_open(): %s\n", myErrorName(status));

    log_verbose("\nTesting unum_open() with french locale and default style(decimal)\n");
    fr=unum_open(style,NULL,0, "fr_FR",NULL, &status);
    if(U_FAILURE(status))
        log_err("Error: could not create NumberFormat (french): %s\n", myErrorName(status));

    log_verbose("\nTesting unum_open(currency,NULL,status)\n");
    style=UNUM_CURRENCY;
    /* Can't hardcode the result to assume the default locale is "en_US". */
    cur_def=unum_open(style, NULL,0,"en_US", NULL, &status);
    if(U_FAILURE(status))
        log_err("Error: could not create NumberFormat using \n unum_open(currency, NULL, &status) %s\n",
                        myErrorName(status) );

    log_verbose("\nTesting unum_open(currency, frenchlocale, status)\n");
    cur_fr=unum_open(style,NULL,0, "fr_FR", NULL, &status);
    if(U_FAILURE(status))
        log_err("Error: could not create NumberFormat using unum_open(currency, french, &status): %s\n", 
                myErrorName(status));

    log_verbose("\nTesting unum_open(percent, NULL, status)\n");
    style=UNUM_PERCENT;
    per_def=unum_open(style,NULL,0, NULL,NULL, &status);
    if(U_FAILURE(status))
        log_err("Error: could not create NumberFormat using unum_open(percent, NULL, &status): %s\n", myErrorName(status));

    log_verbose("\nTesting unum_open(percent,frenchlocale, status)\n");
    per_fr=unum_open(style, NULL,0,"fr_FR", NULL,&status);
    if(U_FAILURE(status))
        log_err("Error: could not create NumberFormat using unum_open(percent, french, &status): %s\n", myErrorName(status));

    log_verbose("\nTesting unum_open(spellout, NULL, status)");
    style=UNUM_SPELLOUT;
    spellout_def=unum_open(style, NULL, 0, "en_US", NULL, &status);
    if(U_FAILURE(status))
        log_err("Error: could not create NumberFormat using unum_open(spellout, NULL, &status): %s\n", myErrorName(status));

    /* Testing unum_clone(..) */
    log_verbose("\nTesting unum_clone(fmt, status)");
    status = U_ZERO_ERROR;
    myclone = unum_clone(def,&status);
    if(U_FAILURE(status))
        log_err("Error: could not clone unum_clone(def, &status): %s\n", myErrorName(status));
    else
    {
        log_verbose("unum_clone() successful\n");
    }

    /*Testing unum_getAvailable() and unum_countAvailable()*/
    log_verbose("\nTesting getAvailableLocales and countAvailable()\n");
    numlocales=unum_countAvailable();
    if(numlocales < 0)
        log_err("error in countAvailable");
    else{
        log_verbose("unum_countAvialable() successful\n");
        log_verbose("The no: of locales where number formattting is applicable is %d\n", numlocales);
    }
    for(i=0;i<numlocales;i++)
    {
        log_verbose("%s\n", unum_getAvailable(i));
        if (unum_getAvailable(i) == 0)
            log_err("No locale for which number formatting patterns are applicable\n");
        else
            log_verbose("A locale %s for which number formatting patterns are applicable\n",unum_getAvailable(i));
    }


    /*Testing unum_format() and unum_formatdouble()*/
    u_uastrcpy(temp1, "$100,000,000.00");

    log_verbose("\nTesting unum_format() \n");
    resultlength=0;
    pos1.field = 0; /* Integer Section */
    resultlengthneeded=unum_format(cur_def, l, NULL, resultlength, &pos1, &status);
    if(status==U_BUFFER_OVERFLOW_ERROR)
    {
        status=U_ZERO_ERROR;
        resultlength=resultlengthneeded+1;
        result=(UChar*)malloc(sizeof(UChar) * resultlength);
/*        for (i = 0; i < 100000; i++) */
        {
            unum_format(cur_def, l, result, resultlength, &pos1, &status);
        }
    }

    if(U_FAILURE(status))
    {
        log_err("Error in formatting using unum_format(.....): %s\n", myErrorName(status) );
    }
    if(u_strcmp(result, temp1)==0)
        log_verbose("Pass: Number formatting using unum_format() successful\n");
    else
        log_err("Fail: Error in number Formatting using unum_format()\n");
    if(pos1.beginIndex == 1 && pos1.endIndex == 12)
        log_verbose("Pass: Complete number formatting using unum_format() successful\n");
    else
        log_err("Fail: Error in complete number Formatting using unum_format()\nGot: b=%d end=%d\nExpected: b=1 end=12\n",
                pos1.beginIndex, pos1.endIndex);

free(result);
    result = 0;

    log_verbose("\nTesting unum_formatDouble()\n");
    u_uastrcpy(temp1, "($10,456.37)");
    resultlength=0;
    pos2.field = 1; /* Fractional Section */
    resultlengthneeded=unum_formatDouble(cur_def, d, NULL, resultlength, &pos2, &status);
    if(status==U_BUFFER_OVERFLOW_ERROR)
    {
        status=U_ZERO_ERROR;
        resultlength=resultlengthneeded+1;
        result=(UChar*)malloc(sizeof(UChar) * resultlength);
/*        for (i = 0; i < 100000; i++) */
        {
            unum_formatDouble(cur_def, d, result, resultlength, &pos2, &status);
        }
    }
    if(U_FAILURE(status))
    {
        log_err("Error in formatting using unum_formatDouble(.....): %s\n", myErrorName(status));
    }
    if(u_strcmp(result, temp1)==0)
        log_verbose("Pass: Number Formatting using unum_formatDouble() Successful\n");
    else
        log_err("FAIL: Error in number formatting using unum_formatDouble()\n");
    if(pos2.beginIndex == 9 && pos2.endIndex == 11)
        log_verbose("Pass: Complete number formatting using unum_format() successful\n");
    else
        log_err("Fail: Error in complete number Formatting using unum_formatDouble()\nGot: b=%d end=%d\nExpected: b=9 end=11",
                pos1.beginIndex, pos1.endIndex);


    /* Testing unum_parse() and unum_parseDouble() */
    log_verbose("\nTesting unum_parseDouble()\n");
/*    for (i = 0; i < 100000; i++)*/
    {
        parsepos=0;
        d1=unum_parseDouble(cur_def, result, u_strlen(result), &parsepos, &status);
    }
    if(U_FAILURE(status))
    {
        log_err("parse failed. The error is  : %s\n", myErrorName(status));
    }

    if(d1!=d)
        log_err("Fail: Error in parsing\n");
    else
        log_verbose("Pass: parsing successful\n");
    if (result)
        free(result);
    result = 0;


    /* Testing unum_formatDoubleCurrency / unum_parseDoubleCurrency */
    log_verbose("\nTesting unum_formatDoubleCurrency\n");
    u_uastrcpy(temp1, "Y1,235");
    temp1[0] = 0xA5; /* Yen sign */
    u_uastrcpy(temp, "JPY");
    resultlength=0;
    pos2.field = 0; /* integer part */
    resultlengthneeded=unum_formatDoubleCurrency(cur_def, a, temp, NULL, resultlength, &pos2, &status);
    if (status==U_BUFFER_OVERFLOW_ERROR) {
        status=U_ZERO_ERROR;
        resultlength=resultlengthneeded+1;
        result=(UChar*)malloc(sizeof(UChar) * resultlength);
        unum_formatDoubleCurrency(cur_def, a, temp, result, resultlength, &pos2, &status);
    }
    if (U_FAILURE(status)) {
        log_err("Error in formatting using unum_formatDouble(.....): %s\n", myErrorName(status));
    }
    if (u_strcmp(result, temp1)==0) {
        log_verbose("Pass: Number Formatting using unum_formatDouble() Successful\n");
    } else {
        log_err("FAIL: Error in number formatting using unum_formatDouble()\n");
    }
    if (pos2.beginIndex == 1 && pos2.endIndex == 6) {
        log_verbose("Pass: Complete number formatting using unum_format() successful\n");
    } else {
        log_err("Fail: Error in complete number Formatting using unum_formatDouble()\nGot: b=%d end=%d\nExpected: b=1 end=6",
                pos1.beginIndex, pos1.endIndex);
    }

    log_verbose("\nTesting unum_parseDoubleCurrency\n");
    parsepos=0;
    d1=unum_parseDoubleCurrency(cur_def, result, u_strlen(result), &parsepos, temp2, &status);
    if (U_FAILURE(status)) {
        log_err("parse failed. The error is  : %s\n", myErrorName(status));
    }
    /* Note: a==1234.56, but on parse expect a1=1235.0 */
    if (d1!=a1) {
        log_err("Fail: Error in parsing currency, got %f, expected %f\n", d1, a1);
    } else {
        log_verbose("Pass: parsed currency ammount successfully\n");
    }
    if (u_strcmp(temp2, temp)==0) {
        log_verbose("Pass: parsed correct currency\n");
    } else {
        log_err("Fail: parsed incorrect currency\n");
    }

free(result);
    result = 0;


/* performance testing */
    u_uastrcpy(temp1, "$462.12345");
    resultlength=u_strlen(temp1);
/*    for (i = 0; i < 100000; i++) */
    {
        parsepos=0;
        d1=unum_parseDouble(cur_def, temp1, resultlength, &parsepos, &status);
    }
    if(U_FAILURE(status))
    {
        log_err("parse failed. The error is  : %s\n", myErrorName(status));
    }

    if(d1!=462.12345)
        log_err("Fail: Error in parsing\n");
    else
        log_verbose("Pass: parsing successful\n");

free(result);

    u_uastrcpy(temp1, "($10,456.3E1])");
    parsepos=0;
    d1=unum_parseDouble(cur_def, temp1, u_strlen(temp1), &parsepos, &status);
    if(U_SUCCESS(status))
    {
        log_err("Error in unum_parseDouble(..., %s, ...): %s\n", temp1, myErrorName(status));
    }


    log_verbose("\nTesting unum_format()\n");
    status=U_ZERO_ERROR;
    resultlength=0;
    parsepos=0;
    resultlengthneeded=unum_format(per_fr, l, NULL, resultlength, &pos1, &status);
    if(status==U_BUFFER_OVERFLOW_ERROR)
    {
        status=U_ZERO_ERROR;
        resultlength=resultlengthneeded+1;
        result=(UChar*)malloc(sizeof(UChar) * resultlength);
/*        for (i = 0; i < 100000; i++)*/
        {
            unum_format(per_fr, l, result, resultlength, &pos1, &status);
        }
    }
    if(U_FAILURE(status))
    {
        log_err("Error in formatting using unum_format(.....): %s\n", myErrorName(status));
    }


    log_verbose("\nTesting unum_parse()\n");
/*    for (i = 0; i < 100000; i++) */
    {
        parsepos=0;
        l1=unum_parse(per_fr, result, u_strlen(result), &parsepos, &status);
    }
    if(U_FAILURE(status))
    {
        log_err("parse failed. The error is  : %s\n", myErrorName(status));
    }

    if(l1!=l)
        log_err("Fail: Error in parsing\n");
    else
        log_verbose("Pass: parsing successful\n");

free(result);

    /* create a number format using unum_openPattern(....)*/
    log_verbose("\nTesting unum_openPattern()\n");
    u_uastrcpy(temp1, "#,##0.0#;(#,##0.0#)");
    pattern=unum_open(UNUM_IGNORE,temp1, u_strlen(temp1), NULL, NULL,&status);
    if(U_FAILURE(status))
    {
        log_err("error in unum_openPattern(): %s\n", myErrorName(status) );;
    }
    else
        log_verbose("Pass: unum_openPattern() works fine\n");

    /*test for unum_toPattern()*/
    log_verbose("\nTesting unum_toPattern()\n");
    resultlength=0;
    resultlengthneeded=unum_toPattern(pattern, FALSE, NULL, resultlength, &status);
    if(status==U_BUFFER_OVERFLOW_ERROR)
    {
        status=U_ZERO_ERROR;
        resultlength=resultlengthneeded+1;
        result=(UChar*)malloc(sizeof(UChar) * resultlength);
        unum_toPattern(pattern, FALSE, result, resultlength, &status);
    }
    if(U_FAILURE(status))
    {
        log_err("error in extracting the pattern from UNumberFormat: %s\n", myErrorName(status));
    }
    else
    {
        if(u_strcmp(result, temp1)!=0)
            log_err("FAIL: Error in extracting the pattern using unum_toPattern()\n");
        else
            log_verbose("Pass: extracted the pattern correctly using unum_toPattern()\n");
free(result);
    }

    /*Testing unum_getSymbols() and unum_setSymbols()*/
    log_verbose("\nTesting unum_getSymbols and unum_setSymbols()\n");
    /*when we try to change the symbols of french to default we need to apply the pattern as well to fetch correct results */
    resultlength=0;
    resultlengthneeded=unum_toPattern(cur_def, FALSE, NULL, resultlength, &status);
    if(status==U_BUFFER_OVERFLOW_ERROR)
    {
        status=U_ZERO_ERROR;
        resultlength=resultlengthneeded+1;
        result=(UChar*)malloc(sizeof(UChar) * resultlength);
        unum_toPattern(cur_def, FALSE, result, resultlength, &status);
    }
    if(U_FAILURE(status))
    {
        log_err("error in extracting the pattern from UNumberFormat: %s\n", myErrorName(status));
    }

    status=U_ZERO_ERROR;
    cur_frpattern=unum_open(UNUM_IGNORE,result, u_strlen(result), "fr_FR",NULL, &status);
    if(U_FAILURE(status))
    {
        log_err("error in unum_openPattern(): %s\n", myErrorName(status));
    }

free(result);

    /*getting the symbols of cur_def */
    /*set the symbols of cur_frpattern to cur_def */
    for (symType = UNUM_DECIMAL_SEPARATOR_SYMBOL; symType < UNUM_FORMAT_SYMBOL_COUNT; symType++) {
        status=U_ZERO_ERROR;
        unum_getSymbol(cur_def, symType, temp1, sizeof(temp1), &status);
        unum_setSymbol(cur_frpattern, symType, temp1, -1, &status);
        if(U_FAILURE(status))
        {
            log_err("Error in get/set symbols: %s\n", myErrorName(status));
        }
    }

    /*format to check the result */
    resultlength=0;
    resultlengthneeded=unum_format(cur_def, l, NULL, resultlength, &pos1, &status);
    if(status==U_BUFFER_OVERFLOW_ERROR)
    {
        status=U_ZERO_ERROR;
        resultlength=resultlengthneeded+1;
        result=(UChar*)malloc(sizeof(UChar) * resultlength);
        unum_format(cur_def, l, result, resultlength, &pos1, &status);
    }
    if(U_FAILURE(status))
    {
        log_err("Error in formatting using unum_format(.....): %s\n", myErrorName(status));
    }

    if(U_FAILURE(status)){
        log_err("Fail: error in unum_setSymbols: %s\n", myErrorName(status));
    }
    unum_applyPattern(cur_frpattern, FALSE, result, u_strlen(result),NULL,NULL);

    for (symType = UNUM_DECIMAL_SEPARATOR_SYMBOL; symType < UNUM_FORMAT_SYMBOL_COUNT; symType++) {
        status=U_ZERO_ERROR;
        unum_getSymbol(cur_def, symType, temp1, sizeof(temp1), &status);
        unum_getSymbol(cur_frpattern, symType, temp2, sizeof(temp2), &status);
        if(U_FAILURE(status) || u_strcmp(temp1, temp2) != 0)
        {
            log_err("Fail: error in getting symbols\n");
        }
        else
            log_verbose("Pass: get and set symbols successful\n");
    }

    /*format and check with the previous result */

    resultlength=0;
    resultlengthneeded=unum_format(cur_frpattern, l, NULL, resultlength, &pos1, &status);
    if(status==U_BUFFER_OVERFLOW_ERROR)
    {
        status=U_ZERO_ERROR;
        resultlength=resultlengthneeded+1;
        unum_format(cur_frpattern, l, temp1, resultlength, &pos1, &status);
    }
    if(U_FAILURE(status))
    {
        log_err("Error in formatting using unum_format(.....): %s\n", myErrorName(status));
    }
    /* TODO: 
     * This test fails because we have not called unum_applyPattern().
     * Currently, such an applyPattern() does not exist on the C API, and
     * we have jitterbug 411 for it.
     * Since it is close to the 1.5 release, I (markus) am disabling this test just
     * for this release (I added the test itself only last week).
     * For the next release, we need to fix this.
     * Then, remove the uprv_strcmp("1.5", ...) and this comment, and the include "cstring.h" at the beginning of this file.
     */
    if(u_strcmp(result, temp1) != 0) {
        log_err("Formatting failed after setting symbols. result=%s temp1=%s\n", result, temp1);
    }


    /*----------- */

free(result);

    /* Testing unum_get/setSymbol() */
    for(i = 0; i < UNUM_FORMAT_SYMBOL_COUNT; ++i) {
        symbol[0] = (UChar)(0x41 + i);
        symbol[1] = (UChar)(0x61 + i);
        unum_setSymbol(cur_frpattern, (UNumberFormatSymbol)i, symbol, 2, &status);
        if(U_FAILURE(status)) {
            log_err("Error from unum_setSymbol(%d): %s\n", i, myErrorName(status));
            return;
        }
    }
    for(i = 0; i < UNUM_FORMAT_SYMBOL_COUNT; ++i) {
        resultlength = unum_getSymbol(cur_frpattern, (UNumberFormatSymbol)i, symbol, sizeof(symbol)/U_SIZEOF_UCHAR, &status);
        if(U_FAILURE(status)) {
            log_err("Error from unum_getSymbol(%d): %s\n", i, myErrorName(status));
            return;
        }
        if(resultlength != 2 || symbol[0] != 0x41 + i || symbol[1] != 0x61 + i) {
            log_err("Failure in unum_getSymbol(%d): got unexpected symbol\n", i);
        }
    }
    /*try getting from a bogus symbol*/
    unum_getSymbol(cur_frpattern, (UNumberFormatSymbol)i, symbol, sizeof(symbol)/U_SIZEOF_UCHAR, &status);
    if(U_SUCCESS(status)){
        log_err("Error : Expected U_ILLEGAL_ARGUMENT_ERROR for bogus symbol");
    }
    if(U_FAILURE(status)){
        if(status != U_ILLEGAL_ARGUMENT_ERROR){
            log_err("Error: Expected U_ILLEGAL_ARGUMENT_ERROR for bogus symbol, Got %s\n", myErrorName(status));
        }
    }
    status=U_ZERO_ERROR;

    /* Testing unum_getTextAttribute() and unum_setTextAttribute()*/
    log_verbose("\nTesting getting and setting text attributes\n");
    resultlength=5;
    unum_getTextAttribute(cur_fr, UNUM_NEGATIVE_SUFFIX, temp, resultlength, &status);
    if(U_FAILURE(status))
    {
        log_err("Failure in gettting the Text attributes of number format: %s\n", myErrorName(status));
    }
    unum_setTextAttribute(cur_def, UNUM_NEGATIVE_SUFFIX, temp, u_strlen(temp), &status);
    if(U_FAILURE(status))
    {
        log_err("Failure in gettting the Text attributes of number format: %s\n", myErrorName(status));
    }
    unum_getTextAttribute(cur_def, UNUM_NEGATIVE_SUFFIX, suffix, resultlength, &status);
    if(U_FAILURE(status))
    {
        log_err("Failure in gettting the Text attributes of number format: %s\n", myErrorName(status));
    }
    if(u_strcmp(suffix,temp)!=0)
        log_err("Fail:Error in setTextAttribute or getTextAttribute in setting and getting suffix\n");
    else
        log_verbose("Pass: setting and getting suffix works fine\n");
    /*set it back to normal */
    u_uastrcpy(temp,"$");
    unum_setTextAttribute(cur_def, UNUM_NEGATIVE_SUFFIX, temp, u_strlen(temp), &status);

    /*checking some more text setter conditions */
    u_uastrcpy(prefix, "+");
    unum_setTextAttribute(def, UNUM_POSITIVE_PREFIX, prefix, u_strlen(prefix) , &status);
    if(U_FAILURE(status))
    {
        log_err("error in setting the text attributes : %s\n", myErrorName(status));
    }
    unum_getTextAttribute(def, UNUM_POSITIVE_PREFIX, temp, resultlength, &status);
    if(U_FAILURE(status))
    {
        log_err("error in getting the text attributes : %s\n", myErrorName(status));
    }

    if(u_strcmp(prefix, temp)!=0) 
        log_err("ERROR: get and setTextAttributes with positive prefix failed\n");
    else
        log_verbose("Pass: get and setTextAttributes with positive prefix works fine\n");

    u_uastrcpy(prefix, "+");
    unum_setTextAttribute(def, UNUM_NEGATIVE_PREFIX, prefix, u_strlen(prefix), &status);
    if(U_FAILURE(status))
    {
        log_err("error in setting the text attributes : %s\n", myErrorName(status));
    }
    unum_getTextAttribute(def, UNUM_NEGATIVE_PREFIX, temp, resultlength, &status);
    if(U_FAILURE(status))
    {
        log_err("error in getting the text attributes : %s\n", myErrorName(status));
    }
    if(u_strcmp(prefix, temp)!=0) 
        log_err("ERROR: get and setTextAttributes with negative prefix failed\n");
    else
        log_verbose("Pass: get and setTextAttributes with negative prefix works fine\n");

    u_uastrcpy(suffix, "+");
    unum_setTextAttribute(def, UNUM_NEGATIVE_SUFFIX, suffix, u_strlen(suffix) , &status);
    if(U_FAILURE(status))
    {
        log_err("error in setting the text attributes: %s\n", myErrorName(status));
    }

    unum_getTextAttribute(def, UNUM_NEGATIVE_SUFFIX, temp, resultlength, &status);
    if(U_FAILURE(status))
    {
        log_err("error in getting the text attributes : %s\n", myErrorName(status));
    }
    if(u_strcmp(suffix, temp)!=0) 
        log_err("ERROR: get and setTextAttributes with negative suffix failed\n");
    else
        log_verbose("Pass: get and settextAttributes with negative suffix works fine\n");

    u_uastrcpy(suffix, "++");
    unum_setTextAttribute(def, UNUM_POSITIVE_SUFFIX, suffix, u_strlen(suffix) , &status);
    if(U_FAILURE(status))
    {
        log_err("error in setting the text attributes: %s\n", myErrorName(status));
    }

    unum_getTextAttribute(def, UNUM_POSITIVE_SUFFIX, temp, resultlength, &status);
    if(U_FAILURE(status))
    {
        log_err("error in getting the text attributes : %s\n", myErrorName(status));
    }
    if(u_strcmp(suffix, temp)!=0) 
        log_err("ERROR: get and setTextAttributes with negative suffix failed\n");
    else
        log_verbose("Pass: get and settextAttributes with negative suffix works fine\n");

    /*Testing unum_getAttribute and  unum_setAttribute() */
    log_verbose("\nTesting get and set Attributes\n");
    attr=UNUM_GROUPING_SIZE;
    newvalue=unum_getAttribute(def, attr);
    newvalue=2;
    unum_setAttribute(def, attr, newvalue);
    if(unum_getAttribute(def,attr)!=2)
        log_err("Fail: error in setting and getting attributes for UNUM_GROUPING_SIZE\n");
    else
        log_verbose("Pass: setting and getting attributes for UNUM_GROUPING_SIZE works fine\n");

    attr=UNUM_MULTIPLIER;
    newvalue=unum_getAttribute(def, attr);
    newvalue=8;
    unum_setAttribute(def, attr, newvalue);
    if(unum_getAttribute(def,attr) != 8)
        log_err("error in setting and getting attributes for UNUM_MULTIPLIER\n");
    else
        log_verbose("Pass:setting and getting attributes for UNUM_MULTIPLIER works fine\n");

    /*testing set and get Attributes extensively */
    log_verbose("\nTesting get and set attributes extensively\n");
    for(attr=UNUM_PARSE_INT_ONLY; attr<= UNUM_PADDING_POSITION; attr=(UNumberFormatAttribute)((int32_t)attr + 1) )
    {
        newvalue=unum_getAttribute(fr, attr);
        unum_setAttribute(def, attr, newvalue);
        if(unum_getAttribute(def,attr)!=unum_getAttribute(fr, attr))
            log_err("error in setting and getting attributes\n");
        else
            log_verbose("Pass: attributes set and retrieved successfully\n");
    }

    /*testing spellout format to make sure we can use it successfully.*/
    log_verbose("\nTesting spellout format\n");
    if (spellout_def)
    {
        static const int32_t values[] = { 0, -5, 105, 1005, 105050 };
        for (i = 0; i < LENGTH(values); ++i) {
            UChar buffer[128];
            int32_t len;
            int32_t value = values[i];
            status = U_ZERO_ERROR;
            len = unum_format(spellout_def, value, buffer, LENGTH(buffer), NULL, &status);
            if(U_FAILURE(status)) {
                log_err("Error in formatting using unum_format(spellout_fmt, ...): %s\n", myErrorName(status));
            } else {
                int32_t pp = 0;
                int32_t parseResult;
                char logbuf[256];
                ustrToAstr(buffer, len, logbuf, LENGTH(logbuf));
                log_verbose("formatted %d as '%s', length: %d\n", value, logbuf, len);

                parseResult = unum_parse(spellout_def, buffer, len, &pp, &status);
                if (U_FAILURE(status)) {
                    log_err("Error in parsing using unum_format(spellout_fmt, ...): %s\n", myErrorName(status));
                } else if (parseResult != value) {
                    log_err("unum_format result %d != value %d\n", parseResult, value);
                }
            }
        }
    }
    else {
        log_err("Spellout format is unavailable\n");
    }

    /*closing the NumberFormat() using unum_close(UNumberFormat*)")*/
    unum_close(def);
    unum_close(fr);
    unum_close(cur_def);
    unum_close(cur_fr);
    unum_close(per_def);
    unum_close(per_fr);
    unum_close(spellout_def);
    unum_close(pattern);
    unum_close(cur_frpattern);
    unum_close(myclone);

}

static void TestSignificantDigits()
{
    UChar temp[128];
    int32_t resultlengthneeded;
    int32_t resultlength;
    UErrorCode status = U_ZERO_ERROR;
    UChar *result = NULL;
    UNumberFormat* fmt;
    double d = 123456.789;

    u_uastrcpy(temp, "###0.0#");
    fmt=unum_open(UNUM_IGNORE, temp, -1, NULL, NULL,&status);
    if (U_FAILURE(status)) {
        log_err("got unexpected error for unum_open: '%s'\n", u_errorName(status));
    }
    unum_setAttribute(fmt, UNUM_SIGNIFICANT_DIGITS_USED, TRUE);
    unum_setAttribute(fmt, UNUM_MAX_SIGNIFICANT_DIGITS, 6);

    u_uastrcpy(temp, "123457");
    resultlength=0;
    resultlengthneeded=unum_formatDouble(fmt, d, NULL, resultlength, NULL, &status);
    if(status==U_BUFFER_OVERFLOW_ERROR)
    {
        status=U_ZERO_ERROR;
        resultlength=resultlengthneeded+1;
        result=(UChar*)malloc(sizeof(UChar) * resultlength);
        unum_formatDouble(fmt, d, result, resultlength, NULL, &status);
    }
    if(U_FAILURE(status))
    {
        log_err("Error in formatting using unum_formatDouble(.....): %s\n", myErrorName(status));
        return;
    }
    if(u_strcmp(result, temp)==0)
        log_verbose("Pass: Number Formatting using unum_formatDouble() Successful\n");
    else
        log_err("FAIL: Error in number formatting using unum_formatDouble()\n");
    free(result);
    unum_close(fmt);
}

static void TestNumberFormatPadding()
{
    UChar *result=NULL;
    UChar temp1[512];

    UErrorCode status=U_ZERO_ERROR;
    int32_t resultlength;
    int32_t resultlengthneeded;
    UNumberFormat *pattern;
    double d1;
    double d = -10456.37;
    UFieldPosition pos1;
    int32_t parsepos;

    /* create a number format using unum_openPattern(....)*/
    log_verbose("\nTesting unum_openPattern() with padding\n");
    u_uastrcpy(temp1, "*#,##0.0#*;(#,##0.0#)");
    status=U_ZERO_ERROR;
    pattern=unum_open(UNUM_IGNORE,temp1, u_strlen(temp1), NULL, NULL,&status);
    if(U_SUCCESS(status))
    {
        log_err("error in unum_openPattern(%s): %s\n", temp1, myErrorName(status) );;
    }
    else
    {
        unum_close(pattern);
    }

/*    u_uastrcpy(temp1, "*x#,###,###,##0.0#;(*x#,###,###,##0.0#)"); */
    u_uastrcpy(temp1, "*x#,###,###,##0.0#;*x(###,###,##0.0#)");
    status=U_ZERO_ERROR;
    pattern=unum_open(UNUM_IGNORE,temp1, u_strlen(temp1), "en_US",NULL, &status);
    if(U_FAILURE(status))
    {
        log_err("error in padding unum_openPattern(%s): %s\n", temp1, myErrorName(status) );;
    }
    else {
        log_verbose("Pass: padding unum_openPattern() works fine\n");

        /*test for unum_toPattern()*/
        log_verbose("\nTesting padding unum_toPattern()\n");
        resultlength=0;
        resultlengthneeded=unum_toPattern(pattern, FALSE, NULL, resultlength, &status);
        if(status==U_BUFFER_OVERFLOW_ERROR)
        {
            status=U_ZERO_ERROR;
            resultlength=resultlengthneeded+1;
            result=(UChar*)malloc(sizeof(UChar) * resultlength);
            unum_toPattern(pattern, FALSE, result, resultlength, &status);
        }
        if(U_FAILURE(status))
        {
            log_err("error in extracting the padding pattern from UNumberFormat: %s\n", myErrorName(status));
        }
        else
        {
            if(u_strcmp(result, temp1)!=0)
                log_err("FAIL: Error in extracting the padding pattern using unum_toPattern()\n");
            else
                log_verbose("Pass: extracted the padding pattern correctly using unum_toPattern()\n");
free(result);
        }
/*        u_uastrcpy(temp1, "(xxxxxxx10,456.37)"); */
        u_uastrcpy(temp1, "xxxxx(10,456.37)");
        resultlength=0;
        pos1.field = 1; /* Fraction field */
        resultlengthneeded=unum_formatDouble(pattern, d, NULL, resultlength, &pos1, &status);
        if(status==U_BUFFER_OVERFLOW_ERROR)
        {
            status=U_ZERO_ERROR;
            resultlength=resultlengthneeded+1;
            result=(UChar*)malloc(sizeof(UChar) * resultlength);
            unum_formatDouble(pattern, d, result, resultlength, NULL, &status);
        }
        if(U_FAILURE(status))
        {
            log_err("Error in formatting using unum_formatDouble(.....) with padding : %s\n", myErrorName(status));
        }
        else
        {
            if(u_strcmp(result, temp1)==0)
                log_verbose("Pass: Number Formatting using unum_formatDouble() padding Successful\n");
            else
                log_err("FAIL: Error in number formatting using unum_formatDouble() with padding\n");
            if(pos1.beginIndex == 13 && pos1.endIndex == 15)
                log_verbose("Pass: Complete number formatting using unum_formatDouble() successful\n");
            else
                log_err("Fail: Error in complete number Formatting using unum_formatDouble()\nGot: b=%d end=%d\nExpected: b=13 end=15\n",
                        pos1.beginIndex, pos1.endIndex);


            /* Testing unum_parse() and unum_parseDouble() */
            log_verbose("\nTesting padding unum_parseDouble()\n");
            parsepos=0;
            d1=unum_parseDouble(pattern, result, u_strlen(result), &parsepos, &status);
            if(U_FAILURE(status))
            {
                log_err("padding parse failed. The error is : %s\n", myErrorName(status));
            }

            if(d1!=d)
                log_err("Fail: Error in padding parsing\n");
            else
                log_verbose("Pass: padding parsing successful\n");
free(result);
        }
    }

    unum_close(pattern);
}

static UBool
withinErr(double a, double b, double err) {
    return uprv_fabs(a - b) < uprv_fabs(a * err); 
}

static void TestInt64Format() {
    UChar temp1[512];
    UChar result[512];
    UNumberFormat *fmt;
    UErrorCode status = U_ZERO_ERROR;
    const double doubleInt64Max = (double)U_INT64_MAX;
    const double doubleInt64Min = (double)U_INT64_MIN;
    const double doubleBig = 10.0 * (double)U_INT64_MAX;      
    int32_t val32;
    int64_t val64;
    double  valDouble;
    int32_t parsepos;

    /* create a number format using unum_openPattern(....) */
    log_verbose("\nTesting Int64Format\n");
    u_uastrcpy(temp1, "#.#E0");
    fmt = unum_open(UNUM_IGNORE, temp1, u_strlen(temp1), NULL, NULL, &status);
    if(U_FAILURE(status)) {
        log_err("error in unum_openPattern(): %s\n", myErrorName(status));
    } else {
        unum_setAttribute(fmt, UNUM_MAX_FRACTION_DIGITS, 20);
        unum_formatInt64(fmt, U_INT64_MAX, result, 512, NULL, &status);
        if (U_FAILURE(status)) {
            log_err("error in unum_format(): %s\n", myErrorName(status));
        } else {
            log_verbose("format int64max: '%s'\n", result);
            parsepos = 0;
            val32 = unum_parse(fmt, result, u_strlen(result), &parsepos, &status);
            if (status != U_INVALID_FORMAT_ERROR) {
                log_err("parse didn't report error: %s\n", myErrorName(status));
            } else if (val32 != INT32_MAX) {
                log_err("parse didn't pin return value, got: %d\n", val32);
            }

            status = U_ZERO_ERROR;
            parsepos = 0;
            val64 = unum_parseInt64(fmt, result, u_strlen(result), &parsepos, &status);
            if (U_FAILURE(status)) {
                log_err("parseInt64 returned error: %s\n", myErrorName(status));
            } else if (val64 != U_INT64_MAX) {
                log_err("parseInt64 returned incorrect value, got: %ld\n", val64);
            }

            status = U_ZERO_ERROR;
            parsepos = 0;
            valDouble = unum_parseDouble(fmt, result, u_strlen(result), &parsepos, &status);
            if (U_FAILURE(status)) {
                log_err("parseDouble returned error: %s\n", myErrorName(status));
            } else if (valDouble != doubleInt64Max) {
                log_err("parseDouble returned incorrect value, got: %g\n", valDouble);
            }
        }

        unum_formatInt64(fmt, U_INT64_MIN, result, 512, NULL, &status);
        if (U_FAILURE(status)) {
            log_err("error in unum_format(): %s\n", myErrorName(status));
        } else {
            log_verbose("format int64min: '%s'\n", result);
            parsepos = 0;
            val32 = unum_parse(fmt, result, u_strlen(result), &parsepos, &status);
            if (status != U_INVALID_FORMAT_ERROR) {
                log_err("parse didn't report error: %s\n", myErrorName(status));
            } else if (val32 != INT32_MIN) {
                log_err("parse didn't pin return value, got: %d\n", val32);
            }

            status = U_ZERO_ERROR;
            parsepos = 0;
            val64 = unum_parseInt64(fmt, result, u_strlen(result), &parsepos, &status);
            if (U_FAILURE(status)) {
                log_err("parseInt64 returned error: %s\n", myErrorName(status));
            } else if (val64 != U_INT64_MIN) {
                log_err("parseInt64 returned incorrect value, got: %ld\n", val64);
            }

            status = U_ZERO_ERROR;
            parsepos = 0;
            valDouble = unum_parseDouble(fmt, result, u_strlen(result), &parsepos, &status);
            if (U_FAILURE(status)) {
                log_err("parseDouble returned error: %s\n", myErrorName(status));
            } else if (valDouble != doubleInt64Min) {
                log_err("parseDouble returned incorrect value, got: %g\n", valDouble);
            }
        }

        unum_formatDouble(fmt, doubleBig, result, 512, NULL, &status);
        if (U_FAILURE(status)) {
            log_err("error in unum_format(): %s\n", myErrorName(status));
        } else {
            log_verbose("format doubleBig: '%s'\n", result);
            parsepos = 0;
            val32 = unum_parse(fmt, result, u_strlen(result), &parsepos, &status);
            if (status != U_INVALID_FORMAT_ERROR) {
                log_err("parse didn't report error: %s\n", myErrorName(status));
            } else if (val32 != INT32_MAX) {
                log_err("parse didn't pin return value, got: %d\n", val32);
            }

            status = U_ZERO_ERROR;
            parsepos = 0;
            val64 = unum_parseInt64(fmt, result, u_strlen(result), &parsepos, &status);
            if (status != U_INVALID_FORMAT_ERROR) {
                log_err("parseInt64 didn't report error error: %s\n", myErrorName(status));
            } else if (val64 != U_INT64_MAX) {
                log_err("parseInt64 returned incorrect value, got: %ld\n", val64);
            }

            status = U_ZERO_ERROR;
            parsepos = 0;
            valDouble = unum_parseDouble(fmt, result, u_strlen(result), &parsepos, &status);
            if (U_FAILURE(status)) {
                log_err("parseDouble returned error: %s\n", myErrorName(status));
            } else if (!withinErr(valDouble, doubleBig, 1e-15)) {
                log_err("parseDouble returned incorrect value, got: %g\n", valDouble);
            }
        }
    }
    unum_close(fmt);
}


static void test_fmt(UNumberFormat* fmt, UBool isDecimal) {
    char temp[512];
    UChar buffer[512];
    int32_t BUFSIZE = sizeof(buffer)/sizeof(buffer[0]);
    double vals[] = {
        -.2, 0, .2, 5.5, 15.2, 250, 123456789
    };
    int i;

    for (i = 0; i < sizeof(vals)/sizeof(vals[0]); ++i) {
        UErrorCode status = U_ZERO_ERROR;
        unum_formatDouble(fmt, vals[i], buffer, BUFSIZE, NULL, &status);
        if (U_FAILURE(status)) {
            log_err("failed to format: %g, returned %s\n", vals[i], u_errorName(status));
        } else {
            u_austrcpy(temp, buffer);
            log_verbose("formatting %g returned '%s'\n", vals[i], temp);
        }
    }

    /* check APIs now */
    {
        UErrorCode status = U_ZERO_ERROR;
        UParseError perr;
        u_uastrcpy(buffer, "#,##0.0#");
        unum_applyPattern(fmt, FALSE, buffer, -1, &perr, &status);
        if (isDecimal ? U_FAILURE(status) : (status != U_UNSUPPORTED_ERROR)) {
            log_err("got unexpected error for applyPattern: '%s'\n", u_errorName(status));
        }
    }

    {
        int isLenient = unum_getAttribute(fmt, UNUM_LENIENT_PARSE);
        log_verbose("lenient: 0x%x\n", isLenient);
        if (isDecimal ? (isLenient != -1) : (isLenient == TRUE)) {
            log_err("didn't expect lenient value: %d\n", isLenient);
        }

        unum_setAttribute(fmt, UNUM_LENIENT_PARSE, TRUE);
        isLenient = unum_getAttribute(fmt, UNUM_LENIENT_PARSE);
        if (isDecimal ? (isLenient != -1) : (isLenient == FALSE)) {
            log_err("didn't expect lenient value after set: %d\n", isLenient);
        }
    }

    {
        double val2;
        double val = unum_getDoubleAttribute(fmt, UNUM_LENIENT_PARSE);
        if (val != -1) {
            log_err("didn't expect double attribute\n");
        }
        val = unum_getDoubleAttribute(fmt, UNUM_ROUNDING_INCREMENT);
        if ((val == -1) == isDecimal) {
            log_err("didn't expect -1 rounding increment\n");
        }
        unum_setDoubleAttribute(fmt, UNUM_ROUNDING_INCREMENT, val+.5);
        val2 = unum_getDoubleAttribute(fmt, UNUM_ROUNDING_INCREMENT);
        if (isDecimal && (val2 - val != .5)) {
            log_err("set rounding increment had no effect on decimal format");
        }
    }

    {
        UErrorCode status = U_ZERO_ERROR;
        int len = unum_getTextAttribute(fmt, UNUM_DEFAULT_RULESET, buffer, BUFSIZE, &status);
        if (isDecimal ? (status != U_UNSUPPORTED_ERROR) : U_FAILURE(status)) {
            log_err("got unexpected error for get default ruleset: '%s'\n", u_errorName(status));
        }
        if (U_SUCCESS(status)) {
            u_austrcpy(temp, buffer);
            log_verbose("default ruleset: '%s'\n", temp);
        }

        status = U_ZERO_ERROR;
        len = unum_getTextAttribute(fmt, UNUM_PUBLIC_RULESETS, buffer, BUFSIZE, &status);
        if (isDecimal ? (status != U_UNSUPPORTED_ERROR) : U_FAILURE(status)) {
            log_err("got unexpected error for get public rulesets: '%s'\n", u_errorName(status));
        }
        if (U_SUCCESS(status)) {
            u_austrcpy(temp, buffer);
            log_verbose("public rulesets: '%s'\n", temp);

            /* set the default ruleset to the first one found, and retry */

            if (len > 0) {
                for (i = 0; i < len && temp[i] != ';'; ++i){};
                if (i < len) {
                    buffer[i] = 0;
                    unum_setTextAttribute(fmt, UNUM_DEFAULT_RULESET, buffer, -1, &status);
                    if (U_FAILURE(status)) {
                        log_err("unexpected error setting default ruleset: '%s'\n", u_errorName(status));
                    } else {
                        int len2 = unum_getTextAttribute(fmt, UNUM_DEFAULT_RULESET, buffer, BUFSIZE, &status);
                        if (U_FAILURE(status)) {
                            log_err("could not fetch default ruleset: '%s'\n", u_errorName(status));
                        } else if (len2 != i) {
                            u_austrcpy(temp, buffer);
                            log_err("unexpected ruleset len: %d ex: %d val: %s\n", len2, i, temp);
                        } else {
                            for (i = 0; i < sizeof(vals)/sizeof(vals[0]); ++i) {
                                status = U_ZERO_ERROR;
                                unum_formatDouble(fmt, vals[i], buffer, BUFSIZE, NULL, &status);
                                if (U_FAILURE(status)) {
                                    log_err("failed to format: %g, returned %s\n", vals[i], u_errorName(status));
                                } else {
                                    u_austrcpy(temp, buffer);
                                    log_verbose("formatting %g returned '%s'\n", vals[i], temp);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    {
        UErrorCode status = U_ZERO_ERROR;
        unum_toPattern(fmt, FALSE, buffer, BUFSIZE, &status);
        if (U_SUCCESS(status)) {
            u_austrcpy(temp, buffer);
            log_verbose("pattern: '%s'\n", temp);
        } else if (status != U_BUFFER_OVERFLOW_ERROR) {
            log_err("toPattern failed unexpectedly: %s\n", u_errorName(status));
        } else {
            log_verbose("pattern too long to display\n");
        }
    }

    {
        UErrorCode status = U_ZERO_ERROR;
        int len = unum_getSymbol(fmt, UNUM_CURRENCY_SYMBOL, buffer, BUFSIZE, &status);
        if (isDecimal ? U_FAILURE(status) : (status != U_UNSUPPORTED_ERROR)) {
            log_err("unexpected error getting symbol: '%s'\n", u_errorName(status));
        }

        unum_setSymbol(fmt, UNUM_CURRENCY_SYMBOL, buffer, len, &status);
        if (isDecimal ? U_FAILURE(status) : (status != U_UNSUPPORTED_ERROR)) {
            log_err("unexpected error setting symbol: '%s'\n", u_errorName(status));
        }
    }
}

static void TestRBNFFormat() {
    UErrorCode status;
    UParseError perr;
    UChar pat[1024];
    UChar tempUChars[512];
    UNumberFormat *formats[5];
    int COUNT = sizeof(formats)/sizeof(formats[0]);
    int i;

    for (i = 0; i < COUNT; ++i) {
        formats[i] = 0;
    }

    /* instantiation */
    status = U_ZERO_ERROR;
    u_uastrcpy(pat, "#,##0.0#;(#,##0.0#)");
    formats[0] = unum_open(UNUM_PATTERN_DECIMAL, pat, -1, "en_US", &perr, &status);
    if (U_FAILURE(status)) {
        log_err("unable to open decimal pattern");
    }

    status = U_ZERO_ERROR;
    formats[1] = unum_open(UNUM_SPELLOUT, NULL, 0, "en_US", &perr, &status);
    if (U_FAILURE(status)) {
        log_err("unable to open spellout");
    }

    status = U_ZERO_ERROR;
    formats[2] = unum_open(UNUM_ORDINAL, NULL, 0, "en_US", &perr, &status);
    if (U_FAILURE(status)) {
        log_err("unable to open ordinal");
    }

    status = U_ZERO_ERROR;
    formats[3] = unum_open(UNUM_DURATION, NULL, 0, "en_US", &perr, &status);
    if (U_FAILURE(status)) {
        log_err("unable to open duration");
    }

    status = U_ZERO_ERROR;
    u_uastrcpy(pat,
        "%standard:\n"
        "-x: minus >>;\n"
        "x.x: << point >>;\n"
        "zero; one; two; three; four; five; six; seven; eight; nine;\n"
        "ten; eleven; twelve; thirteen; fourteen; fifteen; sixteen;\n"
        "seventeen; eighteen; nineteen;\n"
        "20: twenty[->>];\n"
        "30: thirty[->>];\n"
        "40: forty[->>];\n"
        "50: fifty[->>];\n"
        "60: sixty[->>];\n"
        "70: seventy[->>];\n"
        "80: eighty[->>];\n"
        "90: ninety[->>];\n"
        "100: =#,##0=;\n");
    u_uastrcpy(tempUChars,
        "%simple:\n"
        "=%standard=;\n"
        "20: twenty[ and change];\n"
        "30: thirty[ and change];\n"
        "40: forty[ and change];\n"
        "50: fifty[ and change];\n"
        "60: sixty[ and change];\n"
        "70: seventy[ and change];\n"
        "80: eighty[ and change];\n"
        "90: ninety[ and change];\n"
        "100: =#,##0=;\n"
        "%bogus:\n"
        "0.x: tiny;\n"
        "x.x: << point something;\n"
        "=%standard=;\n"
        "20: some reasonable number;\n"
        "100: some substantial number;\n"
        "100,000,000: some huge number;\n");
    /* This is to get around some compiler warnings about char * string length. */
    u_strcat(pat, tempUChars);
    formats[4] = unum_open(UNUM_PATTERN_RULEBASED, pat, -1, "en_US", &perr, &status);
    if (U_FAILURE(status)) {
        log_err("unable to open rulebased pattern");
    }

    for (i = 0; i < COUNT; ++i) {
        log_verbose("\n\ntesting format %d\n", i);
        test_fmt(formats[i], (UBool)(i == 0));
    }

    for (i = 0; i < COUNT; ++i) {
        unum_close(formats[i]);
    }
}

#endif /* #if !UCONFIG_NO_FORMATTING */
