  # It takes an argument N in $a0 and returns the result in $s0 and $t

.text

    fib: li $a0,7
        # It takes an argument N in $a0 and returns the result in $s0 and $t
        # $t0 as a counter, $t1 as the current term, and $t2 as the previous term
        li $t4,1
        li $t0,0 
		# Initialize the counter to zero
        li $t1,0 
		# Initialize the current term to zero
        li $t2, 1 
		# Initialize the previous term to one
    loop: beq $t0, $a0, done 
			# If counter equals N, exit the loop
            add $t3, $t1, $t2 
			# Compute the next term as the sum of the current and previous terms
            move $t2, $t1 
			# Update the previous term to the current term
            move $t1, $t3 
			# Update the current term to the next term
            add $t0,$t0,$t4 

			# Increment the counter
            j loop 
			# Repeat the loop
    done: move $s0, $t1 # Return the current term as the result
            
