	.text
	.globl	traitant_IT_33

#------------------------------------------------------------#
#                Keyboard interrupt handler                  #
#------------------------------------------------------------#

traitant_IT_33:
	pushl	%edx
	pushl	%eax

	inb		$0x60, %al
	cmpl	$1, shell
	jne		1f
	cmpb	$224, %al
	jne		0f
	incl	set
	jmp		1f
0:
	movl	$0, blinkc
	movzbl	%al, %eax

	cmpl	$29, %eax
	jne		0f
	incl	ctrl
0:
	cmpl	$157, %eax
	jne		0f
	decl	ctrl
0:
	cmpl	$1, ctrl
	jne		0f
	cmpl	$46, %eax
	jne		1f
	cmpl	$0, pshell
	je		1f
	movl	pshell, %eax
	leal	128(%eax), %eax
	movl	(%eax), %edx
	subl	$120, %edx
	cmpl	%eax, %edx
	je		1f
	pushl	20(%edx)
	call	kill
	addl	$4, %esp
	jmp		1f
0:
	sall	$1, %eax
	movl	$1, %edx
	subb	%ah, %dl
	movzbl	%al, %eax
	sarl	$1, %eax

	pushl	%edx
	pushl	%eax
	call	keyboard_handle_scancode
	addl	$8, %esp

	movl	$0, set
1:
	movb	$0x20, %al
	outb	%al, $0x20

	popl    %eax
	popl	%edx
	iret

#------------------------------------------------------------#
#    converts integer (1st arg) to binary string (2nd arg)   #
#                     uses eax and edx                       #
#------------------------------------------------------------#

	.data
.CE0:
	.string	"0"
	.text
dectobin:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	movl	12(%ebp), %eax
	movb	$0, (%eax)
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
0:
	movl	-4(%ebp), %eax
	sarl	$1, %eax
	cmpl	-4(%ebp), %eax
	je		0f
	pushl	12(%ebp)
	call	strlen
	incl	%eax
	movl	%eax, (%esp)
	pushl	12(%ebp)
	pushl	12(%ebp)
	incl	(%esp)
	call	memmove
	addl	$12, %esp
	movl	-4(%ebp), %eax
	movl	12(%ebp), %edx
	andl	$1, %eax
	addl	$48, %eax
	movb	%al, (%edx)
	movl	-4(%ebp), %eax
	sarl	$1, %eax
	movl	%eax, -4(%ebp)
	jmp		0b
0:
	movl	12(%ebp), %eax
	cmpb	$0, (%eax)
	jne		0f
	pushl	$.CE0
	pushl	%eax
	call	strcpy
	addl	$8, %esp
0:
	leave
	ret
