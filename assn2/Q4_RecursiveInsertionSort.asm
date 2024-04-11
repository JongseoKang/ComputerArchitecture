.data

n0:
  .align 4
  .word 8
array0:
  .align 4
  .word 93,14,23,53,98,10,9,6

n1:
  .align 4
  .word 10
array1:
  .align 4
  .word 10,9,8,7,6,5,4,3,2,1

n2:
  .align 4
  .word 10
array2:
  .align 4
  .word 10,-9,8,-7,6,-5,4,-3,2,-1

n3:
  .align 4
  .word 4
array3:
  .align 4
  .word 1,2,3,4

newline:
  .asciiz "\n"
space:
  .asciiz " "
str0:
  .asciiz "INFO: array[] = "

str1:
  .asciiz "INFO: recursiveInsertionSort(n0 = 8, array0):\n"
str2:
  .asciiz "INFO: recursiveInsertionSort(n1 = 10, array1):\n"
str3:
  .asciiz "INFO: recursiveInsertionSort(n2 = 10, array2):\n"
str4:
  .asciiz "INFO: recursiveInsertionSort(n3 = 4, array3):\n"

.text

# $a0 = n, $a1 = &array[0]
printArray:
  addi $sp, $sp, -16
  sw $ra,  0($sp)
  sw $s0,  4($sp)
  sw $s1,  8($sp)
  sw $s2, 12($sp)
  move $s0, $a0
  li $s1, 0
  move $s2, $a1
  li $v0, 4
  la $a0, str0
  syscall
printArray_loop:
  beq $s1, $s0, printArray_return
  li $v0, 1
  lw $a0, ($s2)
  syscall
  li $v0, 4
  la $a0, space
  syscall
  addi $s1, $s1, 1
  addi $s2, $s2, 4
  j printArray_loop
printArray_return:
  li $v0, 4
  la $a0, newline
  syscall
  lw $ra,  0($sp)
  lw $s0,  4($sp)
  lw $s1,  8($sp)
  lw $s2, 12($sp)
  addi $sp, $sp, 16
  jr $ra

# $a0 = N, $a1 = &array[0]
recursiveInsertionSort:

  # stack <-- $ra, $s0, $s1
  addi $sp, $sp, -12
  sw $ra, 0($sp)
  sw $s0, 4($sp)
  sw $s1, 8($sp)

  # $s0 = $a0; $s1 = $a1
  move $s0, $a0
  move $s1, $a1

  # printArray(N, array)
  move $a0, $s0
  move $a1, $s1
  jal printArray

################################################################################
# FIXME

  beq $a0, $zero, recursiveInsertionSort_exit

  # Else

  # recursive function call
  addi $sp, $sp, -8
  sw $a0, 0($sp)
  sw $a1, 4($sp)
  addi $a0, $a0, -1
  jal recursiveInsertionSort
  lw $a0, 0($sp)
  lw $a1, 4($sp)
  addi $sp, $sp, 8

  # $t0 = x = array[N - 1];
  addi $t1, $a0, -1
  addi $t2, $zero, 4
  multu $t1, $t2
  mflo $t1
  addu $t1, $t1, $a1
  lw $t0, 0($t1)

  # unsigned int $t1 = j = 2;
  addi $t1, $zero, 2

  # while (j <= N && array[N - j] > x) {
  Loop:
  slt $t2, $t1, $a0 # $t2 == 0 where the condition matches

  subu $t4, $a0, $t1
  addi $t5, $zero, 4
  multu $t4, $t5
  mflo $t4
  addu $t5, $t4, $a1
  lw $t4, 0($t5)

  slt $t3, $t0, $t4 # t3 == 1 where the condition matches
  bne $t2, $zero, Break
  beq $t3, $zero, Break

  # array[N - j + 1] = array[N - j];
  move $t2, $a0
  subu $t2, $t2, $t1
  addi $t3, $zero, 4
  multu $t2, $t3
  mflo $t2
  addu $t2, $t2, $a1
  lw $t4, 0($t2)
  sw $t4, 4($t2)

  # j++ }
  addi $t1, 1
  j Loop

  Break:
  # array[N - j + 1] = x;
  addi $t2, $a0, 1
  subu $t2, $t2, $t1
  addi $t3, $zero, 4
  multu $t2, $t3
  mflo $t2
  addu $t2, $t2, $a1
  sw $t0, 0($t2)

  # $s0 = $a0; $s1 = $a1
  move $s0, $a0
  move $s1, $a1
# FIXME
################################################################################

recursiveInsertionSort_exit:

  # printArray(n, array)
  move $a0, $s0
  move $a1, $s1
  jal printArray

  # $ra, $s0, $s1 <-- stack
  lw $ra, 0($sp)
  lw $s0, 4($sp)
  lw $s1, 8($sp)
  addi $sp, $sp, 12

  # return
  jr $ra

.globl main
main:

  # stack <-- $ra, $s0
  addi $sp, $sp, -8
  sw $ra, 0($sp)
  sw $s0, 4($sp)

  # recursiveInsertionSort(n0, array0)
  li $v0, 4
  la $a0, str1
  syscall
  lw $a0, n0
  la $a1, array0
  jal recursiveInsertionSort

  # recursiveInsertionSort(n1, array1)
  li $v0, 4
  la $a0, str2
  syscall
  lw $a0, n1
  la $a1, array1
  jal recursiveInsertionSort

  # recursiveInsertionSort(n2, array2)
  li $v0, 4
  la $a0, str3
  syscall
  lw $a0, n2
  la $a1, array2
  jal recursiveInsertionSort

  # recursiveInsertionSort(n3, array3)
  li $v0, 4
  la $a0, str4
  syscall
  lw $a0, n3
  la $a1, array3
  jal recursiveInsertionSort

  # $ra, $s0 <-- stack
  lw $ra, 0($sp)
  lw $s0, 4($sp)
  addi $sp, $sp, 8
 
  # return
  jr $ra

