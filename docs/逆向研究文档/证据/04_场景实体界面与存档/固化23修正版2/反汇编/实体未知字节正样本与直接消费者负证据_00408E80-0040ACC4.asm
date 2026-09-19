; 固化23修正版2：Entity +0x71/+0x72 正样本与直接consumer负证据
; Oracle RPG.exe SHA256 b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; 注意：全 .text 字面 +0x71/+0x72 引用仅下列 +0x72 两处；二者均属于 Map record。
; 0x408E80 的输入由 0x40AC9E 调用，0x40ACB5 每条 raw input +=0x473，直接证明为Map SCI记录。


/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00408e80 <.text+0x7e80>:
  408e80:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  408e86:	6a ff                	push   0xffffffff
  408e88:	68 92 d0 45 00       	push   0x45d092
  408e8d:	50                   	push   eax
  408e8e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  408e95:	83 ec 64             	sub    esp,0x64
  408e98:	53                   	push   ebx
  408e99:	8b 5c 24 78          	mov    ebx,DWORD PTR [esp+0x78]
  408e9d:	55                   	push   ebp
  408e9e:	56                   	push   esi
  408e9f:	57                   	push   edi
  408ea0:	8b f1                	mov    esi,ecx
  408ea2:	8d 7b 74             	lea    edi,[ebx+0x74]
  408ea5:	89 1e                	mov    DWORD PTR [esi],ebx
  408ea7:	80 3f 00             	cmp    BYTE PTR [edi],0x0
  408eaa:	0f 84 e1 00 00 00    	je     0x408f91
  408eb0:	33 c0                	xor    eax,eax
  408eb2:	33 ed                	xor    ebp,ebp
  408eb4:	8a 83 d8 00 00 00    	mov    al,BYTE PTR [ebx+0xd8]
  408eba:	2b c5                	sub    eax,ebp
  408ebc:	74 55                	je     0x408f13
  408ebe:	48                   	dec    eax
  408ebf:	0f 85 f1 00 00 00    	jne    0x408fb6
  408ec5:	68 84 00 00 00       	push   0x84
  408eca:	e8 90 87 04 00       	call   0x45165f
  408ecf:	83 c4 04             	add    esp,0x4
  408ed2:	89 84 24 84 00 00 00 	mov    DWORD PTR [esp+0x84],eax
  408ed9:	3b c5                	cmp    eax,ebp
  408edb:	c7 44 24 7c 01 00 00 	mov    DWORD PTR [esp+0x7c],0x1
  408ee2:	00 
  408ee3:	74 09                	je     0x408eee
  408ee5:	8b c8                	mov    ecx,eax
  408ee7:	e8 94 e1 ff ff       	call   0x407080
  408eec:	eb 02                	jmp    0x408ef0
  408eee:	33 c0                	xor    eax,eax
  408ef0:	55                   	push   ebp
  408ef1:	55                   	push   ebp
  408ef2:	6a 01                	push   0x1
  408ef4:	6a 30                	push   0x30
  408ef6:	6a 40                	push   0x40
  408ef8:	57                   	push   edi
  408ef9:	8b c8                	mov    ecx,eax
  408efb:	c7 84 24 94 00 00 00 	mov    DWORD PTR [esp+0x94],0xffffffff
  408f02:	ff ff ff ff 
  408f06:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  408f09:	e8 c2 e1 ff ff       	call   0x4070d0
  408f0e:	e9 a3 00 00 00       	jmp    0x408fb6
  408f13:	6a 30                	push   0x30
  408f15:	e8 45 87 04 00       	call   0x45165f
  408f1a:	83 c4 04             	add    esp,0x4
  408f1d:	89 84 24 84 00 00 00 	mov    DWORD PTR [esp+0x84],eax
  408f24:	3b c5                	cmp    eax,ebp
  408f26:	89 6c 24 7c          	mov    DWORD PTR [esp+0x7c],ebp
  408f2a:	74 09                	je     0x408f35
  408f2c:	8b c8                	mov    ecx,eax
  408f2e:	e8 fd d6 ff ff       	call   0x406630
  408f33:	eb 02                	jmp    0x408f37
  408f35:	33 c0                	xor    eax,eax
  408f37:	33 c9                	xor    ecx,ecx
  408f39:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
  408f3c:	8a 4b 65             	mov    cl,BYTE PTR [ebx+0x65]
  408f3f:	c7 44 24 7c ff ff ff 	mov    DWORD PTR [esp+0x7c],0xffffffff
  408f46:	ff 
  408f47:	0f be 53 6e          	movsx  edx,BYTE PTR [ebx+0x6e]
  408f4b:	51                   	push   ecx
  408f4c:	52                   	push   edx
  408f4d:	0f bf 4b 72          	movsx  ecx,WORD PTR [ebx+0x72]
  408f51:	0f bf 53 70          	movsx  edx,WORD PTR [ebx+0x70]
  408f55:	51                   	push   ecx
  408f56:	33 c9                	xor    ecx,ecx
  408f58:	8a 4b 6f             	mov    cl,BYTE PTR [ebx+0x6f]
  408f5b:	52                   	push   edx
  408f5c:	0f bf 53 68          	movsx  edx,WORD PTR [ebx+0x68]
  408f60:	51                   	push   ecx
  408f61:	52                   	push   edx
  408f62:	0f bf 4b 66          	movsx  ecx,WORD PTR [ebx+0x66]
  408f66:	51                   	push   ecx
  408f67:	57                   	push   edi
  408f68:	8b c8                	mov    ecx,eax
  408f6a:	e8 f1 d6 ff ff       	call   0x406660
  408f6f:	8b 06                	mov    eax,DWORD PTR [esi]
  408f71:	33 d2                	xor    edx,edx
  408f73:	33 c9                	xor    ecx,ecx
  408f75:	66 8b 50 6c          	mov    dx,WORD PTR [eax+0x6c]
  408f79:	66 8b 48 6a          	mov    cx,WORD PTR [eax+0x6a]
  408f7d:	52                   	push   edx
  408f7e:	8b                   	.byte 0x8b
  408f7f:	56                   	push   esi

