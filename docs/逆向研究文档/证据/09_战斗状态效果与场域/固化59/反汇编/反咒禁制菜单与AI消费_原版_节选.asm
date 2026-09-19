; ===== 菜单侧 slot2 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041a3a8 <.text+0x193a8>:
  41a3a8:	83 7d 18 02          	cmp    DWORD PTR [ebp+0x18],0x2
  41a3ac:	75 1e                	jne    0x41a3cc
  41a3ae:	8a 86 8c 07 00 00    	mov    al,BYTE PTR [esi+0x78c]
  41a3b4:	84 c0                	test   al,al
  41a3b6:	75 14                	jne    0x41a3cc
  41a3b8:	8b 8e 7c 07 00 00    	mov    ecx,DWORD PTR [esi+0x77c]
  41a3be:	a1 b0 b2 46 00       	mov    eax,ds:0x46b2b0
  41a3c3:	8b 14 0f             	mov    edx,DWORD PTR [edi+ecx*1]
  41a3c6:	8b 48 08             	mov    ecx,DWORD PTR [eax+0x8]
  41a3c9:	89 4a 38             	mov    DWORD PTR [edx+0x38],ecx

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041a6d0 <.text+0x196d0>:
  41a6d0:	53                   	push   ebx
  41a6d1:	56                   	push   esi
  41a6d2:	57                   	push   edi
  41a6d3:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  41a6d7:	8b f1                	mov    esi,ecx
  41a6d9:	33 db                	xor    ebx,ebx
  41a6db:	83 c8 ff             	or     eax,0xffffffff
  41a6de:	33 c9                	xor    ecx,ecx
  41a6e0:	89 be 7c 05 00 00    	mov    DWORD PTR [esi+0x57c],edi
  41a6e6:	c6 86 8c 07 00 00 01 	mov    BYTE PTR [esi+0x78c],0x1
  41a6ed:	39 1d d8 fc 89 00    	cmp    DWORD PTR ds:0x89fcd8,ebx
  41a6f3:	0f 84 88 00 00 00    	je     0x41a781
  41a6f9:	55                   	push   ebp
  41a6fa:	ba 04 fd 89 00       	mov    edx,0x89fd04
  41a6ff:	83 f8 ff             	cmp    eax,0xffffffff
  41a702:	75 0f                	jne    0x41a713
  41a704:	8b 2a                	mov    ebp,DWORD PTR [edx]
  41a706:	39 7d 00             	cmp    DWORD PTR [ebp+0x0],edi
  41a709:	75 02                	jne    0x41a70d
  41a70b:	8b c1                	mov    eax,ecx
  41a70d:	41                   	inc    ecx
  41a70e:	83 c2 04             	add    edx,0x4
  41a711:	eb ec                	jmp    0x41a6ff
  41a713:	8b 0c 85 04 fd 89 00 	mov    ecx,DWORD PTR [eax*4+0x89fd04]
  41a71a:	5d                   	pop    ebp
  41a71b:	39 99 24 02 00 00    	cmp    DWORD PTR [ecx+0x224],ebx
  41a721:	7e 06                	jle    0x41a729
  41a723:	88 9e 8c 07 00 00    	mov    BYTE PTR [esi+0x78c],bl
  41a729:	8b 14 85 04 fd 89 00 	mov    edx,DWORD PTR [eax*4+0x89fd04]
  41a730:	39 9a f0 01 00 00    	cmp    DWORD PTR [edx+0x1f0],ebx
  41a736:	7e 06                	jle    0x41a73e
  41a738:	88 9e 8c 07 00 00    	mov    BYTE PTR [esi+0x78c],bl
  41a73e:	8b 04 85 04 fd 89 00 	mov    eax,DWORD PTR [eax*4+0x89fd04]
  41a745:	39 98 28 02 00 00    	cmp    DWORD PTR [eax+0x228],ebx
  41a74b:	7e 0c                	jle    0x41a759
  41a74d:	88 9e 8d 07 00 00    	mov    BYTE PTR [esi+0x78d],bl
  41a753:	88 9e 8c 07 00 00    	mov    BYTE PTR [esi+0x78c],bl
  41a759:	8b 0d 58 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fd58
  41a75f:	6a 02                	push   0x2
  41a761:	e8 ea 32 02 00       	call   0x43da50
  41a766:	84 c0                	test   al,al
  41a768:	75 11                	jne    0x41a77b
  41a76a:	8b 0d 54 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fd54
  41a770:	6a 02                	push   0x2
  41a772:	e8 d9 32 02 00       	call   0x43da50
  41a777:	84 c0                	test   al,al
  41a779:	74 06                	je     0x41a781
  41a77b:	88 9e 8c 07 00 00    	mov    BYTE PTR [esi+0x78c],bl
  41a781:	8b ce                	mov    ecx,esi
  41a783:	e8 c8 00 00 00       	call   0x41a850
  41a788:	8b ce                	mov    ecx,esi
  41a78a:	e8 11 f9 ff ff       	call   0x41a0a0
; ===== Enemy AI slot2 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

