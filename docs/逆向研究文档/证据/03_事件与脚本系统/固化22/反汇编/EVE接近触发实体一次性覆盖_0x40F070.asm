
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040f070 <.text+0xe070>:
  40f070:	51                   	push   ecx
  40f071:	56                   	push   esi
  40f072:	8b f1                	mov    esi,ecx
  40f074:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  40f077:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  40f07a:	e8 21 c3 ff ff       	call   0x40b3a0
  40f07f:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  40f082:	8b 80 80 02 00 00    	mov    eax,DWORD PTR [eax+0x280]
  40f088:	83 c1 04             	add    ecx,0x4
  40f08b:	51                   	push   ecx
  40f08c:	8b c8                	mov    ecx,eax
  40f08e:	e8 bd bd ff ff       	call   0x40ae50
  40f093:	8b c8                	mov    ecx,eax
  40f095:	e8 46 ac ff ff       	call   0x409ce0
  40f09a:	85 c0                	test   eax,eax
  40f09c:	75 17                	jne    0x40f0b5
  40f09e:	8d 54 24 04          	lea    edx,[esp+0x4]
  40f0a2:	68 d8 37 46 00       	push   0x4637d8
  40f0a7:	52                   	push   edx
  40f0a8:	c7 44 24 0c 00 8e 46 	mov    DWORD PTR [esp+0xc],0x468e00
  40f0af:	00 
  40f0b0:	e8 12 2b 04 00       	call   0x451bc7
  40f0b5:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  40f0b8:	53                   	push   ebx
  40f0b9:	b3 01                	mov    bl,0x1
  40f0bb:	8a 91 e2 00 00 00    	mov    dl,BYTE PTR [ecx+0xe2]
  40f0c1:	fe c2                	inc    dl
  40f0c3:	88 91 e2 00 00 00    	mov    BYTE PTR [ecx+0xe2],dl
  40f0c9:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  40f0cc:	8b 50 70             	mov    edx,DWORD PTR [eax+0x70]
  40f0cf:	8a 49 19             	mov    cl,BYTE PTR [ecx+0x19]
  40f0d2:	88 8a e3 00 00 00    	mov    BYTE PTR [edx+0xe3],cl
  40f0d8:	8b 50 70             	mov    edx,DWORD PTR [eax+0x70]
  40f0db:	88 5a 74             	mov    BYTE PTR [edx+0x74],bl
  40f0de:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  40f0e1:	8b 50 70             	mov    edx,DWORD PTR [eax+0x70]
  40f0e4:	8a 49 18             	mov    cl,BYTE PTR [ecx+0x18]
  40f0e7:	88 8a ea 00 00 00    	mov    BYTE PTR [edx+0xea],cl
  40f0ed:	8b 50 70             	mov    edx,DWORD PTR [eax+0x70]
  40f0f0:	c6 82 eb 00 00 00 00 	mov    BYTE PTR [edx+0xeb],0x0
  40f0f7:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  40f0fa:	c6 41 73 00          	mov    BYTE PTR [ecx+0x73],0x0
  40f0fe:	8b 50 70             	mov    edx,DWORD PTR [eax+0x70]
  40f101:	c6 42 76 02          	mov    BYTE PTR [edx+0x76],0x2
  40f105:	8b 48 6c             	mov    ecx,DWORD PTR [eax+0x6c]
  40f108:	88 99 80 00 00 00    	mov    BYTE PTR [ecx+0x80],bl
  40f10e:	8b 50 70             	mov    edx,DWORD PTR [eax+0x70]
  40f111:	8b 82 ff 00 00 00    	mov    eax,DWORD PTR [edx+0xff]
  40f117:	3d 41 04 00 00       	cmp    eax,0x441
  40f11c:	74 15                	je     0x40f133
  40f11e:	3d c5 04 00 00       	cmp    eax,0x4c5
  40f123:	74 0e                	je     0x40f133
  40f125:	68 ff 00 00 00       	push   0xff
  40f12a:	50                   	push   eax
  40f12b:	e8 80 bf 03 00       	call   0x44b0b0
  40f130:	83 c4 08             	add    esp,0x8
  40f133:	8b 15 10 f8 89 00    	mov    edx,DWORD PTR ds:0x89f810
  40f139:	56                   	push   esi
  40f13a:	42                   	inc    edx
  40f13b:	c7 05 18 f8 89 00 00 	mov    DWORD PTR ds:0x89f818,0x0
  40f142:	00 00 00 
  40f145:	89 15 10 f8 89 00    	mov    DWORD PTR ds:0x89f810,edx
  40f14b:	88                   	.byte 0x88
  40f14c:	1d                   	.byte 0x1d
  40f14d:	1e                   	push   ds
  40f14e:	f8                   	clc
  40f14f:	89                   	.byte 0x89
