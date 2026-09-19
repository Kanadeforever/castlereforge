; 固化68补充直接反汇编证据。由当前工具从已校验输入只读生成。
; 业务结论仍以机器JSON和完整接档说明中的证据边界为准。

; ===== 原版MiscInfo装载 0x00428400..0x004284C0 =====

/workspace/scratch/3f7398360f84/work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00428400 <.text+0x27400>:
  428400:	6a ff                	push   0xffffffff
  428402:	68 1b e7 45 00       	push   0x45e71b
  428407:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  42840d:	50                   	push   eax
  42840e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  428415:	51                   	push   ecx
  428416:	56                   	push   esi
  428417:	57                   	push   edi
  428418:	8b f9                	mov    edi,ecx
  42841a:	6a 14                	push   0x14
  42841c:	e8 3e 92 02 00       	call   0x45165f
  428421:	83 c4 04             	add    esp,0x4
  428424:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  428428:	85 c0                	test   eax,eax
  42842a:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  428431:	00 
  428432:	74 0d                	je     0x428441
  428434:	6a 01                	push   0x1
  428436:	8b c8                	mov    ecx,eax
  428438:	e8 b3 92 01 00       	call   0x4416f0
  42843d:	8b f0                	mov    esi,eax
  42843f:	eb 02                	jmp    0x428443
  428441:	33 f6                	xor    esi,esi
  428443:	6a 00                	push   0x0
  428445:	6a 01                	push   0x1
  428447:	68 78 a5 46 00       	push   0x46a578
  42844c:	8b ce                	mov    ecx,esi
  42844e:	c7 44 24 20 ff ff ff 	mov    DWORD PTR [esp+0x20],0xffffffff
  428455:	ff 
  428456:	e8 65 93 01 00       	call   0x4417c0
  42845b:	84 c0                	test   al,al
  42845d:	74 44                	je     0x4284a3
  42845f:	8b ce                	mov    ecx,esi
  428461:	e8 ca 49 00 00       	call   0x42ce30
  428466:	3d 1e 04 00 00       	cmp    eax,0x41e
  42846b:	75 2f                	jne    0x42849c
  42846d:	50                   	push   eax
  42846e:	e8 ec 91 02 00       	call   0x45165f
  428473:	83 c4 04             	add    esp,0x4
  428476:	8b ce                	mov    ecx,esi
  428478:	89 47 04             	mov    DWORD PTR [edi+0x4],eax
  42847b:	50                   	push   eax
  42847c:	68 1e 04 00 00       	push   0x41e
  428481:	6a 00                	push   0x0
  428483:	e8 a8 95 01 00       	call   0x441a30
  428488:	8b 47 04             	mov    eax,DWORD PTR [edi+0x4]
  42848b:	68 1e 04 00 00       	push   0x41e
  428490:	50                   	push   eax
  428491:	e8 3a 00 00 00       	call   0x4284d0
  428496:	83 c4 08             	add    esp,0x8
  428499:	c6 07 01             	mov    BYTE PTR [edi],0x1
  42849c:	8b ce                	mov    ecx,esi
  42849e:	e8 5d 95 01 00       	call   0x441a00
  4284a3:	85 f6                	test   esi,esi
  4284a5:	74 10                	je     0x4284b7
  4284a7:	8b ce                	mov    ecx,esi
  4284a9:	e8 62 92 01 00       	call   0x441710
  4284ae:	56                   	push   esi
  4284af:	e8 9c 90 02 00       	call   0x451550
  4284b4:	83 c4 04             	add    esp,0x4
  4284b7:	8a 07                	mov    al,BYTE PTR [edi]
  4284b9:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  4284bd:	5f                   	pop    edi
  4284be:	5e                   	pop    esi
  4284bf:	64                   	fs


; ===== 原版动作池加入删除与全槽清理 0x00423100..0x00423598 =====

