===== 0x41A730..0x41A790 菜单别名与slot2查询 =====

/mnt/data/solid63_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041a730 <.text+0x19730>:
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
  41a78f:	39         	cmp    DWORD PTR [esi+0x58c],ebx

===== 0x41F290..0x41F320 公式manager参数绑定调用 =====

/mnt/data/solid63_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041f290 <.text+0x1e290>:
  41f290:	06                   	push   es
  41f291:	00 00                	add    BYTE PTR [eax],al
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
  41f2bf:	68 08 fe 89 00       	push   0x89fe08
  41f2c4:	89 86 a4 0b 00 00    	mov    DWORD PTR [esi+0xba4],eax
  41f2ca:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  41f2ce:	89 8e 2c 07 00 00    	mov    DWORD PTR [esi+0x72c],ecx
  41f2d4:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  41f2d8:	89 16                	mov    DWORD PTR [esi],edx
  41f2da:	89 86 9c 0b 00 00    	mov    DWORD PTR [esi+0xb9c],eax
  41f2e0:	89 86 a0 0b 00 00    	mov    DWORD PTR [esi+0xba0],eax
  41f2e6:	8b 15 b0 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b0
  41f2ec:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  41f2f0:	89 0d ec fd 89 00    	mov    DWORD PTR ds:0x89fdec,ecx
  41f2f6:	68 d0 ff 89 00       	push   0x89ffd0
  41f2fb:	52                   	push   edx
  41f2fc:	b9 94 01 8a 00       	mov    ecx,0x8a0194
  41f301:	a3 f0 fd 89 00       	mov    ds:0x89fdf0,eax
  41f306:	e8 55 b4 00 00       	call   0x42a760
  41f30b:	8b ce                	mov    ecx,esi
  41f30d:	e8 6e 02 00 00       	call   0x41f580
  41f312:	84 c0                	test   al,al
  41f314:	0f 84 5d 02 00 00    	je     0x41f577
  41f31a:	6a 00                	push   0x0
  41f31c:	6a 00                	push   0x0
  41f31e:	6a 67                	push   0x67

===== 0x420070..0x420120 双manager场域建立 =====

/mnt/data/solid63_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00420070 <.text+0x1f070>:
  420070:	7f 07                	jg     0x420079
  420072:	8b 50 20             	mov    edx,DWORD PTR [eax+0x20]
  420075:	85 d2                	test   edx,edx
  420077:	74 10                	je     0x420089
  420079:	83 f9 08             	cmp    ecx,0x8
  42007c:	7c 06                	jl     0x420084
  42007e:	83 78 20 01          	cmp    DWORD PTR [eax+0x20],0x1
  420082:	74 05                	je     0x420089
  420084:	39 78 20             	cmp    DWORD PTR [eax+0x20],edi
  420087:	75 23                	jne    0x4200ac
  420089:	8b 15 ec fd 89 00    	mov    edx,DWORD PTR ds:0x89fdec
  42008f:	52                   	push   edx
  420090:	8b 15 f0 fd 89 00    	mov    edx,DWORD PTR ds:0x89fdf0
  420096:	52                   	push   edx
  420097:	8b 10                	mov    edx,DWORD PTR [eax]
  420099:	51                   	push   ecx
  42009a:	8b 88 90 00 00 00    	mov    ecx,DWORD PTR [eax+0x90]
  4200a0:	51                   	push   ecx
  4200a1:	52                   	push   edx
  4200a2:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4200a7:	e8 74 da 01 00       	call   0x43db20
  4200ac:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4200ae:	83 f9 07             	cmp    ecx,0x7
  4200b1:	7f 0c                	jg     0x4200bf
  4200b3:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  4200b9:	83 78 20 01          	cmp    DWORD PTR [eax+0x20],0x1
  4200bd:	74 1d                	je     0x4200dc
  4200bf:	83 f9 08             	cmp    ecx,0x8
  4200c2:	7c 0d                	jl     0x4200d1
  4200c4:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  4200ca:	8b 50 20             	mov    edx,DWORD PTR [eax+0x20]
  4200cd:	85 d2                	test   edx,edx
  4200cf:	74 0b                	je     0x4200dc
  4200d1:	8b 86 48 08 00 00    	mov    eax,DWORD PTR [esi+0x848]
  4200d7:	39 78 20             	cmp    DWORD PTR [eax+0x20],edi
  4200da:	75 23                	jne    0x4200ff
  4200dc:	8b 15 ec fd 89 00    	mov    edx,DWORD PTR ds:0x89fdec
  4200e2:	52                   	push   edx
  4200e3:	8b 15 f0 fd 89 00    	mov    edx,DWORD PTR ds:0x89fdf0
  4200e9:	52                   	push   edx
  4200ea:	8b 10                	mov    edx,DWORD PTR [eax]
  4200ec:	51                   	push   ecx
  4200ed:	8b 88 90 00 00 00    	mov    ecx,DWORD PTR [eax+0x90]
  4200f3:	51                   	push   ecx
  4200f4:	52                   	push   edx
  4200f5:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  4200fa:	e8 21 da 01 00       	call   0x43db20
  4200ff:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  420105:	6a 00                	push   0x0
  420107:	e8 f4 40 01 00       	call   0x434200
  42010c:	c6 05 dc 96 46 00 01 	mov    BYTE PTR ds:0x4696dc,0x1
  420113:	6a 00                	push   0x0
  420115:	6a 00                	push   0x0
  420117:	6a 67                	push   0x67
  420119:	8b ce                	mov    ecx,esi
  42011b:	e8 00 20 00 00       	call   0x422120

