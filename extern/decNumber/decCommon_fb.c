/*
 *      PROGRAM:        JRD Access Method
 *      MODULE:         cvt.cpp
 *      DESCRIPTION:    Data mover and converter and comparator, etc.
 *
 * The contents of this file are subject to the Interbase Public
 * License Version 1.0 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy
 * of the License at http://www.Inprise.com/IPL.html
 *
 * Software distributed under the License is distributed on an
 * "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express
 * or implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code was created by Inprise Corporation
 * and its predecessors. Portions created by Inprise Corporation are
 * Copyright (C) Inprise Corporation.
 *
 * All Rights Reserved.
 * Contributor(s): ______________________________________.
*/

//////////////////////////////////////////////////////////////////////////

static Flag decBiStr_fb(const char*       targ,
                        const char* const targ_e,
                        const char*       str1,
                        const char*       str2)
{
 for (;;targ++, str1++, str2++)
 {
  if (*str1==0)
  {
   if (targ==targ_e)
    return 1;

   return 0;
  }

  if (targ==targ_e)
    return 0;

  if (*targ!=*str1 && *targ!=*str2)
   return 0;
 } // forever
} // decBiStr_fb

//////////////////////////////////////////////////////////////////////////

decFloat* decFloatFromString_fb(decFloat*   result,
                                const char* string,
                                size_t      stringLen,
                                decContext* set)
{
 #define MAKE_SPTR(p)   (p)
 #define GET_PTR(p)     (p)

 #define GET_CH(pCH)    (*(pCH))

 #define LOCAL_UTILS__is_digit(ch)         ((ch)>='0' && (ch)<='9')
 #define LOCAL_UTILS__make_digit(ch)       ((uByte)(ch-'0'))
 #define LOCAL_UTILS__decBiChar(ch,v1,v2)  ((ch)==(v1) || (ch)==(v2))

 //----------------------------------------------------------------------
 uInt uiwork; // for macroses

 const char* const string_e=string+stringLen;

 uByte buffer[ROUNDUP(DECSTRING+11, 8)];

 bcdnum num;                          // collects data for finishing

 uInt error=DEC_Conversion_syntax;    // assume the worst

 //----------------------------------------------------------------------
 num.sign=0;                          // assume non-negative
 num.msd=buffer;                      // MSD is here always

 //----------------------------------------------------------------------
 #if DECTRACE
 // printf("FromString %s ...\n", string);
 #endif

 for(; string!=string_e;)
 {                                    // once-only 'loop'
  const char* dotchar=NULL;           // where dot was found [NULL if none]

  const char* cfirst=string;          // -> first character of decimal part
  const char* c;
  size_t      digits2;

  //---------------------------------------
  // first in string...

  switch(GET_CH(cfirst))
  {
   case '-':
   {                  // valid - sign
    ++cfirst;
    num.sign=DECFLOAT_Sign;
    break;
   }

   case '+':
   {                  // valid + sign
    ++cfirst;
    break;
   }
  }//switch

  // detect and validate the coefficient, including any leading,
  // trailing, or embedded '.'
  // [could test four-at-a-time here (saving 10% for decQuads),
  // but that risks storage violation because the position of the
  // terminator is unknown]

  c=cfirst;

  for(; c!=string_e; ++c)
  {
   if(LOCAL_UTILS__is_digit(GET_CH(c)))
    continue;                          // '0' through '9' is good

   if(GET_CH(c)=='.')
   {
    if(dotchar!=NULL)
     break;                            // not first '.'

    dotchar=c;                         // record offset into decimal part
    continue;
   }

   // GET_CH(c) is not a digit, terminator, or a valid +, -, or '.'
   break;
  } // c loop

  digits2=(size_t)(c-cfirst);                        // digits (+1 if a dot)

  if(digits2>0)
  {                                                  // had digits and/or dot
   const char* const cend=c;

   Int exp=0;                                        // exponent accumulator

   if(c!=string_e)
   {
    // something follows the coefficient
    // had some digits and more to come; expect E[+|-]nnn now

    if(!LOCAL_UTILS__decBiChar(GET_CH(c),'E','e'))
     break;

    ++c;                                             // to (optional) sign

    if(c==string_e) break;                           // no digits!  (e.g., '1.2E')

    {
     unsigned exp_is_neg=0;

     switch(GET_CH(c))                                // step over sign
     {
      case '-': { exp_is_neg=1; ++c; break; }
      case '+': { ++c; break; }
     }

     if(c==string_e) break;                           // no digits!  (e.g., '1.2E')

     for(; c!=string_e && GET_CH(c)=='0';) ++c;       // skip leading zeros [even last]

     {
      const char* const firstexp=c;                           // remember start [maybe '\0']

      for(;c!=string_e;++c)
      {
       // gather exponent digits

       const char ch=GET_CH(c);

       if(!LOCAL_UTILS__is_digit(ch))
        break;

       exp=exp*10+LOCAL_UTILS__make_digit(ch);
      }//for

      // if not now on the '\0', GET_CH(c) must not be a digit
      if(c!=string_e) break;

      // (this next test must be after the syntax checks)
      // if definitely more than the possible digits for format then
      // the exponent may have wrapped, so simply set it to a certain
      // over/underflow value

      if(DECEMAXD<(c-firstexp))
       exp=DECEMAX*2;
     } // local

     if(exp_is_neg)
      exp=-exp;                       // was negative
    } // local
   } //if exponent part

   if(dotchar!=NULL)
   {                                 // had a '.'
    --digits2;                       // remove from digits count

    if(digits2==0) break;            // was dot alone: bad syntax

    exp-=(Int)(cend-dotchar-1);      // adjust exponent

    // [the '.' can now be ignored]
   }//if

   num.exponent=exp;                 // exponent is good; store it

   // Here when whole string has been inspected and syntax is good
   // cfirst->first digit or dot, clast->last digit or dot
   error=0;                          // no error possible now

   // if the number of digits in the coefficient will fit in buffer
   // then it can simply be converted to bcd8 and copied -- decFinalize
   // will take care of leading zeros and rounding; the buffer is big
   // enough for all canonical coefficients, including 0.00000nn...
   {
    uByte* ub=buffer;

    if(digits2<=(sizeof(buffer)-3))
    {
     // [-3 allows by-4s copy]
     c=cfirst;

     if(dotchar!=NULL)
     {                                        // a dot to worry about
      if(GET_CH(c+1)=='.')
      {                                       // common canonical case
       *ub=LOCAL_UTILS__make_digit(GET_CH(c));   // copy leading digit

       ++ub;

       c+=2;                                  // prepare to handle rest
      }
      else
      for(;c!=cend;)
      {                                       // '.' could be anywhere
       // as usual, go by fours when safe; NB it has been asserted
       // that a '.' does not have the same mask as a digit
       if((cend-c)>=4)                             // safe for four
       {
        if((UBTOUI(c)&0xf0f0f0f0)==CHARMASK)
        {                                         // test four
         UBFROMUI(ub,UBTOUI(c)&0x0f0f0f0f);     // to BCD8

         ub+=4;

         c+=4;

         continue;
        }//if
       }//if

       if(GET_CH(c)=='.')
       {                               // found the dot
        ++c;                           // step over it ..
        break;                         // .. and handle the rest
       }

       *ub=LOCAL_UTILS__make_digit(GET_CH(c));

       ++ub;

       ++c;
      }//for
     } // if had dot

     // Now no dot; do this by fours (where safe)
     for(; 4<=(cend-c); c+=4,ub+=4)
     {
      UBFROMUI(ub,UBTOUI(c)&0x0f0f0f0f);
     }

     for(; c!=cend; ++c,++ub)
     {
      *ub=LOCAL_UTILS__make_digit(GET_CH(c));
     }

     num.lsd=buffer+(digits2-1);    // record new LSD
    } // fits
    else                                    // too long for buffer
    {
     // [This is a rare and unusual case; arbitrary-length input]
     // strip leading zeros [but leave final 0 if all 0's]

     {
      const char* cfirst2=NULL;

      for(; cfirst!=cend; ++cfirst)
      {
       if(GET_CH(cfirst)=='.')
        continue;                           // [ignore]

       cfirst2=cfirst;

       if(GET_CH(cfirst)!='0')
        break;                              // non-zero found [done]

       --digits2;                           // 0 stripped
      } // for cfirst

      if(cfirst==cend)
      {
       digits2=1;
      }

      cfirst=cfirst2;
     } //for at least one leading 0

     // the coefficient is now as short as possible, but may still
     // be too long; copy up to Pmax+1 digits to the buffer, then
     // just record any non-zeros (set round-for-reround digit)

     c=cfirst;

     for(; c!=cend && ub<=(buffer+DECPMAX);)
     {
      // (see commentary just above)
      if((cend-c)>=4)                       // safe for four
      {
       if((UBTOUI(c)&0xf0f0f0f0)==CHARMASK)
       {
        // four digits
        UBFROMUI(ub,UBTOUI(c)&0x0f0f0f0f);  // to BCD8

        ub+=4;
        c+=4;

        continue;
       }//if
      }//if

      if(GET_CH(c)=='.')
      {
       ++c;
       continue;                            // [ignore]
      }

      *ub=LOCAL_UTILS__make_digit(GET_CH(c));

      ++ub;
      ++c;
     }//for

     --ub;                              // -> LSD

     for(; c!=cend; ++c)
     {                                  // inspect remaining chars
      switch(GET_CH(c))
      {
       case '0': continue;

       case '.': continue;              // [ignore]
      }//switch
                                        // sticky bit needed

      *ub=DECSTICKYTAB[*ub];            // update round-for-reround

      break;                            // no need to look at more
     }//for

     num.lsd=ub;                        // record LSD

     // adjust exponent for dropped digits
     num.exponent+=(Int)(digits2)-(Int)(ub-buffer+1);
    }// else - too long for buffer
   }// local
  }// digits and/or dot
  else
  {
   // no digits or dot were found

   // only Infinities and NaNs are allowed, here

   if(c==string_e) break;            // nothing there is bad

   buffer[0]=0;                      // default a coefficient of 0

   num.lsd=buffer;                   // ..

   if(decBiStr_fb(c,string_e,"infinity","INFINITY"))
   {
    num.exponent=DECFLOAT_Inf;
   }
   else
   if(decBiStr_fb(c,string_e,"inf","INF"))
   {
    num.exponent=DECFLOAT_Inf;
   }
   else
   {                               // should be a NaN
    if(LOCAL_UTILS__decBiChar(GET_CH(c),'s','S'))
    {
     // probably an sNaN
     num.exponent=DECFLOAT_sNaN;   // effect the 's'

     ++c;                          // and step over it
    }
    else
    {
     num.exponent=DECFLOAT_qNaN;   // assume quiet NaN
    }

    if((c==string_e) || !LOCAL_UTILS__decBiChar(GET_CH(c),'N','n'))
     break;  // check caseless "NaN"

    ++c;

    if((c==string_e) || !LOCAL_UTILS__decBiChar(GET_CH(c),'a','A'))
     break;  // ..

    ++c;

    if((c==string_e) || !LOCAL_UTILS__decBiChar(GET_CH(c),'N','n'))
     break;  // ..

    ++c;

    // now either nothing, or nnnn payload (no dots), expected
    // -> start of integer, and skip leading 0s [including plain 0]
    for(cfirst=c; cfirst!=string_e && GET_CH(cfirst)=='0';)
    {
     ++cfirst;
    }

    if(cfirst!=string_e)
    {
     // not empty or all-0, payload

     // payload found; check all valid digits and copy to buffer as bcd8
     uByte* ub=buffer;

     for(c=cfirst;c!=string_e; ++c,++ub)
     {
      if(!LOCAL_UTILS__is_digit(GET_CH(c)))
       break;                        // quit if not 0-9

      if(c-cfirst==DECPMAX-1) break; // too many digits

      *ub=LOCAL_UTILS__make_digit(GET_CH(c));    // good bcd8
     }

     if(c!=string_e) break;          // not all digits, or too many

     num.lsd=ub-1;                   // record new LSD
    } // if
   } // NaN or sNaN

   error=0;                          // syntax is OK
  } // digits=0 (special expected)

  break;                             // drop out
 }                                   // [for(;;) once-loop]

 // decShowNum(&num, "fromStr");

 if(error!=0)
 {
  set->status|=error;
  num.exponent=DECFLOAT_qNaN;         // set up quiet NaN
  num.sign=0;                         // .. with 0 sign
  buffer[0]=0;                        // .. and coefficient
  num.lsd=buffer;                     // ..
  // decShowNum(&num, "oops");
 }

 // decShowNum(&num, "dffs");
 decFinalize(result,&num,set);       // round, check, and lay out
 // decFloatShow(result, "fromString");
 return result;

#undef LOCAL_UTILS__decBiChar
#undef LOCAL_UTILS__make_digit
#undef LOCAL_UTILS__is_digit
#undef GET_CH
} // decFloatFromString_fb

//////////////////////////////////////////////////////////////////////////
