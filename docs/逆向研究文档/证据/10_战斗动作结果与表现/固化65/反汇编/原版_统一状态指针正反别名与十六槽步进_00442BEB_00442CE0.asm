
work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00442beb <.text+0x41beb>:
  442beb:	8d b5 c8 10 00 00    	lea    esi,[ebp+0x10c8]
  442bf1:	b9 10 00 00 00       	mov    ecx,0x10
  442bf6:	8b c6                	mov    eax,esi
  442bf8:	81 38 bc 0b 00 00    	cmp    DWORD PTR [eax],0xbbc
  442bfe:	75 18                	jne    0x442c18
  442c00:	8b 90 98 fb ff ff    	mov    edx,DWORD PTR [eax-0x468]
  442c06:	85 d2                	test   edx,edx
  442c08:	74 08                	je     0x442c12
  442c0a:	39 b8 48 fa ff ff    	cmp    DWORD PTR [eax-0x5b8],edi
  442c10:	75 06                	jne    0x442c18
  442c12:	c7 00 b9 0b 00 00    	mov    DWORD PTR [eax],0xbb9
  442c18:	05 e4 0d 00 00       	add    eax,0xde4
  442c1d:	49                   	dec    ecx
  442c1e:	75 d8                	jne    0x442bf8
  442c20:	8b 85 24 e2 00 00    	mov    eax,DWORD PTR [ebp+0xe224]
  442c26:	3b c7                	cmp    eax,edi
  442c28:	74 50                	je     0x442c7a
  442c2a:	8d 14 c0             	lea    edx,[eax+eax*8]
  442c2d:	8d 0c 90             	lea    ecx,[eax+edx*4]
  442c30:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
  442c33:	8d 14 c8             	lea    edx,[eax+ecx*8]
  442c36:	8b 8c 95 60 0c 00 00 	mov    ecx,DWORD PTR [ebp+edx*4+0xc60]
  442c3d:	85 c9                	test   ecx,ecx
  442c3f:	8d 44 95 00          	lea    eax,[ebp+edx*4+0x0]
  442c43:	74 08                	je     0x442c4d
  442c45:	39 b8 10 0b 00 00    	cmp    DWORD PTR [eax+0xb10],edi
  442c4b:	75 2d                	jne    0x442c7a
  442c4d:	8a 85 2d e5 00 00    	mov    al,BYTE PTR [ebp+0xe52d]
  442c53:	89 bd 24 e2 00 00    	mov    DWORD PTR [ebp+0xe224],edi
  442c59:	84 c0                	test   al,al
  442c5b:	74 13                	je     0x442c70
  442c5d:	8b 85 30 e5 00 00    	mov    eax,DWORD PTR [ebp+0xe530]
  442c63:	c6 85 2d e5 00 00 00 	mov    BYTE PTR [ebp+0xe52d],0x0
  442c6a:	89 85 ac e2 00 00    	mov    DWORD PTR [ebp+0xe2ac],eax
  442c70:	c7 85 e4 02 00 00 b9 	mov    DWORD PTR [ebp+0x2e4],0xbb9
  442c77:	0b 00 00 
  442c7a:	8d bd e4 03 00 00    	lea    edi,[ebp+0x3e4]
  442c80:	bb 10 00 00 00       	mov    ebx,0x10
  442c85:	8b cf                	mov    ecx,edi
  442c87:	e8 04 0a fe ff       	call   0x423690
  442c8c:	81 c7 e4 0d 00 00    	add    edi,0xde4
  442c92:	4b                   	dec    ebx
  442c93:	75 f0                	jne    0x442c85
  442c95:	83 bd 24 e2 00 00 ff 	cmp    DWORD PTR [ebp+0xe224],0xffffffff
  442c9c:	0f 85 1c 01 00 00    	jne    0x442dbe
  442ca2:	a0 dc 96 46 00       	mov    al,ds:0x4696dc
  442ca7:	84 c0                	test   al,al
  442ca9:	0f 84 0f 01 00 00    	je     0x442dbe
  442caf:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  442cb4:	e8 f7 ad ff ff       	call   0x43dab0
  442cb9:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  442cbe:	e8 ed ad ff ff       	call   0x43dab0
  442cc3:	33 ff                	xor    edi,edi
  442cc5:	8b 86 48 fa ff ff    	mov    eax,DWORD PTR [esi-0x5b8]
  442ccb:	85 c0                	test   eax,eax
  442ccd:	0f 8c db 00 00 00    	jl     0x442dae
  442cd3:	8d ae 1c f3 ff ff    	lea    ebp,[esi-0xce4]
  442cd9:	8b cd                	mov    ecx,ebp
  442cdb:	e8 60 d7 fd ff       	call   0x420440