===== 0x4211F0..0x421250 owner清理 =====

/mnt/data/solid63_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

004211f0 <.text+0x201f0>:
  4211f0:	00 2b                	add    BYTE PTR [ebx],ch
  4211f2:	d3 c1                	rol    ecx,cl
  4211f4:	fa                   	cli
  4211f5:	03 8b 99 84 08 00    	add    ecx,DWORD PTR [ebx+0x88499]
  4211fb:	00 8b c2 c1 e8 1f    	add    BYTE PTR [ebx+0x1fe8c1c2],cl
  421201:	03 d0                	add    edx,eax
  421203:	03 da                	add    ebx,edx
  421205:	89 99 84 08 00 00    	mov    DWORD PTR [ecx+0x884],ebx
  42120b:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421212:	39 a8 84 08 00 00    	cmp    DWORD PTR [eax+0x884],ebp
  421218:	7f 1c                	jg     0x421236
  42121a:	56                   	push   esi
  42121b:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  421220:	89 a8 84 08 00 00    	mov    DWORD PTR [eax+0x884],ebp
  421226:	e8 35 ca 01 00       	call   0x43dc60
  42122b:	56                   	push   esi
  42122c:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  421231:	e8 2a ca 01 00       	call   0x43dc60
  421236:	8b 87 48 08 00 00    	mov    eax,DWORD PTR [edi+0x848]
  42123c:	3b c5                	cmp    eax,ebp
  42123e:	74 48                	je     0x421288
  421240:	83 bf bc 01 00 00 ff 	cmp    DWORD PTR [edi+0x1bc],0xffffffff
  421247:	74 3f                	je     0x421288
  421249:	8b 48 38             	mov    ecx,DWORD PTR [eax+0x38]
  42124c:	8b 97 84 08      	mov    edx,DWORD PTR [edi+0x884]

===== 0x423050..0x423110 查询owner与slot有效性 =====

