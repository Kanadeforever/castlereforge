
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004440d0 <.text+0x430d0>:
  4440d0:	25 00 00 00 00       	and    eax,0x0
  4440d5:	83 ec 14             	sub    esp,0x14
  4440d8:	56                   	push   esi
  4440d9:	8b f1                	mov    esi,ecx
  4440db:	57                   	push   edi
  4440dc:	6a 01                	push   0x1
  4440de:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  4440e2:	e8 09 d6 ff ff       	call   0x4416f0
  4440e7:	6a 00                	push   0x0
  4440e9:	6a 01                	push   0x1
  4440eb:	68 d8 c5 46 00       	push   0x46c5d8
  4440f0:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  4440f4:	c7 44 24 30 00 00 00 	mov    DWORD PTR [esp+0x30],0x0
  4440fb:	00 
  4440fc:	c7 86 34 e5 00 00 00 	mov    DWORD PTR [esi+0xe534],0x0
  444103:	00 00 00 
  444106:	c7 86 38 e5 00 00 00 	mov    DWORD PTR [esi+0xe538],0x0
  44410d:	00 00 00 
  444110:	e8 ab d6 ff ff       	call   0x4417c0
  444115:	84 c0                	test   al,al
  444117:	0f 84 f1 00 00 00    	je     0x44420e
  44411d:	8d 4c 24 08          	lea    ecx,[esp+0x8]
  444121:	e8 0a 8d fe ff       	call   0x42ce30
  444126:	8b f8                	mov    edi,eax
  444128:	b8 93 24 49 92       	mov    eax,0x92492493
  44412d:	f7 ef                	imul   edi
  44412f:	03 d7                	add    edx,edi
  444131:	c1 fa 05             	sar    edx,0x5
  444134:	8b c2                	mov    eax,edx
  444136:	c1 e8 1f             	shr    eax,0x1f
  444139:	03 d0                	add    edx,eax
  44413b:	8d 04 d5 00 00 00 00 	lea    eax,[edx*8+0x0]
  444142:	2b c2                	sub    eax,edx
  444144:	c1 e0 03             	shl    eax,0x3
  444147:	3b f8                	cmp    edi,eax
  444149:	0f 85 bf 00 00 00    	jne    0x44420e
  44414f:	50                   	push   eax
  444150:	e8 0a d5 00 00       	call   0x45165f
  444155:	83 c4 04             	add    esp,0x4
  444158:	8d 4c 24 08          	lea    ecx,[esp+0x8]
  44415c:	89 86 34 e5 00 00    	mov    DWORD PTR [esi+0xe534],eax
  444162:	50                   	push   eax
  444163:	57                   	push   edi
  444164:	6a 00                	push   0x0
  444166:	e8 c5 d8 ff ff       	call   0x441a30
  44416b:	8b 8e 34 e5 00 00    	mov    ecx,DWORD PTR [esi+0xe534]
  444171:	57                   	push   edi
  444172:	51                   	push   ecx
  444173:	e8 58 43 fe ff       	call   0x4284d0
  444178:	83 c4 08             	add    esp,0x8
  44417b:	8d 4c 24 08          	lea    ecx,[esp+0x8]
  44417f:	e8 7c d8 ff ff       	call   0x441a00
  444184:	6a 00                	push   0x0
  444186:	6a 01                	push   0x1
  444188:	68 c4 c5 46 00       	push   0x46c5c4
  44418d:	8d 4c 24 14          	lea    ecx,[esp+0x14]
  444191:	e8 2a d6 ff ff       	call   0x4417c0
  444196:	84 c0                	test   al,al
  444198:	74 74                	je     0x44420e
  44419a:	8d 4c 24 08          	lea    ecx,[esp+0x8]
  44419e:	e8 8d 8c fe ff       	call   0x42ce30
  4441a3:	8b f8                	mov    edi,eax
  4441a5:	99                   	cdq
  4441a6:	83 e2 3f             	and    edx,0x3f
  4441a9:	03 c2                	add    eax,edx
  4441ab:	c1 f8 06             	sar    eax,0x6
  4441ae:	c1 e0 06             	shl    eax,0x6
  4441b1:	3b f8                	cmp    edi,eax
  4441b3:	75 59                	jne    0x44420e
  4441b5:	50                   	push   eax
  4441b6:	e8 a4 d4 00 00       	call   0x45165f
  4441bb:	83 c4 04             	add    esp,0x4
  4441be:	8d 4c 24 08          	lea    ecx,[esp+0x8]
  4441c2:	89 86 38 e5 00 00    	mov    DWORD PTR [esi+0xe538],eax
  4441c8:	50                   	push   eax
  4441c9:	57                   	push   edi
  4441ca:	6a 00                	push   0x0
  4441cc:	e8 5f d8 ff ff       	call   0x441a30
  4441d1:	8b 96 38 e5 00 00    	mov    edx,DWORD PTR [esi+0xe538]
  4441d7:	57                   	push   edi
  4441d8:	52                   	push   edx
  4441d9:	e8 f2 42 fe ff       	call   0x4284d0
  4441de:	83 c4 08             	add    esp,0x8
  4441e1:	8d 4c 24 08          	lea    ecx,[esp+0x8]
  4441e5:	e8 16 d8 ff ff       	call   0x441a00
  4441ea:	8d 4c 24 08          	lea    ecx,[esp+0x8]
  4441ee:	c7 44 24 24 ff ff ff 	mov    DWORD PTR [esp+0x24],0xffffffff
  4441f5:	ff 
  4441f6:	e8 15 d5 ff ff       	call   0x441710
  4441fb:	5f                   	pop    edi
  4441fc:	b0 01                	mov    al,0x1
  4441fe:	5e                   	pop    esi
  4441ff:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  444203:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  44420a:	83 c4 20             	add    esp,0x20
  44420d:	c3                   	ret
  44420e:	8d 4c            	lea    ecx,[esp+0x8]