; 第二处 +0x72 在同一Map runtime对象方法 0x409510：

/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00409510 <.text+0x8510>:
  409510:	56                   	push   esi
  409511:	8b f1                	mov    esi,ecx
  409513:	33 c9                	xor    ecx,ecx
  409515:	57                   	push   edi
  409516:	8b 06                	mov    eax,DWORD PTR [esi]
  409518:	8a 88 d8 00 00 00    	mov    cl,BYTE PTR [eax+0xd8]
  40951e:	83 e9 00             	sub    ecx,0x0
  409521:	74 17                	je     0x40953a
  409523:	49                   	dec    ecx
  409524:	75 27                	jne    0x40954d
  409526:	0f bf 48 72          	movsx  ecx,WORD PTR [eax+0x72]
  40952a:	0f bf 50 70          	movsx  edx,WORD PTR [eax+0x70]
  40952e:	51                   	push   ecx
  40952f:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  409532:	52                   	push   edx
  409533:	e8 78 f0 ff ff       	call   0x4085b0
  409538:	eb 13                	jmp    0x40954d
  40953a:	8a 46 35             	mov    al,BYTE PTR [esi+0x35]
  40953d:	84 c0                	test   al,al
  40953f:	74 0c                	je     0x40954d
  409541:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  409544:	6a 00                	push   0x0
  409546:	6a 00                	push   0x0
  409548:	e8 53 d2 ff ff       	call   0x4067a0
  40954d:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  409550:	33 ff                	xor    edi,edi
  409552:	85 c0                	test   eax,eax
  409554:	7e 26                	jle    0x40957c
  409556:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  409559:	8b 0c b8             	mov    ecx,DWORD PTR [eax+edi*4]
  40955c:	85 c9                	test   ecx,ecx
  40955e:	74 14                	je     0x409574
  409560:	68 07 b2 01 00       	push   0x1b207
  409565:	68 07 b2 01 00       	push   0x1b207
  40956a:	68 07 b2 01 00       	push   0x1b207
  40956f:	e8 3c 0d 00 00       	call   0x40a2b0
  409574:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  409577:	47                   	inc    edi
  409578:	3b f8                	cmp    edi,eax
  40957a:	7c da                	jl     0x409556
  40957c:	5f                   	pop    edi
  40957d:	5e                   	pop    esi
  40957e:	c3                   	ret
  40957f:	90                   	nop

; Map构造循环，raw input每条+0x473：

