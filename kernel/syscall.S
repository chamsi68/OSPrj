	.data
table:
	.long	0

	.text
	.globl	sys_exit
	.globl	syscall

syscall:
	pushl	%ebx
	movl	running_proc, %ebx
	movl	$1, 68(%ebx)
	popl	%ebx
	call	*table(, %eax, 4)
	movl	running_proc, %ebx
	movl	%esp, %edi
	addl	$20, %edi
	movl	%edi, 60(%ebx)
	movl	%edi, tss+4
	movl	$0, 68(%ebx)
	iret

	movl	$0, %eax
sys_exit:
	movl	running_proc, %ebx
	movl	60(%ebx), %esp
	movl	%eax, %ebx
handler1:
	subl	$8, %esp
	movl	%ebx, 4(%esp)
	jmp		exit

	.data
	.long	handler1

	.text
handler2:
	subl	$20, %esp
	movl	%cr3, %ebx
	pushl	%ebx
	pushl	%edx
	pushl	%ecx
	pushl	%edi

	leal	16(%esp), %edi
	pushl	%edi
0:
	movsb
	cmpb	$0, -1(%esi)
	je		0f
	jmp		0b
0:

	leal	pgdir, %ebx
	movl	%ebx, %cr3

	call	start

	addl	$16, %esp
	popl	%ebx
	movl	%ebx, %cr3
	addl	$20, %esp
	ret

	.data
	.long	handler2

	.text
handler3:
	pushl	%edi
	pushl	%esi
	call	cons_read
	addl	$8, %esp
	ret

	.data
	.long	handler3

	.text
handler4:
	pushl	%edx
	pushl	%ecx
	call	console_putbytes
	addl	$8, %esp
	ret

	.data
	.long	handler4

	.text
handler7:
	subl	$4, %esp
	pushl	%esp
	pushl	%ecx
	call	waitpid
	addl	$8, %esp
	movl	(%esp), %edx
	addl	$4, %esp
	ret
	
	.data
	.long	0,	0,	handler7

	.text
handler8:
	movl	%cr3, %ebx
	pushl	%ebx
	pushl	$20
	call	mem_alloc
	addl	$4, %esp
	movl	%eax, %edi
	pushl	%edi
0:
	movsb
	cmpb	$0, -1(%esi)
	je		0f
	jmp		0b
0:

	leal	pgdir, %ebx
	movl	%ebx, %cr3

	call	shm_create

	addl	$4, %esp
	popl	%ebx
	movl	%ebx, %cr3
	ret

	.data
	.long	handler8

	.text
handler9:
	subl	$20, %esp
	movl	%cr3, %ebx
	pushl	%ebx

	leal	4(%esp), %edi
	pushl	%edi
0:
	movsb
	cmpb	$0, -1(%esi)
	je		0f
	jmp		0b
0:

	leal	pgdir, %ebx
	movl	%ebx, %cr3

	call	shm_acquire

	addl	$4, %esp
	popl	%ebx
	movl	%ebx, %cr3
	addl	$20, %esp
	ret

	.data
	.long	handler9

	.text
handler10:
	subl	$20, %esp
	movl	%cr3, %ebx
	pushl	%ebx

	leal	4(%esp), %edi
	pushl	%edi
0:
	movsb
	cmpb	$0, -1(%esi)
	je		0f
	jmp		0b
0:

	leal	pgdir, %ebx
	movl	%ebx, %cr3

	call	shm_release

	addl	$4, %esp
	popl	%ebx
	movl	%ebx, %cr3
	addl	$20, %esp
	ret

	.data
	.long	handler10

	.text
handler13:
	pushl	%ebx
	call	clearscreen
	addl	$4, %esp
	ret

	.data
	.long	0,  0,  handler13

	.text
handler20:
	call	getpid
	ret
	
	.data
	.long	0,	0,	0,	0,	0,	0,	handler20

	.text
handler27:
	pushl	%ebx
	call	cons_echo
	addl	$4, %esp
	ret

	.data
	.long	0,	0,	0,	0,	0,	0,	handler27

	.text
handler37:
	pushl	%ebx
	call	kill
	addl	$4, %esp
	ret

	.data
	.long	0,	0,	0,	0,	0,	0,	0,	0,	0,	handler37

	.text
handler48:
	pushl	%ebx
	call	signal
	addl	$4, %esp
	ret

	.data
	.long	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  handler48

	.text
handler49:
	pushl	%edx
	pushl	%ecx
	call	signaln
	addl	$4, %esp
	ret

	.data
	.long	handler49

	.text
handler60:
	pushl	%ebx
	call	try_wait
	addl	$4, %esp
	ret

	.data
	.long	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  handler60

	.text
handler61:
	pushl	%ebx
	call	wait
	addl	$4, %esp
	ret

	.data
	.long	handler61

	.text
handler64:
	pushl	%ebx
	call	screate
	addl	$4, %esp
	ret

	.data
	.long	0,  0,  handler64

	.text
handler65:
	pushl	%ebx
	call	sdelete
	addl	$4, %esp
	ret

	.data
	.long	handler65

	.text
handler66:
	pushl	%edx
	pushl	%ecx
	call	sreset
	addl	$8, %esp
	ret

	.data
	.long	handler66

	.text
handler67:
	pushl	%ebx
	call	scount
	addl	$4, %esp
	ret

	.data
	.long	handler67

	.text
handler96:
	pushl	%ebx
	call	getprio
	addl	$4, %esp
	ret

	.data
	.long	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
	.long	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	handler96

	.text
handler97:
	pushl	%edx
	pushl	%ecx
	call	chprio
	addl	$8, %esp
	ret

	.data
	.long	handler97

	.text
handler116:
	pushl	%ebx
	call	sys_info
	addl	$4, %esp
	ret

	.data
	.long	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
	.long	0,	handler116

	.text
handler265:
	call	current_clock
	ret

	.data
	.long	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,  0,  0
	.long	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
	.long	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
	.long	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
	.long	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
	.long	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
	.long	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
	.long	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0
	.long	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	handler265

	.text
handler266:
	subl	$8, %esp
	leal	4(%esp), %edx
	pushl	%edx
	leal	4(%esp), %ecx
	pushl	%ecx
	call	clock_settings
	addl	$8, %esp
	movl	(%esp), %ecx
	movl	4(%esp), %edx
	addl	$8, %esp
	ret

	.data
	.long	handler266

	.text
handler267:
	pushl	%ebx
	call	wait_clock
	addl	$4, %esp
	ret

	.data
	.long	handler267

	.text
handler277:
	pushl	%ebx
	call	pcreate
	addl	$4, %esp
	ret

	.data
	.long	0,	0,	0,	0,	0,	0,	0,	0,	0,	handler277

	.text
handler278:
	pushl	%ebx
	call	pdelete
	addl	$4, %esp
	ret

	.data
	.long	handler278

	.text
handler279:
	pushl	%edx
	pushl	%ecx
	call	psend
	addl	$8, %esp
	ret

	.data
	.long	handler279

	.text
handler280:
	subl	$4, %esp
	pushl	%esp
	pushl	%ecx
	call	preceive
	addl	$8, %esp
	movl	(%esp), %edx
	addl	$4, %esp
	ret

	.data
	.long	handler280

	.text
handler281:
	pushl	%ebx
	call	preset
	addl	$4, %esp
	ret

	.data
	.long	handler281

	.text
handler282:
	subl	$4, %esp
	pushl	%esp
	pushl	%ecx
	call	pcount
	addl	$8, %esp
	movl	(%esp), %edx
	addl	$4, %esp
	ret

	.data
	.long	handler282
