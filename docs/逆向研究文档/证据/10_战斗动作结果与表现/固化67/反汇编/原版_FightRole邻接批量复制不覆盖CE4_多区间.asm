
work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041f63d <.text+0x1e63d>:
  41f63d:	8b 87 2c 07 00 00    	mov    eax,DWORD PTR [edi+0x72c]
  41f643:	8d 97 4c 08 00 00    	lea    edx,[edi+0x84c]
  41f649:	85 c0                	test   eax,eax
  41f64b:	89 02                	mov    DWORD PTR [edx],eax
  41f64d:	7c 63                	jl     0x41f6b2
  41f64f:	8d 0c 40             	lea    ecx,[eax+eax*2]
  41f652:	8b fa                	mov    edi,edx
  41f654:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  41f657:	d1 e1                	shl    ecx,1
  41f659:	2b c8                	sub    ecx,eax
  41f65b:	c1 e1 04             	shl    ecx,0x4
  41f65e:	8d 34 0b             	lea    esi,[ebx+ecx*1]
  41f661:	b9 d4 00 00 00       	mov    ecx,0xd4
  41f666:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  41f668:	39 02                	cmp    DWORD PTR [edx],eax
  41f66a:	74 57                	je     0x41f6c3
  41f66c:	85 db                	test   ebx,ebx
  41f66e:	74 09                	je     0x41f679
  41f670:	53                   	push   ebx
  41f671:	e8 da 1e 03 00       	call   0x451550
  41f676:	83 c4 04             	add    esp,0x4
  41f679:	68 ac 9e 46 00       	push   0x469eac
  41f67e:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  41f684:	6a 00                	push   0x0
  41f686:	e8 e5 27 01 00       	call   0x431e70
  41f68b:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  41f68f:	8a d8                	mov    bl,al
  41f691:	c7 44 24 28 ff ff ff 	mov    DWORD PTR [esp+0x28],0xffffffff
  41f698:	ff 
  41f699:	e8 72 20 02 00       	call   0x441710
  41f69e:	5f                   	pop    edi
  41f69f:	8a c3                	mov    al,bl
  41f6a1:	5e                   	pop    esi
  41f6a2:	5b                   	pop    ebx
  41f6a3:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  41f6a7:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  41f6ae:	83 c4 20             	add    esp,0x20
  41f6b1:	c3                   	ret
  41f6b2:	b9 d4 00 00 00       	mov    ecx,0xd4
  41f6b7:	8b f3                	mov    esi,ebx
  41f6b9:	8b fa                	mov    edi,edx
  41f6bb:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  41f6bd:	c7 02 ff ff ff ff    	mov    DWORD PTR [edx],0xffffffff

work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00423a00 <.text+0x22a00>:
  423a00:	53                   	push   ebx
  423a01:	55                   	push   ebp
  423a02:	8b 6c 24 0c          	mov    ebp,DWORD PTR [esp+0xc]
  423a06:	8b d9                	mov    ebx,ecx
  423a08:	56                   	push   esi
  423a09:	57                   	push   edi
  423a0a:	8d bb fc 0c 00 00    	lea    edi,[ebx+0xcfc]
  423a10:	b9 39 00 00 00       	mov    ecx,0x39
  423a15:	8b f5                	mov    esi,ebp
  423a17:	33 c0                	xor    eax,eax
  423a19:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  423a1b:	8b 4d 08             	mov    ecx,DWORD PTR [ebp+0x8]
  423a1e:	85 c9                	test   ecx,ecx
  423a20:	89 8b c8 00 00 00    	mov    DWORD PTR [ebx+0xc8],ecx
  423a26:	7e 1e                	jle    0x423a46
  423a28:	8d 93 88 00 00 00    	lea    edx,[ebx+0x88]
  423a2e:	8d 4d 0c             	lea    ecx,[ebp+0xc]
  423a31:	8b 31                	mov    esi,DWORD PTR [ecx]
  423a33:	40                   	inc    eax
  423a34:	89 32                	mov    DWORD PTR [edx],esi
  423a36:	8b b3 c8 00 00 00    	mov    esi,DWORD PTR [ebx+0xc8]
  423a3c:	83 c1 04             	add    ecx,0x4
  423a3f:	83 c2 04             	add    edx,0x4
  423a42:	3b c6                	cmp    eax,esi
  423a44:	7c eb                	jl     0x423a31
  423a46:	8b 8b f4 08 00 00    	mov    ecx,DWORD PTR [ebx+0x8f4]
  423a4c:	8b 45 00             	mov    eax,DWORD PTR [ebp+0x0]
  423a4f:	85 c9                	test   ecx,ecx
  423a51:	89 83 f0 0c 00 00    	mov    DWORD PTR [ebx+0xcf0],eax
  423a57:	7e 05                	jle    0x423a5e
  423a59:	83 f8 03             	cmp    eax,0x3
  423a5c:	74 0c                	je     0x423a6a

work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00441fb8 <.text+0x40fb8>:
  441fb8:	8d 8d 84 f7 ff ff    	lea    ecx,[ebp-0x87c]
  441fbe:	e8 8d 15 fe ff       	call   0x423550
  441fc3:	83 7d 00 00          	cmp    DWORD PTR [ebp+0x0],0x0
  441fc7:	7f 07                	jg     0x441fd0
  441fc9:	c7 45 00 01 00 00 00 	mov    DWORD PTR [ebp+0x0],0x1
  441fd0:	8d 04 5b             	lea    eax,[ebx+ebx*2]
  441fd3:	8d 3c c0             	lea    edi,[eax+eax*8]
  441fd6:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  441fdb:	d1 e7                	shl    edi,1
  441fdd:	8b 88 a4 00 00 00    	mov    ecx,DWORD PTR [eax+0xa4]
  441fe3:	2b fb                	sub    edi,ebx
  441fe5:	c1 e7 04             	shl    edi,0x4
  441fe8:	03 f9                	add    edi,ecx
  441fea:	b9 d4 00 00 00       	mov    ecx,0xd4
  441fef:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]

work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

004426d3 <.text+0x416d3>:
  4426d3:	8b 44 13 d8          	mov    eax,DWORD PTR [ebx+edx*1-0x28]
  4426d7:	8b 74 13 ac          	mov    esi,DWORD PTR [ebx+edx*1-0x54]
  4426db:	8d 0c c0             	lea    ecx,[eax+eax*8]
  4426de:	8d 0c 88             	lea    ecx,[eax+ecx*4]
  4426e1:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
  4426e4:	8d 04 c8             	lea    eax,[eax+ecx*8]
  4426e7:	b9 d4 00 00 00       	mov    ecx,0xd4
  4426ec:	8d 44 85 00          	lea    eax,[ebp+eax*4+0x0]
  4426f0:	8d b8 50 7b 00 00    	lea    edi,[eax+0x7b50]
  4426f6:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  4426f8:	8d 88 04 73 00 00    	lea    ecx,[eax+0x7304]
  4426fe:	c7 80 ec 7b 00 00 00 	mov    DWORD PTR [eax+0x7bec],0x0
  442705:	00 00 00 