/mnt/data/solid63_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00423050 <.text+0x22050>:
  423050:	00 00                	add    BYTE PTR [eax],al
  423052:	8b 96 48 08 00 00    	mov    edx,DWORD PTR [esi+0x848]
  423058:	8b cf                	mov    ecx,edi
  42305a:	8b 82 90 00 00 00    	mov    eax,DWORD PTR [edx+0x90]
  423060:	50                   	push   eax
  423061:	6a 03                	push   0x3
  423063:	6a 0c                	push   0xc
  423065:	e8 26 0f 00 00       	call   0x423f90
  42306a:	5f                   	pop    edi
  42306b:	5e                   	pop    esi
  42306c:	c2 08 00             	ret    0x8
  42306f:	90                   	nop
  423070:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  423074:	85 c0                	test   eax,eax
  423076:	7c 17                	jl     0x42308f
  423078:	83 f8 07             	cmp    eax,0x7
  42307b:	7f 12                	jg     0x42308f
  42307d:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  423081:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  423086:	50                   	push   eax
  423087:	e8 c4 a9 01 00       	call   0x43da50
  42308c:	c2 08 00             	ret    0x8
  42308f:	83 f8 08             	cmp    eax,0x8
  423092:	7c 17                	jl     0x4230ab
  423094:	83 f8 0f             	cmp    eax,0xf
  423097:	7f 12                	jg     0x4230ab
  423099:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  42309d:	51                   	push   ecx
  42309e:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4230a3:	e8 a8 a9 01 00       	call   0x43da50
  4230a8:	c2 08 00             	ret    0x8
  4230ab:	32 c0                	xor    al,al
  4230ad:	c2 08 00             	ret    0x8
  4230b0:	8b 01                	mov    eax,DWORD PTR [ecx]
  4230b2:	8a 4c 24 08          	mov    cl,BYTE PTR [esp+0x8]
  4230b6:	85 c0                	test   eax,eax
  4230b8:	7c 09                	jl     0x4230c3
  4230ba:	83 f8 07             	cmp    eax,0x7
  4230bd:	7f 04                	jg     0x4230c3
  4230bf:	84 c9                	test   cl,cl
  4230c1:	74 0e                	je     0x4230d1
  4230c3:	83 f8 08             	cmp    eax,0x8
  4230c6:	7c 1b                	jl     0x4230e3
  4230c8:	83 f8 0f             	cmp    eax,0xf
  4230cb:	7f 16                	jg     0x4230e3
  4230cd:	84 c9                	test   cl,cl
  4230cf:	74 12                	je     0x4230e3
  4230d1:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4230d5:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4230da:	50                   	push   eax
  4230db:	e8 60 ab 01 00       	call   0x43dc40
  4230e0:	c2 08 00             	ret    0x8
  4230e3:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  4230e7:	51                   	push   ecx
  4230e8:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  4230ed:	e8 4e ab 01 00       	call   0x43dc40
  4230f2:	c2 08 00             	ret    0x8
  4230f5:	90                   	nop
  4230f6:	90                   	nop
  4230f7:	90                   	nop
  4230f8:	90                   	nop
  4230f9:	90                   	nop
  4230fa:	90                   	nop
  4230fb:	90                   	nop
  4230fc:	90                   	nop
  4230fd:	90                   	nop
  4230fe:	90                   	nop
  4230ff:	90                   	nop
  423100:	57                   	push   edi
  423101:	8b f9                	mov    edi,ecx
  423103:	32 c9                	xor    cl,cl
  423105:	8b 87 f4 08 00 00    	mov    eax,DWORD PTR [edi+0x8f4]
  42310b:	83 f8 24             	cmp    eax,0x24
  42310e:	7c 10                	jl     0x423120

===== 0x423580..0x423660 manager包装/复位 =====

