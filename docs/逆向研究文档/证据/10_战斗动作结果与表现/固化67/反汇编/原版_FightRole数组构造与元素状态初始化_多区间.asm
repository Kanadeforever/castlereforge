
work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041f070 <.text+0x1e070>:
  41f070:	6a ff                	push   0xffffffff
  41f072:	68 8a e2 45 00       	push   0x45e28a
  41f077:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  41f07d:	50                   	push   eax
  41f07e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  41f085:	51                   	push   ecx
  41f086:	53                   	push   ebx
  41f087:	55                   	push   ebp
  41f088:	56                   	push   esi
  41f089:	8b f1                	mov    esi,ecx
  41f08b:	57                   	push   edi
  41f08c:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  41f090:	8d 8e 30 07 00 00    	lea    ecx,[esi+0x730]
  41f096:	e8 95 57 02 00       	call   0x444830
  41f09b:	33 db                	xor    ebx,ebx
  41f09d:	8d 8e b4 07 00 00    	lea    ecx,[esi+0x7b4]
  41f0a3:	89 5c 24 1c          	mov    DWORD PTR [esp+0x1c],ebx
  41f0a7:	e8 74 60 02 00       	call   0x445120
  41f0ac:	8d 8e a8 0c 00 00    	lea    ecx,[esi+0xca8]
  41f0b2:	c6 44 24 1c 01       	mov    BYTE PTR [esp+0x1c],0x1
  41f0b7:	e8 c4 4c 00 00       	call   0x423d80
  41f0bc:	83 cd ff             	or     ebp,0xffffffff
  41f0bf:	88 9e d8 0d 00 00    	mov    BYTE PTR [esi+0xdd8],bl
  41f0c5:	89 9e 04 0d 00 00    	mov    DWORD PTR [esi+0xd04],ebx
  41f0cb:	89 ae d0 0d 00 00    	mov    DWORD PTR [esi+0xdd0],ebp
  41f0d1:	89 ae d4 0d 00 00    	mov    DWORD PTR [esi+0xdd4],ebp
  41f0d7:	c7 86 fc 0c 00 00 04 	mov    DWORD PTR [esi+0xcfc],0x4
  41f0de:	00 00 00 
  41f0e1:	a0 f4 fd 89 00       	mov    al,ds:0x89fdf4
  41f0e6:	c6 44 24 1c 02       	mov    BYTE PTR [esp+0x1c],0x2
  41f0eb:	3a c3                	cmp    al,bl
  41f0ed:	75 15                	jne    0x41f104
  41f0ef:	b9 10 00 00 00       	mov    ecx,0x10
  41f0f4:	33 c0                	xor    eax,eax
  41f0f6:	bf 94 fd 89 00       	mov    edi,0x89fd94
  41f0fb:	c6 05 f4 fd 89 00 01 	mov    BYTE PTR ds:0x89fdf4,0x1
  41f102:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  41f104:	8b ce                	mov    ecx,esi
  41f106:	e8 25 01 00 00       	call   0x41f230
  41f10b:	33 c0                	xor    eax,eax
  41f10d:	c7 86 e0 0c 00 00 d1 	mov    DWORD PTR [esi+0xce0],0x7d1
  41f114:	07 00 00 
  41f117:	c7 86 e4 0c 00 00 b9 	mov    DWORD PTR [esi+0xce4],0xbb9
  41f11e:	0b 00 00 
  41f121:	c7 86 e8 0c 00 00 a0 	mov    DWORD PTR [esi+0xce8],0xfa0
  41f128:	0f 00 00 
  41f12b:	89 9e f0 0c 00 00    	mov    DWORD PTR [esi+0xcf0],ebx
  41f131:	89 9e c8 00 00 00    	mov    DWORD PTR [esi+0xc8],ebx
  41f137:	89 9e 68 01 00 00    	mov    DWORD PTR [esi+0x168],ebx
  41f13d:	89 9e 6c 01 00 00    	mov    DWORD PTR [esi+0x16c],ebx
  41f143:	88 9e f8 0c 00 00    	mov    BYTE PTR [esi+0xcf8],bl
  41f149:	89 1e                	mov    DWORD PTR [esi],ebx
  41f14b:	88 5e 04             	mov    BYTE PTR [esi+0x4],bl
  41f14e:	89 ae e0 0d 00 00    	mov    DWORD PTR [esi+0xde0],ebp
  41f154:	89 ae 1c 07 00 00    	mov    DWORD PTR [esi+0x71c],ebp
  41f15a:	89 9e 20 07 00 00    	mov    DWORD PTR [esi+0x720],ebx
  41f160:	89 86 10 07 00 00    	mov    DWORD PTR [esi+0x710],eax
  41f166:	b9 d4 00 00 00       	mov    ecx,0xd4
  41f16b:	89 86 14 07 00 00    	mov    DWORD PTR [esi+0x714],eax
  41f171:	8d be 4c 08 00 00    	lea    edi,[esi+0x84c]
  41f177:	66 89 86 18 07 00 00 	mov    WORD PTR [esi+0x718],ax
  41f17e:	88 86 1a 07 00 00    	mov    BYTE PTR [esi+0x71a],al
  41f184:	f3 ab                	rep stos DWORD PTR es:[edi],eax

