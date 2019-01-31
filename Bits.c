/* 
 * CS:APP Data Lab 
 * 
 * <Oceane Andreis>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
/* We do not support C11 <threads.h>.  */
/* 
 * De Morgan's Law= ~(6|5)= (~6 & ~5) = -8
 * Hence ~(-8) = 7 (Used Bitwise Cmd0)
 * bitOr - x|y using only ~ and & 
 *   Example: bitOr(6, 5) = 7
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitOr(int x, int y) {
  return ~(~x & ~y);
}
/* 
 * evenBits - return word with all even-numbered bits set to 1
 * Used hint from recitation.
 * 0x55= 01010101
 * So I wanted 32 bits to be 01010101 hence shifted 8 then added the 8
 * and then shift it 16 and then add the previous.
 * Used hint from recitation.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */
int evenBits(void) {
    int x = 0x55;
    x=x<<8|x;
  return x<<16|x;
}
/* 
 * minusOne - return a value of -1 
 * -1 = 11111111111111111111111111111111
 * The only way to go from all zero to all 1 was to use ~
 *
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 2
 *   Rating: 1
 */
int minusOne(void) {
  return ~(0);
}
/* 
 * allEvenBits - return 1 if all even-numbered bits in word set to 1
 *  first= 0xaaaaaaaa
 *   In the case of 0xfffffffe = 11111111111111111111111111111110     where 
 *   the first even bit is 0 hence we need to return 0
 *  At first I tried to do the same as anyoddbit but instead of & 
 *   it was | but it didn't work . 
 *   I needed to turn everything into zero
 *   Examples allEvenBits(0xFFFFFFFE) = 0, allEvenBits(0x55555555) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allEvenBits(int x) {
    int first=0xaa; //10101010
    int mask;
    first= first<<8 |first;
    first= first <<16 | first; //0xaaaaaaaa
    mask= x | first; // equal to x = -0x2
    mask=~mask; // 0x1 (every bit is 0 except the first one that's 1)
  return !(mask);// hence we want to return all zero by using !
}
/* 
 * anyOddBit - return 1 if any odd-numbered bit in word set to 1
 * 0xaa= 10101010 . 0x5= 00000101
 * !(0)=1 !(1)=0 but !(anything else) =0 and ~ flips all the bits.
 * when you do 0x5 & 0xaaaaaaaa=0 however it didn't work to just leave 
 *   mask but we wanted 0 hence !! 
 *   Examples anyOddBit(0x5) = 0, anyOddBit(0x7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int anyOddBit(int x) {
    int first=0xaa; //10101010
    int mask;
    first= first<<8 |first;
    first= first <<16 | first; //0xaaaaaaaa
    mask= x & first; //0x5 & first = 0 
  return !!(mask); //just returning mask would work for this scenario but not all scenario
}
/* 
 * byteSwap - swaps the nth byte and the mth byte
 * Had to declare the variable above everything otherwise code wouldn't
 * run.
 * 0x12345678= 00010010 00110100 01010110 01111000
 * 0xff = 00000000 00000000 11111111
 * Let's take (0x12345678, 1, 3)
 * n=n<<3=8  -> multiple by 8 hence isolating nth and mth byte
 * m=m<<3=24
 * (x>>n)^(x>>m)  = 0x123456 ^ 0x12 = 0x123444 = 00010010 00110100
 * 01000100
 * holder is trying to isolate: 
 * 0x123444 & 0xff = 00000000 00000000 01000100 =holder= 0x44
 
 * holder<<n = 0x44<<8= 01000100 00000000 = 0x4400
 * holder<<m = 0x44<<24= 01000100 00000000 00000000 00000000 = 0x44000000
 
 *  x=x^(holder<<n)= 0x12341278 = 00010010 00110100 00010010 01111000
 *           00010010 00110100 01010110 01111000 ^ 01000100 00000000
 *            = 00010010 00110100 00010010 01111000
 *  x=x^(holder<<m)= 0x56341278 = 01010110 00110100 00010010 01111000
 *   = 00010010 00110100 00010010 01111000 ^ 01000100 00000000 00000000
 *   00000000          
 
 *  Examples: byteSwap(0x12345678, 1, 3) = 0x56341278
 *            byteSwap(0xDEADBEEF, 0, 2) = 0xDEEFBEAD
 *  You may assume that 0 <= n <= 3, 0 <= m <= 3
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 25
 *  Rating: 2

    
 */
