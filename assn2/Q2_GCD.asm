.data

newline:
  .asciiz "\n"

str0:
  .asciiz "Enter a 32-bit unsigned integer (in decimal): "
str1:
  .asciiz "INFO: calculateGCD returned:\n"
str2:
  .asciiz "INFO:   $v0 = "

.text

# $a0: a 32-bit unsigned integer
# $a1: a 32-bit unsigned integer
# $v0: the greatest common divisor of $a0 and $a1
calculateGCD:

################################################################################
# FIXME
  

  slt $t0, $a0, $a1
  beq $t0, $zero, changeAB
  move $t1, $a0
  move $a0, $a1
  move $a1, $t1
changeAB:

  beq $a1, $zero, gotoReturn
  addi $sp, $sp, -20
  sw $ra, 16($sp)
  sw $t1, 12($sp)
  sw $t0, 8($sp)
  sw $a1, 4($sp)
  sw $a0, 0($sp)

  divu $a0, $a1
  move $a0, $a1
  mfhi $a1

  jal calculateGCD

  lw $ra, 16($sp)
  lw $t1, 12($sp)
  lw $t0, 8($sp)
  lw $a1, 4($sp)
  lw $a0, 0($sp)
  addi $sp, $sp, 20

  jr Exit

gotoReturn:
  move $v0, $a0

Exit:
  add $v0, $v0, $zero

# FIXME
################################################################################

  # return
  jr $ra

.globl main
main:

  # print_string str0; $t0 = read_int
  li $v0, 4
  la $a0, str0
  syscall
  li $v0, 5
  syscall
  move $t0, $v0

  # print_string str0; $t1 = read_int
  li $v0, 4
  la $a0, str0
  syscall
  li $v0, 5
  syscall
  move $t1, $v0

  # $s0 = $ra; $s1 = $a0; $s2 = $a1
  move $s0, $ra
  move $s1, $a0
  move $s2, $a1

  # call calculateGCD
  move $a0, $t0
  move $a1, $t1
  jal calculateGCD

  # $ra = $s0; $a0 = $s1; $a1 = $s2
  move $ra, $s0
  move $a0, $s1
  move $a1, $s2

  # $t0 = $v0
  move $t0, $v0

  # print_string str1
  li $v0, 4
  la $a0, str1
  syscall

  # print_string str2; print_int $t0; print_string newline
  li $v0, 4
  la $a0, str2
  syscall
  li $v0, 1
  move $a0, $t0
  syscall
  li $v0, 4
  la $a0, newline
  syscall

  # return 0
  li $v0, 0
  jr $ra

