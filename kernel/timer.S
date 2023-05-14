	.text
	.globl	traitant_IT_32
traitant_IT_32:
	pushl	%eax
	movb	$0x20, %al
	outb	%al, $0x20
	pushl	%ecx
	pushl	%edx
	movl	running_proc, %eax
	pushl	68(%eax)
	movl	$1, 68(%eax)
	call	tic_PIT
	cmpl	$0, (%esp)
	jne		0f
	movl	%esp, %edx
	addl	$36, %edx
	movl	running_proc, %eax
	movl	%edx, 60(%eax)
	movl	%edx, tss+4
	movl	$0, 68(%eax)
0:
	addl	$4, %esp
	popl	%edx
	popl	%ecx
	popl	%eax
	iret
