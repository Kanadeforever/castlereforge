; 《幽城幻剑录》固化44静态反汇编证据节选
; 主题：Battle 39槽序列化零块、pending/commit/decrement与后11槽canonical正值生产边界
; 来源：本轮 RPG.exe.org 解包得到的 RPG.exe，使用 objdump 静态反汇编。
; 注意：本文件只保存与固化44直接有关的机器码窗口；完整结论与自动断言见同目录JSON及数据/固化44。
;
; 关键算式：
;   FightRole+0x84C + RoleDefinition+0x1E8 = FightRole+0xA34
;   runtime 39 DWORD = FightRole+0xA34..+0xACC
;   tail indices 28..38 = FightRole+0xAA4..+0xACC
;   pending 39 DWORD = FightRole+0xCC..+0x164
;
; ===== 1) constructor/reset：RoleDefinition整块清零 + pending 39项清零 =====
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
  41f186:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  41f18a:	8b c6                	mov    eax,esi
  41f18c:	5f                   	pop    edi
  41f18d:	5e                   	pop    esi
  41f18e:	5d                   	pop    ebp
  41f18f:	5b                   	pop    ebx
  41f190:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  41f197:	83 c4 10             	add    esp,0x10
  41f19a:	c3                   	ret
  41f19b:	90                   	nop
  41f19c:	90                   	nop
  41f19d:	90                   	nop
  41f19e:	90                   	nop
  41f19f:	90                   	nop
  41f1a0:	6a ff                	push   0xffffffff
  41f1a2:	68 ca e2 45 00       	push   0x45e2ca
  41f1a7:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  41f1ad:	50                   	push   eax
  41f1ae:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  41f1b5:	51                   	push   ecx
  41f1b6:	56                   	push   esi
  41f1b7:	8b f1                	mov    esi,ecx
  41f1b9:	89 74 24 04          	mov    DWORD PTR [esp+0x4],esi
  41f1bd:	8b 06                	mov    eax,DWORD PTR [esi]
  41f1bf:	c7 44 24 10 02 00 00 	mov    DWORD PTR [esp+0x10],0x2
  41f1c6:	00 
  41f1c7:	85 c0                	test   eax,eax
  41f1c9:	7c 10                	jl     0x41f1db
  41f1cb:	83 f8 10             	cmp    eax,0x10
  41f1ce:	7d 0b                	jge    0x41f1db
  41f1d0:	c7 04 85 94 fd 89 00 	mov    DWORD PTR [eax*4+0x89fd94],0x0
  41f1d7:	00 00 00 00 
  41f1db:	8b ce                	mov    ecx,esi
  41f1dd:	e8 4e 00 00 00       	call   0x41f230
  41f1e2:	8d 8e a8 0c 00 00    	lea    ecx,[esi+0xca8]
  41f1e8:	c6 44 24 10 01       	mov    BYTE PTR [esp+0x10],0x1
  41f1ed:	e8 9e 4b 00 00       	call   0x423d90
  41f1f2:	8d 8e b4 07 00 00    	lea    ecx,[esi+0x7b4]
  41f1f8:	c6 44 24 10 00       	mov    BYTE PTR [esp+0x10],0x0
  41f1fd:	e8 2e 5f 02 00       	call   0x445130
  41f202:	8d 8e 30 07 00 00    	lea    ecx,[esi+0x730]
  41f208:	c7 44 24 10 ff ff ff 	mov    DWORD PTR [esp+0x10],0xffffffff
  41f20f:	ff 
  41f210:	e8 5b 9e ff ff       	call   0x419070
  41f215:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  41f219:	5e                   	pop    esi
  41f21a:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  41f221:	83 c4 10             	add    esp,0x10
  41f224:	c3                   	ret
  41f225:	90                   	nop
  41f226:	90                   	nop
  41f227:	90                   	nop
  41f228:	90                   	nop
  41f229:	90                   	nop
  41f22a:	90                   	nop
  41f22b:	90                   	nop
  41f22c:	90                   	nop
  41f22d:	90                   	nop
  41f22e:	90                   	nop
  41f22f:	90                   	nop
  41f230:	53                   	push   ebx
  41f231:	56                   	push   esi
  41f232:	8b f1                	mov    esi,ecx
  41f234:	33 db                	xor    ebx,ebx
  41f236:	57                   	push   edi
  41f237:	c7 86 2c 07 00 00 ff 	mov    DWORD PTR [esi+0x72c],0xffffffff
  41f23e:	ff ff ff 
  41f241:	89 9e 9c 0b 00 00    	mov    DWORD PTR [esi+0xb9c],ebx
  41f247:	89 9e 24 07 00 00    	mov    DWORD PTR [esi+0x724],ebx
  41f24d:	c7 86 a4 07 00 00 e9 	mov    DWORD PTR [esi+0x7a4],0x3e9
  41f254:	03 00 00 
  41f257:	c7 86 e8 0c 00 00 a0 	mov    DWORD PTR [esi+0xce8],0xfa0
  41f25e:	0f 00 00 
  41f261:	89 9e 48 08 00 00    	mov    DWORD PTR [esi+0x848],ebx
  41f267:	89 9e a4 0b 00 00    	mov    DWORD PTR [esi+0xba4],ebx
  41f26d:	88 5e 04             	mov    BYTE PTR [esi+0x4],bl
  41f270:	c6 86 b0 07 00 00 01 	mov    BYTE PTR [esi+0x7b0],0x1
  41f277:	88 9e 28 07 00 00    	mov    BYTE PTR [esi+0x728],bl
  41f27d:	e8 4e 3f 00 00       	call   0x4231d0
  41f282:	33 c0                	xor    eax,eax
  41f284:	8d 8e cc 00 00 00    	lea    ecx,[esi+0xcc]
  41f28a:	89 19                	mov    DWORD PTR [ecx],ebx
  41f28c:	88 9c 06 e8 06 00 00 	mov    BYTE PTR [esi+eax*1+0x6e8],bl
  41f293:	40                   	inc    eax
  41f294:	83 c1 04             	add    ecx,0x4
  41f297:	83 f8 27             	cmp    eax,0x27
  41f29a:	7c ee                	jl     0x41f28a
  41f29c:	8d be 70 06 00 00    	lea    edi,[esi+0x670]
  41f2a2:	b9 0f 00 00 00       	mov    ecx,0xf
  41f2a7:	83 c8 ff             	or     eax,0xffffffff
  41f2aa:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  41f2ac:	5f                   	pop    edi
  41f2ad:	5e                   	pop    esi
  41f2ae:	5b                   	pop    ebx
  41f2af:	c3                   	ret
  41f2b0:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  41f2b4:	8b 54 24 0c          	mov    edx,DWORD PTR [esp+0xc]
  41f2b8:	56                   	push   esi
  41f2b9:	8b f1                	mov    esi,ecx
  41f2bb:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]