/mnt/data/solid63_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00423580 <.text+0x22580>:
  423580:	00 85 c0 7e 08 50    	add    BYTE PTR [ebp+0x50087ec0],al
  423586:	8b cf                	mov    ecx,edi
  423588:	e8 e3 fb ff ff       	call   0x423170
  42358d:	83 c6 08             	add    esi,0x8
  423590:	83 fe 28             	cmp    esi,0x28
  423593:	7c c1                	jl     0x423556
  423595:	5f                   	pop    edi
  423596:	5e                   	pop    esi
  423597:	c3                   	ret
  423598:	90                   	nop
  423599:	90                   	nop
  42359a:	90                   	nop
  42359b:	90                   	nop
  42359c:	90                   	nop
  42359d:	90                   	nop
  42359e:	90                   	nop
  42359f:	90                   	nop
  4235a0:	e8 0b 00 00 00       	call   0x4235b0
  4235a5:	e9 16 00 00 00       	jmp    0x4235c0
  4235aa:	90                   	nop
  4235ab:	90                   	nop
  4235ac:	90                   	nop
  4235ad:	90                   	nop
  4235ae:	90                   	nop
  4235af:	90                   	nop
  4235b0:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4235b5:	e9 56 a4 01 00       	jmp    0x43da10
  4235ba:	90                   	nop
  4235bb:	90                   	nop
  4235bc:	90                   	nop
  4235bd:	90                   	nop
  4235be:	90                   	nop
  4235bf:	90                   	nop
  4235c0:	68 d0 35 42 00       	push   0x4235d0
  4235c5:	e8 6b ef 02 00       	call   0x452535
  4235ca:	59                   	pop    ecx
  4235cb:	c3                   	ret
  4235cc:	90                   	nop
  4235cd:	90                   	nop
  4235ce:	90                   	nop
  4235cf:	90                   	nop
  4235d0:	8a 0d 9c 01 8a 00    	mov    cl,BYTE PTR ds:0x8a019c
  4235d6:	b0 01                	mov    al,0x1
  4235d8:	84 c8                	test   al,cl
  4235da:	75 12                	jne    0x4235ee
  4235dc:	0a c8                	or     cl,al
  4235de:	88 0d 9c 01 8a 00    	mov    BYTE PTR ds:0x8a019c,cl
  4235e4:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4235e9:	e9 32 a4 01 00       	jmp    0x43da20
  4235ee:	c3                   	ret
  4235ef:	90                   	nop
  4235f0:	e8 0b 00 00 00       	call   0x423600
  4235f5:	e9 16 00 00 00       	jmp    0x423610
  4235fa:	90                   	nop
  4235fb:	90                   	nop
  4235fc:	90                   	nop
  4235fd:	90                   	nop
  4235fe:	90                   	nop
  4235ff:	90                   	nop
  423600:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  423605:	e9 06 a4 01 00       	jmp    0x43da10
  42360a:	90                   	nop
  42360b:	90                   	nop
  42360c:	90                   	nop
  42360d:	90                   	nop
  42360e:	90                   	nop
  42360f:	90                   	nop
  423610:	68 20 36 42 00       	push   0x423620
  423615:	e8 1b ef 02 00       	call   0x452535
  42361a:	59                   	pop    ecx
  42361b:	c3                   	ret
  42361c:	90                   	nop
  42361d:	90                   	nop
  42361e:	90                   	nop
  42361f:	90                   	nop
  423620:	8a 0d 9c 01 8a 00    	mov    cl,BYTE PTR ds:0x8a019c
  423626:	b0 02                	mov    al,0x2
  423628:	84 c8                	test   al,cl
  42362a:	75 12                	jne    0x42363e
  42362c:	0a c8                	or     cl,al
  42362e:	88 0d 9c 01 8a 00    	mov    BYTE PTR ds:0x8a019c,cl
  423634:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  423639:	e9 e2 a3 01 00       	jmp    0x43da20
  42363e:	c3                   	ret
  42363f:	90                   	nop
  423640:	e8 0b 00 00 00       	call   0x423650
  423645:	e9 16 00 00 00       	jmp    0x423660
  42364a:	90                   	nop
  42364b:	90                   	nop
  42364c:	90                   	nop
  42364d:	90                   	nop
  42364e:	90                   	nop
  42364f:	90                   	nop
  423650:	b9 94 01 8a 00       	mov    ecx,0x8a0194
  423655:	e9 d6 16 fe ff       	jmp    0x404d30
  42365a:	90                   	nop
  42365b:	90                   	nop
  42365c:	90                   	nop
  42365d:	90                   	nop
  42365e:	90                   	nop
  42365f:	90                   	nop

===== 0x42A750..0x42A7A0 公式层alias绑定 =====

/mnt/data/solid63_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042a750 <.text+0x29750>:
  42a750:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  42a754:	75 c4                	jne    0x42a71a
  42a756:	5e                   	pop    esi
  42a757:	5b                   	pop    ebx
  42a758:	5f                   	pop    edi
  42a759:	5d                   	pop    ebp
  42a75a:	c3                   	ret
  42a75b:	90                   	nop
  42a75c:	90                   	nop
  42a75d:	90                   	nop
  42a75e:	90                   	nop
  42a75f:	90                   	nop
  42a760:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  42a764:	85 d2                	test   edx,edx
  42a766:	74 26                	je     0x42a78e
  42a768:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  42a76c:	85 c9                	test   ecx,ecx
  42a76e:	74 1e                	je     0x42a78e
  42a770:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  42a774:	85 c0                	test   eax,eax
  42a776:	74 16                	je     0x42a78e
  42a778:	a3 d0 01 8b 00       	mov    ds:0x8b01d0,eax
  42a77d:	89 15 c8 01 8b 00    	mov    DWORD PTR ds:0x8b01c8,edx
  42a783:	89 0d cc 01 8b 00    	mov    DWORD PTR ds:0x8b01cc,ecx
  42a789:	b0 01                	mov    al,0x1
  42a78b:	c2 0c 00             	ret    0xc
  42a78e:	32 c0                	xor    al,al
  42a790:	c2 0c 00             	ret    0xc
  42a793:	90                   	nop
  42a794:	90                   	nop
  42a795:	90                   	nop
  42a796:	90                   	nop
  42a797:	90                   	nop
  42a798:	90                   	nop
  42a799:	90                   	nop
  42a79a:	90                   	nop
  42a79b:	90                   	nop
  42a79c:	90                   	nop
  42a79d:	90                   	nop
  42a79e:	90                   	nop
  42a79f:	90                   	nop

