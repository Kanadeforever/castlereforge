
/mnt/data/freeze54_work/org/RPG.exe:     file format pei-i386


Disassembly of section .text:

00447110 <.text+0x46110>:
  447110:	6a ff                	push   0xffffffff
  447112:	68 73 fb 45 00       	push   0x45fb73
  447117:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  44711d:	50                   	push   eax
  44711e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  447125:	83 ec 20             	sub    esp,0x20
  447128:	53                   	push   ebx
  447129:	56                   	push   esi
  44712a:	8b f1                	mov    esi,ecx
  44712c:	89 74 24 08          	mov    DWORD PTR [esp+0x8],esi
  447130:	e8 db 9d fe ff       	call   0x430f10
  447135:	33 db                	xor    ebx,ebx
  447137:	8b ce                	mov    ecx,esi
  447139:	53                   	push   ebx
  44713a:	68 48 85 1e 00       	push   0x1e8548
  44713f:	56                   	push   esi
  447140:	89 5c 24 3c          	mov    DWORD PTR [esp+0x3c],ebx
  447144:	c7 06 44 11 46 00    	mov    DWORD PTR [esi],0x461144
  44714a:	e8 e1 d2 fe ff       	call   0x434430
  44714f:	8d 8e 18 02 00 00    	lea    ecx,[esi+0x218]
  447155:	e8 d6 db fb ff       	call   0x404d30
  44715a:	68 60 ca 46 00       	push   0x46ca60
  44715f:	8d 8e 6c 05 00 00    	lea    ecx,[esi+0x56c]
  447165:	53                   	push   ebx
  447166:	89 44 24 2c          	mov    DWORD PTR [esp+0x2c],eax
  44716a:	53                   	push   ebx
  44716b:	8d 86 68 05 00 00    	lea    eax,[esi+0x568]
  447171:	89 4c 24 20          	mov    DWORD PTR [esp+0x20],ecx
  447175:	68 f4 01 00 00       	push   0x1f4
  44717a:	68 f4 00 00 00       	push   0xf4
  44717f:	8b ce                	mov    ecx,esi
  447181:	89 44 24 24          	mov    DWORD PTR [esp+0x24],eax
  447185:	e8 56 9f fe ff       	call   0x4310e0
  44718a:	8b 8e 64 05 00 00    	mov    ecx,DWORD PTR [esi+0x564]
  447190:	53                   	push   ebx
  447191:	e8 5a 75 ff ff       	call   0x43e6f0
  447196:	6a 48                	push   0x48
  447198:	88 9e 79 05 00 00    	mov    BYTE PTR [esi+0x579],bl
  44719e:	e8 bc a4 00 00       	call   0x45165f
  4471a3:	83 c4 04             	add    esp,0x4
  4471a6:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  4471aa:	3b c3                	cmp    eax,ebx
  4471ac:	c6 44 24 30 01       	mov    BYTE PTR [esp+0x30],0x1
  4471b1:	74 0e                	je     0x4471c1
  4471b3:	8d 54 24 10          	lea    edx,[esp+0x10]
  4471b7:	8b c8                	mov    ecx,eax
  4471b9:	52                   	push   edx
  4471ba:	e8 31 a7 fe ff       	call   0x4318f0
  4471bf:	eb 02                	jmp    0x4471c3
  4471c1:	33 c0                	xor    eax,eax
  4471c3:	53                   	push   ebx
  4471c4:	68 bc ba 46 00       	push   0x46babc
  4471c9:	6a 0a                	push   0xa
  4471cb:	6a 0f                	push   0xf
  4471cd:	8b c8                	mov    ecx,eax
  4471cf:	88 5c 24 40          	mov    BYTE PTR [esp+0x40],bl
  4471d3:	89 86 7c 05 00 00    	mov    DWORD PTR [esi+0x57c],eax
  4471d9:	e8 12 a8 fe ff       	call   0x4319f0
  4471de:	8b 8e 7c 05 00 00    	mov    ecx,DWORD PTR [esi+0x57c]
  4471e4:	6a 30                	push   0x30
  4471e6:	6a 40                	push   0x40
  4471e8:	e8 a3 a8 fe ff       	call   0x431a90
  4471ed:	8b 86 7c 05 00 00    	mov    eax,DWORD PTR [esi+0x57c]
  4471f3:	c7 40 40 09 00 00 00 	mov    DWORD PTR [eax+0x40],0x9
  4471fa:	8b 15 b0 b2 46 00    	mov    edx,DWORD PTR ds:0x46b2b0
  447200:	8b 8e 7c 05 00 00    	mov    ecx,DWORD PTR [esi+0x57c]
  447206:	8b 02                	mov    eax,DWORD PTR [edx]
  447208:	89 41 38             	mov    DWORD PTR [ecx+0x38],eax
  44720b:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  44720f:	89 9e 8c 05 00 00    	mov    DWORD PTR [esi+0x58c],ebx
  447215:	89 9e 88 05 00 00    	mov    DWORD PTR [esi+0x588],ebx
  44721b:	c7 86 80 05 00 00 f4 	mov    DWORD PTR [esi+0x580],0x1f4
  447222:	01 00 00 
  447225:	c7 86 84 05 00 00 ae 	mov    DWORD PTR [esi+0x584],0x1ae
  44722c:	01 00 00 
  44722f:	89 9e 90 05 00 00    	mov    DWORD PTR [esi+0x590],ebx
  447235:	8b c6                	mov    eax,esi
  447237:	5e                   	pop    esi
  447238:	5b                   	pop    ebx
  447239:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  447240:	83 c4 2c             	add    esp,0x2c
  447243:	c3                   	ret
  447244:	90                   	nop
  447245:	90                   	nop
  447246:	90                   	nop
  447247:	90                   	nop
  447248:	90                   	nop
  447249:	90                   	nop
  44724a:	90                   	nop
  44724b:	90                   	nop
  44724c:	90                   	nop
  44724d:	90                   	nop
  44724e:	90                   	nop
  44724f:	90                   	nop
  447250:	6a ff                	push   0xffffffff
  447252:	68 88 fb 45 00       	push   0x45fb88
  447257:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  44725d:	50                   	push   eax
  44725e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  447265:	51                   	push   ecx
  447266:	56                   	push   esi
  447267:	8b f1                	mov    esi,ecx
  447269:	57                   	push   edi
  44726a:	89 74 24 08          	mov    DWORD PTR [esp+0x8],esi
  44726e:	c7 06 44 11 46 00    	mov    DWORD PTR [esi],0x461144
  447274:	8b be 7c 05 00 00    	mov    edi,DWORD PTR [esi+0x57c]
  44727a:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  447281:	00 
  447282:	85 ff                	test   edi,edi
  447284:	74 1a                	je     0x4472a0
  447286:	8b cf                	mov    ecx,edi
  447288:	e8 13 a7 fe ff       	call   0x4319a0
  44728d:	57                   	push   edi
  44728e:	e8 bd a2 00 00       	call   0x451550
  447293:	83 c4 04             	add    esp,0x4
  447296:	c7 86 7c 05 00 00 00 	mov    DWORD PTR [esi+0x57c],0x0
  44729d:	00 00 00 
  4472a0:	8b ce                	mov    ecx,esi
  4472a2:	c7 44 24 14 ff ff ff 	mov    DWORD PTR [esp+0x14],0xffffffff
  4472a9:	ff 
  4472aa:	e8 91 9d fe ff       	call   0x431040
  4472af:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  4472b3:	5f                   	pop    edi
  4472b4:	5e                   	pop    esi
  4472b5:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  4472bc:	83 c4 10             	add    esp,0x10
  4472bf:	c3                   	ret
  4472c0:	56                   	push   esi
  4472c1:	57                   	push   edi
  4472c2:	8b 7c 24 0c          	mov    edi,DWORD PTR [esp+0xc]
  4472c6:	8b f1                	mov    esi,ecx
  4472c8:	57                   	push   edi
  4472c9:	8b 8e 7c 05 00 00    	mov    ecx,DWORD PTR [esi+0x57c]
  4472cf:	e8 4c a7 fe ff       	call   0x431a20
  4472d4:	83 c9 ff             	or     ecx,0xffffffff
  4472d7:	33 c0                	xor    eax,eax
  4472d9:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  4472db:	f7 d1                	not    ecx
  4472dd:	8b 96 7c 05 00 00    	mov    edx,DWORD PTR [esi+0x57c]
  4472e3:	49                   	dec    ecx
  4472e4:	8d 04 49             	lea    eax,[ecx+ecx*2]
  4472e7:	b9 4c 00 00 00       	mov    ecx,0x4c
  4472ec:	d1 e0                	shl    eax,1
  4472ee:	2b c8                	sub    ecx,eax
  4472f0:	89 4a 24             	mov    DWORD PTR [edx+0x24],ecx
  4472f3:	8b ce                	mov    ecx,esi
  4472f5:	e8 a6 00 00 00       	call   0x4473a0
  4472fa:	5f                   	pop    edi
  4472fb:	5e                   	pop    esi
  4472fc:	c2 04 00             	ret    0x4
  4472ff:	90                   	nop
  447300:	56                   	push   esi
  447301:	8b f1                	mov    esi,ecx
  447303:	e8 28 00 00 00       	call   0x447330
  447308:	8a 86 79 05 00 00    	mov    al,BYTE PTR [esi+0x579]
  44730e:	84 c0                	test   al,al
  447310:	74 1c                	je     0x44732e
  447312:	8b 86 90 05 00 00    	mov    eax,DWORD PTR [esi+0x590]
  447318:	85 c0                	test   eax,eax
  44731a:	7e 12                	jle    0x44732e
  44731c:	48                   	dec    eax
  44731d:	85 c0                	test   eax,eax
  44731f:	89 86 90 05 00 00    	mov    DWORD PTR [esi+0x590],eax
  447325:	7f 07                	jg     0x44732e
  447327:	8b ce                	mov    ecx,esi
  447329:	e8 a2 00 00 00       	call   0x4473d0
  44732e:	5e                   	pop    esi
  44732f:	c3                   	ret
  447330:	56                   	push   esi
  447331:	8b f1                	mov    esi,ecx
  447333:	8b 86 8c 05 00 00    	mov    eax,DWORD PTR [esi+0x58c]
  447339:	83 f8 01             	cmp    eax,0x1
  44733c:	74 05                	je     0x447343
  44733e:	83 f8 02             	cmp    eax,0x2
  447341:	75 54                	jne    0x447397
  447343:	8b 8e 84 05 00 00    	mov    ecx,DWORD PTR [esi+0x584]
  447349:	8b 96 80 05 00 00    	mov    edx,DWORD PTR [esi+0x580]
  44734f:	50                   	push   eax
  447350:	8b 86 88 05 00 00    	mov    eax,DWORD PTR [esi+0x588]
  447356:	50                   	push   eax
  447357:	51                   	push   ecx
  447358:	8b 8e 60 05 00 00    	mov    ecx,DWORD PTR [esi+0x560]
  44735e:	52                   	push   edx
  44735f:	e8 6c a9 fe ff       	call   0x431cd0
  447364:	8b 8e 88 05 00 00    	mov    ecx,DWORD PTR [esi+0x588]
  44736a:	89 86 6c 05 00 00    	mov    DWORD PTR [esi+0x56c],eax
  447370:	41                   	inc    ecx
  447371:	8b c1                	mov    eax,ecx
  447373:	89 8e 88 05 00 00    	mov    DWORD PTR [esi+0x588],ecx
  447379:	83 f8 09             	cmp    eax,0x9
  44737c:	7e 19                	jle    0x447397
  44737e:	8b 8e 8c 05 00 00    	mov    ecx,DWORD PTR [esi+0x58c]
  447384:	33 c0                	xor    eax,eax
  447386:	83 f9 02             	cmp    ecx,0x2
  447389:	75 06                	jne    0x447391
  44738b:	88 86 79 05 00 00    	mov    BYTE PTR [esi+0x579],al
  447391:	89 86 8c 05 00 00    	mov    DWORD PTR [esi+0x58c],eax
  447397:	5e                   	pop    esi
  447398:	c3                   	ret
  447399:	90                   	nop
  44739a:	90                   	nop
  44739b:	90                   	nop
  44739c:	90                   	nop
  44739d:	90                   	nop
  44739e:	90                   	nop
  44739f:	90                   	nop
  4473a0:	b8 01 00 00 00       	mov    eax,0x1
  4473a5:	c7 81 88 05 00 00 00 	mov    DWORD PTR [ecx+0x588],0x0
  4473ac:	00 00 00 
  4473af:	89 81 8c 05 00 00    	mov    DWORD PTR [ecx+0x58c],eax
  4473b5:	88 81 79 05 00 00    	mov    BYTE PTR [ecx+0x579],al
  4473bb:	c7 81 90 05 00 00 27 	mov    DWORD PTR [ecx+0x590],0x27
  4473c2:	00 00 00 
  4473c5:	c3                   	ret
  4473c6:	90                   	nop
  4473c7:	90                   	nop
  4473c8:	90                   	nop
  4473c9:	90                   	nop
  4473ca:	90                   	nop
  4473cb:	90                   	nop
  4473cc:	90                   	nop
  4473cd:	90                   	nop
  4473ce:	90                   	nop
  4473cf:	90                   	nop
  4473d0:	c7 81 8c 05 00 00 02 	mov    DWORD PTR [ecx+0x58c],0x2
  4473d7:	00 00 00 
  4473da:	c7 81 88 05 00 00 00 	mov    DWORD PTR [ecx+0x588],0x0
  4473e1:	00 00 00 
  4473e4:	c3                   	ret
