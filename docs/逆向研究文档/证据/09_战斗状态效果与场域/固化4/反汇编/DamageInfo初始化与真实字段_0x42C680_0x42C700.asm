
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042c680 <.text+0x2b680>:
  42c680:	83 ec 0c             	sub    esp,0xc
  42c683:	53                   	push   ebx
  42c684:	55                   	push   ebp
  42c685:	56                   	push   esi
  42c686:	8b 74 24 24          	mov    esi,DWORD PTR [esp+0x24]
  42c68a:	8b e9                	mov    ebp,ecx
  42c68c:	33 c0                	xor    eax,eax
  42c68e:	33 c9                	xor    ecx,ecx
  42c690:	8d 56 10             	lea    edx,[esi+0x10]
  42c693:	88 0e                	mov    BYTE PTR [esi],cl
  42c695:	89 4e 04             	mov    DWORD PTR [esi+0x4],ecx
  42c698:	89 4e 08             	mov    DWORD PTR [esi+0x8],ecx
  42c69b:	88 4e 0c             	mov    BYTE PTR [esi+0xc],cl
  42c69e:	89 02                	mov    DWORD PTR [edx],eax
  42c6a0:	57                   	push   edi
  42c6a1:	8b 7c 24 2c          	mov    edi,DWORD PTR [esp+0x2c]
  42c6a5:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  42c6a9:	89 42 04             	mov    DWORD PTR [edx+0x4],eax
  42c6ac:	3b f9                	cmp    edi,ecx
  42c6ae:	88 4c 24 13          	mov    BYTE PTR [esp+0x13],cl
  42c6b2:	89 42 08             	mov    DWORD PTR [edx+0x8],eax
  42c6b5:	89 42 0c             	mov    DWORD PTR [edx+0xc],eax
  42c6b8:	89 42 10             	mov    DWORD PTR [edx+0x10],eax
  42c6bb:	8d 56 24             	lea    edx,[esi+0x24]
  42c6be:	89 46 24             	mov    DWORD PTR [esi+0x24],eax
  42c6c1:	89 42 04             	mov    DWORD PTR [edx+0x4],eax
  42c6c4:	89 42 08             	mov    DWORD PTR [edx+0x8],eax
  42c6c7:	89 42 0c             	mov    DWORD PTR [edx+0xc],eax
  42c6ca:	89 42 10             	mov    DWORD PTR [edx+0x10],eax
  42c6cd:	89 4e 38             	mov    DWORD PTR [esi+0x38],ecx
  42c6d0:	89 4e 3c             	mov    DWORD PTR [esi+0x3c],ecx
  42c6d3:	88 46 40             	mov    BYTE PTR [esi+0x40],al
  42c6d6:	c7 46 44 01 00 00 00 	mov    DWORD PTR [esi+0x44],0x1
  42c6dd:	88 46 48             	mov    BYTE PTR [esi+0x48],al
  42c6e0:	c7 46 4c ff ff ff ff 	mov    DWORD PTR [esi+0x4c],0xffffffff
  42c6e7:	0f 84 9f 05 00 00    	je     0x42cc8c
  42c6ed:	8b 47 18             	mov    eax,DWORD PTR [edi+0x18]
  42c6f0:	3b c1                	cmp    eax,ecx
  42c6f2:	89 44 24 28          	mov    DWORD PTR [esp+0x28],eax
  42c6f6:	89 46 4c             	mov    DWORD PTR [esi+0x4c],eax
  42c6f9:	0f 85 aa 00 00 00    	jne    0x42c7a9
  42c6ff:	8b             	mov    eax,DWORD PTR [esp+0x20]