===== 0x42BF30..0x42BF90 公式层alias查询 =====

/mnt/data/solid63_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042bf30 <.text+0x2af30>:
  42bf30:	01 10                	add    DWORD PTR [eax],edx
  42bf32:	c2 14 00             	ret    0x14
  42bf35:	90                   	nop
  42bf36:	90                   	nop
  42bf37:	90                   	nop
  42bf38:	90                   	nop
  42bf39:	90                   	nop
  42bf3a:	90                   	nop
  42bf3b:	90                   	nop
  42bf3c:	90                   	nop
  42bf3d:	90                   	nop
  42bf3e:	90                   	nop
  42bf3f:	90                   	nop
  42bf40:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  42bf44:	53                   	push   ebx
  42bf45:	32 db                	xor    bl,bl
  42bf47:	85 c0                	test   eax,eax
  42bf49:	75 1a                	jne    0x42bf65
  42bf4b:	8b 44 24 0c          	mov    eax,DWORD PTR [esp+0xc]
  42bf4f:	8b 0d cc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01cc
  42bf55:	50                   	push   eax
  42bf56:	e8 f5 1a 01 00       	call   0x43da50
  42bf5b:	84 c0                	test   al,al
  42bf5d:	74 21                	je     0x42bf80
  42bf5f:	b0 01                	mov    al,0x1
  42bf61:	5b                   	pop    ebx
  42bf62:	c2 08 00             	ret    0x8
  42bf65:	83 f8 01             	cmp    eax,0x1
  42bf68:	75 16                	jne    0x42bf80
  42bf6a:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  42bf6e:	51                   	push   ecx
  42bf6f:	8b 0d d0 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01d0
  42bf75:	e8 d6 1a 01 00       	call   0x43da50
  42bf7a:	84 c0                	test   al,al
  42bf7c:	b0 01                	mov    al,0x1
  42bf7e:	75 02                	jne    0x42bf82
  42bf80:	8a c3                	mov    al,bl
  42bf82:	5b                   	pop    ebx
  42bf83:	c2 08 00             	ret    0x8
  42bf86:	90                   	nop
  42bf87:	90                   	nop
  42bf88:	90                   	nop
  42bf89:	90                   	nop
  42bf8a:	90                   	nop
  42bf8b:	90                   	nop
  42bf8c:	90                   	nop
  42bf8d:	90                   	nop
  42bf8e:	90                   	nop
  42bf8f:	90                   	nop

===== 0x442760..0x442890 Battle manager绑定/初始化/+0x1C0 =====

