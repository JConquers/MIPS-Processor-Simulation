.#data
 # ans: .asciiz "\nFactorial is: "
.text

  main: li $t1, 1 # this will store the multiplication result after every iteration. Ultimately this will become the factorial 
    li $t3,1
    li $t0, 4 
    # load 4 into $t0, basically this is the number whose factorial is to be calculated.
    # initialize result in $t1.
    loop: beq $t0,$0,done 
      # if $t0 is zero, exit loop.
      mult $t1,$t0
       # multiply result by number and store lower bits of result in LI
      mflo $t1
      sub $t0,$t0,$t3 
      # decrement number by one.
    j loop 
      
  done: move $s0,$t1 
  # store answer, i.e. factorial in $s0.

      
