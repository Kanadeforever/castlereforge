
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040c7e0 <.text+0xb7e0>:
  40c7e0:	53                   	push   ebx
  40c7e1:	56                   	push   esi
  40c7e2:	8b f1                	mov    esi,ecx
  40c7e4:	57                   	push   edi
  40c7e5:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  40c7e8:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  40c7eb:	e8 b0 eb ff ff       	call   0x40b3a0
  40c7f0:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  40c7f3:	8b 80 80 02 00 00    	mov    eax,DWORD PTR [eax+0x280]
  40c7f9:	83 c1 04             	add    ecx,0x4
  40c7fc:	51                   	push   ecx
  40c7fd:	8b c8                	mov    ecx,eax
  40c7ff:	e8 4c e6 ff ff       	call   0x40ae50
  40c804:	8b c8                	mov    ecx,eax
  40c806:	e8 d5 d4 ff ff       	call   0x409ce0
  40c80b:	8b 56 08             	mov    edx,DWORD PTR [esi+0x8]
  40c80e:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  40c811:	8a 52 18             	mov    dl,BYTE PTR [edx+0x18]
  40c814:	88 91 e2 00 00 00    	mov    BYTE PTR [ecx+0xe2],dl
  40c81a:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  40c81d:	8a 51 1c             	mov    dl,BYTE PTR [ecx+0x1c]
  40c820:	8a 59 18             	mov    bl,BYTE PTR [ecx+0x18]
  40c823:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  40c826:	2a d3                	sub    dl,bl
  40c828:	fe c2                	inc    dl
  40c82a:	88 91 e3 00 00 00    	mov    BYTE PTR [ecx+0xe3],dl
  40c830:	8b 56 08             	mov    edx,DWORD PTR [esi+0x8]
  40c833:	b1 01                	mov    cl,0x1
  40c835:	8b 7a 20             	mov    edi,DWORD PTR [edx+0x20]
  40c838:	85 ff                	test   edi,edi
  40c83a:	75 16                	jne    0x40c852
  40c83c:	8b 50 70             	mov    edx,DWORD PTR [eax+0x70]
  40c83f:	c6 42 76 02          	mov    BYTE PTR [edx+0x76],0x2
  40c843:	8b 40 6c             	mov    eax,DWORD PTR [eax+0x6c]
  40c846:	88 88 80 00 00 00    	mov    BYTE PTR [eax+0x80],cl
  40c84c:	88 0d 1e f8 89 00    	mov    BYTE PTR ds:0x89f81e,cl
  40c852:	8b 15 10 f8 89 00    	mov    edx,DWORD PTR ds:0x89f810
  40c858:	56                   	push   esi
  40c859:	42                   	inc    edx
  40c85a:	c7 05 18 f8 89 00 00 	mov    DWORD PTR ds:0x89f818,0x0
  40c861:	00 00 00 
  40c864:	89 15 10 f8 89 00    	mov    DWORD PTR ds:0x89f810,edx
  40c86a:	88 0d 1d f8 89 00    	mov    BYTE PTR ds:0x89f81d,cl
  40c870:	e8 db 4c 04 00       	call   0x451550
  40c875:	83 c4 04             	add    esp,0x4
  40c878:	33 c0                	xor    eax,eax
  40c87a:	5f                   	pop    edi
  40c87b:	5e                   	pop    esi
  40c87c:	5b                   	pop    ebx
  40c87d:	c3                   	ret
  40c87e:	90                   	nop
  40c87f:	90                   	nop