; ===== 2) Public\\API.ENC loader：0x350字节整记录复制到 FightRole+0x84C =====
  41f59d:	6a 01                	push   0x1
  41f59f:	8d 4c 24 10          	lea    ecx,[esp+0x10]
  41f5a3:	e8 48 21 02 00       	call   0x4416f0
  41f5a8:	33 f6                	xor    esi,esi
  41f5aa:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  41f5ae:	56                   	push   esi
  41f5af:	6a 01                	push   0x1
  41f5b1:	68 2c 9f 46 00       	push   0x469f2c
  41f5b6:	c7 44 24 34 00 00 00 	mov    DWORD PTR [esp+0x34],0x0
  41f5bd:	00 
  41f5be:	e8 fd 21 02 00       	call   0x4417c0
  41f5c3:	84 c0                	test   al,al
  41f5c5:	75 0a                	jne    0x41f5d1
  41f5c7:	68 10 9f 46 00       	push   0x469f10
  41f5cc:	e9 ad 00 00 00       	jmp    0x41f67e
  41f5d1:	8b 87 2c 07 00 00    	mov    eax,DWORD PTR [edi+0x72c]
  41f5d7:	85 c0                	test   eax,eax
  41f5d9:	7c 37                	jl     0x41f612
  41f5db:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  41f5df:	e8 4c d8 00 00       	call   0x42ce30
  41f5e4:	8b 8f 2c 07 00 00    	mov    ecx,DWORD PTR [edi+0x72c]
  41f5ea:	8b f0                	mov    esi,eax
  41f5ec:	8d 04 49             	lea    eax,[ecx+ecx*2]
  41f5ef:	8d 04 c0             	lea    eax,[eax+eax*8]
  41f5f2:	d1 e0                	shl    eax,1
  41f5f4:	2b c1                	sub    eax,ecx
  41f5f6:	c1 e0 04             	shl    eax,0x4
  41f5f9:	05 50 03 00 00       	add    eax,0x350
  41f5fe:	3b c6                	cmp    eax,esi
  41f600:	7e 10                	jle    0x41f612
  41f602:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  41f606:	e8 f5 23 02 00       	call   0x441a00
  41f60b:	68 e4 9e 46 00       	push   0x469ee4
  41f610:	eb 6c                	jmp    0x41f67e
  41f612:	56                   	push   esi
  41f613:	e8 47 20 03 00       	call   0x45165f
  41f618:	83 c4 04             	add    esp,0x4
  41f61b:	8b d8                	mov    ebx,eax
  41f61d:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  41f621:	53                   	push   ebx
  41f622:	56                   	push   esi
  41f623:	6a 00                	push   0x0
  41f625:	e8 06 24 02 00       	call   0x441a30
  41f62a:	56                   	push   esi
  41f62b:	53                   	push   ebx
  41f62c:	e8 9f 8e 00 00       	call   0x4284d0
  41f631:	83 c4 08             	add    esp,0x8
  41f634:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  41f638:	e8 c3 23 02 00       	call   0x441a00
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
  41f6c3:	85 db                	test   ebx,ebx
  41f6c5:	74 09                	je     0x41f6d0
  41f6c7:	53                   	push   ebx
  41f6c8:	e8 83 1e 03 00       	call   0x451550
  41f6cd:	83 c4 04             	add    esp,0x4
  41f6d0:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  41f6d4:	c7 44 24 28 ff ff ff 	mov    DWORD PTR [esp+0x28],0xffffffff
  41f6db:	ff 
  41f6dc:	e8 2f 20 02 00       	call   0x441710
  41f6e1:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  41f6e5:	5f                   	pop    edi
  41f6e6:	5e                   	pop    esi
  41f6e7:	b0 01                	mov    al,0x1
  41f6e9:	5b                   	pop    ebx
  41f6ea:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  41f6f1:	83 c4 20             	add    esp,0x20
  41f6f4:	c3                   	ret
  41f6f5:	90                   	nop
  41f6f6:	90                   	nop
  41f6f7:	90                   	nop
  41f6f8:	90                   	nop

