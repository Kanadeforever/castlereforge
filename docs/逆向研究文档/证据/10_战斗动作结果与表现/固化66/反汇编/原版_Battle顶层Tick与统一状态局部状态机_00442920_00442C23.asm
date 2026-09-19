
work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00442920 <.text+0x41920>:
  442920:	a0 0c 24 8e 00       	mov    al,ds:0x8e240c
  442925:	57                   	push   edi
  442926:	84 c0                	test   al,al
  442928:	8b f9                	mov    edi,ecx
  44292a:	0f 84 af 00 00 00    	je     0x4429df
  442930:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  442936:	53                   	push   ebx
  442937:	bb 01 00 00 00       	mov    ebx,0x1
  44293c:	56                   	push   esi
  44293d:	8b 81 f4 00 00 00    	mov    eax,DWORD PTR [ecx+0xf4]
  442943:	3b c3                	cmp    eax,ebx
  442945:	7e 11                	jle    0x442958
  442947:	8a 15 dc 96 46 00    	mov    dl,BYTE PTR ds:0x4696dc
  44294d:	84 d2                	test   dl,dl
  44294f:	74 07                	je     0x442958
  442951:	48                   	dec    eax
  442952:	89 81 f4 00 00 00    	mov    DWORD PTR [ecx+0xf4],eax
  442958:	a1 74 fd 89 00       	mov    eax,ds:0x89fd74
  44295d:	8a 88 fc 05 00 00    	mov    cl,BYTE PTR [eax+0x5fc]
  442963:	84 c9                	test   cl,cl
  442965:	74 3c                	je     0x4429a3
  442967:	c6 05 dc 96 46 00 00 	mov    BYTE PTR ds:0x4696dc,0x0
  44296e:	8a 87 30 e3 00 00    	mov    al,BYTE PTR [edi+0xe330]
  442974:	84 c0                	test   al,al
  442976:	c7 87 24 e2 00 00 08 	mov    DWORD PTR [edi+0xe224],0x8
  44297d:	00 00 00 
  442980:	74 0a                	je     0x44298c
  442982:	8a 87 2d e5 00 00    	mov    al,BYTE PTR [edi+0xe52d]
  442988:	84 c0                	test   al,al
  44298a:	75 17                	jne    0x4429a3
  44298c:	8b 0d 74 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fd74
  442992:	e8 c9 94 fd ff       	call   0x41be60
  442997:	88 9f 30 e3 00 00    	mov    BYTE PTR [edi+0xe330],bl
  44299d:	88 9f 2d e5 00 00    	mov    BYTE PTR [edi+0xe52d],bl
  4429a3:	8b cf                	mov    ecx,edi
  4429a5:	e8 d6 00 00 00       	call   0x442a80
  4429aa:	8d b7 e4 03 00 00    	lea    esi,[edi+0x3e4]
  4429b0:	bb 10 00 00 00       	mov    ebx,0x10
  4429b5:	8b 86 2c 07 00 00    	mov    eax,DWORD PTR [esi+0x72c]
  4429bb:	85 c0                	test   eax,eax
  4429bd:	7c 0e                	jl     0x4429cd
  4429bf:	8b ce                	mov    ecx,esi
  4429c1:	e8 3a d4 fd ff       	call   0x41fe00
  4429c6:	8b ce                	mov    ecx,esi
  4429c8:	e8 43 d9 fd ff       	call   0x420310
  4429cd:	81 c6 e4 0d 00 00    	add    esi,0xde4
  4429d3:	4b                   	dec    ebx
  4429d4:	75 df                	jne    0x4429b5
  4429d6:	8b cf                	mov    ecx,edi
  4429d8:	e8 93 ae ff ff       	call   0x43d870
  4429dd:	5e                   	pop    esi
  4429de:	5b                   	pop    ebx
  4429df:	5f                   	pop    edi
  4429e0:	c3                   	ret
  4429e1:	90                   	nop
  4429e2:	90                   	nop
  4429e3:	90                   	nop
  4429e4:	90                   	nop
  4429e5:	90                   	nop
  4429e6:	90                   	nop
  4429e7:	90                   	nop
  4429e8:	90                   	nop
  4429e9:	90                   	nop
  4429ea:	90                   	nop
  4429eb:	90                   	nop
  4429ec:	90                   	nop
  4429ed:	90                   	nop
  4429ee:	90                   	nop
  4429ef:	90                   	nop
  4429f0:	a0 0c 24 8e 00       	mov    al,ds:0x8e240c
  4429f5:	53                   	push   ebx
  4429f6:	33 db                	xor    ebx,ebx
  4429f8:	55                   	push   ebp
  4429f9:	3a c3                	cmp    al,bl
  4429fb:	8b e9                	mov    ebp,ecx
  4429fd:	74 78                	je     0x442a77
  4429ff:	53                   	push   ebx
  442a00:	e8 6b af ff ff       	call   0x43d970
  442a05:	84 c0                	test   al,al
  442a07:	74 6e                	je     0x442a77
  442a09:	56                   	push   esi
  442a0a:	57                   	push   edi
  442a0b:	33 ff                	xor    edi,edi
  442a0d:	8d b5 88 0f 00 00    	lea    esi,[ebp+0xf88]
  442a13:	39 9e 88 fb ff ff    	cmp    DWORD PTR [esi-0x478],ebx
  442a19:	7c 4e                	jl     0x442a69
  442a1b:	8b 85 30 02 00 00    	mov    eax,DWORD PTR [ebp+0x230]
  442a21:	38 1c 38             	cmp    BYTE PTR [eax+edi*1],bl
  442a24:	74 43                	je     0x442a69
  442a26:	8b 0e                	mov    ecx,DWORD PTR [esi]
  442a28:	53                   	push   ebx
  442a29:	8b 49 20             	mov    ecx,DWORD PTR [ecx+0x20]
  442a2c:	e8 ff c2 ff ff       	call   0x43ed30
  442a31:	8b 16                	mov    edx,DWORD PTR [esi]
  442a33:	53                   	push   ebx
  442a34:	8b 42 20             	mov    eax,DWORD PTR [edx+0x20]
  442a37:	c7 80 80 00 00 00 ff 	mov    DWORD PTR [eax+0x80],0xffffffff
  442a3e:	ff ff ff 
  442a41:	89 98 84 00 00 00    	mov    DWORD PTR [eax+0x84],ebx
  442a47:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  442a4d:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  442a53:	8b 06                	mov    eax,DWORD PTR [esi]
  442a55:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  442a58:	e8 d3 c9 ff ff       	call   0x43f430
  442a5d:	8b 0e                	mov    ecx,DWORD PTR [esi]
  442a5f:	6a 01                	push   0x1
  442a61:	8b 49 20             	mov    ecx,DWORD PTR [ecx+0x20]
  442a64:	e8 c7 c2 ff ff       	call   0x43ed30
  442a69:	47                   	inc    edi
  442a6a:	81 c6 e4 0d 00 00    	add    esi,0xde4
  442a70:	83 ff 10             	cmp    edi,0x10
  442a73:	7c 9e                	jl     0x442a13
  442a75:	5f                   	pop    edi
  442a76:	5e                   	pop    esi
  442a77:	5d                   	pop    ebp
  442a78:	5b                   	pop    ebx
  442a79:	c3                   	ret
  442a7a:	90                   	nop
  442a7b:	90                   	nop
  442a7c:	90                   	nop
  442a7d:	90                   	nop
  442a7e:	90                   	nop
  442a7f:	90                   	nop
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
  442b58:	8b 85 24 e2 00 00    	mov    eax,DWORD PTR [ebp+0xe224]
  442b5e:	83 cf ff             	or     edi,0xffffffff
  442b61:	3b c7                	cmp    eax,edi
  442b63:	0f 84 82 00 00 00    	je     0x442beb
  442b69:	8d 14 c0             	lea    edx,[eax+eax*8]
  442b6c:	8d 0c 90             	lea    ecx,[eax+edx*4]
  442b6f:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
  442b72:	8d 14 c8             	lea    edx,[eax+ecx*8]
  442b75:	8b 84 95 c8 10 00 00 	mov    eax,DWORD PTR [ebp+edx*4+0x10c8]
  442b7c:	3d bd 0b 00 00       	cmp    eax,0xbbd
  442b81:	74 68                	je     0x442beb
  442b83:	3d bc 0b 00 00       	cmp    eax,0xbbc
  442b88:	74 61                	je     0x442beb
  442b8a:	3d c1 0b 00 00       	cmp    eax,0xbc1
  442b8f:	74 5a                	je     0x442beb
  442b91:	8a 85 2d e5 00 00    	mov    al,BYTE PTR [ebp+0xe52d]
  442b97:	89 bd 24 e2 00 00    	mov    DWORD PTR [ebp+0xe224],edi
  442b9d:	84 c0                	test   al,al
  442b9f:	74 13                	je     0x442bb4
  442ba1:	8b 85 30 e5 00 00    	mov    eax,DWORD PTR [ebp+0xe530]
  442ba7:	c6 85 2d e5 00 00 00 	mov    BYTE PTR [ebp+0xe52d],0x0
  442bae:	89 85 ac e2 00 00    	mov    DWORD PTR [ebp+0xe2ac],eax
  442bb4:	8b 0d a4 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01a4
  442bba:	e8 d1 3f fe ff       	call   0x426b90
  442bbf:	8b 0d a4 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01a4
  442bc5:	e8 b6 3f fe ff       	call   0x426b80
  442bca:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  442bce:	33 c0                	xor    eax,eax
  442bd0:	6a 05                	push   0x5
  442bd2:	51                   	push   ecx
  442bd3:	8b 0d a4 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01a4
  442bd9:	50                   	push   eax
  442bda:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  442bde:	89 44 24 24          	mov    DWORD PTR [esp+0x24],eax
  442be2:	89 44 24 28          	mov    DWORD PTR [esp+0x28],eax
  442be6:	e8 95 3e fe ff       	call   0x426a80
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
  442c20:	8b                   	.byte 0x8b
  442c21:	85                   	.byte 0x85
  442c22:	24                   	.byte 0x24
