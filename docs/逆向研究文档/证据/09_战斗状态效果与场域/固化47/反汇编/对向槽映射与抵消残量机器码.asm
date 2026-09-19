
/mnt/data/work47/input/rpg/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042b500 <.text+0x2a500>:
  42b500:	03 ca                	add    ecx,edx
  42b502:	0f af 74 24 18       	imul   esi,DWORD PTR [esp+0x18]
  42b507:	8b 7c 24 40          	mov    edi,DWORD PTR [esp+0x40]
  42b50b:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  42b50f:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b514:	89 54 9f 10          	mov    DWORD PTR [edi+ebx*4+0x10],edx
  42b518:	f7 ee                	imul   esi
  42b51a:	c1 fa 05             	sar    edx,0x5
  42b51d:	8b c2                	mov    eax,edx
  42b51f:	c1 e8 1f             	shr    eax,0x1f
  42b522:	03 d0                	add    edx,eax
  42b524:	2b d1                	sub    edx,ecx
  42b526:	2b d5                	sub    edx,ebp
  42b528:	89 54 9f 24          	mov    DWORD PTR [edi+ebx*4+0x24],edx
  42b52c:	79 08                	jns    0x42b536
  42b52e:	c7 44 9f 24 00 00 00 	mov    DWORD PTR [edi+ebx*4+0x24],0x0
  42b535:	00 
  42b536:	8b 4c 9f 10          	mov    ecx,DWORD PTR [edi+ebx*4+0x10]
  42b53a:	8b 14 cd 68 a6 46 00 	mov    edx,DWORD PTR [ecx*8+0x46a668]
  42b541:	85 d2                	test   edx,edx
  42b543:	7c 24                	jl     0x42b569
  42b545:	8b 74 24 34          	mov    esi,DWORD PTR [esp+0x34]
  42b549:	8b 4c 9f 24          	mov    ecx,DWORD PTR [edi+ebx*4+0x24]
  42b54d:	8b 84 96 e8 01 00 00 	mov    eax,DWORD PTR [esi+edx*4+0x1e8]
  42b554:	3b c1                	cmp    eax,ecx
  42b556:	7e 0b                	jle    0x42b563
  42b558:	2b c1                	sub    eax,ecx
  42b55a:	89 84 96 e8 01 00 00 	mov    DWORD PTR [esi+edx*4+0x1e8],eax
  42b561:	eb 06                	jmp    0x42b569
  42b563:	2b c8                	sub    ecx,eax
  42b565:	89 4c 9f 24          	mov    DWORD PTR [edi+ebx*4+0x24],ecx
  42b569:	8b 44 9f 24          	mov    eax,DWORD PTR [edi+ebx*4+0x24]
  42b56d:	85 c0                	test   eax,eax
  42b56f:	7d 08                	jge    0x42b579
  42b571:	c7 44 9f 24 00 00 00 	mov    DWORD PTR [edi+ebx*4+0x24],0x0
  42b578:	00 
  42b579:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
  42b57d:	43                   	inc    ebx
  42b57e:	83 fb 05             	cmp    ebx,0x5
  42b581:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  42b585:	0f 8c 7d fb ff ff    	jl     0x42b108
  42b58b:	5f                   	pop    edi
  42b58c:	5e                   	pop    esi
  42b58d:	5d                   	pop    ebp
  42b58e:	5b                   	pop    ebx
  42b58f:	83               	add    esp,0x1c
