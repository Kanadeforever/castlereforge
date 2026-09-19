; 《幽城幻剑录》固化43 D7结果分派参数4与detector4改写链静态证据
; 输入：用户当前 RPG.exe.org 解压得到的 RPG.exe
; SHA-256：8294839343b1a7845ddae31ed16216b05850efd39a742e5ca7701aadca97287f
;
; 阅读重点：
; 1. 0x4320C6 把结果容器vtable写为0x460DC4；该vtable第一项为0x4328A0。
; 2. 0x4328A0调用0x443590 detector；detector=1或4都会汇合到push 1 -> 0x443660。
; 3. detector=2/3分别把2/3传给0x443660；detector=4不会传4。
; 4. detector4处理后又在0x43295A命中cmp edi,4并跳过0x412440普通结果UI启动。
; 5. 0x443660自身仍保留参数4分支，并把0x46C4DC的CP950“因  事  停  戰”送进UI。
;
; 证据边界：当前EXE全.text只有0x432908/0x432929/0x432948三个direct E8 caller到0x443660，
; 且整个文件没有0x00443660作为little-endian绝对函数指针常量。不能据此形式化排除运行时计算地址、
; 外部补丁或其他版本间接调用，所以参数4只能称“当前canonical静态调用面无生产者”。
;
; ===== 结果容器构造器与vtable写入 =====


/mnt/data/work43/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

004320b7 <.text+0x310b7>:
  4320b7:	8d 8e 2c 02 00 00    	lea    ecx,[esi+0x22c]
  4320bd:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  4320c1:	e8 2a f6 00 00       	call   0x4416f0
  4320c6:	c7 06 c4 0d 46 00    	mov    DWORD PTR [esi],0x460dc4
  4320cc:	88 9e 20 02 00 00    	mov    BYTE PTR [esi+0x220],bl
  4320d2:	88 9e 40 02 00 00    	mov    BYTE PTR [esi+0x240],bl
  4320d8:	a1 4c 1c 8e 00       	mov    eax,ds:0x8e1c4c
  4320dd:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  4320e1:	c6 80 48 02 00 00 01 	mov    BYTE PTR [eax+0x248],0x1
  4320e8:	89 1d dc 01 8b 00    	mov    DWORD PTR ds:0x8b01dc,ebx
  4320ee:	89 9e 18 02 00 00    	mov    DWORD PTR [esi+0x218],ebx
  4320f4:	89 1d a4 01 8a 00    	mov    DWORD PTR ds:0x8a01a4,ebx
  4320fa:	c7 86 24 02 00 00 19 	mov    DWORD PTR [esi+0x224],0x19
  432101:	00 00 00 
  432104:	89 1d ac 01 8a 00    	mov    DWORD PTR ds:0x8a01ac,ebx
  43210a:	89 1d b0 01 8a 00    	mov    DWORD PTR ds:0x8a01b0,ebx
  432110:	c7 86 44 02 00 00 0f 	mov    DWORD PTR [esi+0x244],0xf
  432117:	00 00 00 
  43211a:	88 9e 48 02 00 00    	mov    BYTE PTR [esi+0x248],bl
  432120:	8b c6                	mov    eax,esi
  432122:	5e                   	pop    esi
  432123:	5b                   	pop    ebx

; ===== canonical结果update：detector到dispatcher参数映射 =====

/mnt/data/work43/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