/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040ac88 <.text+0x9c88>:
  40ac88:	89 5e 10             	mov    DWORD PTR [esi+0x10],ebx
  40ac8b:	7e 32                	jle    0x40acbf
  40ac8d:	8b 45 00             	mov    eax,DWORD PTR [ebp+0x0]
  40ac90:	33 db                	xor    ebx,ebx
  40ac92:	33 ed                	xor    ebp,ebp
  40ac94:	8d 0c 03             	lea    ecx,[ebx+eax*1]
  40ac97:	57                   	push   edi
  40ac98:	51                   	push   ecx
  40ac99:	8b 4e 10             	mov    ecx,DWORD PTR [esi+0x10]
  40ac9c:	03 cd                	add    ecx,ebp
  40ac9e:	e8 dd e1 ff ff       	call   0x408e80
  40aca3:	8b 46 0c             	mov    eax,DWORD PTR [esi+0xc]
  40aca6:	80 3c 03 00          	cmp    BYTE PTR [ebx+eax*1],0x0
  40acaa:	74 03                	je     0x40acaf
  40acac:	89 7e 04             	mov    DWORD PTR [esi+0x4],edi
  40acaf:	8b 0e                	mov    ecx,DWORD PTR [esi]
  40acb1:	47                   	inc    edi
  40acb2:	83 c5 38             	add    ebp,0x38
  40acb5:	81 c3 73 04 00 00    	add    ebx,0x473
  40acbb:	3b f9                	cmp    edi,ecx
  40acbd:	7c d5                	jl     0x40ac94
  40acbf:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  40acc3:	5f                   	pop    edi
  40acc4:	5e                   	pop    esi

; Entity构造器保存 raw record pointer 到 runtime+0x70，并直接消费+0x74/+0x76等，但没有+0x71/+0x72：

/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0040a160 <.text+0x9160>:
  40a160:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  40a166:	6a ff                	push   0xffffffff
  40a168:	68 cb d0 45 00       	push   0x45d0cb
  40a16d:	50                   	push   eax
  40a16e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  40a175:	53                   	push   ebx
  40a176:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
  40a17a:	56                   	push   esi
  40a17b:	57                   	push   edi
  40a17c:	8b 7c 24 1c          	mov    edi,DWORD PTR [esp+0x1c]
  40a180:	8b f1                	mov    esi,ecx
  40a182:	89 1e                	mov    DWORD PTR [esi],ebx
  40a184:	89 7e 70             	mov    DWORD PTR [esi+0x70],edi
  40a187:	8a 47 74             	mov    al,BYTE PTR [edi+0x74]
  40a18a:	88 46 56             	mov    BYTE PTR [esi+0x56],al
  40a18d:	8a 4f 76             	mov    cl,BYTE PTR [edi+0x76]
  40a190:	88 4e 57             	mov    BYTE PTR [esi+0x57],cl
  40a193:	0f be 57 77          	movsx  edx,BYTE PTR [edi+0x77]
  40a197:	83 c2 06             	add    edx,0x6
  40a19a:	89 56 1c             	mov    DWORD PTR [esi+0x1c],edx
  40a19d:	0f be 47 78          	movsx  eax,BYTE PTR [edi+0x78]
  40a1a1:	83 c0 04             	add    eax,0x4
  40a1a4:	89 46 20             	mov    DWORD PTR [esi+0x20],eax
  40a1a7:	0f be 4f 79          	movsx  ecx,BYTE PTR [edi+0x79]
  40a1ab:	83 c1 09             	add    ecx,0x9
  40a1ae:	89 4e 24             	mov    DWORD PTR [esi+0x24],ecx
  40a1b1:	0f be 57 7a          	movsx  edx,BYTE PTR [edi+0x7a]
  40a1b5:	83 c2 06             	add    edx,0x6
  40a1b8:	89 56 28             	mov    DWORD PTR [esi+0x28],edx
  40a1bb:	8b 47 64             	mov    eax,DWORD PTR [edi+0x64]
  40a1be:	89                   	.byte 0x89
  40a1bf:	46                   	inc    esi

; Entity模块典型raw-pointer consumers（+73/+74/+75/+76/+EA/+EB），用于对照：

/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