work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00441cb0 <.text+0x40cb0>:
  441cb0:	6a ff                	push   0xffffffff
  441cb2:	68 08 f9 45 00       	push   0x45f908
  441cb7:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  441cbd:	50                   	push   eax
  441cbe:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  441cc5:	83 ec 08             	sub    esp,0x8
  441cc8:	53                   	push   ebx
  441cc9:	56                   	push   esi
  441cca:	8b f1                	mov    esi,ecx
  441ccc:	89 74 24 08          	mov    DWORD PTR [esp+0x8],esi
  441cd0:	e8 9b b6 ff ff       	call   0x43d370
  441cd5:	68 a0 f1 41 00       	push   0x41f1a0
  441cda:	68 70 f0 41 00       	push   0x41f070
  441cdf:	6a 10                	push   0x10
  441ce1:	8d 86 e4 03 00 00    	lea    eax,[esi+0x3e4]
  441ce7:	33 db                	xor    ebx,ebx
  441ce9:	68 e4 0d 00 00       	push   0xde4
  441cee:	50                   	push   eax
  441cef:	89 5c 24 2c          	mov    DWORD PTR [esp+0x2c],ebx
  441cf3:	e8 d7 04 01 00       	call   0x4521cf

work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0045f8b8 <.text+0x5e8b8>:
  45f8b8:	68 a0 f1 41 00       	push   0x41f1a0
  45f8bd:	6a 10                	push   0x10
  45f8bf:	68 e4 0d 00 00       	push   0xde4
  45f8c4:	8b 45 ec             	mov    eax,DWORD PTR [ebp-0x14]
  45f8c7:	05 e4 03 00 00       	add    eax,0x3e4
  45f8cc:	50                   	push   eax
  45f8cd:	e8 7f 29 ff ff       	call   0x452251
  45f8d2:	c3                   	ret
  45f8d3:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
  45f8d6:	81 c1 b0 e2 00 00    	add    ecx,0xe2b0
  45f8dc:	e9 1f 48 fc ff       	jmp    0x424100
  45f8e1:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
  45f8e4:	81 c1 d4 e2 00 00    	add    ecx,0xe2d4
  45f8ea:	e9 11 48 fc ff       	jmp    0x424100
  45f8ef:	8b 4d ec             	mov    ecx,DWORD PTR [ebp-0x14]
  45f8f2:	81 c1 f8 e2 00 00    	add    ecx,0xe2f8
  45f8f8:	e9 03 48 fc ff       	jmp    0x424100
  45f8fd:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  45f900:	50                   	push   eax
  45f901:	e8 4a 1c ff ff       	call   0x451550
  45f906:	59                   	pop    ecx
  45f907:	c3                   	ret
  45f908:	b8 f8 65 46 00       	mov    eax,0x4665f8
  45f90d:	e9 ec 1d ff ff       	jmp    0x4516fe
  45f912:	cc                   	int3
  45f913:	cc                   	int3
  45f914:	cc                   	int3
  45f915:	cc                   	int3
  45f916:	cc                   	int3
  45f917:	cc                   	int3
  45f918:	cc                   	int3
  45f919:	cc                   	int3
  45f91a:	cc                   	int3
  45f91b:	cc                   	int3
  45f91c:	cc                   	int3
  45f91d:	cc                   	int3
  45f91e:	cc                   	int3
  45f91f:	cc                   	int3
  45f920:	8b 4d f0             	mov    ecx,DWORD PTR [ebp-0x10]
  45f923:	e9 98 da fd ff       	jmp    0x43d3c0
  45f928:	68 a0 f1 41 00       	push   0x41f1a0
  45f92d:	6a 10                	push   0x10
  45f92f:	68 e4 0d 00 00       	push   0xde4
  45f934:	8b 45 f0             	mov    eax,DWORD PTR [ebp-0x10]
  45f937:	05 e4 03 00 00       	add    eax,0x3e4
  45f93c:	50                   	push   eax
  45f93d:	e8 0f 29 ff ff       	call   0x452251
  45f942:	c3                   	ret