004442a9 <.text+0x432a9>:
  4442a9:	8b 87 54 0e 00 00    	mov    eax,DWORD PTR [edi+0xe54]
  4442af:	85 c0                	test   eax,eax
  4442b1:	7f 0a                	jg     0x4442bd
  4442b3:	8b 87 20 0e 00 00    	mov    eax,DWORD PTR [edi+0xe20]
  4442b9:	85 c0                	test   eax,eax
  4442bb:	7e 05                	jle    0x4442c2
  4442bd:	c6 44 24 20 00       	mov    BYTE PTR [esp+0x20],0x0
  4442c2:	6a 02                	push   0x2
  4442c4:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  4442c9:	e8 82 97 ff ff       	call   0x43da50
  4442ce:	84 c0                	test   al,al
  4442d0:	75 10                	jne    0x4442e2
  4442d2:	6a 02                	push   0x2
  4442d4:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4442d9:	e8 72 97 ff ff       	call   0x43da50
  4442de:	84 c0                	test   al,al
  4442e0:	74 05                	je     0x4442e7
  4442e2:	c6 44 24 20 00       	mov    BYTE PTR [esp+0x20],0x0
  4442e7:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  4442eb:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  4442ef:	3b c1                	cmp    eax,ecx
  4442f1:	7d 05                	jge    0x4442f8
  4442f3:	83 c9 ff             	or     ecx,0xffffffff
  4442f6:	eb 19                	jmp    0x444311
  4442f8:	3b c5                	cmp    eax,ebp
  4442fa:	7d 11                	jge    0x44430d
  4442fc:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  444300:	51                   	push   ecx
  444301:	56                   	push   esi
  444302:	8b cb                	mov    ecx,ebx
  444304:	e8 27 00 00 00       	call   0x444330
  444309:	8b c8                	mov    ecx,eax
  44430b:	eb 04                	jmp    0x444311
  44430d:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  444311:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  444315:	8b c1                	mov    eax,ecx
  444317:	2b c7                	sub    eax,edi
  444319:	5f                   	pop    edi
  44431a:	f7 d8                	neg    eax
  44431c:	1b c0                	sbb    eax,eax
  44431e:	5e                   	pop    esi
  44431f:	5d                   	pop    ebp
  444320:	23 c1                	and    eax,ecx
  444322:	5b                   	pop    ebx
  444323:	83 c4 0c             	add    esp,0xc
  444326:	c2 04 00             	ret    0x4
  444329:	90                   	nop
  44432a:	90                   	nop
  44432b:	90                   	nop
  44432c:	90                   	nop
  44432d:	90                   	nop
  44432e:	90                   	nop
  44432f:	90                   	nop
  444330:	83 ec 70             	sub    esp,0x70
  444333:	53                   	push   ebx
  444334:	8b 5c 24 78          	mov    ebx,DWORD PTR [esp+0x78]
  444338:	55                   	push   ebp
  444339:	89 4c 24 0c          	mov    DWORD PTR [esp+0xc],ecx
  44433d:	8d 04 db             	lea    eax,[ebx+ebx*8]
  444340:	8d 04 83             	lea    eax,[ebx+eax*4]
  444343:	8d 14 40             	lea    edx,[eax+eax*2]
  444346:	8d 04 d3             	lea    eax,[ebx+edx*8]
  444349:	8b ac 81 d8 0c 00 00 	mov    ebp,DWORD PTR [ecx+eax*4+0xcd8]
  444350:	8d 14 81             	lea    edx,[ecx+eax*4]
  444353:	85 ed                	test   ebp,ebp
  444355:	8b 82 30 0c 00 00    	mov    eax,DWORD PTR [edx+0xc30]
  44435b:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  44435f:	89 44 24 24          	mov    DWORD PTR [esp+0x24],eax
  444363:	89 6c 24 20          	mov    DWORD PTR [esp+0x20],ebp
  444367:	0f 8e 1b 02 00 00    	jle    0x444588
  44436d:	8d 4c 24 28          	lea    ecx,[esp+0x28]
  444371:	8d 82 dc 0c 00 00    	lea    eax,[edx+0xcdc]
  444377:	56                   	push   esi
  444378:	57                   	push   edi
  444379:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  44437d:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  444381:	89 6c 24 24          	mov    DWORD PTR [esp+0x24],ebp
  444385:	eb 04                	jmp    0x44438b
  444387:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  44438b:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
  44438f:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  444393:	c6 44 24 13 01       	mov    BYTE PTR [esp+0x13],0x1
  444398:	8b 30                	mov    esi,DWORD PTR [eax]
  44439a:	8a 84 24 88 00 00 00 	mov    al,BYTE PTR [esp+0x88]
  4443a1:	c7 01 00 00 00 00    	mov    DWORD PTR [ecx],0x0
  4443a7:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  4443ad:	84 c0                	test   al,al
  4443af:	75 26                	jne    0x4443d7
  4443b1:	8b b9 b4 00 00 00    	mov    edi,DWORD PTR [ecx+0xb4]
  4443b7:	8d 04 f5 00 00 00 00 	lea    eax,[esi*8+0x0]
  4443be:	2b c6                	sub    eax,esi
  4443c0:	8d 04 80             	lea    eax,[eax+eax*4]
  4443c3:	c1 e0 04             	shl    eax,0x4
  4443c6:	83 7c 38 18 02       	cmp    DWORD PTR [eax+edi*1+0x18],0x2
  4443cb:	75 0a                	jne    0x4443d7
  4443cd:	c6 44 24 13 00       	mov    BYTE PTR [esp+0x13],0x0
  4443d2:	e9 0a 01 00 00       	jmp    0x4444e1
