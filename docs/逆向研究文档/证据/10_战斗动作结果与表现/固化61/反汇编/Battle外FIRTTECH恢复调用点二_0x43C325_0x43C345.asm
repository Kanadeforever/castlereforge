
/mnt/data/solid61_inputs/RPG_original.exe:     file format pei-i386


Disassembly of section .text:

0043c325 <.text+0x3b325>:
  43c325:	83 c0 04             	add    eax,0x4
  43c328:	3b ca                	cmp    ecx,edx
  43c32a:	7c ea                	jl     0x43c316
  43c32c:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  43c332:	55                   	push   ebp
  43c333:	e8 38 ee ff ff       	call   0x43b170
  43c338:	39 9e 84 05 00 00    	cmp    DWORD PTR [esi+0x584],ebx
  43c33e:	7e 06                	jle    0x43c346
  43c340:	89 9e 88 07 00     	mov    DWORD PTR [esi+0x788],ebx
