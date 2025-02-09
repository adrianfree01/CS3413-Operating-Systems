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

input: .word 0

.text
main:
	li 	$v0, 4
	la 	$a0, promptInput
	syscall
	
	li 	$v0, 8
	syscall
	sw 	$v0, input
	
	la 	$a0, input
	
	
string_length:
	li 	$t0, 0 #int length = 0
	j 	loop

loop:
	beqz 	$a0, return
	addi 	$a0, $a0, 1
	addi	$t0, $t0, 1
	j	loop
return:
	li	$v0, 4
	la	$a0, promptOutput
	syscall
	
	li	$v0, 1
	move	$a0, $t0
	syscall
	j 	exit

exit:
	li 	$v0, 10
	syscall
		
	