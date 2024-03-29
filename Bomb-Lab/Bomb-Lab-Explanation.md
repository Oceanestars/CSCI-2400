# Bomb Lab:

WRITTEN BY: Oceane Andreis

This is a readme file that will hopefully provide you with helpful hints. This is a really fun assignment and it’s really cool once you get the hang of it.
I will attach the handout I received to this repository. 
First and foremost you need to get your own bomb through a server and every bomb is unique however sometimes some bombs have similar “logic” or “answer”.
Before you dive in the phases you need to have some debugging skills.
In this particular bomb I will be using GDB and I will also attach a nice GDB cheat sheet.  
**IMPORTANT: Don’t forget to set a breakpoint at explode_bomb.** 


# Phase 1:


You need to look at the function name(for most phases); it’ll give you a hint as to what the input 
might be.
In my case:
         ```
         0x0000000000400e36 <+9>:     callq  0x4012df <strings_not_equal>
         ```
I suggest you go through each line(ni in gdb) and print the registers( i r is the command in gdb) to see what’s going on(It’s a good habit to get into).
x/s $xxxxxx will print in the format of a string (and x/d print in the format of an integer) 
So Basically phase 1 is gonna compare your string with theirs. 
Fortunately, their string is being passed in a register right before string not equal.
Hence, I suggest you check(with the x/s command):
   ```     
   0x0000000000400e31 <+4>:     mov    $0x4023f0,%esi
   0x0000000000400e36 <+9>:     callq  0x4012df <strings_not_equal>
```



# Phase 2:


Similarly as phase 1, you want to look at the function name:

```
0x0000000000400e52 <+9>:     callq  0x4014ec <read_six_numbers>
```
Hence, your input should be 6 numbers.
You should pay attention to the compare instruction. One of the first one should give you your first number.
If your bomb is like mine(by the way, I’ve also shared my objdump in this repository) , then you might notice a loop. That loop will change your registers. Thus, go through the loop and print the register and see if you see a pattern. If you don’t then you might realize one of the line such as:
```
 0x0000000000400e74 <+43>:    add    (%rbx),%eax
 ```
Might be the line that creates the pattern of your 6 numbers.
This lines adds two number, (%rbx) is your previous number added to your current number. 




# Phase 3:


Phase 3 might seem overwhelming but I promise it’s not.
It’s a jump table/switch case. Basically, most of the code doesn’t matter. Only a small portion does. From now on, it might be helpful to use a disassembler(it’ll help you visualize things).
The key is your first input. Your first input will determine where it’ll jump. 


Anyway, you first want to check what kind of input they are expecting. Most likely before a function call they are moving a hex number into a register:

```
 0x0000000000400ea3 <+19>:    mov    $0x402446,%esi 
 ```
With the x/s $0x402446 command it printed this "%d %c %d" hence I need 3 inputs (d=integer c=characters).
After the function the assembly code will make sure you have the right amount of input.
So now we are trying to find the first input. 

```
 0x0000000000400ebc <+44>:    cmpl   $0x7,0xc(%rsp) //range 0-7 
 0x0000000000400ec1 <+49>:    ja     0x400fc3 <phase_3+307>//jump if above
 ```
(307 is explode bomb, hence if number higher than 7 it’ll explode)
So pick at number between your range, and then do ni till something like that:

        
        0x0000000000400ecb <+59>:    jmpq   *0x402460(,%rax,8)
       
After a line like that you will start jumping around
and see where it goes. From the line you are at there aren’t many line to go through till explode bomb. You’ll want to check your register. I had a char as my second input so I saw what was going into my eax register:

        
         0x0000000000400ef4 <+100>:   mov    $0x79,%eax //convert 0x79 into char
        
That was my second input and my third input was right after and I looked at what was being compared:

         0x0000000000400ef9 <+105>:   cmpl   $0x1ae,0x8(%rsp) //3rd thing,converted into decimal
         
Your bomb might just ask for two number, hence first number is a number within a certain range and second number will be a hex number to convert into a decimal.(Trial and error can be very useful) 


# Phase 4:


I was lucky and had an easy phase 4.
Like the previous phase, you want to check what kind of input they require(do same as phase 3)
You can go through the second function (func4). It’s a recursive function(it calls itself).
My first number was found by looking for compare statement and once again I had a range.

```
 0x000000000040102a <+34>:    cmpl   $0xe,0xc(%rsp)//range 0-14
 0x000000000040102f <+39>:    jbe    0x401036 <phase_4+46>  //jump below equal 
``` 
If you step in func4, you might encounter:

         400fe2:        d1 eb                        shr    %ebx  
       
//shift right  by 1 since no operand saying how much to shift  //shift by 1 is like dividing by 2 (hence whatever value ebx holds will be divided by 2)


Anyway, toward the end of your function they are comparing a hex number to eax, what’s in eax is what came out of func4:

           0x0000000000401049 <+65>:    cmp    $0x25,%eax  //compare $eax to 37
           0x000000000040104c <+68>:    jne    0x401055 <phase_4+77>
       
So eax needs to be equal to 0x25 and that is your second number.
(Side note, if you are absolutely confident of your 2 numbers try adding a 0 in front of them, for example 5 05)