004328a0 <.text+0x318a0>:
  4328a0:	56                   	push   esi
  4328a1:	57                   	push   edi
  4328a2:	8b f1                	mov    esi,ecx
  4328a4:	6a 64                	push   0x64
  4328a6:	56                   	push   esi
  4328a7:	e8 54 1c 00 00       	call   0x434500
  4328ac:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  4328b2:	e8 d9 0c 01 00       	call   0x443590
  4328b7:	8b f8                	mov    edi,eax
  4328b9:	a1 74 fd 89 00       	mov    eax,ds:0x89fd74
  4328be:	8a 88 fc 05 00 00    	mov    cl,BYTE PTR [eax+0x5fc]
  4328c4:	84 c9                	test   cl,cl
  4328c6:	74 05                	je     0x4328cd
  4328c8:	bf 03 00 00 00       	mov    edi,0x3
  4328cd:	8a 86 40 02 00 00    	mov    al,BYTE PTR [esi+0x240]
  4328d3:	84 c0                	test   al,al
  4328d5:	0f 85 9d 00 00 00    	jne    0x432978
  4328db:	83 ff 01             	cmp    edi,0x1
  4328de:	74 0f                	je     0x4328ef
  4328e0:	83 ff 02             	cmp    edi,0x2
  4328e3:	74 0a                	je     0x4328ef
  4328e5:	83 ff 04             	cmp    edi,0x4
  4328e8:	74 05                	je     0x4328ef
  4328ea:	83 ff 03             	cmp    edi,0x3
  4328ed:	75 7b                	jne    0x43296a
  4328ef:	83 ff 01             	cmp    edi,0x1
  4328f2:	c6 86 40 02 00 00 01 	mov    BYTE PTR [esi+0x240],0x1
  4328f9:	74 05                	je     0x432900
  4328fb:	83 ff 04             	cmp    edi,0x4
  4328fe:	75 1d                	jne    0x43291d
  432900:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  432906:	6a 01                	push   0x1
  432908:	e8 53 0d 01 00       	call   0x443660
  43290d:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  432913:	c7 81 18 01 00 00 01 	mov    DWORD PTR [ecx+0x118],0x1
  43291a:	00 00 00 
  43291d:	83 ff 02             	cmp    edi,0x2
  432920:	75 1a                	jne    0x43293c
  432922:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  432928:	57                   	push   edi
  432929:	e8 32 0d 01 00       	call   0x443660
  43292e:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  432934:	89 ba 18 01 00 00    	mov    DWORD PTR [edx+0x118],edi
  43293a:	eb 23                	jmp    0x43295f
  43293c:	83 ff 03             	cmp    edi,0x3
  43293f:	75 19                	jne    0x43295a
  432941:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  432947:	57                   	push   edi
  432948:	e8 13 0d 01 00       	call   0x443660
  43294d:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  432952:	89 b8 18 01 00 00    	mov    DWORD PTR [eax+0x118],edi
  432958:	eb 05                	jmp    0x43295f
  43295a:	83 ff 04             	cmp    edi,0x4
  43295d:	74 0b                	je     0x43296a
  43295f:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  432965:	e8 d6 fa fd ff       	call   0x412440
  43296a:	8a 86 40 02 00 00    	mov    al,BYTE PTR [esi+0x240]
  432970:	84 c0                	test   al,al
  432972:	0f 84 a2 00 00 00    	je     0x432a1a
  432978:	a1 c8 fc 89 00       	mov    eax,ds:0x89fcc8
  43297d:	8a 88 79 05 00 00    	mov    cl,BYTE PTR [eax+0x579]
  432983:	84 c9                	test   cl,cl
  432985:	0f 85 8f 00 00 00    	jne    0x432a1a
  43298b:	8b 88 00 06 00 00    	mov    ecx,DWORD PTR [eax+0x600]
  432991:	8a 81 79 05 00 00    	mov    al,BYTE PTR [ecx+0x579]
  432997:	84 c0                	test   al,al
  432999:	75 7f                	jne    0x432a1a
  43299b:	8a 86 48 02 00 00    	mov    al,BYTE PTR [esi+0x248]
  4329a1:	84 c0                	test   al,al
  4329a3:	75 42                	jne    0x4329e7
  4329a5:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  4329ab:	6a 01                	push   0x1
  4329ad:	6a 00                	push   0x0
  4329af:	68 40 ac 27 00       	push   0x27ac40
  4329b4:	68 f6 00 00 00       	push   0xf6
  4329b9:	68 25 01 00 00       	push   0x125
  4329be:	68 f6 00 00 00       	push   0xf6
  4329c3:	68 25 01 00 00       	push   0x125
  4329c8:	68 98 a4 46 00       	push   0x46a498
  4329cd:	e8 fe aa ff ff       	call   0x42d4d0
  4329d2:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  4329d8:	6a 00                	push   0x0
  4329da:	50                   	push   eax
  4329db:	e8 00 ad ff ff       	call   0x42d6e0
  4329e0:	c6 86 48 02 00 00 01 	mov    BYTE PTR [esi+0x248],0x1
  4329e7:	8b 86 44 02 00 00    	mov    eax,DWORD PTR [esi+0x244]
  4329ed:	85 c0                	test   eax,eax
  4329ef:	7f 22                	jg     0x432a13
  4329f1:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  4329f7:	e8 64 f5 00 00       	call   0x441f60
  4329fc:	85 f6                	test   esi,esi
  4329fe:	74 1a                	je     0x432a1a
  432a00:	8b ce                	mov    ecx,esi
  432a02:	e8 29 f7 ff ff       	call   0x432130
  432a07:	56                   	push   esi
  432a08:	e8 43 eb 01 00       	call   0x451550
  432a0d:	83 c4 04             	add    esp,0x4
  432a10:	5f                   	pop    edi
  432a11:	5e                   	pop    esi
  432a12:	c3                   	ret
  432a13:	48                   	dec    eax
  432a14:	89 86 44 02 00 00    	mov    DWORD PTR [esi+0x244],eax
  432a1a:	5f                   	pop    edi
  432a1b:	5e                   	pop    esi
  432a1c:	c3                   	ret
  432a1d:	90                   	nop
  432a1e:	90                   	nop
  432a1f:	90                   	nop

