.data
promptA: .asciiz "Enter int (a): "
promptB: .asciiz "Enter int (b): "
promptC: .asciiz "Enter int (c): "
promptEnd: .asciiz "The result is: "

.text
main:
	# Prompt for a
	li	$v0, 4
	la	$a0, promptA
	syscall
	
	li	$v0, 5		# Read integer
	syscall
	move 	$t0, $v0	# Store input in $t0 (a)

	# Prompt for b
	li	$v0, 4
	la	$a0, promptB
	syscall
	
	li	$v0, 5		# Read integer
	syscall
	move 	$t1, $v0	# Store input in $t1 (b)

	# Prompt for c
	li	$v0, 4
	la	$a0, promptC
	syscall
	
	li	$v0, 5		# Read integer
	syscall
	move 	$t2, $v0	# Store input in $t2 (c)

	# Call compute_sum(a, b, c)
	move	$a0, $t0	# Pass a
	move	$a1, $t1	# Pass b
	move	$a2, $t2	# Pass c
	jal 	compute_sum

	move	$t3, $v0
	# Print the result
	li 	$v0, 4
	la 	$a0, promptEnd
	syscall

	li 	$v0, 1
	move	$a0, $t3	# $result is in a0
	syscall

	j	exit

######################################## THIS IS THE LEAF FUNCTION compute_sum #####################################################
compute_sum:
	mul	$v0, $a0, $a1	# $v0 = a * b
	add	$v0, $v0, $a2	# $v0 = (a * b) + c
	jr 	$ra		# jump to return address

exit:
	li	$v0, 10
	syscall