/mnt/data/solid63_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00442760 <.text+0x41760>:
  442760:	00 8d 9d 54 02 00    	add    BYTE PTR [ebp+0x2549d],cl
  442766:	00 56 57             	add    BYTE PTR [esi+0x57],dl
  442769:	53                   	push   ebx
  44276a:	89 2d 80 fd 89 00    	mov    DWORD PTR ds:0x89fd80,ebp
  442770:	e8 fb 36 00 00       	call   0x445e70
  442775:	8b 0d e4 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01e4
  44277b:	56                   	push   esi
  44277c:	57                   	push   edi
  44277d:	53                   	push   ebx
  44277e:	e8 0d dd fe ff       	call   0x430490
  442783:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  442788:	33 ff                	xor    edi,edi
  44278a:	8b 48 3c             	mov    ecx,DWORD PTR [eax+0x3c]
  44278d:	c7 05 54 fd 89 00 08 	mov    DWORD PTR ds:0x89fd54,0x89fe08
  442794:	fe 89 00 
  442797:	89 0d d8 fc 89 00    	mov    DWORD PTR ds:0x89fcd8,ecx
  44279d:	c7 05 58 fd 89 00 d0 	mov    DWORD PTR ds:0x89fd58,0x89ffd0
  4427a4:	ff 89 00 
  4427a7:	8b 48 3c             	mov    ecx,DWORD PTR [eax+0x3c]
  4427aa:	85 c9                	test   ecx,ecx
  4427ac:	0f 8e 8a 00 00 00    	jle    0x44283c
  4427b2:	be dc fc 89 00       	mov    esi,0x89fcdc
  4427b7:	bb 68 00 00 00       	mov    ebx,0x68
  4427bc:	b9 40 00 00 00       	mov    ecx,0x40
  4427c1:	ba bc 05 00 00       	mov    edx,0x5bc
  4427c6:	2b de                	sub    ebx,esi
  4427c8:	2b ce                	sub    ecx,esi
  4427ca:	2b d6                	sub    edx,esi
  4427cc:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  4427d0:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  4427d4:	eb 04                	jmp    0x4427da
  4427d6:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  4427da:	8d 14 33             	lea    edx,[ebx+esi*1]
  4427dd:	03 ce                	add    ecx,esi
  4427df:	03 c8                	add    ecx,eax
  4427e1:	8b 14 02             	mov    edx,DWORD PTR [edx+eax*1]
  4427e4:	89 16                	mov    DWORD PTR [esi],edx
  4427e6:	8b 01                	mov    eax,DWORD PTR [ecx]
  4427e8:	8d 14 c0             	lea    edx,[eax+eax*8]
  4427eb:	8d 14 90             	lea    edx,[eax+edx*4]
  4427ee:	8d 14 52             	lea    edx,[edx+edx*2]
  4427f1:	8d 04 d0             	lea    eax,[eax+edx*8]
  4427f4:	8d 94 85 50 7b 00 00 	lea    edx,[ebp+eax*4+0x7b50]
  4427fb:	89 56 28             	mov    DWORD PTR [esi+0x28],edx
  4427fe:	8b 01                	mov    eax,DWORD PTR [ecx]
  442800:	8d 0c c0             	lea    ecx,[eax+eax*8]
  442803:	8d 0c 88             	lea    ecx,[eax+ecx*4]
  442806:	8d 14 49             	lea    edx,[ecx+ecx*2]
  442809:	8d 04 d0             	lea    eax,[eax+edx*8]
  44280c:	8b 15 74 fd 89 00    	mov    edx,DWORD PTR ds:0x89fd74
  442812:	8d 8c 85 ac 7f 00 00 	lea    ecx,[ebp+eax*4+0x7fac]
  442819:	89 4e 50             	mov    DWORD PTR [esi+0x50],ecx
  44281c:	8b 82 f0 05 00 00    	mov    eax,DWORD PTR [edx+0x5f0]
  442822:	03 44 24 14          	add    eax,DWORD PTR [esp+0x14]
  442826:	8b 0c 30             	mov    ecx,DWORD PTR [eax+esi*1]
  442829:	e8 42 c5 fd ff       	call   0x41ed70
  44282e:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  442833:	47                   	inc    edi
  442834:	83 c6 04             	add    esi,0x4
  442837:	3b 78 3c             	cmp    edi,DWORD PTR [eax+0x3c]
  44283a:	7c 9a                	jl     0x4427d6
  44283c:	33 f6                	xor    esi,esi
  44283e:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  442843:	89 35 f8 fd 89 00    	mov    DWORD PTR ds:0x89fdf8,esi
  442849:	e8 e2 b1 ff ff       	call   0x43da30
  44284e:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  442853:	c7 05 c8 ff 89 00 01 	mov    DWORD PTR ds:0x89ffc8,0x1
  44285a:	00 00 00 
  44285d:	e8 ce b1 ff ff       	call   0x43da30
  442862:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  442866:	89 35 90 01 8a 00    	mov    DWORD PTR ds:0x8a0190,esi
  44286c:	c7 84 24 88 00 00 00 	mov    DWORD PTR [esp+0x88],0xffffffff
  442873:	ff ff ff ff 
  442877:	e8 94 ee ff ff       	call   0x441710
  44287c:	8b 8c 24 80 00 00 00 	mov    ecx,DWORD PTR [esp+0x80]
  442883:	8a 44 24 13          	mov    al,BYTE PTR [esp+0x13]
  442887:	5f                   	pop    edi
  442888:	5e                   	pop    esi
  442889:	5d                   	pop    ebp
  44288a:	5b                   	pop    ebx
  44288b:	64 89 0d 00 00   	mov    DWORD PTR fs:0x0,ecx

===== 0x442C90..0x442D60 tick与owner任一active查询 =====