; ===== outcome detector 0x443590 =====

/mnt/data/work43/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00443590 <.text+0x42590>:
  443590:	53                   	push   ebx
  443591:	55                   	push   ebp
  443592:	56                   	push   esi
  443593:	57                   	push   edi
  443594:	8b f9                	mov    edi,ecx
  443596:	33 ed                	xor    ebp,ebp
  443598:	33 f6                	xor    esi,esi
  44359a:	32 db                	xor    bl,bl
  44359c:	8d 87 34 0c 00 00    	lea    eax,[edi+0xc34]
  4435a2:	ba 10 00 00 00       	mov    edx,0x10
  4435a7:	83 b8 dc fe ff ff ff 	cmp    DWORD PTR [eax-0x124],0xffffffff
  4435ae:	74 19                	je     0x4435c9
  4435b0:	8b 88 24 02 00 00    	mov    ecx,DWORD PTR [eax+0x224]
  4435b6:	85 c9                	test   ecx,ecx
  4435b8:	75 0f                	jne    0x4435c9
  4435ba:	8b 08                	mov    ecx,DWORD PTR [eax]
  4435bc:	85 c9                	test   ecx,ecx
  4435be:	75 03                	jne    0x4435c3
  4435c0:	46                   	inc    esi
  4435c1:	eb 06                	jmp    0x4435c9
  4435c3:	83 f9 01             	cmp    ecx,0x1
  4435c6:	75 01                	jne    0x4435c9
  4435c8:	45                   	inc    ebp
  4435c9:	05 e4 0d 00 00       	add    eax,0xde4
  4435ce:	4a                   	dec    edx
  4435cf:	75 d6                	jne    0x4435a7
  4435d1:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  4435d6:	83 b8 f4 00 00 00 01 	cmp    DWORD PTR [eax+0xf4],0x1
  4435dd:	75 0a                	jne    0x4435e9
  4435df:	33 f6                	xor    esi,esi
  4435e1:	b3 01                	mov    bl,0x1
  4435e3:	89 b0 f4 00 00 00    	mov    DWORD PTR [eax+0xf4],esi
  4435e9:	85 ed                	test   ebp,ebp
  4435eb:	74 08                	je     0x4435f5
  4435ed:	85 f6                	test   esi,esi
  4435ef:	74 04                	je     0x4435f5
  4435f1:	84 db                	test   bl,bl
  4435f3:	74 34                	je     0x443629
  4435f5:	8b 0d 74 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fd74
  4435fb:	6a ff                	push   0xffffffff
  4435fd:	e8 2e 89 fd ff       	call   0x41bf30
  443602:	8b 0d 74 fd 89 00    	mov    ecx,DWORD PTR ds:0x89fd74
  443608:	e8 53 88 fd ff       	call   0x41be60
  44360d:	84 db                	test   bl,bl
  44360f:	c6 87 30 e3 00 00 01 	mov    BYTE PTR [edi+0xe330],0x1
  443616:	74 11                	je     0x443629
  443618:	5f                   	pop    edi
  443619:	5e                   	pop    esi
  44361a:	5d                   	pop    ebp
  44361b:	c6 05 dc 96 46 00 00 	mov    BYTE PTR ds:0x4696dc,0x0
  443622:	b8 04 00 00 00       	mov    eax,0x4
  443627:	5b                   	pop    ebx
  443628:	c3                   	ret
  443629:	85 ed                	test   ebp,ebp
  44362b:	75 11                	jne    0x44363e
  44362d:	5f                   	pop    edi
  44362e:	5e                   	pop    esi
  44362f:	5d                   	pop    ebp
  443630:	c6 05 dc 96 46 00 00 	mov    BYTE PTR ds:0x4696dc,0x0
  443637:	b8 02 00 00 00       	mov    eax,0x2
  44363c:	5b                   	pop    ebx
  44363d:	c3                   	ret
  44363e:	85 f6                	test   esi,esi
  443640:	75 11                	jne    0x443653
  443642:	5f                   	pop    edi
  443643:	5e                   	pop    esi
  443644:	5d                   	pop    ebp
  443645:	c6 05 dc 96 46 00 00 	mov    BYTE PTR ds:0x4696dc,0x0
  44364c:	b8 01 00 00 00       	mov    eax,0x1
  443651:	5b                   	pop    ebx
  443652:	c3                   	ret
  443653:	5f                   	pop    edi
  443654:	5e                   	pop    esi
  443655:	5d                   	pop    ebp
  443656:	33 c0                	xor    eax,eax
  443658:	5b                   	pop    ebx
  443659:	c3                   	ret
  44365a:	90                   	nop
  44365b:	90                   	nop
  44365c:	90                   	nop
  44365d:	90                   	nop
  44365e:	90                   	nop
  44365f:	90                   	nop

