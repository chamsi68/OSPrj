#----------------------------------------------------------#
#                                                          #
#	 VGa functions. change video mode etc.                 #
#                                                          #
#----------------------------------------------------------#
	.text
    .globl  set_regs

#---------------------------------#
#  Set VGa regs to choosen mode   #
#	  internal use.               #
#---------------------------------#

set_regs:
	movw	$0x3c2, %dx
	lodsb
	outb	%al, %dx

	movw	$0x3da, %dx
	lodsb
	outb	%al, %dx

	xorl	%ecx, %ecx
	movw	$0x3c4, %dx
0:
	lodsb
	xchgb	%ah, %al
	movb	%cl, %al
	outw	%ax, %dx
	incl	%ecx
	cmpb	$4, %cl
	jbe	 	0b

	movw	$0x3d4, %dx
	movw	$0xe11, %ax
	outw	%ax, %dx

	xorl	%ecx, %ecx
	movw	$0x3d4, %dx
0:
	lodsb
	xchgb	%ah, %al
	movb	%cl, %al
	outw	%ax, %dx
	incl	%ecx
	cmpb	$0x18, %cl
	jbe	 	0b

	xorl	%ecx, %ecx
	movw	$0x3ce, %dx
0:
	lodsb
	xchgb	%ah, %al
	movb	%cl, %al
	outw	%ax, %dx
	incl	%ecx
	cmpb	$8, %cl
	jbe	 	0b

	movw	$0x3da, %dx
	inb		%dx, %al

	xorl	%ecx, %ecx
	movw	$0x3c0, %dx
0:
	inw		%dx, %ax
	movb	%cl, %al
	outb	%al, %dx
	lodsb
	outb	%al, %dx
	incl	%ecx
	cmpb	$0x14, %cl
	jbe	 	0b

	movb	$0x20, %al
	outb	%al, %dx

	ret
