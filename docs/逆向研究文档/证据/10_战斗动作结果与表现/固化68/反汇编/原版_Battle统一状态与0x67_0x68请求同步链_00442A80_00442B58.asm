
work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00442a80 <.text+0x41a80>:
  442a80:	83 ec 10             	sub    esp,0x10
  442a83:	53                   	push   ebx
  442a84:	55                   	push   ebp
  442a85:	8b e9                	mov    ebp,ecx
  442a87:	56                   	push   esi
  442a88:	57                   	push   edi
  442a89:	89 6c 24 10          	mov    DWORD PTR [esp+0x10],ebp
  442a8d:	8d b5 d0 10 00 00    	lea    esi,[ebp+0x10d0]
  442a93:	bb 10 00 00 00       	mov    ebx,0x10
  442a98:	bf ba 0b 00 00       	mov    edi,0xbba
  442a9d:	8b 86 40 fa ff ff    	mov    eax,DWORD PTR [esi-0x5c0]
  442aa3:	85 c0                	test   eax,eax
  442aa5:	0f 8c a0 00 00 00    	jl     0x442b4b
  442aab:	81 7e f4 d4 07 00 00 	cmp    DWORD PTR [esi-0xc],0x7d4
  442ab2:	75 19                	jne    0x442acd
  442ab4:	83 3e 67             	cmp    DWORD PTR [esi],0x67
  442ab7:	75 14                	jne    0x442acd
  442ab9:	6a 00                	push   0x0
  442abb:	6a 00                	push   0x0
  442abd:	6a 68                	push   0x68
  442abf:	8d 8e 14 f3 ff ff    	lea    ecx,[esi-0xcec]
  442ac5:	e8 56 f6 fd ff       	call   0x422120
  442aca:	89 7e f8             	mov    DWORD PTR [esi-0x8],edi
  442acd:	39 7e f8             	cmp    DWORD PTR [esi-0x8],edi
  442ad0:	75 23                	jne    0x442af5
  442ad2:	83 3e 68             	cmp    DWORD PTR [esi],0x68
  442ad5:	74 1e                	je     0x442af5
  442ad7:	8b 86 b8 fe ff ff    	mov    eax,DWORD PTR [esi-0x148]
  442add:	8a 48 14             	mov    cl,BYTE PTR [eax+0x14]
  442ae0:	84 c9                	test   cl,cl
  442ae2:	75 11                	jne    0x442af5
  442ae4:	6a 00                	push   0x0
  442ae6:	6a 00                	push   0x0
  442ae8:	6a 68                	push   0x68
  442aea:	8d 8e 14 f3 ff ff    	lea    ecx,[esi-0xcec]
  442af0:	e8 2b f6 fd ff       	call   0x422120
  442af5:	81 7e f8 b9 0b 00 00 	cmp    DWORD PTR [esi-0x8],0xbb9
  442afc:	75 23                	jne    0x442b21
  442afe:	83 3e 67             	cmp    DWORD PTR [esi],0x67
  442b01:	74 1e                	je     0x442b21
  442b03:	8b 8e b8 fe ff ff    	mov    ecx,DWORD PTR [esi-0x148]
  442b09:	8a 41 14             	mov    al,BYTE PTR [ecx+0x14]
  442b0c:	84 c0                	test   al,al
  442b0e:	75 11                	jne    0x442b21
  442b10:	6a 00                	push   0x0
  442b12:	6a 00                	push   0x0
  442b14:	6a 67                	push   0x67
  442b16:	8d 8e 14 f3 ff ff    	lea    ecx,[esi-0xcec]
  442b1c:	e8 ff f5 fd ff       	call   0x422120
  442b21:	81 7e f8 be 0b 00 00 	cmp    DWORD PTR [esi-0x8],0xbbe
  442b28:	75 21                	jne    0x442b4b
  442b2a:	81 7e f4 d1 07 00 00 	cmp    DWORD PTR [esi-0xc],0x7d1
  442b31:	75 18                	jne    0x442b4b
  442b33:	6a 00                	push   0x0
  442b35:	6a 00                	push   0x0
  442b37:	6a 67                	push   0x67
  442b39:	8d 8e 14 f3 ff ff    	lea    ecx,[esi-0xcec]
  442b3f:	e8 dc f5 fd ff       	call   0x422120
  442b44:	c7 46 f8 b9 0b 00 00 	mov    DWORD PTR [esi-0x8],0xbb9
  442b4b:	81 c6 e4 0d 00 00    	add    esi,0xde4
  442b51:	4b                   	dec    ebx
  442b52:	0f 85 45 ff ff ff    	jne    0x442a9d
