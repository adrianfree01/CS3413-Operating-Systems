#Write a MIPS assembly program to calculate the length of a null-terminated string. The C
#equivalent is:
#int string_length(char *str) {
# int length = 0;
# while (str[length] != '\0') {
# length++;
# }
# return length;
#}
#• Assume the address of the string is passed in $a0.
#• Store the length in $v0.
#• Include comments explaining each step.

.data
promptInput: .asciiz "Enter a string to calculate its' length: "
promptOutput: .asciiz "Length: "

inputBuffer: .space 100

.text
main:
	li 	$v0, 4
	la 	$a0, promptInput
	syscall
	
	li 	$v0, 8
	la	$a0, inputBuffer
	li	$a1, 100 # Max Length 99 + null terminator
	syscall
	
	la $a0, inputBuffer
	jal string_length
	
	move	$t0, $v0
	
	li	$v0, 4
	la	$a0, promptOutput
	syscall
	
	li	$v0, 1
	move	$a0, $t0
	syscall
	
	j 	exit
	
string_length:
	
	li 	$t0, 0 #int length = 0
	move 	$t1, $a0

loop:
	lb	$t2, 0($t1) #load byte of string
	beqz 	$t2, return # Check for null terminator
	beq	$t2, 10, return # Check for newLine character
	addi 	$t0, $t0, 1 #increment length counter
	addi	$t1, $t1, 1 #increment string position
	j	loop

return:
	move	$v0, $t0 #put string length in v0
	jr 	$ra # jump back to return address

exit:
	li 	$v0, 10
	syscall
		
	