004096b0 <.text+0x86b0>:
  4096b0:	84 90 01 00 00 8b    	test   BYTE PTR [eax-0x74ffffff],dl
  4096b6:	50                   	push   eax
  4096b7:	18 8b 40 70 8a 48    	sbb    BYTE PTR [ebx+0x488a7040],cl
  4096bd:	76 80                	jbe    0x40963f
  4096bf:	f9                   	stc
  4096c0:	01 74 09 80          	add    DWORD PTR [ecx+ecx*1-0x80],esi
  4096c4:	f9                   	stc
  4096c5:	03 0f                	add    ecx,DWORD PTR [edi]
  4096c7:	85 5f 01             	test   DWORD PTR [edi+0x1],ebx
  4096ca:	00 00                	add    BYTE PTR [eax],al
  4096cc:	80 fa 02             	cmp    dl,0x2
  4096cf:	0f 85 56 01 00 00    	jne    0x40982b
  4096d5:	8a 48 74             	mov    cl,BYTE PTR [eax+0x74]
  4096d8:	84 c9                	test   cl,cl
  4096da:	0f 85 4b 01 00 00    	jne    0x40982b
  4096e0:	56                   	push   esi
  4096e1:	8b cf                	mov    ecx,edi
  4096e3:	c6 44 24 17 00       	mov    BYTE PTR [esp+0x17],0x0
  4096e8:	e8 63 03 00 00       	call   0x409a50
  4096ed:	84 c0                	test   al,al
  4096ef:	0f 84 36 01 00 00    	je     0x40982b
  4096f5:	8b 57 2c             	mov    edx,DWORD PTR [edi+0x2c]
  4096f8:	8b 0c b2             	mov    ecx,DWORD PTR [edx+esi*4]
  4096fb:	8d 04 b2             	lea    eax,[edx+esi*4]
  4096fe:	8b 51 70             	mov    edx,DWORD PTR [ecx+0x70]
  409701:	80 7a 76 03          	cmp    BYTE PTR [edx+0x76],0x3
  409705:	75 3c                	jne    0x409743
  409707:	e8 64 10 00 00       	call   0x40a770
  40970c:	8b 47 2c             	mov    eax,DWORD PTR [edi+0x2c]
  40970f:	6a 01                	push   0x1
  409711:	8b 0c b0             	mov    ecx,DWORD PTR [eax+esi*4]
  409714:	e8 d7 13 00 00       	call   0x40aaf0
  409719:	8b 57 2c             	mov    edx,DWORD PTR [edi+0x2c]
  40971c:	8d 4c 24 14          	lea    ecx,[esp+0x14]