/mnt/data/solid63_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00442c90 <.text+0x41c90>:
  442c90:	00 00                	add    BYTE PTR [eax],al
  442c92:	4b                   	dec    ebx
  442c93:	75 f0                	jne    0x442c85
  442c95:	83 bd 24 e2 00 00 ff 	cmp    DWORD PTR [ebp+0xe224],0xffffffff
  442c9c:	0f 85 1c 01 00 00    	jne    0x442dbe
  442ca2:	a0 dc 96 46 00       	mov    al,ds:0x4696dc
  442ca7:	84 c0                	test   al,al
  442ca9:	0f 84 0f 01 00 00    	je     0x442dbe
  442caf:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  442cb4:	e8 f7 ad ff ff       	call   0x43dab0
  442cb9:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  442cbe:	e8 ed ad ff ff       	call   0x43dab0
  442cc3:	33 ff                	xor    edi,edi
  442cc5:	8b 86 48 fa ff ff    	mov    eax,DWORD PTR [esi-0x5b8]
  442ccb:	85 c0                	test   eax,eax
  442ccd:	0f 8c db 00 00 00    	jl     0x442dae
  442cd3:	8d ae 1c f3 ff ff    	lea    ebp,[esi-0xce4]
  442cd9:	8b cd                	mov    ecx,ebp
  442cdb:	e8 60 d7 fd ff       	call   0x420440
  442ce0:	8d 5e c4             	lea    ebx,[esi-0x3c]
  442ce3:	8b cb                	mov    ecx,ebx
  442ce5:	e8 f6 10 fe ff       	call   0x423de0
  442cea:	83 e8 02             	sub    eax,0x2
  442ced:	0f 84 8e 00 00 00    	je     0x442d81
  442cf3:	83 e8 02             	sub    eax,0x2
  442cf6:	74 67                	je     0x442d5f
  442cf8:	83 e8 02             	sub    eax,0x2
  442cfb:	0f 85 a9 00 00 00    	jne    0x442daa
  442d01:	81 7e fc d6 07 00 00 	cmp    DWORD PTR [esi-0x4],0x7d6
  442d08:	0f 85 9c 00 00 00    	jne    0x442daa
  442d0e:	8b 06                	mov    eax,DWORD PTR [esi]
  442d10:	3d c6 0b 00 00       	cmp    eax,0xbc6
  442d15:	0f 84 8f 00 00 00    	je     0x442daa
  442d1b:	3d c0 0b 00 00       	cmp    eax,0xbc0
  442d20:	0f 84 84 00 00 00    	je     0x442daa
  442d26:	57                   	push   edi
  442d27:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  442d2c:	e8 df ae ff ff       	call   0x43dc10
  442d31:	83 f8 ff             	cmp    eax,0xffffffff
  442d34:	75 74                	jne    0x442daa
  442d36:	57                   	push   edi
  442d37:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  442d3c:	e8 cf ae ff ff       	call   0x43dc10
  442d41:	83 f8 ff             	cmp    eax,0xffffffff
  442d44:	75 64                	jne    0x442daa
  442d46:	6a 01                	push   0x1
  442d48:	8b cb                	mov    ecx,ebx
  442d4a:	e8 31 12 fe ff       	call   0x423f80
  442d4f:	8b cd                	mov    ecx,ebp
  442d51:	c7 46 fc d1 07 00 00 	mov    DWORD PTR [esi-0x4],0x7d1
  442d58:	e8 13 07 fe ff       	call   0x423470
  442d5d:	eb 4b                	jmp    0x442daa
  442d5f:	81       	cmp    DWORD PTR [esi-0x4],0x7d4

===== 0x444290..0x444310 AI slot2查询 =====

/mnt/data/solid63_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00444290 <.text+0x43290>:
  444290:	8b 4c d0 1c          	mov    ecx,DWORD PTR [eax+edx*8+0x1c]
  444294:	8d 04 d0             	lea    eax,[eax+edx*8]
  444297:	8b 87 d8 0c 00 00    	mov    eax,DWORD PTR [edi+0xcd8]
  44429d:	03 e9                	add    ebp,ecx
  44429f:	85 c0                	test   eax,eax
  4442a1:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  4442a5:	75 02                	jne    0x4442a9
  4442a7:	33 ed                	xor    ebp,ebp
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
  44430d:	8b 4c 24           	mov    ecx,DWORD PTR [esp+0x10]
