#Question 4: Reverse a String (20 Marks)
#Write a MIPS assembly program to reverse a null-terminated string. The C equivalent is:
#void reverse_string(char *str) {
# int len = 0;
# while (str[len] != '\0') len++;
# for (int i = 0; i < len / 2; i++) {
# char temp = str[i];
# str[i] = str[len - i - 1];
# str[len - i - 1] = temp;
# }
#}
#• Assume the address of the string is passed in $a0.
#• Ensure proper handling of the null-terminator.
#• Comment on each part of your code

.data

promptInput:  .asciiz "Enter a string to be reversed: "
promptOutput: .asciiz "Your string reversed is: "

inputBuffer:  .space 100

.text
main:
    	li  	$v0, 4
    	la  	$a0, promptInput
    	syscall

    	li  	$v0, 8
    	la  	$a0, inputBuffer
    	li  	$a1, 100 # Max length 99 + null terminator
    	syscall

    	la  	$a0, inputBuffer
    	jal 	reverse_string
    
    	li  	$v0, 4
    	la  	$a0, promptOutput
    	syscall

    	li  	$v0, 4
    	la  	$a0, inputBuffer
    	syscall

    	li  	$v0, 10
    	syscall

reverse_string:
    	li   	$t0, 0      # len = 0
    	move 	$t1, $a0    # Pointer to start of string

find_length:
    	lb   	$t2, 0($t1) # Load current character
    	beqz 	$t2, swap_chars # If '\0', go to swap phase
    	beq  	$t2, 10, swap_chars # If newline, go to swap phase
    	addi 	$t0, $t0, 1 # len++
    	addi 	$t1, $t1, 1 # Move pointer
    	j    	find_length

swap_chars:
    	subi 	$t0, $t0, 1 # len - 1 (last character index)
    	move 	$t3, $a0    # Start pointer (i = 0)
    	add  	$t4, $a0, $t0 # End pointer (str + len - 1)

swap_loop:
    	bge  	$t3, $t4, done # If start >= end, stop swapping

    	lb   	$t5, 0($t3) # Load str[i]
    	lb   	$t6, 0($t4) # Load str[len - i - 1]

    	sb   	$t6, 0($t3) # Store str[len - i - 1] in str[i]
    	sb   	$t5, 0($t4) # Store str[i] in str[len - i - 1]

    	addi	$t3, $t3, 1 # Move start pointer forward
    	subi 	$t4, $t4, 1 # Move end pointer backward

    	j    swap_loop

done:
    	jr   	$ra # Return
