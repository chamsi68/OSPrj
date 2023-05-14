#include "segment.h"
	.text
	.globl	ctx_sw
// Structure de la pile en entree :
// %esp : adresse de retour (old %eip)
ctx_sw:
	// sauvegarde du contexte de l'ancien processus
	pushl	%eax
	movl	previous_running_proc, %eax
	popl	28(%eax)
	movl	%ebx, 32(%eax)
	movl	%ecx, 36(%eax)
	movl	%edx, 40(%eax)
	movl	%esi, 44(%eax)
	movl	%edi, 48(%eax)
	movl	%ebp, 52(%eax)
#if (!VM)
	movl	%esp, 56(%eax)
#else
	movl	%esp, 60(%eax)
#endif
	// restauration du contexte du nouveau processus
	movl	running_proc, %eax
	movl	16(%eax), %ebx
	movl	%ebx, %cr3
#if (!VM)
	movl	56(%eax), %esp
#else
	movl	60(%eax), %esp
	cmpl	$0, 68(%eax)
	jne		0f

	movl	%esp, tss+4
	movl	56(%eax), %ebx
	addl	$4, %ebx
	movl	%ebx, -8(%esp)
	movl	-4(%ebx), %ebx
	movl	%ebx, -20(%esp)
	pushl	$0x4b
	subl	$4, %esp
	pushfl
	orl		$0x200, (%esp)
	pushl	$0x43
	subl	$4, %esp
	iret
0:
#endif
	movl	52(%eax), %ebp
	movl	48(%eax), %edi
	movl	44(%eax), %esi
	movl	40(%eax), %edx
	movl	36(%eax), %ecx
	movl	32(%eax), %ebx
	movl	28(%eax), %eax
	ret