/workspace/scratch/3f7398360f84/work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00423100 <.text+0x22100>:
  423100:	57                   	push   edi
  423101:	8b f9                	mov    edi,ecx
  423103:	32 c9                	xor    cl,cl
  423105:	8b 87 f4 08 00 00    	mov    eax,DWORD PTR [edi+0x8f4]
  42310b:	83 f8 24             	cmp    eax,0x24
  42310e:	7c 10                	jl     0x423120
  423110:	c7 87 f4 08 00 00 24 	mov    DWORD PTR [edi+0x8f4],0x24
  423117:	00 00 00 
  42311a:	32 c0                	xor    al,al
  42311c:	5f                   	pop    edi
  42311d:	c2 04 00             	ret    0x4
  423120:	53                   	push   ebx
  423121:	8b 5c 24 0c          	mov    ebx,DWORD PTR [esp+0xc]
  423125:	33 d2                	xor    edx,edx
  423127:	85 c0                	test   eax,eax
  423129:	56                   	push   esi
  42312a:	7e 1c                	jle    0x423148
  42312c:	8d b7 f8 08 00 00    	lea    esi,[edi+0x8f8]
  423132:	84 c9                	test   cl,cl
  423134:	75 28                	jne    0x42315e
  423136:	39 1e                	cmp    DWORD PTR [esi],ebx
  423138:	75 02                	jne    0x42313c
  42313a:	b1 01                	mov    cl,0x1
  42313c:	42                   	inc    edx
  42313d:	83 c6 04             	add    esi,0x4
  423140:	3b d0                	cmp    edx,eax
  423142:	7c ee                	jl     0x423132
  423144:	84 c9                	test   cl,cl
  423146:	75 16                	jne    0x42315e
  423148:	89 9c 87 f8 08 00 00 	mov    DWORD PTR [edi+eax*4+0x8f8],ebx
  42314f:	8b 87 f4 08 00 00    	mov    eax,DWORD PTR [edi+0x8f4]
  423155:	40                   	inc    eax
  423156:	89 87 f4 08 00 00    	mov    DWORD PTR [edi+0x8f4],eax
  42315c:	84 c9                	test   cl,cl
  42315e:	5e                   	pop    esi
  42315f:	5b                   	pop    ebx
  423160:	5f                   	pop    edi
  423161:	0f 94 c0             	sete   al
  423164:	c2 04 00             	ret    0x4
  423167:	90                   	nop
  423168:	90                   	nop
  423169:	90                   	nop
  42316a:	90                   	nop
  42316b:	90                   	nop
  42316c:	90                   	nop
  42316d:	90                   	nop
  42316e:	90                   	nop
  42316f:	90                   	nop
  423170:	8b 81 f4 08 00 00    	mov    eax,DWORD PTR [ecx+0x8f4]
  423176:	53                   	push   ebx
  423177:	55                   	push   ebp
  423178:	56                   	push   esi
  423179:	32 db                	xor    bl,bl
  42317b:	33 f6                	xor    esi,esi
  42317d:	85 c0                	test   eax,eax
  42317f:	57                   	push   edi
  423180:	7e 47                	jle    0x4231c9
  423182:	8d b9 f8 08 00 00    	lea    edi,[ecx+0x8f8]
  423188:	84 db                	test   bl,bl
  42318a:	75 3d                	jne    0x4231c9
  42318c:	8b 17                	mov    edx,DWORD PTR [edi]
  42318e:	8b 6c 24 14          	mov    ebp,DWORD PTR [esp+0x14]
  423192:	3b d5                	cmp    edx,ebp
  423194:	75 25                	jne    0x4231bb
  423196:	48                   	dec    eax
  423197:	b3 01                	mov    bl,0x1
  423199:	3b f0                	cmp    esi,eax
  42319b:	8b d6                	mov    edx,esi
  42319d:	7d 16                	jge    0x4231b5
  42319f:	8b c7                	mov    eax,edi
  4231a1:	8b 68 04             	mov    ebp,DWORD PTR [eax+0x4]
  4231a4:	42                   	inc    edx
  4231a5:	89 28                	mov    DWORD PTR [eax],ebp
  4231a7:	8b a9 f4 08 00 00    	mov    ebp,DWORD PTR [ecx+0x8f4]
  4231ad:	83 c0 04             	add    eax,0x4
  4231b0:	4d                   	dec    ebp
  4231b1:	3b d5                	cmp    edx,ebp
  4231b3:	7c ec                	jl     0x4231a1
  4231b5:	ff 89 f4 08 00 00    	dec    DWORD PTR [ecx+0x8f4]
  4231bb:	8b 81 f4 08 00 00    	mov    eax,DWORD PTR [ecx+0x8f4]
  4231c1:	46                   	inc    esi
  4231c2:	83 c7 04             	add    edi,0x4
  4231c5:	3b f0                	cmp    esi,eax
  4231c7:	7c bf                	jl     0x423188
  4231c9:	5f                   	pop    edi
  4231ca:	5e                   	pop    esi
  4231cb:	5d                   	pop    ebp
  4231cc:	5b                   	pop    ebx
  4231cd:	c2 04 00             	ret    0x4
  4231d0:	57                   	push   edi
  4231d1:	8d b9 34 0a 00 00    	lea    edi,[ecx+0xa34]
  4231d7:	b9 1c 00 00 00       	mov    ecx,0x1c
  4231dc:	33 c0                	xor    eax,eax
  4231de:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  4231e0:	5f                   	pop    edi
  4231e1:	c3                   	ret
  4231e2:	90                   	nop
  4231e3:	90                   	nop
  4231e4:	90                   	nop
  4231e5:	90                   	nop
  4231e6:	90                   	nop
  4231e7:	90                   	nop
  4231e8:	90                   	nop
  4231e9:	90                   	nop
  4231ea:	90                   	nop
  4231eb:	90                   	nop
  4231ec:	90                   	nop
  4231ed:	90                   	nop
  4231ee:	90                   	nop
  4231ef:	90                   	nop
  4231f0:	56                   	push   esi
  4231f1:	8b f1                	mov    esi,ecx
  4231f3:	8b 86 7c 08 00 00    	mov    eax,DWORD PTR [esi+0x87c]
  4231f9:	85 c0                	test   eax,eax
  4231fb:	0f 8f c2 00 00 00    	jg     0x4232c3
  423201:	6a 01                	push   0x1
  423203:	e8 a8 d5 ff ff       	call   0x4207b0
  423208:	8b 86 a4 0b 00 00    	mov    eax,DWORD PTR [esi+0xba4]
  42320e:	c7 86 2c 07 00 00 ff 	mov    DWORD PTR [esi+0x72c],0xffffffff
  423215:	ff ff ff 
  423218:	c7 86 7c 08 00 00 00 	mov    DWORD PTR [esi+0x87c],0x0
  42321f:	00 00 00 
  423222:	6a ff                	push   0xffffffff
  423224:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  423227:	c6 81 58 03 00 00 00 	mov    BYTE PTR [ecx+0x358],0x0
  42322e:	8b 96 a4 0b 00 00    	mov    edx,DWORD PTR [esi+0xba4]
  423234:	8b 4a 20             	mov    ecx,DWORD PTR [edx+0x20]
  423237:	e8 d4 bf 01 00       	call   0x43f210
  42323c:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  423242:	c7 86 e4 0c 00 00 b9 	mov    DWORD PTR [esi+0xce4],0xbb9
  423249:	0b 00 00 
  42324c:	85 c0                	test   eax,eax
  42324e:	75 06                	jne    0x423256
  423250:	ff 05 f8 fd 89 00    	inc    DWORD PTR ds:0x89fdf8
  423256:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  42325c:	55                   	push   ebp
  42325d:	57                   	push   edi
  42325e:	8b 41 20             	mov    eax,DWORD PTR [ecx+0x20]
  423261:	8b 78 24             	mov    edi,DWORD PTR [eax+0x24]
  423264:	8b 68 28             	mov    ebp,DWORD PTR [eax+0x28]
  423267:	81 c7 25 01 00 00    	add    edi,0x125
  42326d:	81 c5 f6 00 00 00    	add    ebp,0xf6
  423273:	e8 78 9f 01 00       	call   0x43d1f0
  423278:	6a 01                	push   0x1
  42327a:	50                   	push   eax
  42327b:	55                   	push   ebp
  42327c:	57                   	push   edi
  42327d:	8d 86 d4 0a 00 00    	lea    eax,[esi+0xad4]
  423283:	6a 00                	push   0x0
  423285:	50                   	push   eax
  423286:	8b ce                	mov    ecx,esi
  423288:	e8 73 c4 ff ff       	call   0x41f700
  42328d:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  423293:	50                   	push   eax
  423294:	e8 27 a7 00 00       	call   0x42d9c0
  423299:	8b ce                	mov    ecx,esi
  42329b:	e8 30 ff ff ff       	call   0x4231d0
  4232a0:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4232a2:	51                   	push   ecx
  4232a3:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4232a8:	e8 b3 a9 01 00       	call   0x43dc60
  4232ad:	8b 16                	mov    edx,DWORD PTR [esi]
  4232af:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  4232b4:	52                   	push   edx
  4232b5:	e8 a6 a9 01 00       	call   0x43dc60
  4232ba:	8b ce                	mov    ecx,esi
  4232bc:	e8 af 01 00 00       	call   0x423470
  4232c1:	5f                   	pop    edi
  4232c2:	5d                   	pop    ebp
  4232c3:	5e                   	pop    esi
  4232c4:	c3                   	ret
  4232c5:	90                   	nop
  4232c6:	90                   	nop
  4232c7:	90                   	nop
  4232c8:	90                   	nop
  4232c9:	90                   	nop
  4232ca:	90                   	nop
  4232cb:	90                   	nop
  4232cc:	90                   	nop
  4232cd:	90                   	nop
  4232ce:	90                   	nop
  4232cf:	90                   	nop
  4232d0:	53                   	push   ebx
  4232d1:	55                   	push   ebp
  4232d2:	56                   	push   esi
  4232d3:	8b f1                	mov    esi,ecx
  4232d5:	57                   	push   edi
  4232d6:	8b 86 4c 08 00 00    	mov    eax,DWORD PTR [esi+0x84c]
  4232dc:	83 f8 02             	cmp    eax,0x2
  4232df:	0f 84 7f 01 00 00    	je     0x423464
  4232e5:	83 f8 05             	cmp    eax,0x5
  4232e8:	0f 84 76 01 00 00    	je     0x423464
  4232ee:	83 f8 08             	cmp    eax,0x8
  4232f1:	0f 84 6d 01 00 00    	je     0x423464
  4232f7:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  4232fd:	85 c0                	test   eax,eax
  4232ff:	0f 84 5f 01 00 00    	je     0x423464
  423305:	8b be e4 08 00 00    	mov    edi,DWORD PTR [esi+0x8e4]
  42330b:	85 ff                	test   edi,edi
  42330d:	7e 2d                	jle    0x42333c
  42330f:	a1 b4 01 8a 00       	mov    eax,ds:0x8a01b4
  423314:	ba 01 00 00 00       	mov    edx,0x1
  423319:	8b 48 04             	mov    ecx,DWORD PTR [eax+0x4]
  42331c:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  423320:	81 c1 6c 01 00 00    	add    ecx,0x16c
  423326:	39 79 fc             	cmp    DWORD PTR [ecx-0x4],edi
  423329:	74 04                	je     0x42332f
  42332b:	39 39                	cmp    DWORD PTR [ecx],edi
  42332d:	75 02                	jne    0x423331
  42332f:	8b c2                	mov    eax,edx
  423331:	42                   	inc    edx
  423332:	83 c1 08             	add    ecx,0x8
  423335:	83 fa 05             	cmp    edx,0x5
  423338:	7e ec                	jle    0x423326
  42333a:	eb 04                	jmp    0x423340
  42333c:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  423340:	85 c0                	test   eax,eax
  423342:	0f 84 1c 01 00 00    	je     0x423464
  423348:	83 f8 01             	cmp    eax,0x1
  42334b:	0f 8c 13 01 00 00    	jl     0x423464
  423351:	83 f8 05             	cmp    eax,0x5
  423354:	0f 8f 0a 01 00 00    	jg     0x423464
  42335a:	89 86 20 07 00 00    	mov    DWORD PTR [esi+0x720],eax
  423360:	8b 0d b4 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01b4
  423366:	33 ff                	xor    edi,edi
  423368:	8b 49 04             	mov    ecx,DWORD PTR [ecx+0x4]
  42336b:	8b 9c c1 30 01 00 00 	mov    ebx,DWORD PTR [ecx+eax*8+0x130]
  423372:	8b ac c1 34 01 00 00 	mov    ebp,DWORD PTR [ecx+eax*8+0x134]
  423379:	8b 8e 80 08 00 00    	mov    ecx,DWORD PTR [esi+0x880]
  42337f:	85 c9                	test   ecx,ecx
  423381:	7e 12                	jle    0x423395
  423383:	8b 84 86 84 09 00 00 	mov    eax,DWORD PTR [esi+eax*4+0x984]
  42338a:	99                   	cdq
  42338b:	83 e2 03             	and    edx,0x3
  42338e:	03 c2                	add    eax,edx
  423390:	8b f8                	mov    edi,eax
  423392:	c1 ff 02             	sar    edi,0x2
  423395:	e8 cb e1 02 00       	call   0x451565
  42339a:	99                   	cdq
  42339b:	b9 64 00 00 00       	mov    ecx,0x64
  4233a0:	f7 f9                	idiv   ecx
  4233a2:	3b fa                	cmp    edi,edx
  4233a4:	0f 8c ba 00 00 00    	jl     0x423464
  4233aa:	85 db                	test   ebx,ebx
  4233ac:	7e 08                	jle    0x4233b6
  4233ae:	53                   	push   ebx
  4233af:	8b ce                	mov    ecx,esi
  4233b1:	e8 4a fd ff ff       	call   0x423100
  4233b6:	85 ed                	test   ebp,ebp
  4233b8:	7e 08                	jle    0x4233c2
  4233ba:	55                   	push   ebp
  4233bb:	8b ce                	mov    ecx,esi
  4233bd:	e8 3e fd ff ff       	call   0x423100
  4233c2:	85 db                	test   ebx,ebx
  4233c4:	75 11                	jne    0x4233d7
  4233c6:	85 ed                	test   ebp,ebp
  4233c8:	75 0d                	jne    0x4233d7
  4233ca:	89 ae 20 07 00 00    	mov    DWORD PTR [esi+0x720],ebp
  4233d0:	5f                   	pop    edi
  4233d1:	5e                   	pop    esi
  4233d2:	5d                   	pop    ebp
  4233d3:	5b                   	pop    ebx
  4233d4:	c2 04 00             	ret    0x4
  4233d7:	8b 86 9c 0b 00 00    	mov    eax,DWORD PTR [esi+0xb9c]
  4233dd:	8b 15 f0 fd 89 00    	mov    edx,DWORD PTR ds:0x89fdf0
  4233e3:	8b 0d ec fd 89 00    	mov    ecx,DWORD PTR ds:0x89fdec
  4233e9:	c1 e0 02             	shl    eax,0x2
  4233ec:	8b 3c 10             	mov    edi,DWORD PTR [eax+edx*1]
  4233ef:	8b 1c 08             	mov    ebx,DWORD PTR [eax+ecx*1]
  4233f2:	8b 86 1c 07 00 00    	mov    eax,DWORD PTR [esi+0x71c]
  4233f8:	83 ef 17             	sub    edi,0x17
  4233fb:	83 eb 46             	sub    ebx,0x46
  4233fe:	85 c0                	test   eax,eax
  423400:	7c 0c                	jl     0x42340e
  423402:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  423408:	50                   	push   eax
  423409:	e8 e2 a3 00 00       	call   0x42d7f0
  42340e:	8b 8e a4 0b 00 00    	mov    ecx,DWORD PTR [esi+0xba4]
  423414:	6a 00                	push   0x0
  423416:	6a 00                	push   0x0
  423418:	e8 d3 9d 01 00       	call   0x43d1f0
  42341d:	05 40 01 00 00       	add    eax,0x140
  423422:	50                   	push   eax
  423423:	8b 86 20 07 00 00    	mov    eax,DWORD PTR [esi+0x720]
  423429:	53                   	push   ebx
  42342a:	57                   	push   edi
  42342b:	8d 04 80             	lea    eax,[eax+eax*4]
  42342e:	53                   	push   ebx
  42342f:	57                   	push   edi
  423430:	8d 14 80             	lea    edx,[eax+eax*4]
  423433:	a1 b4 01 8a 00       	mov    eax,ds:0x8a01b4
  423438:	8b 48 04             	mov    ecx,DWORD PTR [eax+0x4]
  42343b:	8d 54 51 04          	lea    edx,[ecx+edx*2+0x4]
  42343f:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  423445:	52                   	push   edx
  423446:	e8 85 a0 00 00       	call   0x42d4d0
  42344b:	83 f8 ff             	cmp    eax,0xffffffff
  42344e:	89 86 1c 07 00 00    	mov    DWORD PTR [esi+0x71c],eax
  423454:	74 0e                	je     0x423464
  423456:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  42345c:	6a 01                	push   0x1
  42345e:	50                   	push   eax
  42345f:	e8 7c a2 00 00       	call   0x42d6e0
  423464:	5f                   	pop    edi
  423465:	5e                   	pop    esi
  423466:	5d                   	pop    ebp
  423467:	5b                   	pop    ebx
  423468:	c2 04 00             	ret    0x4
  42346b:	90                   	nop
  42346c:	90                   	nop
  42346d:	90                   	nop
  42346e:	90                   	nop
  42346f:	90                   	nop
  423470:	51                   	push   ecx
  423471:	56                   	push   esi
  423472:	8b f1                	mov    esi,ecx
  423474:	33 c0                	xor    eax,eax
  423476:	8b 8e 20 07 00 00    	mov    ecx,DWORD PTR [esi+0x720]
  42347c:	88 44 24 07          	mov    BYTE PTR [esp+0x7],al
  423480:	3b c8                	cmp    ecx,eax
  423482:	0f 84 c4 00 00 00    	je     0x42354c
  423488:	53                   	push   ebx
  423489:	55                   	push   ebp
  42348a:	57                   	push   edi
  42348b:	8b be 48 08 00 00    	mov    edi,DWORD PTR [esi+0x848]
  423491:	85 ff                	test   edi,edi
  423493:	74 19                	je     0x4234ae
  423495:	8b 1d b4 01 8a 00    	mov    ebx,DWORD PTR ds:0x8a01b4
  42349b:	8b 2f                	mov    ebp,DWORD PTR [edi]
  42349d:	8d 54 48 4c          	lea    edx,[eax+ecx*2+0x4c]
  4234a1:	8b 5b 04             	mov    ebx,DWORD PTR [ebx+0x4]
  4234a4:	3b 2c 93             	cmp    ebp,DWORD PTR [ebx+edx*4]
  4234a7:	75 05                	jne    0x4234ae
  4234a9:	c6 44 24 13 01       	mov    BYTE PTR [esp+0x13],0x1
  4234ae:	40                   	inc    eax
  4234af:	83 f8 02             	cmp    eax,0x2
  4234b2:	7c dd                	jl     0x423491
  4234b4:	8a 44 24 13          	mov    al,BYTE PTR [esp+0x13]
  4234b8:	84 c0                	test   al,al
  4234ba:	74 2e                	je     0x4234ea
  4234bc:	8b 94 8e 84 09 00 00 	mov    edx,DWORD PTR [esi+ecx*4+0x984]
  4234c3:	42                   	inc    edx
  4234c4:	89 94 8e 84 09 00 00 	mov    DWORD PTR [esi+ecx*4+0x984],edx
  4234cb:	8b 86 20 07 00 00    	mov    eax,DWORD PTR [esi+0x720]
  4234d1:	8b 8c 86 84 09 00 00 	mov    ecx,DWORD PTR [esi+eax*4+0x984]
  4234d8:	8d 84 86 84 09 00 00 	lea    eax,[esi+eax*4+0x984]
  4234df:	83 f9 64             	cmp    ecx,0x64
  4234e2:	7e 06                	jle    0x4234ea
  4234e4:	c7 00 64 00 00 00    	mov    DWORD PTR [eax],0x64
  4234ea:	8b 0d b4 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01b4
  4234f0:	8b 86 20 07 00 00    	mov    eax,DWORD PTR [esi+0x720]
  4234f6:	8b 49 04             	mov    ecx,DWORD PTR [ecx+0x4]
  4234f9:	8b 94 c1 30 01 00 00 	mov    edx,DWORD PTR [ecx+eax*8+0x130]
  423500:	8b bc c1 34 01 00 00 	mov    edi,DWORD PTR [ecx+eax*8+0x134]
  423507:	85 d2                	test   edx,edx
  423509:	7e 08                	jle    0x423513
  42350b:	52                   	push   edx
  42350c:	8b ce                	mov    ecx,esi
  42350e:	e8 5d fc ff ff       	call   0x423170
  423513:	85 ff                	test   edi,edi
  423515:	7e 08                	jle    0x42351f
  423517:	57                   	push   edi
  423518:	8b ce                	mov    ecx,esi
  42351a:	e8 51 fc ff ff       	call   0x423170
  42351f:	8b 86 1c 07 00 00    	mov    eax,DWORD PTR [esi+0x71c]
  423525:	5f                   	pop    edi
  423526:	5d                   	pop    ebp
  423527:	c7 86 20 07 00 00 00 	mov    DWORD PTR [esi+0x720],0x0
  42352e:	00 00 00 
  423531:	85 c0                	test   eax,eax
  423533:	5b                   	pop    ebx
  423534:	7c 16                	jl     0x42354c
  423536:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  42353c:	50                   	push   eax
  42353d:	e8 ae a2 00 00       	call   0x42d7f0
  423542:	c7 86 1c 07 00 00 ff 	mov    DWORD PTR [esi+0x71c],0xffffffff
  423549:	ff ff ff 
  42354c:	5e                   	pop    esi
  42354d:	59                   	pop    ecx
  42354e:	c3                   	ret
  42354f:	90                   	nop
  423550:	56                   	push   esi
  423551:	57                   	push   edi
  423552:	8b f9                	mov    edi,ecx
  423554:	33 f6                	xor    esi,esi
  423556:	a1 b4 01 8a 00       	mov    eax,ds:0x8a01b4
  42355b:	8b 48 04             	mov    ecx,DWORD PTR [eax+0x4]
  42355e:	8b 84 31 30 01 00 00 	mov    eax,DWORD PTR [ecx+esi*1+0x130]
  423565:	85 c0                	test   eax,eax
  423567:	7e 08                	jle    0x423571
  423569:	50                   	push   eax
  42356a:	8b cf                	mov    ecx,edi
  42356c:	e8 ff fb ff ff       	call   0x423170
  423571:	8b 15 b4 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b4
  423577:	8b 42 04             	mov    eax,DWORD PTR [edx+0x4]
  42357a:	8b 84 30 34 01 00 00 	mov    eax,DWORD PTR [eax+esi*1+0x134]
  423581:	85 c0                	test   eax,eax
  423583:	7e 08                	jle    0x42358d
  423585:	50                   	push   eax
  423586:	8b cf                	mov    ecx,edi
  423588:	e8 e3 fb ff ff       	call   0x423170
  42358d:	83 c6 08             	add    esi,0x8
  423590:	83 fe 28             	cmp    esi,0x28
  423593:	7c c1                	jl     0x423556
  423595:	5f                   	pop    edi
  423596:	5e                   	pop    esi
  423597:	c3                   	ret


