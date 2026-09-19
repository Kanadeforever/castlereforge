
/mnt/data/solid61_inputs/RPG_original.exe:     file format pei-i386


Disassembly of section .text:

0042c460 <.text+0x2b460>:
  42c460:	8a 44 24 10          	mov    al,BYTE PTR [esp+0x10]
  42c464:	53                   	push   ebx
  42c465:	56                   	push   esi
  42c466:	57                   	push   edi
  42c467:	84 c0                	test   al,al
  42c469:	74 11                	je     0x42c47c
  42c46b:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  42c46f:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  42c473:	8b 57 40             	mov    edx,DWORD PTR [edi+0x40]
  42c476:	0f af 53 2c          	imul   edx,DWORD PTR [ebx+0x2c]
  42c47a:	eb 0f                	jmp    0x42c48b
  42c47c:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  42c480:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  42c484:	8b 53 34             	mov    edx,DWORD PTR [ebx+0x34]
  42c487:	0f af 57 40          	imul   edx,DWORD PTR [edi+0x40]
  42c48b:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42c490:	6a 04                	push   0x4
  42c492:	f7 ea                	imul   edx
  42c494:	c1 fa 05             	sar    edx,0x5
  42c497:	8b c2                	mov    eax,edx
  42c499:	c1 e8 1f             	shr    eax,0x1f
  42c49c:	03 d0                	add    edx,eax
  42c49e:	8b f2                	mov    esi,edx
  42c4a0:	8b 57 4c             	mov    edx,DWORD PTR [edi+0x4c]
  42c4a3:	03 f2                	add    esi,edx
  42c4a5:	8b 53 04             	mov    edx,DWORD PTR [ebx+0x4]
  42c4a8:	52                   	push   edx
  42c4a9:	e8 92 fa ff ff       	call   0x42bf40
  42c4ae:	84 c0                	test   al,al
  42c4b0:	74 02                	je     0x42c4b4
  42c4b2:	33 f6                	xor    esi,esi
  42c4b4:	85 f6                	test   esi,esi
  42c4b6:	8b c6                	mov    eax,esi
  42c4b8:	7d 02                	jge    0x42c4bc
  42c4ba:	33 c0                	xor    eax,eax
  42c4bc:	8b 8b 14 02 00 00    	mov    ecx,DWORD PTR [ebx+0x214]
  42c4c2:	5f                   	pop    edi
  42c4c3:	5e                   	pop    esi
  42c4c4:	5b                   	pop    ebx
  42c4c5:	85 c9                	test   ecx,ecx
  42c4c7:	7f 02                	jg     0x42c4cb
  42c4c9:	f7 d8                	neg    eax
  42c4cb:	c2               	ret    0x10
