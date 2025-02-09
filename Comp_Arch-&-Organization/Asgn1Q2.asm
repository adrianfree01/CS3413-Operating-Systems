#Write a MIPS assembly program to implement the following C function as a non-leaf
#procedure:
#int calculate(int x, int y) {
# int square(int n) {
# return n * n;
# }
# return square(x) + square(y);
#}
#• Implement both calculate and square as separate procedures.
#• Assume x and y are passed in $a0 and $a1.
#• Use $v0 to return the final result.
#• Manage the stack appropriately for nested procedure calls.

.data
promptX: 	.asciiz "Input integer (x): "
promptY: 	.asciiz "Input integer (y): "
promptSquare: 	.asciiz "\nn * n = "				#Never ended up using this
promptSum: 	.asciiz "\nsquare(x) + square(y) = "

.text
main:
	li 	$v0, 4		#Syscall Print String
	la	$a0, promptX	
	syscall
	
	li 	$v0, 5		#Syscall Read Int
	syscall
	move 	$t0, $v0
	
	li 	$v0, 4		#Syscall Print String
	la	$a0, promptY
	syscall
	
	li 	$v0, 5		#Syscall Read Int
	syscall
	move 	$t1, $v0
	
	move 	$a0, $t0	#Load x into $a0
	move 	$a1, $t1	#Load y into $a1
	
	jal 	calculate	#Call calculate
	move	$t0, $v0	#Move returned variable (square(x) + square(y)0) into $t0
	
	li	$v0, 4		#Syscall Print String
	la	$a0, promptSum
	syscall
	
	li	$v0, 1		#Syscall Print Int
	move	$a0, $t0	#Move returned variable into $a0 to be printed
	syscall
	
	j 	exit
	
calculate:
	addi 	$sp, $sp, -8	#Make Room for 2 words on the stack
	sw 	$ra, 0($sp)	#Store return address on stack (first spot)
	sw	$a0, 4($sp)	#Store x on stack for square()
	
	jal 	square		#Call Square
	lw	$t0, 4($sp)	#Load squared x
	sw	$a1, 4($sp)	#Store y on stack
	
	jal	square		#Call Square
	lw	$t1, 4($sp)	#Load squared y
	add	$v0, $t0, $t1	#Add the two squares together, store in $v0
	lw	$ra, 0($sp)	#Get the original return address of stack
	addi	$sp, $sp 8	#free space on stack
	jr	$ra		#return to main

square:
	lw	$t2, 4($sp)	#Load int off  the stack
	mul 	$t2, $t2, $t2	#Square int
	li	$v0, 4
	la	$a0, promptSquare
	syscall
	
	li	$v0, 1
	move	$a0, $t2
	syscall
	sw	$t2, 4($sp)	#Store squared int back onto the stack
	
	
	
	jr	$ra		#Return to calculate
	
exit:
	li $v0, 10		#Syscall for exit
	syscall
	
	