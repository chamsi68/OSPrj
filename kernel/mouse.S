	.text
	.globl	PIC2_IRQ
	.globl	initMouse
	.globl	traitant_IT_44

#------------------------------------------------------------#
#     set slave's nth (1st arg) IRQ byte to b (2nd arg)      #
#                    uses eax and ecx                        #
#------------------------------------------------------------#

PIC2_IRQ:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$1, %esp
	inb		$0xa1, %al
	movb	%al, -1(%ebp)
	cmpl	$0, 12(%ebp)
	jne		0f
	movb	$1, %al
	movl	8(%ebp), %ecx
	subl	$8, %ecx
	salb	%cl, %al
	notb	%al
	andb	%al, -1(%ebp)
	jmp		1f
0:
	movb	$1, %al
	movl	8(%ebp), %ecx
	subl	$8, %ecx
	salb	%cl, %al
	orb		%al, -1(%ebp)
1:
	movb	-1(%ebp), %al
	outb	%al, $0xa1
	leave
	ret

#------------------------------------------------------------#
#         initializes PS/2 mouse to send interrupts          #
#------------------------------------------------------------#

initMouse:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$1, %esp

	movl	$160, x
	movl	$100, y

	pushl	$1
	call	mouse_timeout

	movb	$0xa8, %al
	outb	%al, $0x64

	call	mouse_timeout

	movb	$0x20, %al
	outb	%al, $0x64

	movl	$0, (%esp)
	call	mouse_timeout

	inb		$0x60, %al
	orb		$2, %al
	movb	%al, -1(%ebp)

	movl	$1, (%esp)
	call	mouse_timeout

	movb	$0x60, %al
	outb	%al, $0x64

	call	mouse_timeout

	movb	-1(%ebp), %al
	outb	%al, $0x60

	movl	$0xf6, (%esp)
	call	mousewrite

	call	mouseread

	movl	$0xf4, (%esp)
	call	mousewrite
	addl	$4, %esp

	call	mouseread

	leave
	ret

#------------------------------------------------------------#
#               PS/2 mouse interrupt handler                 #
#------------------------------------------------------------#

traitant_IT_44:
	pushl	%eax
	pushl	%edx
	pushl	%ebp
	movl	%esp, %ebp
	subl	$1, %esp

	inb		$0x60, %al
	movb	%al, -1(%ebp)

	movl	Mcounter, %edx
	incl	%edx

	cmpb	$1, %dl
	jne		0f
	movb	-1(%ebp), %al
	movb	%al, mice
	jmp		1f
0:
	cmpb	$2, %dl
	jne		0f
	movb	-1(%ebp), %al
	movb	%al, mice+1
	jmp		1f
0:
	cmpb	$3, %dl
	jne		1f
	movb	-1(%ebp), %al
	movb	%al, mice+2

	cmpl	$0x13, VGAMode
	jne		0f

	movsbl	mice+2, %eax
	pushl	%eax
	movsbl	mice+1, %eax
	pushl	%eax
	call	drawcursor
	addl	$8, %esp
0:
	xorb	%dl, %dl
1:
	movb	%dl, Mcounter

	movb	$0x20, %al
	outb	%al, $0xa0
	outb	%al, $0x20

	leave
	popl	%edx
	popl	%eax
	iret

#------------------------------------------------------------#
#     functions to read and write into PS/2 mouse ports      #
#  timeout function that waits for i/o for an amout of time  #
#------------------------------------------------------------#

mouse_timeout:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	movl	$0x186a0, -4(%ebp)
	cmpl	$0, 8(%ebp)
	jne		1f
0:
	decl	-4(%ebp)
	cmpl	$0, -4(%ebp)
	je		0f
	inb		$0x64, %al
	orb		$1, %al
	cmpb	$1, %al
	je		0f
	jmp		0b
1:
	decl	-4(%ebp)
	cmpl	$0, -4(%ebp)
	je		0f
	inb		$0x64, %al
	orb		$2, %al
	cmpb	$0, %al
	je		0f
	jmp		1b
0:
	leave
	ret

mousewrite:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	$1
	call	mouse_timeout

	movb	$0xd4, %al
	outb	%al, $0x64

	call	mouse_timeout
	addl	$4, %esp

	movb	8(%ebp), %al
	outb	%al, $0x60

	leave
	ret

mouseread:
	pushl	%ebp
	movl	%esp, %ebp

	pushl	$0
	call	mouse_timeout
	addl	$4, %esp

	inb		$0x60, %al
	movzbl	%al, %eax

	leave
	ret

#------------------------------------------------------------#
#      draw mouse cursor, (1st arg) dx, (2nd arg), dy        #
#                uses eax, edi and esi                       #
#------------------------------------------------------------#

drawcursor:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	movl	$0, -4(%ebp)
0:
	cmpl	$0x12, -4(%ebp)
	jg		0f
	movl	-4(%ebp), %esi
	movl	%esi, %eax
	sall	$3, %esi
	sall	$2, %eax
	leal	tmp(%eax, %esi), %esi
	movl	y, %edi
	addl	-4(%ebp), %edi
	movl	%edi, %eax
	sall	$8, %edi
	sall	$6, %eax
	addl	%eax, %edi
	addl	x, %edi
	addl	$0xa0000, %edi
	movl	$12, %ecx
	rep		movsb
	incl	-4(%ebp)
	jmp		0b
0:
	movl	x, %eax
	addl	8(%ebp), %eax
	cmpl	$320, %eax
	jg		1f
	cmpl	$0, %eax
	jl		0f
	movl	%eax, x
	jmp		2f
0:
	movl	$0, x
	jmp		2f
1:
	movl	$320, x
2:
	movl	y, %eax
	subl	12(%ebp), %eax
	cmpl	$200, %eax
	jg		1f
	cmpl	$0, %eax
	jl		0f
	movl	%eax, y
	jmp		2f
0:
	movl	$0, y
	jmp		2f
1:
	movl	$200, y
2:
	movl	$0, -4(%ebp)
0:
	cmpl	$0x12, -4(%ebp)
	jg		0f
	movl	y, %esi
	addl	-4(%ebp), %esi
	movl	%esi, %eax
	sall	$8, %esi
	sall	$6, %eax
	addl	%eax, %esi
	addl	x, %esi
	addl	$0xa0000, %esi
	movl	-4(%ebp), %edi
	movl	%edi, %eax
	sall	$3, %edi
	sall	$2, %eax
	leal	tmp(%eax, %edi), %edi
	movl	$12, %ecx
	rep		movsb
	incl	-4(%ebp)
	jmp		0b
0:
	movl	$0, -4(%ebp)
0:
	cmpl	$0x12, -4(%ebp)
	jg		0f
	movl	$0, -8(%ebp)
1:
	cmpl	$0xb, -8(%ebp)
	jg		1f
	movl	-4(%ebp), %esi
	movl	%esi, %eax
	sall	$3, %esi
	sall	$2, %eax
	addl	%eax, %esi
	addl	-8(%ebp), %esi
	addl	$cursor, %esi
	cmpb	$1, (%esi)
	je		2f
	movl	y, %edi
	addl	-4(%ebp), %edi
	movl	%edi, %eax
	sall	$8, %edi
	sall	$6, %eax
	addl	%eax, %edi
	addl	x, %edi
	addl	-8(%ebp), %edi
	addl	$0xa0000, %edi
	movsb
2:
	incl	-8(%ebp)
	jmp		1b
1:
	incl	-4(%ebp)
	jmp		0b
0:
	leave
	ret
