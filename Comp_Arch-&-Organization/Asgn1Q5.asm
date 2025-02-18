#Question 5: Find Maximum Character in a String (20 Marks)
#Write a MIPS assembly program to find the maximum (lexicographically largest)
#character in a null-terminated string. The C equivalent is:
#char find_max_char(char *str) {
# char max = str[0];
# for (int i = 1; str[i] != '\0'; i++) {
# if (str[i] > max) {
# max = str[i];
# }
# }
# return max;
#}
#• Assume the address of the string is passed in $a0.
#• Store the result in $v0.
#• Include comments to explain each part of the code.

.data
promptInput: .asciiz "Enter a string: "
promptOutput: .asciiz "The lexicographically largest character is: "

inputBuffer: .space 100

.text
main:
    	li  	$v0, 4
    	la  	$a0, promptInput
    	syscall

    	li  	$v0, 8
    	la  	$a0, inputBuffer
    	li  	$a1, 100  # Max length 99 + null terminator
    	syscall

    
    	la  	$a0, inputBuffer
    	jal 	find_max_char # Call find_max_char function

	move	$t0, $v0
    	li  	$v0, 4
    	la  	$a0, promptOutput
    	syscall

    	li  	$v0, 11  # syscall for printing a single character
    	move 	$a0, $t0
    	syscall

    	li  	$v0, 10
	syscall

find_max_char:
    	lb   	$v0, 0($a0)   # max = str[0] (load first character)
    	addi 	$a0, $a0, 1   # Move to the next character

find_loop:
    	lb   	$t0, 0($a0)   # Load current character
    	beqz 	$t0, done     # If '\0', exit loop

    	ble  	$t0, $v0, skip_update  # If current char <= max, skip update

    	move 	$v0, $t0     # Update max = str[i]

skip_update:
    	addi 	$a0, $a0, 1   # Move to the next character
    	j    	find_loop     # Repeat

done:
    	jr 	$ra           # Return with result in $v0
