
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004425f0 <.text+0x415f0>:
  4425f0:	83 fa 08             	cmp    edx,0x8
  4425f3:	7d 0c                	jge    0x442601
  4425f5:	8d 42 14             	lea    eax,[edx+0x14]
  4425f8:	89 84 95 5c e5 00 00 	mov    DWORD PTR [ebp+edx*4+0xe55c],eax
  4425ff:	eb 17                	jmp    0x442618
  442601:	68 3c c3 46 00       	push   0x46c33c
  442606:	eb 05                	jmp    0x44260d
  442608:	68 10 c3 46 00       	push   0x46c310
  44260d:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  442613:	e8 58 f9 fe ff       	call   0x431f70
  442618:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  44261e:	47                   	inc    edi
  44261f:	83 c6 04             	add    esi,0x4
  442622:	3b 7a 3c             	cmp    edi,DWORD PTR [edx+0x3c]
  442625:	7c b3                	jl     0x4425da
  442627:	b8 c4 1a ff ff       	mov    eax,0xffff1ac4
  44262c:	33 db                	xor    ebx,ebx
  44262e:	2b c5                	sub    eax,ebp
  442630:	c6 44 24 13 01       	mov    BYTE PTR [esp+0x13],0x1
  442635:	8d b5 3c e5 00 00    	lea    esi,[ebp+0xe53c]
  44263b:	8d bd 80 0f 00 00    	lea    edi,[ebp+0xf80]
  442641:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  442645:	8a 44 24 13          	mov    al,BYTE PTR [esp+0x13]
  442649:	84 c0                	test   al,al
  44264b:	74 56                	je     0x4426a3
  44264d:	8b 85 4c 02 00 00    	mov    eax,DWORD PTR [ebp+0x24c]
  442653:	8d 8d c4 02 00 00    	lea    ecx,[ebp+0x2c4]
  442659:	51                   	push   ecx
  44265a:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  44265e:	03 c1                	add    eax,ecx
  442660:	8d 95 54 02 00 00    	lea    edx,[ebp+0x254]
  442666:	52                   	push   edx
  442667:	8b 16                	mov    edx,DWORD PTR [esi]
  442669:	8b 0c 30             	mov    ecx,DWORD PTR [eax+esi*1]
  44266c:	8b 86 68 1e ff ff    	mov    eax,DWORD PTR [esi-0xe198]
  442672:	51                   	push   ecx
  442673:	53                   	push   ebx
  442674:	52                   	push   edx
  442675:	50                   	push   eax
  442676:	8d 8f 64 f4 ff ff    	lea    ecx,[edi-0xb9c]
  44267c:	e8 2f cc fd ff       	call   0x41f2b0
  442681:	8b 0e                	mov    ecx,DWORD PTR [esi]
  442683:	43                   	inc    ebx
  442684:	89 0f                	mov    DWORD PTR [edi],ecx
  442686:	8b 16                	mov    edx,DWORD PTR [esi]
  442688:	89 57 04             	mov    DWORD PTR [edi+0x4],edx
  44268b:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  442691:	83 c6 04             	add    esi,0x4
  442694:	81 c7 e4 0d 00 00    	add    edi,0xde4
  44269a:	83 fb 10             	cmp    ebx,0x10
  44269d:	88 44 24 13          	mov    BYTE PTR [esp+0x13],al
  4426a1:	7c a2                	jl     0x442645
  4426a3:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  4426a7:	85 c0                	test   eax,eax
  4426a9:	0f 85 a0 00 00 00    	jne    0x44274f
  4426af:	8b                   	.byte 0x8b

/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00443ee0 <.text+0x42ee0>:
  443ee0:	85 c7                	test   edi,eax
  443ee2:	00 00                	add    BYTE PTR [eax],al
  443ee4:	00 85 c0 0f 8e bf    	add    BYTE PTR [ebp-0x4071f040],al
  443eea:	00 00                	add    BYTE PTR [eax],al
  443eec:	00 33                	add    BYTE PTR [ebx],dh
  443eee:	f6 8a 5c 34 24 8d 54 	test   BYTE PTR [edx-0x72dbcba4],0x54
  443ef5:	34 24                	xor    al,0x24
  443ef7:	84 db                	test   bl,bl
  443ef9:	89 54 24 18          	mov    DWORD PTR [esp+0x18],edx
  443efd:	0f 84 9f 00 00 00    	je     0x443fa2
  443f03:	8d 04 f6             	lea    eax,[esi+esi*8]
  443f06:	8d 04 86             	lea    eax,[esi+eax*4]
  443f09:	8d 14 40             	lea    edx,[eax+eax*2]
  443f0c:	8d 04 d6             	lea    eax,[esi+edx*8]
  443f0f:	8d 97 c4 02 00 00    	lea    edx,[edi+0x2c4]
  443f15:	52                   	push   edx
  443f16:	8b 97 4c 02 00 00    	mov    edx,DWORD PTR [edi+0x24c]
  443f1c:	8d 2c 87             	lea    ebp,[edi+eax*4]
  443f1f:	8d 87 54 02 00 00    	lea    eax,[edi+0x254]
  443f25:	50                   	push   eax
  443f26:	8b 04 b2             	mov    eax,DWORD PTR [edx+esi*4]
  443f29:	50                   	push   eax
  443f2a:	56                   	push   esi
  443f2b:	51                   	push   ecx
  443f2c:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  443f30:	8d 9d e4 03 00 00    	lea    ebx,[ebp+0x3e4]
  443f36:	51                   	push   ecx
  443f37:	8b cb                	mov    ecx,ebx
  443f39:	e8 72 b3 fd ff       	call   0x41f2b0
  443f3e:	8b cb                	mov    ecx,ebx
  443f40:	e8 8b f2 fd ff       	call   0x4231d0
  443f45:	6a 00                	push   0x0
  443f47:	6a 00                	push   0x0
  443f49:	6a 67                	push   0x67
  443f4b:	8b cb                	mov    ecx,ebx
  443f4d:	e8 ce e1 fd ff       	call   0x422120
  443f52:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  443f56:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  443f5a:	89 94 b7 a4 03 00 00 	mov    DWORD PTR [edi+esi*4+0x3a4],edx
  443f61:	89 84 b7 3c e5 00 00 	mov    DWORD PTR [edi+esi*4+0xe53c],eax
  443f68:	8b 8d 5c 0c 00 00    	mov    ecx,DWORD PTR [ebp+0xc5c]
  443f6e:	c6                   	.byte 0xc6
  443f6f:	44                   	inc    esp
