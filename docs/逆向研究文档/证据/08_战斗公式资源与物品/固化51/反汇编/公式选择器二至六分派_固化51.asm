; 固化51：FIRTTECH+0x30 selector 2..6 分派与五个公式helper。

/mnt/data/work50/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042c7a9 <.text+0x2b7a9>:
  42c7a9:	83 f8 04             	cmp    eax,0x4
  42c7ac:	0f 84 be 04 00 00    	je     0x42cc70
  42c7b2:	8b 47 30             	mov    eax,DWORD PTR [edi+0x30]
  42c7b5:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  42c7b9:	83 c0 fe             	add    eax,0xfffffffe
  42c7bc:	83 f8 04             	cmp    eax,0x4
  42c7bf:	77 6b                	ja     0x42c82c
  42c7c1:	ff 24 85 98 cc 42 00 	jmp    DWORD PTR [eax*4+0x42cc98]
  42c7c8:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  42c7cc:	8b 5c 24 20          	mov    ebx,DWORD PTR [esp+0x20]
  42c7d0:	57                   	push   edi
  42c7d1:	50                   	push   eax
  42c7d2:	53                   	push   ebx
  42c7d3:	8b cd                	mov    ecx,ebp
  42c7d5:	e8 56 ee ff ff       	call   0x42b630
  42c7da:	eb 62                	jmp    0x42c83e
  42c7dc:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42c7e0:	8b 5c 24 20          	mov    ebx,DWORD PTR [esp+0x20]
  42c7e4:	57                   	push   edi
  42c7e5:	51                   	push   ecx
  42c7e6:	53                   	push   ebx
  42c7e7:	8b cd                	mov    ecx,ebp
  42c7e9:	e8 d2 ee ff ff       	call   0x42b6c0
  42c7ee:	eb 4e                	jmp    0x42c83e
  42c7f0:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
  42c7f4:	8b 5c 24 20          	mov    ebx,DWORD PTR [esp+0x20]
  42c7f8:	57                   	push   edi
  42c7f9:	52                   	push   edx
  42c7fa:	53                   	push   ebx
  42c7fb:	8b cd                	mov    ecx,ebp
  42c7fd:	e8 4e ef ff ff       	call   0x42b750
  42c802:	eb 3a                	jmp    0x42c83e
  42c804:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  42c808:	8b 5c 24 20          	mov    ebx,DWORD PTR [esp+0x20]
  42c80c:	57                   	push   edi
  42c80d:	50                   	push   eax
  42c80e:	53                   	push   ebx
  42c80f:	8b cd                	mov    ecx,ebp
  42c811:	e8 ba ef ff ff       	call   0x42b7d0
  42c816:	eb 26                	jmp    0x42c83e
  42c818:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42c81c:	8b 5c 24 20          	mov    ebx,DWORD PTR [esp+0x20]
  42c820:	57                   	push   edi
  42c821:	51                   	push   ecx
  42c822:	53                   	push   ebx
  42c823:	8b cd                	mov    ecx,ebp
  42c825:	e8 16 f0 ff ff       	call   0x42b840
  42c82a:	eb 12                	jmp    0x42c83e
  42c82c:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
  42c830:	8b 5c 24 20          	mov    ebx,DWORD PTR [esp+0x20]
  42c834:	57                   	push   edi
  42c835:	52                   	push   edx
  42c836:	53                   	push   ebx
  42c837:	8b cd                	mov    ecx,ebp
  42c839:	e8 b2 ed ff ff       	call   0x42b5f0
  42c83e:	53                   	push   ebx
  42c83f:	8b cd                	mov    ecx,ebp
  42c841:	89 44 24 24          	mov    DWORD PTR [esp+0x24],eax
  42c845:	e8 06 05 00 00       	call   0x42cd50
  42c84a:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  42c84e:	03 c8                	add    ecx,eax

; 五项jump table：

/mnt/data/work50/input/RPG.exe:     file format pei-i386

Contents of section .text:
 42cc98 c8c74200 dcc74200 f0c74200 04c84200  ..B...B...B...B.
 42cca8 18c84200                             ..B.            
