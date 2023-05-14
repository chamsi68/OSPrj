	.text
	.globl	exit
	.globl	start
	.globl	cons_read
	.globl	cons_write
	.globl	waitpid
	.globl	shm_create
	.globl	shm_acquire
	.globl	shm_release
	.globl	clear
	.globl	getpid
	.globl	cons_echo
	.globl	kill
	.globl	signal
	.globl	signaln
	.globl	try_wait
	.globl	wait
	.globl	screate
	.globl	sdelete
	.globl	sreset
	.globl	getprio
	.globl	chprio
	.globl	sys_info
	.globl	current_clock
	.globl	clock_settings
	.globl	wait_clock
	.globl	pcreate
	.globl	pdelete
	.globl	psend
	.globl	preceive
	.globl	preset
	.globl	pcount

exit:
	pushl	%ebp
	movl	%esp, %ebp
	movl	$1, %eax
	movl	8(%ebp), %ebx
	int		$0x49
0:
	jmp		0b

start:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$2, %eax
	movl	8(%ebp), %esi
	movl	12(%ebp), %edi
	movl	16(%ebp), %ecx
	movl	20(%ebp), %edx
	int		$0x49
	movl	%eax, -0x4(%ebp)
	popal
	movl	-0x4(%ebp), %eax
	leave
	ret

cons_read:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$260, %esp
	pushal
	movl	$3, %eax
	leal	-260(%ebp), %esi
	movl	12(%ebp), %edi
	int		$0x49
	movl	%eax, -0x4(%ebp)
	movl	8(%ebp), %eax
	cmpl	$0, %eax
	je		1f
	cmpl	$0x40000000, %eax
	jae		0f
	movl	$-1, -4(%ebp)
	jmp		1f
0:
	leal	-260(%ebp), %esi
	movl	8(%ebp), %edi
	movl	12(%ebp), %ecx
	rep		movsb
	movb	$0, (%edi)
1:
	popal
	movl	-0x4(%ebp), %eax
	leave
	ret

cons_write:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	cmpl	$0x40000000, 8(%ebp)
	jae		0f
	movl	$-1, -4(%ebp)
	jmp		1f
0:
	pushal
	movl	$4, %eax
	movl	8(%ebp), %ecx
	movl	12(%ebp), %edx
	int		$0x49
	movl	%eax, -4(%ebp)
	popal
1:
	movl	-4(%ebp), %eax
	leave
	ret

waitpid:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$7, %eax
	movl	8(%ebp), %ecx
	int		$0x49
	movl	%eax, -0x4(%ebp)
	movl	12(%ebp), %eax
	cmpl	$0, %eax
	je		1f
	cmpl	$0x40000000, %eax
	jae		0f
	movl	$-1, -4(%ebp)
	jmp		1f
0:
	movl	%edx, (%eax)
1:
	popal
	movl	-0x4(%ebp), %eax
	leave
	ret

shm_create:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$8, %eax
	movl	8(%ebp), %esi
	int		$0x49
	movl	%eax, -0x4(%ebp)
	popal
	movl	-0x4(%ebp), %eax
	leave
	ret

shm_acquire:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$9, %eax
	movl	8(%ebp), %esi
	int		$0x49
	movl	%eax, -0x4(%ebp)
	popal
	movl	-0x4(%ebp), %eax
	leave
	ret

shm_release:
	pushl	%ebp
	movl	%esp, %ebp
	pushal
	movl	$10, %eax
	movl	8(%ebp), %esi
	int		$0x49
	popal
	leave
	ret

clear:
	pushl	%ebp
	movl	%esp, %ebp
	pushal
	movl	$13, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	popal
	leave
	ret

getpid:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$20, %eax
	int		$0x49
	movl	%eax, -0x4(%ebp)
	popal
	movl	-0x4(%ebp), %eax
	leave
	ret

cons_echo:
	pushl	%ebp
	movl	%esp, %ebp
	pushal
	movl	$27, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	popal
	leave
	ret

