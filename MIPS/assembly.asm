# NOTE: OUR CODE FOR BUUBLE SORT STARTS FROM 47 ONWARDS TILL 129----------------------------------------------->

#run in linux terminal by java -jar Mars4_5.jar nc filename.asm(take inputs from console)

#system calls by MARS simulator:
#http://courses.missouristate.edu/kenvollmar/mars/help/syscallhelp.html
.data
	next_line: .asciiz "\n"
	inp_statement: .asciiz "Enter No. of integers to be taken as input: "
	inp_int_statement: .asciiz "Enter starting address of inputs(in decimal format): "
	out_int_statement: .asciiz "Enter starting address of outputs (in decimal format): "
	enter_int: .asciiz "Enter the integer: "	
.text
#input: N= how many numbers to sort should be entered from terminal. 
#It is stored in $t1
jal print_inp_statement	
jal input_int 
move $t1,$t4

#input: X=The Starting address of input numbers (each 32bits) should be entered from
# terminal in decimal format. It is stored in $t2
jal print_inp_int_statement
jal input_int
move $t2,$t4

#input:Y= The Starting address of output numbers(each 32bits) should be entered
# from terminal in decimal. It is stored in $t3
jal print_out_int_statement
jal input_int
move $t3,$t4 

#input: The numbers to be sorted are now entered from terminal.
# They are stored in memory array whose starting address is given by $t2
move $t8,$t2
move $s7,$zero	#i = 0
loop1:  beq $s7,$t1,loop1end
	jal print_enter_int
	jal input_int
	sw $t4,0($t2)
	addi $t2,$t2,4
      	addi $s7,$s7,1
        j loop1      
loop1end: move $t2,$t8       
#############################################################
#Do not change any code above this line
#Occupied registers $t1,$t2,$t3. Don't use them in your sort function.
#############################################################

# Initialize i = 0
add $t4,$zero,$zero

# Initialize addresses for reading and writing
add $t5,$t2,$zero  # Address to read from
add $t6,$t3,$zero  # Address to write to

# Jump to the sorting process
j begin_sort

# Sorting process begins
begin_sort: add $t4,$zero,$zero  # Initialize i = 0

outer_loop: # Load the address for reading
    add $t9,$t5,$zero

    # Check if i == n (ending the loop)
    beq $t4, $t1, end_outer_loop

    add $s3,$zero,$zero  # Initialize j = 1
    addi $s3,$s3,1
    add $s0,$zero,$zero  # Initialize swapped = 0

    # Calculate n - i - 1 # ----->
    sub $t7, $t1, $zero
    #sub $t7, $t7, 1

    # Jump to the inner loop
    j inner_loop

inner_loop: # Check if j == n - i - 1 (ending the inner loop) ------
    beq $s3, $t7, end_inner_loop

    # Load a[j] and a[j+1]
    lw $s1, 0($t9)
    lw $s2, 4($t9)

    # Compare a[j] and a[j+1]
    slt $s4,$s1, $s2
    bne $s4,$zero,else
    sw $s1,4($t9)
    sw $s2,0($t9)
    addi $s0,$s0,1

    else: addi $t9, $t9, 4  # Increment memory read location (j)
    addi $s3, $s3, 1  # Increment j
    j inner_loop

end_inner_loop: # Check if no swaps were done
    beq $s0,$zero, end_outer_loop

    addi $t4, $t4, 1  # Increment i
    jal check_swapped  # Check if a swap was done
    j outer_loop  # Go back to the outer loop

swap: # Swap a[j] and a[j+1]
    #lw $t7, 0($t9)    # Load a[j] into $t7
    #lw $t8, 4($t9)    # Load a[j+1] into $t8
    #sw $t8, 0($t9)    # Store a[j+1] to a[j]
    #sw $t7, 4($t9)    # Store temp (a[j]) to a[j+1]
    #li $s0, 1         # Set swapped to 1
    #j back_from_swap

check_swapped: # Check if swapped is 0 (no swap was done)
    beq $s0,$0, end_outer_loop

    jr $ra  # Return to the end_inner_loop



end_outer_loop: add $t4,$zero,$zero
add $t5,$zero,$t2
add $t6,$zero,$t3
copy_sorted: beq $t4, $t1,end_copy_sorted
    lw $s5, 0($t5)  # Read from sorted array
    sw $s5, 0($t6)  # Store in the given location

    # Increment counters
    addi $t4, $t4, 1
    addi $t5, $t5, 4
    addi $t6, $t6, 4
    j copy_sorted
end_copy_sorted: #############################################################
#You need not change any code below this line

#print sorted numbers
move $s7,$zero	#i = 0
loop: beq $s7,$t1,end
      lw $t4,0($t3)
      jal print_int
      jal print_line
      addi $t3,$t3,4
      addi $s7,$s7,1
      j loop 
#end
end:  li $v0,10
      syscall
#input from command line(takes input and stores it in $t6)
input_int: li $v0,5
	   syscall
	   move $t4,$v0
	   jr $ra
#print integer(prints the value of $t6 )
print_int: li $v0,1	
	   move $a0,$t4
	   syscall
	   jr $ra
#print nextline
print_line: li $v0,4
	   la $a0,next_line
	   syscall
	   jr $ra

#print number of inputs statement
print_inp_statement: li $v0,4
		la $a0,inp_statement
		syscall 
		jr $ra
#print input address statement
print_inp_int_statement: li $v0,4
		la $a0,inp_int_statement
		syscall 
		jr $ra
#print output address statement
print_out_int_statement: li $v0,4
		la $a0,out_int_statement
		syscall 
		jr $ra
#print enter integer statement
print_enter_int: li $v0,4
		la $a0,enter_int
		syscall 
		jr $ra