# Phase 5:
To me this was the hardest, as usual check what kind of input it wants(for me it was 2 integer).
A huge very helpful hint is that there is a loop here and that the loop is iterating through an array. So you want to figure out what’s in it. After the previous phases, you should be able to find the loop (like a jump instruction should be the last instruction of the loop)(if you have an issue with this phase feel free to shoot me a tweet, I may or may not be able to help).


In that loop you should find an address “out of the ordinary”(your array) and use this command in gdb
x/16dw _______(without dollar sign)
It’ll print your array so you can see where it’s iterating and you’ll notice that there is no pattern.


You have a counter that’s increased by one everytime you go through the loop. If you want you can do trial and error( you have “x” amount of values in your array hence you can try that many times). 
```
  0x00000000004010a0 <+65>:    add    $0x1,%edx
  0x00000000004010a3 <+68>:    cltq //convert doubleword(16) to quadword
  0x00000000004010a5 <+70>:    mov    0x4024a0(,%rax,4),%eax  //goes through an array
   0x00000000004010ac <+77>:    add    %eax,%ecx  //ecx=will accumulate all the index value i hit(basically add every number in the array except the last one)
   0x00000000004010ae <+79>:    cmp    $0xf,%eax  //15 cmp 13 //eax =the value at the index
   0x00000000004010b1 <+82>:    jne    0x4010a0 <phase_5+65>
   ```
It’s kind of a difficult loop to understand so I’ll explain it:
* First line is your counter increasing by 1(you add 1 to its value)
* 3rd line this is a really interesting line so I will “draw” it in a smaller format to help you visualize it.
   * Array: {1,3,5,2,4}
   * If your first input is 0(eax), then you’ll go to the zero th position which holds 1. So eax becomes 1, so in the next loop you go to position 1 which has number 3 so eax becomes 3 so then in the next loop you go to the 3rd position which holds 2 so your eax becomes 2 and so on. 
* Also ecx becomes bigger and bigger because you are adding every value you hit (so basically all of them except the 15th value. ecx= your 2nd input
* Will get out of the loop once the counter reaches 15 
* Another phase 5 requires 6 letters, translate your array into like an alphabet, do the math to figure the combo to be equal to the sum
* Another phase 5 is crypotagraphy, You'll have to figure out what word it wants and then for example if you enter abcdef your input string will change and you can do x/6c $rsp+0x9(might be a different place in memory for you) and you'll see what your abcdef became. Create some sort of new alphabet from that and then make the word they want.
Example:
    a = o
    b = c
    d = s
    e = l
    f = q
    q= a 
    The word they want is locsqa, thus your input would be "eabdfq"
    

# Phase 6:
(Same as previous look at function name, for me it was read six number hence input needs to be 6 numbers)
Phase 6 was a linked list for me. Hence I was like trying to find the nodes. You can do trial and error and find where the node starts. Use the command x/3x $___(register) .


You can do this 2 ways(actually more):


1st way(easiest):
x/32dw $edx  //x /[Length][Format] [Address expression]
-This will print all of your nodes.(It took me a while to find which register had the nodes but I suggest you go find which register has the nodes). I figured what register it was after the loop.




2nd way(probably best to know):
There is a loop that goes through the linked list:

         0x000000000040112f <+95>:    mov    0x8(%rdx),%rdx 
     
        This line updates the pointer, so the first time it went through 
        I printed: rdx            0x6042e0
And if you use x/w $rdx it’ll print the node 
So you can go through the loop and get 2 nodes like that and if you continue through your code you should be able to find the other node by looking into registers(it is a long way hence I recommend the 1st way to find the answer but you should “understand” the second way.

           0x000000000040119a <+202>:   cmp    %eax,(%rbx) //compared 2nd and 1st node
           0x000000000040119c <+204>:   jge    0x4011a3 <phase_6+211>
 
        Near the end, so after you know the decimal value of your 6 nodes, you have to arrange them in a certain order. The two line above basically compare and say the 1st node need to be bigger than the second node, hence it should from biggest to smallest.

# Phase 7(secret phase):
Alright this one is long so I’ll try to say some key things.


First, you need to find the entrance of the secret phase. In my case it was in phase defused.
So I put a breakpoint into phase_defused and there are multiple so you want to go to the one after phase 6.


Then similar to previous phases you are going to want to know what input or where to put it.
For me, I had to add a string to my phase 4(I recognized it was phase 4 because I printed a register and it contained my phase 4).


Once you figure out the string(which is being put into a register) you add it to your text file next to your answer for phase 4(with a space).
In phase defused it’ll test if the string is equal to what they want it’ll basically let you go pass some code and so then you’ll be able to reach secret phase and step into it.


Spoiler… in secret phase there is a fun7 which you need to step in and it’s a binary search tree.
Hence when you are in fun7 you need to find all the children and the root.
I found the root in secret phase and the rest in fun7.


At first I just found the first node and increase the address so I could find the other node or you can use x/32 _____(address/hex of root) and it’ll print out quite a lot of the nodes.
Then drawing the Binary search tree will be helpful because then you have an idea of what number to try.


For me fun7 needed to return 3 so that was supposed to tell me which node in the tree it was but I wasn’t sure how so I just tried a bunch of node and then it worked. 


Disclaimer: I am not a pro