; ===== 研究版全槽清理上界 0x00423550..0x00423598 =====

/workspace/scratch/3f7398360f84/upload/RPG.exe:     file format pei-i386


Disassembly of section .text:

00423550 <.text+0x22550>:
  423550:	56                   	push   esi
  423551:	57                   	push   edi
  423552:	8b f9                	mov    edi,ecx
  423554:	33 f6                	xor    esi,esi
  423556:	a1 b4 01 8a 00       	mov    eax,ds:0x8a01b4
  42355b:	8b 48 04             	mov    ecx,DWORD PTR [eax+0x4]
  42355e:	8b 84 31 30 01 00 00 	mov    eax,DWORD PTR [ecx+esi*1+0x130]
  423565:	85 c0                	test   eax,eax
  423567:	7e 08                	jle    0x423571
  423569:	50                   	push   eax
  42356a:	8b cf                	mov    ecx,edi
  42356c:	e8 ff fb ff ff       	call   0x423170
  423571:	8b 15 b4 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b4
  423577:	8b 42 04             	mov    eax,DWORD PTR [edx+0x4]
  42357a:	8b 84 30 34 01 00 00 	mov    eax,DWORD PTR [eax+esi*1+0x134]
  423581:	85 c0                	test   eax,eax
  423583:	7e 08                	jle    0x42358d
  423585:	50                   	push   eax
  423586:	8b cf                	mov    ecx,edi
  423588:	e8 e3 fb ff ff       	call   0x423170
  42358d:	83 c6 08             	add    esi,0x8
  423590:	83 fe 30             	cmp    esi,0x30
  423593:	7c c1                	jl     0x423556
  423595:	5f                   	pop    edi
  423596:	5e                   	pop    esi
  423597:	c3                   	ret