; ===== result dispatcher 0x443660：参数1/2/3/4分支 =====

/mnt/data/work43/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00443660 <.text+0x42660>:
  443660:	6a ff                	push   0xffffffff
  443662:	68 bb f9 45 00       	push   0x45f9bb
  443667:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  44366d:	50                   	push   eax
  44366e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  443675:	81 ec 94 01 00 00    	sub    esp,0x194
  44367b:	b8 70 00 00 00       	mov    eax,0x70
  443680:	8b d1                	mov    edx,ecx
  443682:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  443687:	33 c9                	xor    ecx,ecx
  443689:	57                   	push   edi
  44368a:	89 54 24 04          	mov    DWORD PTR [esp+0x4],edx
  44368e:	8b 88 90 00 00 00    	mov    ecx,DWORD PTR [eax+0x90]
  443694:	8b 84 24 a8 01 00 00 	mov    eax,DWORD PTR [esp+0x1a8]
  44369b:	83 f8 01             	cmp    eax,0x1
  44369e:	8d 7c 24 2c          	lea    edi,[esp+0x2c]
  4436a2:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  4436a6:	0f 85 1f 04 00 00    	jne    0x443acb
  4436ac:	53                   	push   ebx
  4436ad:	55                   	push   ebp
  4436ae:	56                   	push   esi
  4436af:	8d b2 38 7d 00 00    	lea    esi,[edx+0x7d38]
  4436b5:	bd 08 00 00 00       	mov    ebp,0x8
  4436ba:	bb c8 0b 00 00       	mov    ebx,0xbc8
  4436bf:	8b 86 48 fe ff ff    	mov    eax,DWORD PTR [esi-0x1b8]
  4436c5:	85 c0                	test   eax,eax
  4436c7:	7e 31                	jle    0x4436fa
  4436c9:	b9 1c 00 00 00       	mov    ecx,0x1c
  4436ce:	33 c0                	xor    eax,eax
  4436d0:	8b fe                	mov    edi,esi
  4436d2:	6a 01                	push   0x1
  4436d4:	f3 ab                	rep stos DWORD PTR es:[edi],eax
  4436d6:	8d be cc f5 ff ff    	lea    edi,[esi-0xa34]
  4436dc:	8b cf                	mov    ecx,edi
  4436de:	e8 cd d0 fd ff       	call   0x4207b0
  4436e3:	6a 00                	push   0x0
  4436e5:	6a 00                	push   0x0
  4436e7:	6a 73                	push   0x73
  4436e9:	8b cf                	mov    ecx,edi
  4436eb:	89 9e b0 02 00 00    	mov    DWORD PTR [esi+0x2b0],ebx
  4436f1:	e8 2a ea fd ff       	call   0x422120
  4436f6:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  4436fa:	81 c6 e4 0d 00 00    	add    esi,0xde4
  443700:	4d                   	dec    ebp
  443701:	75 bc                	jne    0x4436bf
  443703:	8b b2 31 e3 00 00    	mov    esi,DWORD PTR [edx+0xe331]
  443709:	33 db                	xor    ebx,ebx
  44370b:	85 f6                	test   esi,esi
  44370d:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  443711:	7e 34                	jle    0x443747
  443713:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  443718:	81 c2 35 e3 00 00    	add    edx,0xe335
  44371e:	8b b8 a4 00 00 00    	mov    edi,DWORD PTR [eax+0xa4]
  443724:	8b 02                	mov    eax,DWORD PTR [edx]
  443726:	83 c2 04             	add    edx,0x4
  443729:	8d 0c 40             	lea    ecx,[eax+eax*2]
  44372c:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  44372f:	d1 e1                	shl    ecx,1
  443731:	2b c8                	sub    ecx,eax
  443733:	c1 e1 04             	shl    ecx,0x4
  443736:	8b 6c 39 24          	mov    ebp,DWORD PTR [ecx+edi*1+0x24]
  44373a:	03 dd                	add    ebx,ebp
  44373c:	4e                   	dec    esi
  44373d:	75 e5                	jne    0x443724
  44373f:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  443743:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  443747:	8b 82 a9 e3 00 00    	mov    eax,DWORD PTR [edx+0xe3a9]
  44374d:	85 c0                	test   eax,eax
  44374f:	75 08                	jne    0x443759
  443751:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  443755:	33 d2                	xor    edx,edx
  443757:	eb 10                	jmp    0x443769
  443759:	e8 07 de 00 00       	call   0x451565
  44375e:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  443762:	99                   	cdq
  443763:	f7 be a9 e3 00 00    	idiv   DWORD PTR [esi+0xe3a9]
  443769:	8b ae a5 e3 00 00    	mov    ebp,DWORD PTR [esi+0xe3a5]
  44376f:	6a 01                	push   0x1
  443771:	03 ea                	add    ebp,edx
  443773:	8d 4c 24 28          	lea    ecx,[esp+0x28]
  443777:	89 6c 24 24          	mov    DWORD PTR [esp+0x24],ebp
  44377b:	e8 70 df ff ff       	call   0x4416f0
  443780:	8d 94 24 0c 01 00 00 	lea    edx,[esp+0x10c]
  443787:	b9 90 1c 8e 00       	mov    ecx,0x8e1c90
  44378c:	52                   	push   edx
  44378d:	68 a4 c5 46 00       	push   0x46c5a4
  443792:	c7 84 24 b4 01 00 00 	mov    DWORD PTR [esp+0x1b4],0x0
  443799:	00 00 00 00 
  44379d:	e8 4e e4 ff ff       	call   0x441bf0
  4437a2:	84 c0                	test   al,al
  4437a4:	74 36                	je     0x4437dc
  4437a6:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  4437ac:	e8 ff 1e fc ff       	call   0x4056b0
  4437b1:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  4437b7:	8d 84 24 0c 01 00 00 	lea    eax,[esp+0x10c]
  4437be:	6a 01                	push   0x1
  4437c0:	50                   	push   eax
  4437c1:	e8 aa 1e fc ff       	call   0x405670
  4437c6:	84 c0                	test   al,al
  4437c8:	75 12                	jne    0x4437dc
  4437ca:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  4437d0:	68 88 c5 46 00       	push   0x46c588
  4437d5:	6a 00                	push   0x0
  4437d7:	e8 94 e6 fe ff       	call   0x431e70
  4437dc:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  4437e2:	e8 c9 ec fc ff       	call   0x4124b0
  4437e7:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  4437ed:	68 6c c5 46 00       	push   0x46c56c
  4437f2:	e8 c9 ec fc ff       	call   0x4124c0
  4437f7:	8b 0d b0 b2 46 00    	mov    ecx,DWORD PTR ds:0x46b2b0
  4437fd:	53                   	push   ebx
  4437fe:	8d 84 24 ac 00 00 00 	lea    eax,[esp+0xac]
  443805:	8b 51 08             	mov    edx,DWORD PTR [ecx+0x8]
  443808:	52                   	push   edx
  443809:	68 58 c5 46 00       	push   0x46c558
  44380e:	50                   	push   eax
  44380f:	e8 5f e2 00 00       	call   0x451a73
  443814:	83 c4 10             	add    esp,0x10
  443817:	8d 8c 24 a8 00 00 00 	lea    ecx,[esp+0xa8]
  44381e:	51                   	push   ecx
  44381f:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443825:	e8 96 ec fc ff       	call   0x4124c0
  44382a:	8d be 50 7b 00 00    	lea    edi,[esi+0x7b50]
  443830:	c7 44 24 18 08 00 00 	mov    DWORD PTR [esp+0x18],0x8
  443837:	00 
  443838:	8b 47 30             	mov    eax,DWORD PTR [edi+0x30]
  44383b:	85 c0                	test   eax,eax
  44383d:	0f 8e 52 01 00 00    	jle    0x443995
  443843:	8b 07                	mov    eax,DWORD PTR [edi]
  443845:	85 c0                	test   eax,eax
  443847:	0f 8e 48 01 00 00    	jle    0x443995
  44384d:	89 87 e0 fe ff ff    	mov    DWORD PTR [edi-0x120],eax
  443853:	8b 47 24             	mov    eax,DWORD PTR [edi+0x24]
  443856:	03 c3                	add    eax,ebx
  443858:	89 47 24             	mov    DWORD PTR [edi+0x24],eax
  44385b:	8b 15 b4 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b4
  443861:	8b 4f 20             	mov    ecx,DWORD PTR [edi+0x20]
  443864:	8b 42 04             	mov    eax,DWORD PTR [edx+0x4]
  443867:	3b 88 f4 01 00 00    	cmp    ecx,DWORD PTR [eax+0x1f4]
  44386d:	0f 8d 22 01 00 00    	jge    0x443995
  443873:	8d 54 24 38          	lea    edx,[esp+0x38]
  443877:	8b ce                	mov    ecx,esi
  443879:	52                   	push   edx
  44387a:	57                   	push   edi
  44387b:	e8 e0 02 00 00       	call   0x443b60
  443880:	84 c0                	test   al,al
  443882:	0f 84 0d 01 00 00    	je     0x443995
  443888:	33 ed                	xor    ebp,ebp
  44388a:	8d 8f b4 f7 ff ff    	lea    ecx,[edi-0x84c]
  443890:	55                   	push   ebp
  443891:	55                   	push   ebp
  443892:	6a 74                	push   0x74
  443894:	e8 87 e8 fd ff       	call   0x422120
  443899:	b8 39 8e e3 38       	mov    eax,0x38e38e39
  44389e:	89 6c 24 7c          	mov    DWORD PTR [esp+0x7c],ebp
  4438a2:	f7 2d 04 24 8e 00    	imul   DWORD PTR ds:0x8e2404
  4438a8:	d1 fa                	sar    edx,1
  4438aa:	8b c2                	mov    eax,edx
  4438ac:	c1 e8 1f             	shr    eax,0x1f
  4438af:	03 d0                	add    edx,eax
  4438b1:	85 d2                	test   edx,edx
  4438b3:	0f 8e c2 00 00 00    	jle    0x44397b
  4438b9:	8b 0d 00 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2400
  4438bf:	33 f6                	xor    esi,esi
  4438c1:	8b 07                	mov    eax,DWORD PTR [edi]
  4438c3:	83 f8 01             	cmp    eax,0x1
  4438c6:	75 10                	jne    0x4438d8
  4438c8:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4438ce:	8b 9a f8 00 00 00    	mov    ebx,DWORD PTR [edx+0xf8]
  4438d4:	85 db                	test   ebx,ebx
  4438d6:	75 7b                	jne    0x443953
  4438d8:	39 04 0e             	cmp    DWORD PTR [esi+ecx*1],eax
  4438db:	75 76                	jne    0x443953
  4438dd:	8b 47 20             	mov    eax,DWORD PTR [edi+0x20]
  4438e0:	8b 54 0e 04          	mov    edx,DWORD PTR [esi+ecx*1+0x4]
  4438e4:	3b c2                	cmp    eax,edx
  4438e6:	7c 37                	jl     0x44391f
  4438e8:	8b 57 54             	mov    edx,DWORD PTR [edi+0x54]
  4438eb:	8b 5c 0e 0c          	mov    ebx,DWORD PTR [esi+ecx*1+0xc]
  4438ef:	3b d3                	cmp    edx,ebx
  4438f1:	7c 2c                	jl     0x44391f
  4438f3:	8b 57 58             	mov    edx,DWORD PTR [edi+0x58]
  4438f6:	8b 5c 0e 10          	mov    ebx,DWORD PTR [esi+ecx*1+0x10]
  4438fa:	3b d3                	cmp    edx,ebx
  4438fc:	7c 21                	jl     0x44391f
  4438fe:	8b 57 5c             	mov    edx,DWORD PTR [edi+0x5c]
  443901:	8b 5c 0e 14          	mov    ebx,DWORD PTR [esi+ecx*1+0x14]
  443905:	3b d3                	cmp    edx,ebx
  443907:	7c 16                	jl     0x44391f
  443909:	8b 57 60             	mov    edx,DWORD PTR [edi+0x60]
  44390c:	8b 5c 0e 18          	mov    ebx,DWORD PTR [esi+ecx*1+0x18]
  443910:	3b d3                	cmp    edx,ebx
  443912:	7c 0b                	jl     0x44391f
  443914:	8b 57 64             	mov    edx,DWORD PTR [edi+0x64]
  443917:	8b 5c 0e 1c          	mov    ebx,DWORD PTR [esi+ecx*1+0x1c]
  44391b:	3b d3                	cmp    edx,ebx
  44391d:	7d 06                	jge    0x443925
  44391f:	3b 44 0e 08          	cmp    eax,DWORD PTR [esi+ecx*1+0x8]
  443923:	7c 2e                	jl     0x443953
  443925:	8b 5c 0e 20          	mov    ebx,DWORD PTR [esi+ecx*1+0x20]
  443929:	8d 8f b4 f7 ff ff    	lea    ecx,[edi-0x84c]
  44392f:	53                   	push   ebx
  443930:	e8 cb f7 fd ff       	call   0x423100
  443935:	84 c0                	test   al,al
  443937:	74 14                	je     0x44394d
  443939:	8b 44 24 7c          	mov    eax,DWORD PTR [esp+0x7c]
  44393d:	89 9c 84 80 00 00 00 	mov    DWORD PTR [esp+eax*4+0x80],ebx
  443944:	8b 44 24 7c          	mov    eax,DWORD PTR [esp+0x7c]
  443948:	40                   	inc    eax
  443949:	89 44 24 7c          	mov    DWORD PTR [esp+0x7c],eax
  44394d:	8b 0d 00 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2400
  443953:	b8 39 8e e3 38       	mov    eax,0x38e38e39
  443958:	45                   	inc    ebp
  443959:	f7 2d 04 24 8e 00    	imul   DWORD PTR ds:0x8e2404
  44395f:	d1 fa                	sar    edx,1
  443961:	8b c2                	mov    eax,edx
  443963:	83 c6 24             	add    esi,0x24
  443966:	c1 e8 1f             	shr    eax,0x1f
  443969:	03 d0                	add    edx,eax
  44396b:	3b ea                	cmp    ebp,edx
  44396d:	0f 8c 4e ff ff ff    	jl     0x4438c1
  443973:	8b 74 24 10          	mov    esi,DWORD PTR [esp+0x10]
  443977:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  44397b:	8b 15 c8 fc 89 00    	mov    edx,DWORD PTR ds:0x89fcc8
  443981:	8d 4c 24 38          	lea    ecx,[esp+0x38]
  443985:	51                   	push   ecx
  443986:	8b 8a 00 06 00 00    	mov    ecx,DWORD PTR [edx+0x600]
  44398c:	e8 cf f2 fc ff       	call   0x412c60
  443991:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
  443995:	8b 44 24 18          	mov    eax,DWORD PTR [esp+0x18]
  443999:	81 c7 e4 0d 00 00    	add    edi,0xde4
  44399f:	48                   	dec    eax
  4439a0:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  4439a4:	0f 85 8e fe ff ff    	jne    0x443838
  4439aa:	a1 b0 b2 46 00       	mov    eax,ds:0x46b2b0
  4439af:	8b 08                	mov    ecx,DWORD PTR [eax]
  4439b1:	8b 50 08             	mov    edx,DWORD PTR [eax+0x8]
  4439b4:	51                   	push   ecx
  4439b5:	55                   	push   ebp
  4439b6:	52                   	push   edx
  4439b7:	8d 84 24 b4 00 00 00 	lea    eax,[esp+0xb4]
  4439be:	68 40 c5 46 00       	push   0x46c540
  4439c3:	50                   	push   eax
  4439c4:	e8 aa e0 00 00       	call   0x451a73
  4439c9:	83 c4 14             	add    esp,0x14
  4439cc:	8d 8c 24 a8 00 00 00 	lea    ecx,[esp+0xa8]
  4439d3:	51                   	push   ecx
  4439d4:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  4439da:	e8 e1 ea fc ff       	call   0x4124c0
  4439df:	55                   	push   ebp
  4439e0:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
  4439e4:	8b cd                	mov    ecx,ebp
  4439e6:	e8 35 5e ff ff       	call   0x439820
  4439eb:	8b 86 ad e3 00 00    	mov    eax,DWORD PTR [esi+0xe3ad]
  4439f1:	85 c0                	test   eax,eax
  4439f3:	0f 8e b9 00 00 00    	jle    0x443ab2
  4439f9:	33 db                	xor    ebx,ebx
  4439fb:	85 c0                	test   eax,eax
  4439fd:	0f 8e af 00 00 00    	jle    0x443ab2
  443a03:	81 c6 b1 e3 00 00    	add    esi,0xe3b1
  443a09:	83 fb 04             	cmp    ebx,0x4
  443a0c:	0f 8d a0 00 00 00    	jge    0x443ab2
  443a12:	8b 46 10             	mov    eax,DWORD PTR [esi+0x10]
  443a15:	85 c0                	test   eax,eax
  443a17:	0f 8e 81 00 00 00    	jle    0x443a9e
  443a1d:	e8 43 db 00 00       	call   0x451565
  443a22:	99                   	cdq
  443a23:	b9 64 00 00 00       	mov    ecx,0x64
  443a28:	f7 f9                	idiv   ecx
  443a2a:	3b 56 10             	cmp    edx,DWORD PTR [esi+0x10]
  443a2d:	7f 6f                	jg     0x443a9e
  443a2f:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  443a35:	8b 06                	mov    eax,DWORD PTR [esi]
  443a37:	3b 81 a8 00 00 00    	cmp    eax,DWORD PTR [ecx+0xa8]
  443a3d:	7d 5f                	jge    0x443a9e
  443a3f:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  443a46:	2b d0                	sub    edx,eax
  443a48:	8d 14 90             	lea    edx,[eax+edx*4]
  443a4b:	c1 e2 04             	shl    edx,0x4
  443a4e:	2b d0                	sub    edx,eax
  443a50:	8b 81 ac 00 00 00    	mov    eax,DWORD PTR [ecx+0xac]
  443a56:	83 c9 ff             	or     ecx,0xffffffff
  443a59:	8d 14 50             	lea    edx,[eax+edx*2]
  443a5c:	33 c0                	xor    eax,eax
  443a5e:	8b fa                	mov    edi,edx
  443a60:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  443a62:	f7 d1                	not    ecx
  443a64:	49                   	dec    ecx
  443a65:	74 16                	je     0x443a7d
  443a67:	52                   	push   edx
  443a68:	8d 8c 24 ac 00 00 00 	lea    ecx,[esp+0xac]
  443a6f:	68 30 c5 46 00       	push   0x46c530
  443a74:	51                   	push   ecx
  443a75:	e8 f9 df 00 00       	call   0x451a73
  443a7a:	83 c4 0c             	add    esp,0xc
  443a7d:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443a83:	8d 94 24 a8 00 00 00 	lea    edx,[esp+0xa8]
  443a8a:	52                   	push   edx
  443a8b:	e8 30 ea fc ff       	call   0x4124c0
  443a90:	8b 06                	mov    eax,DWORD PTR [esi]
  443a92:	6a 00                	push   0x0
  443a94:	6a 01                	push   0x1
  443a96:	50                   	push   eax
  443a97:	8b cd                	mov    ecx,ebp
  443a99:	e8 82 56 ff ff       	call   0x439120
  443a9e:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  443aa2:	43                   	inc    ebx
  443aa3:	83 c6 04             	add    esi,0x4
  443aa6:	3b 99 ad e3 00 00    	cmp    ebx,DWORD PTR [ecx+0xe3ad]
  443aac:	0f 8c 57 ff ff ff    	jl     0x443a09
  443ab2:	8d 4c 24 24          	lea    ecx,[esp+0x24]
  443ab6:	c7 84 24 ac 01 00 00 	mov    DWORD PTR [esp+0x1ac],0xffffffff
  443abd:	ff ff ff ff 
  443ac1:	e8 4a dc ff ff       	call   0x441710
  443ac6:	5e                   	pop    esi
  443ac7:	5d                   	pop    ebp
  443ac8:	5b                   	pop    ebx
  443ac9:	eb 6e                	jmp    0x443b39
  443acb:	83 f8 02             	cmp    eax,0x2
  443ace:	75 12                	jne    0x443ae2
  443ad0:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443ad6:	e8 d5 e9 fc ff       	call   0x4124b0
  443adb:	68 14 c5 46 00       	push   0x46c514
  443ae0:	eb 2c                	jmp    0x443b0e
  443ae2:	83 f8 03             	cmp    eax,0x3
  443ae5:	75 12                	jne    0x443af9
  443ae7:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443aed:	e8 be e9 fc ff       	call   0x4124b0
  443af2:	68 f8 c4 46 00       	push   0x46c4f8
  443af7:	eb 15                	jmp    0x443b0e
  443af9:	83 f8 04             	cmp    eax,0x4
  443afc:	75 3b                	jne    0x443b39
  443afe:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443b04:	e8 a7 e9 fc ff       	call   0x4124b0
  443b09:	68 dc c4 46 00       	push   0x46c4dc
  443b0e:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443b14:	e8 a7 e9 fc ff       	call   0x4124c0
  443b19:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443b1f:	68 bc 92 46 00       	push   0x4692bc
  443b24:	e8 97 e9 fc ff       	call   0x4124c0
  443b29:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  443b2f:	68 bc 92 46 00       	push   0x4692bc
  443b34:	e8 87 e9 fc ff       	call   0x4124c0
  443b39:	8b 8c 24 98 01 00 00 	mov    ecx,DWORD PTR [esp+0x198]
  443b40:	5f                   	pop    edi
  443b41:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  443b48:	81 c4 a0 01 00 00    	add    esp,0x1a0
  443b4e:	c2 04              	ret    0x4