/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00409960 <.text+0x8960>:
  409960:	ef                   	out    dx,eax
  409961:	f7 c5 00 ff 00 00    	test   ebp,0xff00
  409967:	75 5c                	jne    0x4099c5
  409969:	83 7c 24 1c 01       	cmp    DWORD PTR [esp+0x1c],0x1
  40996e:	75 55                	jne    0x4099c5
  409970:	8b 56 30             	mov    edx,DWORD PTR [esi+0x30]
  409973:	8b 04 9a             	mov    eax,DWORD PTR [edx+ebx*4]
  409976:	33 d2                	xor    edx,edx
  409978:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  40997b:	8a 91 eb 00 00 00    	mov    dl,BYTE PTR [ecx+0xeb]
  409981:	52                   	push   edx
  409982:	e8 a9 18 00 00       	call   0x40b230
  409987:	8b 46 30             	mov    eax,DWORD PTR [esi+0x30]
  40998a:	8b 4e 1c             	mov    ecx,DWORD PTR [esi+0x1c]
  40998d:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  409990:	83 c4 04             	add    esp,0x4
  409993:	8b 1c 98             	mov    ebx,DWORD PTR [eax+ebx*4]
  409996:	39 1c 8a             	cmp    DWORD PTR [edx+ecx*4],ebx
  409999:	74 2a                	je     0x4099c5
  40999b:	c6 43 54 01          	mov    BYTE PTR [ebx+0x54],0x1
  40999f:	e8 6c f2 ff ff       	call   0x408c10
  4099a4:	8b 3d d4 f7 89 00    	mov    edi,DWORD PTR ds:0x89f7d4
  4099aa:	8b 40 0c             	mov    eax,DWORD PTR [eax+0xc]
  4099ad:	81 e7 ff 00 00 00    	and    edi,0xff
  4099b3:	25 ff ff 00 ff       	and    eax,0xff00ffff
  4099b8:	c1 e7 10             	shl    edi,0x10
  4099bb:	0b f8                	or     edi,eax
  4099bd:	e8 4e f2 ff ff       	call   0x408c10
  4099c2:	89 78 18             	mov    DWORD PTR [eax+0x18],edi
  4099c5:	a1 d0 f7 89 00       	mov    eax,ds:0x89f7d0
  4099ca:	81 e5 02 ff ff ff    	and    ebp,0xffffff02
  4099d0:	83 cd 02             	or     ebp,0x2
  4099d3:	85 c0                	test   eax,eax
  4099d5:	75 17                	jne    0x4099ee
  4099d7:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  4099db:	05 ff ff 00 00       	add    eax,0xffff
  4099e0:	c1 e0 10             	shl    eax,0x10
  4099e3:	33 c5                	xor    eax,ebp
  4099e5:	25 00 00 ff 00       	and    eax,0xff0000
  4099ea:	33 e8                	xor    ebp,eax
  4099ec:	eb 0c                	jmp    0x4099fa
  4099ee:	81 e5 ff ff 04 ff    	and    ebp,0xff04ffff
  4099f4:	81 cd 00 00 04 00    	or     ebp,0x40000
  4099fa:	8b 4e 1c             	mov    ecx,DWORD PTR [esi+0x1c]
  4099fd:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  409a00:	68 07 b2 01 00       	push   0x1b207
  409a05:	68 07 b2 01 00       	push   0x1b207
  409a0a:	8b 4c 8a 04          	mov    ecx,DWORD PTR [edx+ecx*4+0x4]
  409a0e:	55                   	push   ebp
  409a0f:	e8 9c 08 00 00       	call   0x40a2b0
  409a14:	5d                   	pop    ebp
  409a15:	5f                   	pop    edi
  409a16:	5e                   	pop    esi
  409a17:	5b                   	pop    ebx
  409a18:	83 c4 08             	add    esp,0x8
  409a1b:	c2 04 00             	ret    0x4
  409a1e:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  409a21:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  409a24:	68 07 b2 01 00       	push   0x1b207
  409a29:	68 07 b2 01 00       	push   0x1b207
  409a2e:	8b 4c 81 04          	mov    ecx,DWORD PTR [ecx+eax*4+0x4]
  409a32:	68 07 b2 01 00       	push   0x1b207
  409a37:	e8 74 08 00 00       	call   0x40a2b0
  409a3c:	5f                   	pop    edi
  409a3d:	5e                   	pop    esi
  409a3e:	5b                   	pop    ebx
  409a3f:	83 c4 08             	add    esp,0x8
  409a42:	c2 04 00             	ret    0x4
  409a45:	90                   	nop
  409a46:	90                   	nop
  409a47:	90                   	nop
  409a48:	90                   	nop
  409a49:	90                   	nop
  409a4a:	90                   	nop
  409a4b:	90                   	nop
  409a4c:	90                   	nop
  409a4d:	90                   	nop
  409a4e:	90                   	nop
  409a4f:	90                   	nop
  409a50:	53                   	push   ebx
  409a51:	55                   	push   ebp
  409a52:	8b 6c 24 0c          	mov    ebp,DWORD PTR [esp+0xc]
  409a56:	56                   	push   esi
  409a57:	8b f1                	mov    esi,ecx
  409a59:	57                   	push   edi
  409a5a:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  409a5d:	8b 0c a8             	mov    ecx,DWORD PTR [eax+ebp*4]
  409a60:	8b 59 6c             	mov    ebx,DWORD PTR [ecx+0x6c]
  409a63:	8b 4e 10             	mov    ecx,DWORD PTR [esi+0x10]
  409a66:	85 c9                	test   ecx,ecx
  409a68:	74 1f                	je     0x409a89
  409a6a:	6a 01                	push   0x1
  409a6c:	53                   	push   ebx
  409a6d:	e8 5e e6 ff ff       	call   0x4080d0
  409a72:	84 c0                	test   al,al
  409a74:	74 13                	je     0x409a89
  409a76:	a1 f0 8b 46 00       	mov    eax,ds:0x468bf0
  409a7b:	85 c0                	test   eax,eax
  409a7d:	75 59                	jne    0x409ad8
  409a7f:	6a 00                	push   0x0
  409a81:	e8 9a f2 ff ff       	call   0x408d20
  409a86:	83 c4 04             	add    esp,0x4
  409a89:	8b 56 1c             	mov    edx,DWORD PTR [esi+0x1c]
  409a8c:	33 ff                	xor    edi,edi
  409a8e:	42                   	inc    edx
  409a8f:	85 d2                	test   edx,edx
  409a91:	7e 61                	jle    0x409af4
  409a93:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  409a96:	8b 04 b8             	mov    eax,DWORD PTR [eax+edi*4]
  409a99:	85 c0                	test   eax,eax
  409a9b:	74 29                	je     0x409ac6
  409a9d:	3b fd                	cmp    edi,ebp
  409a9f:	74 25                	je     0x409ac6
  409aa1:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  409aa4:	8a 51 74             	mov    dl,BYTE PTR [ecx+0x74]
  409aa7:	84 d2                	test   dl,dl
  409aa9:	74 0a                	je     0x409ab5
  409aab:	8a 91 ea 00 00 00    	mov    dl,BYTE PTR [ecx+0xea]
  409ab1:	84 d2                	test   dl,dl
  409ab3:	74 11                	je     0x409ac6
  409ab5:	8b 40 6c             	mov    eax,DWORD PTR [eax+0x6c]
  409ab8:	6a 01                	push   0x1
  409aba:	53                   	push   ebx
  409abb:	8b c8                	mov    ecx,eax
  409abd:	e8 0e e6 ff ff       	call   0x4080d0
  409ac2:	84 c0                	test   al,al
  409ac4:	75 1b                	jne    0x409ae1
  409ac6:	8b 4e 1c             	mov    ecx,DWORD PTR [esi+0x1c]
  409ac9:	47                   	inc    edi
  409aca:	41                   	inc    ecx
  409acb:	3b f9                	cmp    edi,ecx
  409acd:	7c c4                	jl     0x409a93
  409acf:	5f                   	pop    edi