; ===== 清理后角色数据回写 0x00441F80..0x00442034 =====

/workspace/scratch/3f7398360f84/work/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00441f80 <.text+0x40f80>:
  441f80:	53                   	push   ebx
  441f81:	55                   	push   ebp
  441f82:	56                   	push   esi
  441f83:	57                   	push   edi
  441f84:	c7 44 24 10 68 00 00 	mov    DWORD PTR [esp+0x10],0x68
  441f8b:	00 
  441f8c:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  441f90:	8b 1c 08             	mov    ebx,DWORD PTR [eax+ecx*1]
  441f93:	85 db                	test   ebx,ebx
  441f95:	7e 71                	jle    0x442008
  441f97:	83 fb 09             	cmp    ebx,0x9
  441f9a:	7d 6c                	jge    0x442008
  441f9c:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  441fa0:	c7 44 24 14 08 00 00 	mov    DWORD PTR [esp+0x14],0x8
  441fa7:	00 
  441fa8:	8d aa 80 7b 00 00    	lea    ebp,[edx+0x7b80]
  441fae:	8b 45 d0             	mov    eax,DWORD PTR [ebp-0x30]
  441fb1:	8d 75 d0             	lea    esi,[ebp-0x30]
  441fb4:	3b c3                	cmp    eax,ebx
  441fb6:	75 3f                	jne    0x441ff7
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
  441ff1:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  441ff7:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  441ffb:	81 c5 e4 0d 00 00    	add    ebp,0xde4
  442001:	48                   	dec    eax
  442002:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  442006:	75 a6                	jne    0x441fae
  442008:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  44200c:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  442010:	8b 51 3c             	mov    edx,DWORD PTR [ecx+0x3c]
  442013:	40                   	inc    eax
  442014:	83 c6 04             	add    esi,0x4
  442017:	3b c2                	cmp    eax,edx
  442019:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  44201d:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  442021:	0f 8c 65 ff ff ff    	jl     0x441f8c
  442027:	5f                   	pop    edi
  442028:	5e                   	pop    esi
  442029:	5d                   	pop    ebp
  44202a:	5b                   	pop    ebx
  44202b:	e8 50 8e ff ff       	call   0x43ae80
  442030:	83 c4 10             	add    esp,0x10
  442033:	c3                   	ret