int byteSwap(int x, int n, int m) {
   int holder;
    n=n<<3; //8
    m=m<<3; //24
    holder=0xff & ((x>>n)^(x>>m)); //0x44= isolate the 44 from 0x123444
    x=x^(holder<<n);// 0x12341278
    x=x^(holder<<m);//0x56341278
   
return x;
 
}
/* 
 * addOK - Determine if can compute x+y without overflow(Overflow when x
 * and y have same sign, but s is different Compute sum)
 *!Takes any nonzero input and changes it to 0, or it takes 0 and changes it to the value 
 * of 1. ~ flips each bit.
 * sum= 0x80000000+0x70000000 = -0x10000000  (or and add can be similar)
 * x^sum= 0x80000000^-0x10000000=0x70000000
 * y^sum= 0x70000000^-0x10000000=-0x80000000
 * samex&samey=0x70000000 & -0x80000000 = 0x0
 
 * 0x0>>31 = 0 (In the example were it's addOK(0x80000000,0x80000000) we need >>31 to have 1's everywhere and then we apply ! to turn it into 0)
 * !(0)=1
 *   Example: addOK(0x80000000,0x80000000) = 0,(same sign )(not overflow?)
 *            addOK(0x80000000,0x70000000) = 1, (different sign)(overflow?)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int addOK(int x, int y) {
    int sum = x+y; // -0x10000000
    int samex= x^sum; //check if they have the same sign(look at leading bit), + num
    int samey= y^sum;// negative num
    
  return !((samex&samey) >>31); //shifts samex&samey which is 0 but we need to return 1 hence the !
}
/* 
 * conditional - same as x ? y : z 
 * Used hint from recitation, 0xff=11111111
 *!(2) = 0
 * 0+~0x00 = 0 + -0x1 = 00000000000000000000000000000000 + 11111111111111111111111111111111 = 0xff = converting x to binary and adding one
 * Mask = 0xff=11111111
 * (~mask)&z = (~mask)& 5 = 0x0
 * (mask)&y = (mask)& 4 = 0x4
 * ((~mask)&z)|((mask)&y) = 0x4
 *if x was a nonzero number return y and if x was 0 return z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
    int mask= !x+~0x00; // 0xff= 11111111, !x= will turn any x in 0 except if x is 0
                        //^hence 0+1=1
  return ((~mask)&z)|((mask)&y); //(~mask)&z=0 because of the ~mask=0 
                                // 0 | anything = anything (in this case anything is 4)
}
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *  RANGE =  00110000 <= x <= 00111001
 
 *   0x0f=00001111
 *   0xf0 = 11110000
 *  ((0x0f & x)= 00001111 & 0x35 = 00001111 & 00110101 = 0x5 = 00000101
 *  0x5+0x6 = 0xb = 00001011
 *  range = 0xb & 0xf0= 0x0
 * 0x35>>4 = 0x3 = 00000011
 * Check = 0x3 ^ 0x3 = 0x0 = 00000000
 (0x0|0x0) = 0
 !(0)= 1 and yes 0x35 belongs to the range
 * Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
    int range = ((0x0f & x) + 0x6) & 0xf0; //0x0 (if it belongs to the range it will always equal 0)
    int check=(x>>4)^0x3; //(0x30>>4 = 0x3), 
    //every number btw 0x30 and 0x39 when shifted >>4 will equal 0x3
    //hence 0x3^0x3 = 0 (so if it belongs to the range check will equal 0)
  return !(range|check);//(if it belongs we will get 0|0 which is 0 but we want to return 1 hence the !)
}
/* 
 * replaceByte(x,n,c) - Replace byte n in x with c
 * 0xab=10101011
 * 0x12345678= 00010010 00110100 01010110 01111000
 *  0xff<<8 = 0xff00
 *  mask= ~0xff00= -0xff01
 *   c=c<<8 = 0xab<<8 = 10101011 00000000 = 0xab00
 *   0x12345678 & -0xff01 | 0xab00
 * = 0x1234ab78 = 00010010 00110100 10101011 01111000
 
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: replaceByte(0x12345678,1,0xab) = 0x1234ab78
 *   You can assume 0 <= n <= 3 and 0 <= c <= 255
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 3
 */
