; 固化39证据：标准SF2第一套Layer构造与绘制直读链
; 来源：用户本轮 RPG.exe.org 解包所得 RPG.exe。
; 目的：证明 0x407270 以 Section0Record+0x5E+layer_index*29 构造Layer指针并存入对象+0x50；
;       随后 canonical 绘制路径读取 +0x00/+0x02/+0x04/+0x0A/+0x0B/+0x0D/+0x0F，未直接读取 +0x06..+0x09。
; 注意：这是“已确认canonical直接路径”的边界证据，不等价于全程序绝不存在指针别名/间接consumer。


/mnt/data/work39/oracle/RPG.exe:     file format pei-i386


Disassembly of section .text:

00407200 <.text+0x6200>:
  407200:	56                   	push   esi
  407201:	57                   	push   edi
  407202:	8b 7c 24 0c          	mov    edi,DWORD PTR [esp+0xc]
  407206:	8b f1                	mov    esi,ecx
  407208:	85 ff                	test   edi,edi
  40720a:	7c 3f                	jl     0x40724b
  40720c:	8b 46 34             	mov    eax,DWORD PTR [esi+0x34]
  40720f:	33 c9                	xor    ecx,ecx
  407211:	66 8b 48 14          	mov    cx,WORD PTR [eax+0x14]
  407215:	3b f9                	cmp    edi,ecx
  407217:	7d 32                	jge    0x40724b
  407219:	8b 56 38             	mov    edx,DWORD PTR [esi+0x38]
  40721c:	8b 0c ba             	mov    ecx,DWORD PTR [edx+edi*4]
  40721f:	03 c1                	add    eax,ecx
  407221:	89 46 48             	mov    DWORD PTR [esi+0x48],eax
  407224:	66 83 78 54 00       	cmp    WORD PTR [eax+0x54],0x0
  407229:	75 0f                	jne    0x40723a
  40722b:	89 7e 4c             	mov    DWORD PTR [esi+0x4c],edi
  40722e:	c7 46 54 ff ff ff ff 	mov    DWORD PTR [esi+0x54],0xffffffff
  407235:	5f                   	pop    edi
  407236:	5e                   	pop    esi
  407237:	c2 04 00             	ret    0x4
  40723a:	6a 00                	push   0x0
  40723c:	8b ce                	mov    ecx,esi
  40723e:	e8 2d 00 00 00       	call   0x407270
  407243:	89 7e 4c             	mov    DWORD PTR [esi+0x4c],edi
  407246:	5f                   	pop    edi
  407247:	5e                   	pop    esi
  407248:	c2 04 00             	ret    0x4
  40724b:	8d 54 24 0c          	lea    edx,[esp+0xc]
  40724f:	68 d8 37 46 00       	push   0x4637d8
  407254:	52                   	push   edx
  407255:	c7 44 24 14 14 8b 46 	mov    DWORD PTR [esp+0x14],0x468b14
  40725c:	00 
  40725d:	e8 65 a9 04 00       	call   0x451bc7
  407262:	90                   	nop
  407263:	90                   	nop
  407264:	90                   	nop
  407265:	90                   	nop
  407266:	90                   	nop
  407267:	90                   	nop
  407268:	90                   	nop
  407269:	90                   	nop
  40726a:	90                   	nop
  40726b:	90                   	nop
  40726c:	90                   	nop
  40726d:	90                   	nop
  40726e:	90                   	nop
  40726f:	90                   	nop
  407270:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  407274:	56                   	push   esi
  407275:	85 c0                	test   eax,eax
  407277:	7c 31                	jl     0x4072aa
  407279:	8b 51 48             	mov    edx,DWORD PTR [ecx+0x48]
  40727c:	33 f6                	xor    esi,esi
  40727e:	66 8b 72 54          	mov    si,WORD PTR [edx+0x54]
  407282:	3b c6                	cmp    eax,esi
  407284:	7d 24                	jge    0x4072aa
  407286:	8d 34 c5 00 00 00 00 	lea    esi,[eax*8+0x0]
  40728d:	03 d0                	add    edx,eax
  40728f:	2b f0                	sub    esi,eax
  407291:	89 41 54             	mov    DWORD PTR [ecx+0x54],eax
  407294:	33 c0                	xor    eax,eax
  407296:	8d 54 b2 5e          	lea    edx,[edx+esi*4+0x5e]
  40729a:	89 51 50             	mov    DWORD PTR [ecx+0x50],edx
  40729d:	66 8b 02             	mov    ax,WORD PTR [edx]
  4072a0:	50                   	push   eax
  4072a1:	e8 2a 00 00 00       	call   0x4072d0
  4072a6:	5e                   	pop    esi
  4072a7:	c2 04 00             	ret    0x4
  4072aa:	8d 4c 24 08          	lea    ecx,[esp+0x8]
  4072ae:	68 d8 37 46 00       	push   0x4637d8
  4072b3:	51                   	push   ecx
  4072b4:	c7 44 24 10 40 8b 46 	mov    DWORD PTR [esp+0x10],0x468b40
  4072bb:	00 
  4072bc:	e8 06 a9 04 00       	call   0x451bc7
  4072c1:	90                   	nop
  4072c2:	90                   	nop
  4072c3:	90                   	nop
  4072c4:	90                   	nop
  4072c5:	90                   	nop
  4072c6:	90                   	nop
  4072c7:	90                   	nop
  4072c8:	90                   	nop
  4072c9:	90                   	nop
  4072ca:	90                   	nop
  4072cb:	90                   	nop
  4072cc:	90                   	nop
  4072cd:	90                   	nop
  4072ce:	90                   	nop
  4072cf:	90                   	nop
  4072d0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4072d4:	56                   	push   esi
  4072d5:	85 c0                	test   eax,eax
  4072d7:	7c 22                	jl     0x4072fb
  4072d9:	8b 51 34             	mov    edx,DWORD PTR [ecx+0x34]
  4072dc:	33 f6                	xor    esi,esi
  4072de:	66 8b 72 1e          	mov    si,WORD PTR [edx+0x1e]
  4072e2:	3b c6                	cmp    eax,esi
  4072e4:	7d 15                	jge    0x4072fb
  4072e6:	8b 51 3c             	mov    edx,DWORD PTR [ecx+0x3c]
  4072e9:	8b 71 30             	mov    esi,DWORD PTR [ecx+0x30]
  4072ec:	8b 14 82             	mov    edx,DWORD PTR [edx+eax*4]
  4072ef:	89 41 5c             	mov    DWORD PTR [ecx+0x5c],eax
  4072f2:	03 d6                	add    edx,esi
  4072f4:	5e                   	pop    esi
  4072f5:	89 51 58             	mov    DWORD PTR [ecx+0x58],edx
  4072f8:	c2 04 00             	ret    0x4
  4072fb:	8d 44 24 08          	lea    eax,[esp+0x8]
  4072ff:	68 d8 37 46 00       	push   0x4637d8
  407304:	50                   	push   eax
  407305:	c7 44 24 10 6c 8b 46 	mov    DWORD PTR [esp+0x10],0x468b6c
  40730c:	00 
  40730d:	e8 b5 a8 04 00       	call   0x451bc7
  407312:	90                   	nop
  407313:	90                   	nop
  407314:	90                   	nop
  407315:	90                   	nop
  407316:	90                   	nop
  407317:	90                   	nop
  407318:	90                   	nop
  407319:	90                   	nop
  40731a:	90                   	nop
  40731b:	90                   	nop
  40731c:	90                   	nop
  40731d:	90                   	nop
  40731e:	90                   	nop
  40731f:	90                   	nop
  407320:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  407324:	56                   	push   esi
  407325:	83 f8 ff             	cmp    eax,0xffffffff
  407328:	74 22                	je     0x40734c
  40732a:	8b 51 34             	mov    edx,DWORD PTR [ecx+0x34]
  40732d:	33 f6                	xor    esi,esi
  40732f:	66 8b 72 32          	mov    si,WORD PTR [edx+0x32]
  407333:	3b c6                	cmp    eax,esi
  407335:	7d 15                	jge    0x40734c
  407337:	8b 51 44             	mov    edx,DWORD PTR [ecx+0x44]
  40733a:	8b 71 30             	mov    esi,DWORD PTR [ecx+0x30]
  40733d:	8b 14 82             	mov    edx,DWORD PTR [edx+eax*4]
  407340:	89 41 64             	mov    DWORD PTR [ecx+0x64],eax
  407343:	03 d6                	add    edx,esi
  407345:	5e                   	pop    esi
  407346:	89 51 60             	mov    DWORD PTR [ecx+0x60],edx
  407349:	c2 04 00             	ret    0x4
  40734c:	c7 41 64 ff ff ff ff 	mov    DWORD PTR [ecx+0x64],0xffffffff
  407353:	5e                   	pop    esi
  407354:	c2 04 00             	ret    0x4
  407357:	90                   	nop
  407358:	90                   	nop
  407359:	90                   	nop
  40735a:	90                   	nop
  40735b:	90                   	nop
  40735c:	90                   	nop
  40735d:	90                   	nop
  40735e:	90                   	nop
  40735f:	90                   	nop
  407360:	8b 41 64             	mov    eax,DWORD PTR [ecx+0x64]
  407363:	83 f8 ff             	cmp    eax,0xffffffff
  407366:	75 03                	jne    0x40736b
  407368:	33 c0                	xor    eax,eax
  40736a:	c3                   	ret
  40736b:	8b 51 44             	mov    edx,DWORD PTR [ecx+0x44]
  40736e:	8b 49 30             	mov    ecx,DWORD PTR [ecx+0x30]
  407371:	8b 04 82             	mov    eax,DWORD PTR [edx+eax*4]
  407374:	8d 44 08 10          	lea    eax,[eax+ecx*1+0x10]
  407378:	c3                   	ret
  407379:	90                   	nop
  40737a:	90                   	nop
  40737b:	90                   	nop
  40737c:	90                   	nop
  40737d:	90                   	nop
  40737e:	90                   	nop
  40737f:	90                   	nop
  407380:	56                   	push   esi
  407381:	8b f1                	mov    esi,ecx
  407383:	8b 46 48             	mov    eax,DWORD PTR [esi+0x48]
  407386:	0f bf 48 20          	movsx  ecx,WORD PTR [eax+0x20]
  40738a:	51                   	push   ecx
  40738b:	8b ce                	mov    ecx,esi
  40738d:	e8 8e ff ff ff       	call   0x407320
  407392:	8b 46 64             	mov    eax,DWORD PTR [esi+0x64]
  407395:	85 c0                	test   eax,eax
  407397:	7c 35                	jl     0x4073ce
  407399:	a1 bc f6 46 00       	mov    eax,ds:0x46f6bc
  40739e:	85 c0                	test   eax,eax
  4073a0:	74 2c                	je     0x4073ce
  4073a2:	8b 46 60             	mov    eax,DWORD PTR [esi+0x60]
  4073a5:	33 d2                	xor    edx,edx
  4073a7:	33 c9                	xor    ecx,ecx
  4073a9:	66 8b 50 02          	mov    dx,WORD PTR [eax+0x2]
  4073ad:	8a 48 01             	mov    cl,BYTE PTR [eax+0x1]
  4073b0:	52                   	push   edx
  4073b1:	33 d2                	xor    edx,edx
  4073b3:	8a 10                	mov    dl,BYTE PTR [eax]
  4073b5:	8b 40 04             	mov    eax,DWORD PTR [eax+0x4]
  4073b8:	51                   	push   ecx
  4073b9:	52                   	push   edx
  4073ba:	50                   	push   eax
  4073bb:	8b ce                	mov    ecx,esi
  4073bd:	e8 9e ff ff ff       	call   0x407360
  4073c2:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  4073c8:	50                   	push   eax
  4073c9:	e8 02 e4 ff ff       	call   0x4057d0
  4073ce:	5e                   	pop    esi
  4073cf:	c3                   	ret
  4073d0:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  4073d4:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  4073d8:	89 01                	mov    DWORD PTR [ecx],eax
  4073da:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  4073de:	89 51 04             	mov    DWORD PTR [ecx+0x4],edx
  4073e1:	8b 54 24 08          	mov    edx,DWORD PTR [esp+0x8]
  4073e5:	50                   	push   eax
  4073e6:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  4073ea:	52                   	push   edx
  4073eb:	50                   	push   eax
  4073ec:	e8 0f 00 00 00       	call   0x407400
  4073f1:	c2 14 00             	ret    0x14
  4073f4:	90                   	nop
  4073f5:	90                   	nop
  4073f6:	90                   	nop
  4073f7:	90                   	nop
  4073f8:	90                   	nop
  4073f9:	90                   	nop
  4073fa:	90                   	nop
  4073fb:	90                   	nop
  4073fc:	90                   	nop
  4073fd:	90                   	nop
  4073fe:	90                   	nop
  4073ff:	90                   	nop
  407400:	8b 41 18             	mov    eax,DWORD PTR [ecx+0x18]
  407403:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  407407:	85 c0                	test   eax,eax
  407409:	56                   	push   esi
  40740a:	74 11                	je     0x40741d
  40740c:	39 51 10             	cmp    DWORD PTR [ecx+0x10],edx
  40740f:	75 0c                	jne    0x40741d
  407411:	48                   	dec    eax
  407412:	c6 41 2d 00          	mov    BYTE PTR [ecx+0x2d],0x0
  407416:	89 41 18             	mov    DWORD PTR [ecx+0x18],eax
  407419:	5e                   	pop    esi
  40741a:	c2 0c 00             	ret    0xc
  40741d:	8b 41 10             	mov    eax,DWORD PTR [ecx+0x10]
  407420:	3b c2                	cmp    eax,edx
  407422:	74 1c                	je     0x407440
  407424:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  407428:	89 51 10             	mov    DWORD PTR [ecx+0x10],edx
  40742b:	89 41 0c             	mov    DWORD PTR [ecx+0xc],eax
  40742e:	89 51 14             	mov    DWORD PTR [ecx+0x14],edx
  407431:	c7 41 18 01 00 00 00 	mov    DWORD PTR [ecx+0x18],0x1
  407438:	c6 41 2d 01          	mov    BYTE PTR [ecx+0x2d],0x1
  40743c:	5e                   	pop    esi
  40743d:	c2 0c 00             	ret    0xc
  407440:	8b 51 14             	mov    edx,DWORD PTR [ecx+0x14]
  407443:	8b 71 0c             	mov    esi,DWORD PTR [ecx+0xc]
  407446:	42                   	inc    edx
  407447:	03 f0                	add    esi,eax
  407449:	3b d6                	cmp    edx,esi
  40744b:	89 51 14             	mov    DWORD PTR [ecx+0x14],edx
  40744e:	72 28                	jb     0x407478
  407450:	80 7c 24 10 00       	cmp    BYTE PTR [esp+0x10],0x0
  407455:	74 05                	je     0x40745c
  407457:	89 41 14             	mov    DWORD PTR [ecx+0x14],eax
  40745a:	eb 1c                	jmp    0x407478
  40745c:	8a 81 80 00 00 00    	mov    al,BYTE PTR [ecx+0x80]
  407462:	4a                   	dec    edx
  407463:	84 c0                	test   al,al
  407465:	89 51 14             	mov    DWORD PTR [ecx+0x14],edx
  407468:	74 0e                	je     0x407478
  40746a:	c6 05 1e f8 89 00 00 	mov    BYTE PTR ds:0x89f81e,0x0
  407471:	c6 81 80 00 00 00 00 	mov    BYTE PTR [ecx+0x80],0x0
  407478:	8b 51 14             	mov    edx,DWORD PTR [ecx+0x14]
  40747b:	8b 41 38             	mov    eax,DWORD PTR [ecx+0x38]
  40747e:	33 f6                	xor    esi,esi
  407480:	8b 14 90             	mov    edx,DWORD PTR [eax+edx*4]
  407483:	8b 41 34             	mov    eax,DWORD PTR [ecx+0x34]
  407486:	66 8b 74 02 22       	mov    si,WORD PTR [edx+eax*1+0x22]
  40748b:	c6 41 2d 01          	mov    BYTE PTR [ecx+0x2d],0x1
  40748f:	89 71 18             	mov    DWORD PTR [ecx+0x18],esi
  407492:	5e                   	pop    esi
  407493:	c2 0c 00             	ret    0xc
  407496:	90                   	nop
  407497:	90                   	nop
  407498:	90                   	nop
  407499:	90                   	nop
  40749a:	90                   	nop
  40749b:	90                   	nop
  40749c:	90                   	nop
  40749d:	90                   	nop
  40749e:	90                   	nop
  40749f:	90                   	nop
  4074a0:	56                   	push   esi
  4074a1:	8b f1                	mov    esi,ecx
  4074a3:	81 7e 10 0e 64 03 00 	cmp    DWORD PTR [esi+0x10],0x3640e
  4074aa:	74 5e                	je     0x40750a
  4074ac:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  4074af:	53                   	push   ebx
  4074b0:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
  4074b4:	55                   	push   ebp
  4074b5:	8b 6c 24 18          	mov    ebp,DWORD PTR [esp+0x18]
  4074b9:	57                   	push   edi
  4074ba:	8b 7c 24 24          	mov    edi,DWORD PTR [esp+0x24]
  4074be:	85 c0                	test   eax,eax
  4074c0:	74 18                	je     0x4074da
  4074c2:	6a 00                	push   0x0
  4074c4:	e8 37 fd ff ff       	call   0x407200
  4074c9:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  4074cd:	57                   	push   edi
  4074ce:	53                   	push   ebx
  4074cf:	55                   	push   ebp
  4074d0:	50                   	push   eax
  4074d1:	6a 00                	push   0x0
  4074d3:	8b ce                	mov    ecx,esi
  4074d5:	e8 36 00 00 00       	call   0x407510
  4074da:	8b 4e 14             	mov    ecx,DWORD PTR [esi+0x14]
  4074dd:	51                   	push   ecx
  4074de:	8b ce                	mov    ecx,esi
  4074e0:	e8 1b fd ff ff       	call   0x407200
  4074e5:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
  4074e9:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  4074ed:	57                   	push   edi
  4074ee:	53                   	push   ebx
  4074ef:	55                   	push   ebp
  4074f0:	52                   	push   edx
  4074f1:	50                   	push   eax
  4074f2:	8b ce                	mov    ecx,esi
  4074f4:	e8 17 00 00 00       	call   0x407510
  4074f9:	8a 46 2d             	mov    al,BYTE PTR [esi+0x2d]
  4074fc:	5f                   	pop    edi
  4074fd:	5d                   	pop    ebp
  4074fe:	5b                   	pop    ebx
  4074ff:	84 c0                	test   al,al
  407501:	74 07                	je     0x40750a
  407503:	8b ce                	mov    ecx,esi
  407505:	e8 76 fe ff ff       	call   0x407380
  40750a:	5e                   	pop    esi
  40750b:	c2 14 00             	ret    0x14
  40750e:	90                   	nop
  40750f:	90                   	nop
  407510:	51                   	push   ecx
  407511:	56                   	push   esi
  407512:	8b f1                	mov    esi,ecx
  407514:	c7 44 24 04 00 00 00 	mov    DWORD PTR [esp+0x4],0x0
  40751b:	00 
  40751c:	8b 46 48             	mov    eax,DWORD PTR [esi+0x48]
  40751f:	66 83 78 54 00       	cmp    WORD PTR [eax+0x54],0x0
  407524:	0f 86 a8 00 00 00    	jbe    0x4075d2
  40752a:	53                   	push   ebx
  40752b:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  40752f:	55                   	push   ebp
  407530:	8b 6c 24 24          	mov    ebp,DWORD PTR [esp+0x24]
  407534:	57                   	push   edi
  407535:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  407539:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  40753d:	51                   	push   ecx
  40753e:	8b ce                	mov    ecx,esi
  407540:	e8 2b fd ff ff       	call   0x407270
  407545:	83 fb ff             	cmp    ebx,0xffffffff
  407548:	75 38                	jne    0x407582
  40754a:	8b 15 c0 f6 89 00    	mov    edx,DWORD PTR ds:0x89f6c0
  407550:	57                   	push   edi
  407551:	8b 42 3c             	mov    eax,DWORD PTR [edx+0x3c]
  407554:	83 f8 10             	cmp    eax,0x10
  407557:	8b 46 50             	mov    eax,DWORD PTR [esi+0x50]
  40755a:	0f bf 48 0f          	movsx  ecx,WORD PTR [eax+0xf]
  40755e:	0f bf 50 0d          	movsx  edx,WORD PTR [eax+0xd]
  407562:	51                   	push   ecx
  407563:	52                   	push   edx
  407564:	0f bf 48 0b          	movsx  ecx,WORD PTR [eax+0xb]
  407568:	51                   	push   ecx
  407569:	75 0f                	jne    0x40757a
  40756b:	33 d2                	xor    edx,edx
  40756d:	8b ce                	mov    ecx,esi
  40756f:	8a 50 0a             	mov    dl,BYTE PTR [eax+0xa]
  407572:	52                   	push   edx
  407573:	e8 48 05 00 00       	call   0x407ac0
  407578:	eb 3b                	jmp    0x4075b5
  40757a:	33 d2                	xor    edx,edx
  40757c:	8a 50 0a             	mov    dl,BYTE PTR [eax+0xa]
  40757f:	52                   	push   edx
  407580:	eb 2c                	jmp    0x4075ae
  407582:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  407587:	57                   	push   edi
  407588:	55                   	push   ebp
  407589:	83 78 3c 10          	cmp    DWORD PTR [eax+0x3c],0x10
  40758d:	75 14                	jne    0x4075a3
  40758f:	8b 4c 24 2c          	mov    ecx,DWORD PTR [esp+0x2c]
  407593:	8b 54 24 28          	mov    edx,DWORD PTR [esp+0x28]
  407597:	51                   	push   ecx
  407598:	52                   	push   edx
  407599:	53                   	push   ebx
  40759a:	8b ce                	mov    ecx,esi
  40759c:	e8 1f 05 00 00       	call   0x407ac0
  4075a1:	eb 12                	jmp    0x4075b5
  4075a3:	8b 44 24 2c          	mov    eax,DWORD PTR [esp+0x2c]
  4075a7:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  4075ab:	50                   	push   eax
  4075ac:	51                   	push   ecx
  4075ad:	53                   	push   ebx
  4075ae:	8b ce                	mov    ecx,esi
  4075b0:	e8 2b 00 00 00       	call   0x4075e0
  4075b5:	8b 56 48             	mov    edx,DWORD PTR [esi+0x48]
  4075b8:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  4075bc:	33 c9                	xor    ecx,ecx
  4075be:	40                   	inc    eax
  4075bf:	66 8b 4a 54          	mov    cx,WORD PTR [edx+0x54]
  4075c3:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  4075c7:	3b c1                	cmp    eax,ecx
  4075c9:	0f 8c 6a ff ff ff    	jl     0x407539
  4075cf:	5f                   	pop    edi
  4075d0:	5d                   	pop    ebp
  4075d1:	5b                   	pop    ebx
  4075d2:	5e                   	pop    esi
  4075d3:	59                   	pop    ecx
  4075d4:	c2 14 00             	ret    0x14
  4075d7:	90                   	nop
  4075d8:	90                   	nop
  4075d9:	90                   	nop
  4075da:	90                   	nop
  4075db:	90                   	nop
  4075dc:	90                   	nop
  4075dd:	90                   	nop
  4075de:	90                   	nop
  4075df:	90                   	nop
  4075e0:	83 ec 2c             	sub    esp,0x2c
  4075e3:	53                   	push   ebx
  4075e4:	55                   	push   ebp
  4075e5:	56                   	push   esi
  4075e6:	57                   	push   edi
  4075e7:	8b f9                	mov    edi,ecx
  4075e9:	33 f6                	xor    esi,esi
  4075eb:	33 db                	xor    ebx,ebx
  4075ed:	8b 2d 14 85 97 00    	mov    ebp,DWORD PTR ds:0x978514
  4075f3:	8b 47 34             	mov    eax,DWORD PTR [edi+0x34]
  4075f6:	33 d2                	xor    edx,edx
  4075f8:	89 7c 24 2c          	mov    DWORD PTR [esp+0x2c],edi
  4075fc:	89 54 24 28          	mov    DWORD PTR [esp+0x28],edx
  407600:	66 8b 70 07          	mov    si,WORD PTR [eax+0x7]
  407604:	66 8b 58 09          	mov    bx,WORD PTR [eax+0x9]
  407608:	8b 47 50             	mov    eax,DWORD PTR [edi+0x50]
  40760b:	89 54 24 24          	mov    DWORD PTR [esp+0x24],edx
  40760f:	0f bf 48 02          	movsx  ecx,WORD PTR [eax+0x2]
  407613:	2b cd                	sub    ecx,ebp
  407615:	8b 2f                	mov    ebp,DWORD PTR [edi]
  407617:	03 cd                	add    ecx,ebp
  407619:	0f bf 68 04          	movsx  ebp,WORD PTR [eax+0x4]
  40761d:	8b 47 04             	mov    eax,DWORD PTR [edi+0x4]
  407620:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  407624:	03 e8                	add    ebp,eax
  407626:	a1 18 85 97 00       	mov    eax,ds:0x978518
  40762b:	2b e8                	sub    ebp,eax
  40762d:	3b ca                	cmp    ecx,edx
  40762f:	7d                   	.byte 0x7d