; ===== 3) runtime 39槽通用递减 =====
  420624:	85 c0                	test   eax,eax
  420626:	7e 05                	jle    0x42062d
  420628:	ba 0a 00 00 00       	mov    edx,0xa
  42062d:	8b 86 74 0a 00 00    	mov    eax,DWORD PTR [esi+0xa74]
  420633:	85 c0                	test   eax,eax
  420635:	7e 1a                	jle    0x420651
  420637:	8d 04 52             	lea    eax,[edx+edx*2]
  42063a:	8d 04 80             	lea    eax,[eax+eax*4]
  42063d:	8d 0c 80             	lea    ecx,[eax+eax*4]
  420640:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  420645:	f7 e9                	imul   ecx
  420647:	c1 fa 05             	sar    edx,0x5
  42064a:	8b c2                	mov    eax,edx
  42064c:	c1 e8 1f             	shr    eax,0x1f
  42064f:	03 d0                	add    edx,eax
  420651:	6a 03                	push   0x3
  420653:	52                   	push   edx
  420654:	8b cb                	mov    ecx,ebx
  420656:	e8 35 3a 00 00       	call   0x424090
  42065b:	8b cb                	mov    ecx,ebx
  42065d:	e8 2e 38 00 00       	call   0x423e90
  420662:	8d be 34 0a 00 00    	lea    edi,[esi+0xa34]
  420668:	33 d2                	xor    edx,edx
  42066a:	8b c7                	mov    eax,edi
  42066c:	5b                   	pop    ebx
  42066d:	8b 08                	mov    ecx,DWORD PTR [eax]
  42066f:	85 c9                	test   ecx,ecx
  420671:	7e 1d                	jle    0x420690
  420673:	83 fa 10             	cmp    edx,0x10
  420676:	75 09                	jne    0x420681
  420678:	85 c9                	test   ecx,ecx
  42067a:	7e 14                	jle    0x420690
  42067c:	83 fa 13             	cmp    edx,0x13
  42067f:	74 0f                	je     0x420690
  420681:	83 c1 f6             	add    ecx,0xfffffff6
  420684:	85 c9                	test   ecx,ecx
  420686:	89 08                	mov    DWORD PTR [eax],ecx
  420688:	7f 06                	jg     0x420690
  42068a:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
  420690:	42                   	inc    edx
  420691:	83 c0 04             	add    eax,0x4
  420694:	83 fa 27             	cmp    edx,0x27
  420697:	7c d4                	jl     0x42066d
  420699:	8b 3f                	mov    edi,DWORD PTR [edi]
  42069b:	b8 b5 81 4e 1b       	mov    eax,0x1b4e81b5
  4206a0:	8d 4f 0a             	lea    ecx,[edi+0xa]
  4206a3:	f7 e9                	imul   ecx
  4206a5:	8b ca                	mov    ecx,edx
  4206a7:	b8 b5 81 4e 1b       	mov    eax,0x1b4e81b5
  4206ac:	c1 f9 06             	sar    ecx,0x6
  4206af:	8b d1                	mov    edx,ecx