/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00409b10 <.text+0x8b10>:
  409b10:	00 8b 56 2c 8b 04    	add    BYTE PTR [ebx+0x48b2c56],cl
  409b16:	ba 33 d2 8b 48       	mov    edx,0x488bd233
  409b1b:	70 8a                	jo     0x409aa7
  409b1d:	91                   	xchg   ecx,eax
  409b1e:	ea 00 00 00 52 e8 08 	jmp    0x8e8:0x52000000
  409b25:	17                   	pop    ss
  409b26:	00 00                	add    BYTE PTR [eax],al
  409b28:	83 c4 04             	add    esp,0x4
  409b2b:	8b 46 2c             	mov    eax,DWORD PTR [esi+0x2c]
  409b2e:	8b 0c b8             	mov    ecx,DWORD PTR [eax+edi*4]
  409b31:	5f                   	pop    edi
  409b32:	5e                   	pop    esi
  409b33:	5d                   	pop    ebp
  409b34:	8b 51 70             	mov    edx,DWORD PTR [ecx+0x70]
  409b37:	5b                   	pop    ebx
  409b38:	8a 42 74             	mov    al,BYTE PTR [edx+0x74]
  409b3b:	84 c0                	test   al,al
  409b3d:	0f 94 c0             	sete   al
  409b40:	c2 04 00             	ret    0x4
  409b43:	90                   	nop
  409b44:	90                   	nop
  409b45:	90                   	nop
  409b46:	90                   	nop
  409b47:	90                   	nop
  409b48:	90                   	nop
  409b49:	90                   	nop
  409b4a:	90                   	nop
  409b4b:	90                   	nop
  409b4c:	90                   	nop
  409b4d:	90                   	nop
  409b4e:	90                   	nop
  409b4f:	90                   	nop
  409b50:	53                   	push   ebx
  409b51:	55                   	push   ebp
  409b52:	8b 6c 24 0c          	mov    ebp,DWORD PTR [esp+0xc]
  409b56:	56                   	push   esi
  409b57:	8b f1                	mov    esi,ecx
  409b59:	57                   	push   edi
  409b5a:	8d 7d ff             	lea    edi,[ebp-0x1]
  409b5d:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  409b60:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  409b63:	85 ff                	test   edi,edi
  409b65:	8b 54 81 04          	mov    edx,DWORD PTR [ecx+eax*4+0x4]
  409b69:	8b 5a 6c             	mov    ebx,DWORD PTR [edx+0x6c]
  409b6c:	7c 5e                	jl     0x409bcc
  409b6e:	8b 46 30             	mov    eax,DWORD PTR [esi+0x30]
  409b71:	8b 0c b8             	mov    ecx,DWORD PTR [eax+edi*4]
  409b74:	8b 51 70             	mov    edx,DWORD PTR [ecx+0x70]
  409b77:	8a 42 73             	mov    al,BYTE PTR [edx+0x73]
  409b7a:	84 c0                	test   al,al
  409b7c:	74 22                	je     0x409ba0
  409b7e:	6a 00                	push   0x0
  409b80:	6a 00                	push   0x0
  409b82:	6a 00                	push   0x0
  409b84:	6a ff                	push   0xffffffff
  409b86:	e8 05 0c 00 00       	call   0x40a790
  409b8b:	8b 46 30             	mov    eax,DWORD PTR [esi+0x30]
  409b8e:	6a 00                	push   0x0
  409b90:	53                   	push   ebx
  409b91:	8b 0c b8             	mov    ecx,DWORD PTR [eax+edi*4]
  409b94:	8b 49 6c             	mov    ecx,DWORD PTR [ecx+0x6c]
  409b97:	e8 34 e5 ff ff       	call   0x4080d0
  409b9c:	84 c0                	test   al,al
  409b9e:	75 0c                	jne    0x409bac
  409ba0:	4f                   	dec    edi
  409ba1:	79 cb                	jns    0x409b6e
  409ba3:	5f                   	pop    edi
  409ba4:	8b c5                	mov    eax,ebp
  409ba6:	5e                   	pop    esi
  409ba7:	5d                   	pop    ebp
  409ba8:	5b                   	pop    ebx
  409ba9:	c2 04 00             	ret    0x4
  409bac:	8b 56 30             	mov    edx,DWORD PTR [esi+0x30]
  409baf:	8b 0c ba             	mov    ecx,DWORD PTR [edx+edi*4]
  409bb2:	8b 41 70             	mov    eax,DWORD PTR [ecx+0x70]
  409bb5:	80 78 73 02          	cmp    BYTE PTR [eax+0x73],0x2
  409bb9:	75 0d                	jne    0x409bc8
  409bbb:	6a 00                	push   0x0
  409bbd:	6a 00                	push   0x0
  409bbf:	6a 18                	push   0x18
  409bc1:	6a 02                	push   0x2
  409bc3:	e8 c8 0b 00 00       	call   0x40a790
  409bc8:	85 ff                	test   edi,edi
  409bca:	7d 09                	jge    0x409bd5
  409bcc:	5f                   	pop    edi
  409bcd:	8b c5                	mov    eax,ebp
  409bcf:	5e                   	pop    esi
  409bd0:	5d                   	pop    ebp
  409bd1:	5b                   	pop    ebx
  409bd2:	c2 04 00             	ret    0x4
  409bd5:	8b c7                	mov    eax,edi
  409bd7:	5f                   	pop    edi
  409bd8:	5e                   	pop    esi
  409bd9:	5d                   	pop    ebp
  409bda:	5b                   	pop    ebx
  409bdb:	c2 04 00             	ret    0x4
  409bde:	90                   	nop
  409bdf:	90                   	nop
  409be0:	56                   	push   esi
  409be1:	8b f1                	mov    esi,ecx
  409be3:	8b 4e 0c             	mov    ecx,DWORD PTR [esi+0xc]
  409be6:	85 c9                	test   ecx,ecx
  409be8:	74 0c                	je     0x409bf6
  409bea:	6a 00                	push   0x0
  409bec:	e8 7f e3 ff ff       	call   0x407f70
  409bf1:	e8 6a cf ff ff       	call   0x406b60
  409bf6:	8a 46 35             	mov    al,BYTE PTR [esi+0x35]
  409bf9:	84 c0                	test   al,al
  409bfb:	74 08                	je     0x409c05
  409bfd:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  409c00:	e8 9b cc ff ff       	call   0x4068a0
  409c05:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  409c08:	57                   	push   edi
  409c09:	33 ff                	xor    edi,edi
  409c0b:	85 c0                	test   eax,eax
  409c0d:	7e 21                	jle    0x409c30
  409c0f:	8b 46 30             	mov    eax,DWORD PTR [esi+0x30]
  409c12:	8b 0c b8             	mov    ecx,DWORD PTR [eax+edi*4]
  409c15:	8b 51 70             	mov    edx,DWORD PTR [ecx+0x70]
  409c18:	8a 42 75             	mov    al,BYTE PTR [edx+0x75]
  409c1b:	84 c0                	test   al,al
  409c1d:	75 09                	jne    0x409c28
  409c1f:	8b 46 0c             	mov    eax,DWORD PTR [esi+0xc]
  409c22:	50                   	push   eax
  409c23:	e8 a8 0a 00 00       	call   0x40a6d0
  409c28:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  409c2b:	47                   	inc    edi
  409c2c:	3b f8                	cmp    edi,eax
  409c2e:	7c df                	jl     0x409c0f
  409c30:	8b 4e 0c             	mov    ecx,DWORD PTR [esi+0xc]
  409c33:	5f                   	pop    edi
  409c34:	85 c9                	test   ecx,ecx
  409c36:	74 09                	je     0x409c41
  409c38:	8b 56 04             	mov    edx,DWORD PTR [esi+0x4]
  409c3b:	52                   	push   edx
  409c3c:	e8 ff e7 ff ff       	call   0x408440
  409c41:	5e                   	pop    esi
  409c42:	c3                   	ret
  409c43:	90                   	nop
  409c44:	90                   	nop
  409c45:	90                   	nop
  409c46:	90                   	nop
  409c47:	90                   	nop
  409c48:	90                   	nop
  409c49:	90                   	nop
  409c4a:	90                   	nop
  409c4b:	90                   	nop
  409c4c:	90                   	nop
  409c4d:	90                   	nop
  409c4e:	90                   	nop
  409c4f:	90                   	nop
  409c50:	56                   	push   esi
  409c51:	8b f1                	mov    esi,ecx
  409c53:	33 c9                	xor    ecx,ecx
  409c55:	57                   	push   edi
  409c56:	8b 06                	mov    eax,DWORD PTR [esi]
  409c58:	8a 88 d8 00 00 00    	mov    cl,BYTE PTR [eax+0xd8]
  409c5e:	8b c1                	mov    eax,ecx
  409c60:	83 e8 00             	sub    eax,0x0
  409c63:	74 0d                	je     0x409c72
  409c65:	48                   	dec    eax
  409c66:	75 19                	jne    0x409c81
  409c68:	8b 4e 08             	mov    ecx,DWORD PTR [esi+0x8]
  409c6b:	e8 80 e9 ff ff       	call   0x4085f0
  409c70:	eb 0f                	jmp    0x409c81
  409c72:	8a 46 35             	mov    al,BYTE PTR [esi+0x35]
  409c75:	84 c0                	test   al,al
  409c77:	74 08                	je     0x409c81
  409c79:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  409c7c:	e8 1f cc ff ff       	call   0x4068a0
  409c81:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  409c84:	33 ff                	xor    edi,edi
  409c86:	85 c0                	test   eax,eax
  409c88:	7e 23                	jle    0x409cad
  409c8a:	8b 56 2c             	mov    edx,DWORD PTR [esi+0x2c]
  409c8d:	8b 0c ba             	mov    ecx,DWORD PTR [edx+edi*4]
  409c90:	85 c9                	test   ecx,ecx
  409c92:	74 11                	je     0x409ca5
  409c94:	8b 41 70             	mov    eax,DWORD PTR [ecx+0x70]
  409c97:	8a 50 75             	mov    dl,BYTE PTR [eax+0x75]
  409c9a:	84 d2                	test   dl,dl
  409c9c:	75 07                	jne    0x409ca5
  409c9e:	6a 00                	push   0x0
  409ca0:	e8 2b 0a 00 00       	call   0x40a6d0
  409ca5:	8b 46 1c             	mov    eax,DWORD PTR [esi+0x1c]
  409ca8:	47                   	inc    edi
  409ca9:	3b f8                	cmp    edi,eax
  409cab:	7c dd                	jl     0x409c8a
  409cad:	5f                   	pop    edi
  409cae:	5e                   	pop    esi
  409caf:	c3                   	ret
  409cb0:	8b 41 1c             	mov    eax,DWORD PTR [ecx+0x1c]
  409cb3:	56                   	push   esi
  409cb4:	33 d2                	xor    edx,edx
  409cb6:	57                   	push   edi
  409cb7:	8d 70 01             	lea    esi,[eax+0x1]
  409cba:	85 f6                	test   esi,esi
  409cbc:	7c 18                	jl     0x409cd6
  409cbe:	8b                   	.byte 0x8b
  409cbf:	49                   	dec    ecx