int replaceByte(int x, int n, int c) {
    int mask;
    n=n<<3; //multiply by 8 (when n=1)
    
    mask=0xff;//11111111
    mask=~(mask<<n); //~0xff00 = -0xff01
    c=c<<n; //0xab00(this represent the position where we want to add it)  (c= 0xab before)

    
  return (x & mask)|c; //0x12340078 |0xab00   (where we want to add became 00)
}
/* reverseBits - reverse the bits in a 32-bit integer,
              i.e. b0 swaps with b31, b1 with b30, etc
              example: 10101100 reverse bit -> 00110101
    My original code:
 *  x = ((x >> 1) & 0x55555555) | ((x << 1) & 0xaaaaaaaa);
    x = ((x >> 2) & 0x33333333) | ((x << 2) & 0xcccccccc);
    x = ((x >> 4) & 0x0f0f0f0f) | ((x << 4) & 0xf0f0f0f0);
    x = ((x >> 8) & 0x00ff00ff) | ((x << 8) & 0xff00ff00);
    x = ((x >> 16) & 0x0000ffff)| ((x << 16) & 0xffff0000);
    
   (everybyte<<8)|0xff = 0xff
 *  Examples: reverseBits(0x11111111) = 0x88888888
 *            reverseBits(0xdeadbeef) = 0xf77db57b
 *            reverseBits(0x88888888) = 0x11111111
 *            reverseBits(0)  = 0
 *            reverseBits(-1) = -1
 *            reverseBits(0x9) = 0x90000000
 *  Legal ops: ! ~ & ^ | + << >> and unsigned int type
 *  Max ops: 90
 *  Rating: 4
 * 
 */
int reverseBits(int x) {
    unsigned int x2 = x;
    int one = 0x55; //01010101
    int two = 0x33; //00110011
    int four = 0x0f; //00001111
    int everybyte = 0x00; //00000000
    
    one = (one<<8)|one;
    one = (one<<16)|one;//0x55555555
    
    two = (two<<8)|two;
    two = (two<<16)|two;//0x33333333
    
    four = (four<<8)|four;
    four = (four<<16)|four;//0x0f0f0f0f
    
    everybyte = (everybyte<<8)|0xff;
    everybyte = (everybyte<<16)|everybyte;//0xff00ff00
    
    
    x2 = ((x2 >>1) & one)|((x2 & one)<<1); //0x12345678->0x2138a9b4 (swap every other bit)
    x2 = ((x2 >>2) & two)|((x2 & two)<<2); //0x2138a9b4->-0x7b3d591f(swap each pair)  
    x2 = ((x2 >>4) & four)|((x2 & four)<<4); 
    x2 = ((x2 >>8) & everybyte)|((x2 & everybyte)<<8); 
    x2 = (x2 >>16)|(x2 <<16); 

  return x2;
}
/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          maximum possible value, and when negative overflow occurs,
 *          it returns minimum positive value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000(min)
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 *   Max value =1111 1111
 * example: 
 (x= 0x40000000 and y)sum = 0x80000000
 *, (sum^x)&(sum^y)=  -0x40000000 & -0x40000000
 *, check(1st)= -0x1
 *  sum>>check= -0x1 (straight 1)
 *  check<<31= -0x80000000=10000000000000000000000000000000
 * check(2nd)=  0x7fffffff  =01111111111111111111111111111111 
 */
int satAdd(int x, int y) {
    int sum= x+y; //check if + or -, 0x80000000
    int check=((sum^x)&(sum^y))>>31; //-0x1
    check=(sum>>check)^(check<<31);//0x7fffffff(max value)
  return check;
}
/*
 * Extra credit
 * Worked on the extra credit with Ngozi
 */
/* 
 * float_abs - Return bit-level equivalent of absolute value of f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument..
 
 * . min = minimum of not a number
 *   unsigned result = uf & mask = uf 
 * 0x7F800001=01111111100000000000000000000001
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_abs(unsigned uf) {
    
    unsigned mask = 0x7FFFFFFF; //max (from above function)
    unsigned min= 0x7F800003; // cannot be 0 for the first 3 byte
    unsigned result = uf & mask; // if it's small it'll equal uf
    if(result >=min)
    {
        return uf; //number given
    }
    else
    {
        return result; //if result is smaller than min
    }
    
    

}
/* 
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf) {
    /*
 
    
    */
    
  return 2;
}
/* 
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf) {
    
  return 2;
}