kill:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$37, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	movl	%eax, -0x4(%ebp)
	popal
	movl	-0x4(%ebp), %eax
	leave
	ret

signal:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$48, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	movl	%eax, -4(%ebp)
	popal
	movl	-4(%ebp), %eax
	leave
	ret

signaln:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$49, %eax
	movl	8(%ebp), %ecx
	movl	12(%ebp), %edx
	movzwl	%dx, %edx
	int		$0x49
	movl	%eax, -4(%ebp)
	popal
	movl	-4(%ebp), %eax
	leave
	ret

try_wait:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$60, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	movl	%eax, -4(%ebp)
	popal
	movl	-4(%ebp), %eax
	leave
	ret

wait:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$61, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	movl	%eax, -4(%ebp)
	popal
	movl	-4(%ebp), %eax
	leave
	ret

screate:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$64, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	movl	%eax, -4(%ebp)
	popal
	movl	-4(%ebp), %eax
	leave
	ret

sdelete:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$65, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	movl	%eax, -4(%ebp)
	popal
	movl	-4(%ebp), %eax
	leave
	ret

sreset:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$66, %eax
	movl	8(%ebp), %ecx
	movl	12(%ebp), %edx
	int		$0x49
	movl	%eax, -4(%ebp)
	popal
	movl	-4(%ebp), %eax
	leave
	ret

scount:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$67, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	movl	%eax, -4(%ebp)
	popal
	movl	-4(%ebp), %eax
	leave
	ret

getprio:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$96, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	movl	%eax, -0x4(%ebp)
	popal
	movl	-0x4(%ebp), %eax
	leave
	ret

chprio:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$97, %eax
	movl	8(%ebp), %ecx
	movl	12(%ebp), %edx
	int		$0x49
	movl	%eax, -0x4(%ebp)
	popal
	movl	-0x4(%ebp), %eax
	leave
	ret

sys_info:
	pushl	%ebp
	movl	%esp, %ebp
	pushal
	movl	$116, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	popal
	leave
	ret

current_clock:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$265, %eax
	int		$0x49
	movl	%eax, -0x4(%ebp)
	popal
	movl	-0x4(%ebp), %eax
	leave
	ret

clock_settings:
	pushl	%ebp
	movl	%esp, %ebp
	pushal
	movl	$266, %eax
	int		$0x49
	movl	8(%ebp), %eax
	movl	%ecx, (%eax)
	movl	12(%ebp), %eax
	movl	%edx, (%eax)
	popal
	leave
	ret

wait_clock:
	pushl	%ebp
	movl	%esp, %ebp
	pushal
	movl	$267, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	popal
	leave
	ret

pcreate:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$277, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	movl	%eax, -4(%ebp)
	popal
	movl	-4(%ebp), %eax
	leave
	ret

pdelete:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$278, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	movl	%eax, -4(%ebp)
	popal
	movl	-4(%ebp), %eax
	leave
	ret

psend:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$279, %eax
	movl	8(%ebp), %ecx
	movl	12(%ebp), %edx
	int		$0x49
	movl	%eax, -4(%ebp)
	popal
	movl	-4(%ebp), %eax
	leave
	ret

preceive:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$280, %eax
	movl	8(%ebp), %ecx
	int		$0x49
	movl	%eax, -4(%ebp)
	movl	12(%ebp), %eax
	cmpl	$0, %eax
	je		0f
	movl	%edx, (%eax)
0:
	popal
	movl	-4(%ebp), %eax
	leave
	ret

preset:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$281, %eax
	movl	8(%ebp), %ebx
	int		$0x49
	movl	%eax, -4(%ebp)
	popal
	movl	-4(%ebp), %eax
	leave
	ret

pcount:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
	pushal
	movl	$282, %eax
	movl	8(%ebp), %ecx
	int		$0x49
	movl	%eax, -4(%ebp)
	movl	12(%ebp), %eax
	cmpl	$0, %eax
	je		0f
	movl	%edx, (%eax)
0:
	popal
	movl	-4(%ebp), %eax
	leave
	ret