; ===== 4) pending 39槽 -> runtime 39槽 commit =====
  4215e9:	52                   	push   edx
  4215ea:	eb 16                	jmp    0x421602
  4215ec:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  4215f0:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  4215f4:	6a 01                	push   0x1
  4215f6:	6a 01                	push   0x1
  4215f8:	83 c0 ce             	add    eax,0xffffffce
  4215fb:	6a 00                	push   0x0
  4215fd:	83 c1 ec             	add    ecx,0xffffffec
  421600:	50                   	push   eax
  421601:	51                   	push   ecx
  421602:	8b 0d 14 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2414
  421608:	e8 e3 57 02 00       	call   0x446df0
  42160d:	b8 cc 00 00 00       	mov    eax,0xcc
  421612:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421619:	8b 14 01             	mov    edx,DWORD PTR [ecx+eax*1]
  42161c:	85 d2                	test   edx,edx
  42161e:	7e 1f                	jle    0x42163f
  421620:	8b b9 6c 0a 00 00    	mov    edi,DWORD PTR [ecx+0xa6c]
  421626:	85 ff                	test   edi,edi
  421628:	7f 15                	jg     0x42163f
  42162a:	89 94 01 68 09 00 00 	mov    DWORD PTR [ecx+eax*1+0x968],edx
  421631:	8b 14 b5 94 fd 89 00 	mov    edx,DWORD PTR [esi*4+0x89fd94]
  421638:	c7 04 02 00 00 00 00 	mov    DWORD PTR [edx+eax*1],0x0
  42163f:	83 c0 04             	add    eax,0x4
  421642:	3d 68 01 00 00       	cmp    eax,0x168
  421647:	7c c9                	jl     0x421612
  421649:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421650:	6a 00                	push   0x0
  421652:	e8 59 f1 ff ff       	call   0x4207b0
  421657:	8b 7c 24 14          	mov    edi,DWORD PTR [esp+0x14]
  42165b:	33 ed                	xor    ebp,ebp
  42165d:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  421661:	8b 74 24 18          	mov    esi,DWORD PTR [esp+0x18]
  421665:	8b 8f c8 00 00 00    	mov    ecx,DWORD PTR [edi+0xc8]
  42166b:	40                   	inc    eax
  42166c:	83 c6 04             	add    esi,0x4
  42166f:	83 c3 50             	add    ebx,0x50
  421672:	3b c1                	cmp    eax,ecx
  421674:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  421678:	89 74 24 18          	mov    DWORD PTR [esp+0x18],esi

; ===== 5) canonical normal effect writer：raw ID<=27 后写 pending =====
  422c26:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  422c2a:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
  422c30:	eb 45                	jmp    0x422c77
  422c32:	8a 4b 0c             	mov    cl,BYTE PTR [ebx+0xc]
  422c35:	33 d2                	xor    edx,edx
  422c37:	88 4e 04             	mov    BYTE PTR [esi+0x4],cl
  422c3a:	8d 6b 24             	lea    ebp,[ebx+0x24]
  422c3d:	8b 7d 00             	mov    edi,DWORD PTR [ebp+0x0]
  422c40:	85 ff                	test   edi,edi
  422c42:	7e 2a                	jle    0x422c6e
  422c44:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  422c48:	03 c2                	add    eax,edx
  422c4a:	8b 04 86             	mov    eax,DWORD PTR [esi+eax*4]
  422c4d:	83 f8 1b             	cmp    eax,0x1b
  422c50:	7f 1c                	jg     0x422c6e
  422c52:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  422c56:	8b 0c 8d 94 fd 89 00 	mov    ecx,DWORD PTR [ecx*4+0x89fd94]
  422c5d:	83 bc 81 34 0a 00 00 	cmp    DWORD PTR [ecx+eax*4+0xa34],0x0
  422c64:	00 
  422c65:	7f 07                	jg     0x422c6e
  422c67:	89 bc 81 cc 00 00 00 	mov    DWORD PTR [ecx+eax*4+0xcc],edi
  422c6e:	42                   	inc    edx
  422c6f:	83 c5 04             	add    ebp,0x4
  422c72:	83 fa 05             	cmp    edx,0x5
  422c75:	7c c6                	jl     0x422c3d
  422c77:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
  422c7b:	8b 6c 24 18          	mov    ebp,DWORD PTR [esp+0x18]
  422c7f:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  422c83:	8b 8e c8 00 00 00    	mov    ecx,DWORD PTR [esi+0xc8]
  422c89:	40                   	inc    eax
  422c8a:	83 c5 04             	add    ebp,0x4

; ===== 6) runtime direct clear：只清前28槽 =====
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