/mnt/data/work39/oracle/RPG.exe:     file format pei-i386


Disassembly of section .text:

00407ac0 <.text+0x6ac0>:
  407ac0:	83 ec 2c             	sub    esp,0x2c
  407ac3:	53                   	push   ebx
  407ac4:	55                   	push   ebp
  407ac5:	56                   	push   esi
  407ac6:	57                   	push   edi
  407ac7:	8b f9                	mov    edi,ecx
  407ac9:	33 f6                	xor    esi,esi
  407acb:	33 db                	xor    ebx,ebx
  407acd:	8b 2d 14 85 97 00    	mov    ebp,DWORD PTR ds:0x978514
  407ad3:	8b 47 34             	mov    eax,DWORD PTR [edi+0x34]
  407ad6:	33 d2                	xor    edx,edx
  407ad8:	89 7c 24 2c          	mov    DWORD PTR [esp+0x2c],edi
  407adc:	89 54 24 28          	mov    DWORD PTR [esp+0x28],edx
  407ae0:	66 8b 70 07          	mov    si,WORD PTR [eax+0x7]
  407ae4:	66 8b 58 09          	mov    bx,WORD PTR [eax+0x9]
  407ae8:	8b 47 50             	mov    eax,DWORD PTR [edi+0x50]
  407aeb:	89 54 24 24          	mov    DWORD PTR [esp+0x24],edx
  407aef:	0f bf 48 02          	movsx  ecx,WORD PTR [eax+0x2]
  407af3:	2b cd                	sub    ecx,ebp
  407af5:	8b 2f                	mov    ebp,DWORD PTR [edi]
  407af7:	03 cd                	add    ecx,ebp
  407af9:	0f bf 68 04          	movsx  ebp,WORD PTR [eax+0x4]
  407afd:	8b 47 04             	mov    eax,DWORD PTR [edi+0x4]
  407b00:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  407b04:	03 e8                	add    ebp,eax
  407b06:	a1 18 85 97 00       	mov    eax,ds:0x978518
  407b0b:	2b e8                	sub    ebp,eax
  407b0d:	3b ca                	cmp    ecx,edx
  407b0f:	7d 34                	jge    0x407b45
  407b11:	8b c1                	mov    eax,ecx
  407b13:	99                   	cdq
  407b14:	8b c8                	mov    ecx,eax
  407b16:	8b 47 58             	mov    eax,DWORD PTR [edi+0x58]
  407b19:	33 ca                	xor    ecx,edx
  407b1b:	2b ca                	sub    ecx,edx
  407b1d:	33 d2                	xor    edx,edx
  407b1f:	66 8b 10             	mov    dx,WORD PTR [eax]
  407b22:	0f af d6             	imul   edx,esi
  407b25:	3b ca                	cmp    ecx,edx
  407b27:	0f 8d f8 03 00 00    	jge    0x407f25
  407b2d:	8b c1                	mov    eax,ecx
  407b2f:	99                   	cdq
  407b30:	f7 fe                	idiv   esi
  407b32:	89 44 24 28          	mov    DWORD PTR [esp+0x28],eax
  407b36:	8b c1                	mov    eax,ecx
  407b38:	99                   	cdq
  407b39:	f7 fe                	idiv   esi
  407b3b:	f7 da                	neg    edx
  407b3d:	89                   	.byte 0x89
  407b3e:	54                   	push   esp
  407b3f:	24                   	.byte 0x24
