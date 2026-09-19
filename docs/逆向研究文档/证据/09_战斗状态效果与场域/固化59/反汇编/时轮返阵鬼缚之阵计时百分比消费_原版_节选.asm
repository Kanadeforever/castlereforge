; ===== slot7/8角色计时消费 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

004204c6 <.text+0x1f4c6>:
  4204c6:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  4204cc:	bf 64 00 00 00       	mov    edi,0x64
  4204d1:	83 f8 01             	cmp    eax,0x1
  4204d4:	75 10                	jne    0x4204e6
  4204d6:	6a 07                	push   0x7
  4204d8:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  4204dd:	e8 6e d5 01 00       	call   0x43da50
  4204e2:	84 c0                	test   al,al
  4204e4:	75 1a                	jne    0x420500
  4204e6:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  4204ec:	85 c0                	test   eax,eax
  4204ee:	75 15                	jne    0x420505
  4204f0:	6a 07                	push   0x7
  4204f2:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  4204f7:	e8 54 d5 01 00       	call   0x43da50
  4204fc:	84 c0                	test   al,al
  4204fe:	74 05                	je     0x420505
  420500:	bf c8 00 00 00       	mov    edi,0xc8
  420505:	83 be 50 08 00 00 01 	cmp    DWORD PTR [esi+0x850],0x1
  42050c:	75 10                	jne    0x42051e
  42050e:	6a 08                	push   0x8
  420510:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  420515:	e8 36 d5 01 00       	call   0x43da50
  42051a:	84 c0                	test   al,al
  42051c:	75 1a                	jne    0x420538
  42051e:	8b 86 50 08 00 00    	mov    eax,DWORD PTR [esi+0x850]
  420524:	85 c0                	test   eax,eax
  420526:	75 2b                	jne    0x420553
  420528:	6a 08                	push   0x8
  42052a:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  42052f:	e8 1c d5 01 00       	call   0x43da50
  420534:	84 c0                	test   al,al
  420536:	74 1b                	je     0x420553
  420538:	8d 04 bf             	lea    eax,[edi+edi*4]
  42053b:	8d 0c 80             	lea    ecx,[eax+eax*4]
  42053e:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  420543:	d1 e1                	shl    ecx,1
  420545:	f7 e9                	imul   ecx
  420547:	c1 fa 05             	sar    edx,0x5
  42054a:	8b ca                	mov    ecx,edx
  42054c:	c1 e9 1f             	shr    ecx,0x1f
  42054f:	03 d1                	add    edx,ecx
  420551:	8b fa                	mov    edi,edx
  420553:	8b 86 38 0a 00 00    	mov    eax,DWORD PTR [esi+0xa38]
  420559:	85 c0                	test   eax,eax
  42055b:	7e 13                	jle    0x420570
  42055d:	b8 67 66 66 66       	mov    eax,0x66666667
  420562:	f7 ef                	imul   edi
  420564:	c1 fa 02             	sar    edx,0x2
  420567:	8b c2                	mov    eax,edx
  420569:	c1 e8 1f             	shr    eax,0x1f
  42056c:	03 d0                	add    edx,eax
  42056e:	8b fa                	mov    edi,edx
  420570:	8b 86 74 0a 00 00    	mov    eax,DWORD PTR [esi+0xa74]
  420576:	85 c0                	test   eax,eax
  420578:	7e 1c                	jle    0x420596
  42057a:	8d 04 7f             	lea    eax,[edi+edi*2]
  42057d:	8d 04 80             	lea    eax,[eax+eax*4]
  420580:	8d 0c 80             	lea    ecx,[eax+eax*4]
  420583:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  420588:	f7 e9                	imul   ecx
  42058a:	c1 fa 05             	sar    edx,0x5
  42058d:	8b ca                	mov    ecx,edx
  42058f:	c1 e9 1f             	shr    ecx,0x1f
  420592:	03 d1                	add    edx,ecx
  420594:	8b fa                	mov    edi,edx
  420596:	8b 86 44 0a 00 00    	mov    eax,DWORD PTR [esi+0xa44]
  42059c:	85 c0                	test   eax,eax
  42059e:	7e 1b                	jle    0x4205bb
  4205a0:	8d 04 bf             	lea    eax,[edi+edi*4]
  4205a3:	8d 0c 80             	lea    ecx,[eax+eax*4]
  4205a6:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  4205ab:	d1 e1                	shl    ecx,1
  4205ad:	f7 e9                	imul   ecx
  4205af:	c1 fa 05             	sar    edx,0x5
  4205b2:	8b c2                	mov    eax,edx
  4205b4:	c1 e8 1f             	shr    eax,0x1f
  4205b7:	03 d0                	add    edx,eax
  4205b9:	8b fa                	mov    edi,edx
  4205bb:	8b 86 90 0a 00 00    	mov    eax,DWORD PTR [esi+0xa90]
  4205c1:	85 c0                	test   eax,eax
  4205c3:	7e 1e                	jle    0x4205e3
  4205c5:	8d 04 7f             	lea    eax,[edi+edi*2]
  4205c8:	8d 04 80             	lea    eax,[eax+eax*4]
  4205cb:	8d 0c 80             	lea    ecx,[eax+eax*4]
  4205ce:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  4205d3:	d1 e1                	shl    ecx,1
  4205d5:	f7 e9                	imul   ecx
  4205d7:	c1 fa 05             	sar    edx,0x5
  4205da:	8b ca                	mov    ecx,edx
  4205dc:	c1 e9 1f             	shr    ecx,0x1f
  4205df:	03 d1                	add    edx,ecx
  4205e1:	8b fa                	mov    edi,edx
  4205e3:	8b 86 58 0a 00 00    	mov    eax,DWORD PTR [esi+0xa58]
  4205e9:	85 c0                	test   eax,eax
  4205eb:	7e 04                	jle    0x4205f1
  4205ed:	33 ff                	xor    edi,edi
  4205ef:	eb 0d                	jmp    0x4205fe
  4205f1:	81 ff c8 00 00 00    	cmp    edi,0xc8
  4205f7:	7e 05                	jle    0x4205fe
  4205f9:	bf c8 00 00 00       	mov    edi,0xc8
  4205fe:	53                   	push   ebx
  4205ff:	8d 9e a8 0c 00 00    	lea    ebx,[esi+0xca8]
  420605:	6a 01                	push   0x1
  420607:	57                   	push   edi
  420608:	8b cb                	mov    ecx,ebx
  42060a:	e8 81 3a 00 00       	call   0x424090
  42060f:	6a 02                	push   0x2
  420611:	57                   	push   edi
  420612:	8b cb                	mov    ecx,ebx
  420614:	e8 77 3a 00 00       	call   0x424090
; ===== 计时器结构初始化与更新 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00423da0 <.text+0x22da0>:
  423da0:	33 c0                	xor    eax,eax
  423da2:	ba 64 00 00 00       	mov    edx,0x64
  423da7:	89 41 04             	mov    DWORD PTR [ecx+0x4],eax
  423daa:	89 41 08             	mov    DWORD PTR [ecx+0x8],eax
  423dad:	89 41 0c             	mov    DWORD PTR [ecx+0xc],eax
  423db0:	89 41 10             	mov    DWORD PTR [ecx+0x10],eax
  423db3:	89 41 14             	mov    DWORD PTR [ecx+0x14],eax
  423db6:	89 41 18             	mov    DWORD PTR [ecx+0x18],eax
  423db9:	89 41 1c             	mov    DWORD PTR [ecx+0x1c],eax
  423dbc:	89 41 20             	mov    DWORD PTR [ecx+0x20],eax
  423dbf:	89 41 24             	mov    DWORD PTR [ecx+0x24],eax
  423dc2:	89 41 28             	mov    DWORD PTR [ecx+0x28],eax
  423dc5:	89 51 2c             	mov    DWORD PTR [ecx+0x2c],edx
  423dc8:	89 51 30             	mov    DWORD PTR [ecx+0x30],edx
  423dcb:	89 51 34             	mov    DWORD PTR [ecx+0x34],edx
  423dce:	88 01                	mov    BYTE PTR [ecx],al
  423dd0:	c6 41 01 01          	mov    BYTE PTR [ecx+0x1],0x1
  423dd4:	c3                   	ret
  423dd5:	90                   	nop
  423dd6:	90                   	nop
  423dd7:	90                   	nop
  423dd8:	90                   	nop
  423dd9:	90                   	nop
  423dda:	90                   	nop
  423ddb:	90                   	nop
  423ddc:	90                   	nop
  423ddd:	90                   	nop
  423dde:	90                   	nop
  423ddf:	90                   	nop
  423de0:	8a 01                	mov    al,BYTE PTR [ecx]
  423de2:	56                   	push   esi
  423de3:	84 c0                	test   al,al
  423de5:	75 04                	jne    0x423deb
  423de7:	33 c0                	xor    eax,eax
  423de9:	5e                   	pop    esi
  423dea:	c3                   	ret
  423deb:	8b 41 28             	mov    eax,DWORD PTR [ecx+0x28]
  423dee:	48                   	dec    eax
  423def:	74 32                	je     0x423e23
  423df1:	48                   	dec    eax
  423df2:	74 1b                	je     0x423e0f
  423df4:	48                   	dec    eax
  423df5:	74 04                	je     0x423dfb
  423df7:	33 c0                	xor    eax,eax
  423df9:	5e                   	pop    esi
  423dfa:	c3                   	ret
  423dfb:	8b 41 1c             	mov    eax,DWORD PTR [ecx+0x1c]
  423dfe:	8b 71 24             	mov    esi,DWORD PTR [ecx+0x24]
  423e01:	33 d2                	xor    edx,edx
  423e03:	3b c6                	cmp    eax,esi
  423e05:	0f 9e c2             	setle  dl
  423e08:	83 c2 05             	add    edx,0x5
  423e0b:	5e                   	pop    esi
  423e0c:	8b c2                	mov    eax,edx
  423e0e:	c3                   	ret
  423e0f:	8b 41 10             	mov    eax,DWORD PTR [ecx+0x10]
  423e12:	8b 71 18             	mov    esi,DWORD PTR [ecx+0x18]
  423e15:	33 d2                	xor    edx,edx
  423e17:	3b c6                	cmp    eax,esi
  423e19:	0f 9e c2             	setle  dl
  423e1c:	83 c2 03             	add    edx,0x3
  423e1f:	5e                   	pop    esi
  423e20:	8b c2                	mov    eax,edx
  423e22:	c3                   	ret
  423e23:	8b 41 04             	mov    eax,DWORD PTR [ecx+0x4]
  423e26:	8b 71 0c             	mov    esi,DWORD PTR [ecx+0xc]
  423e29:	33 d2                	xor    edx,edx
  423e2b:	3b c6                	cmp    eax,esi
  423e2d:	0f 9e c2             	setle  dl
  423e30:	42                   	inc    edx
  423e31:	5e                   	pop    esi
  423e32:	8b c2                	mov    eax,edx
  423e34:	c3                   	ret
  423e35:	90                   	nop
  423e36:	90                   	nop
  423e37:	90                   	nop
  423e38:	90                   	nop
  423e39:	90                   	nop
  423e3a:	90                   	nop
  423e3b:	90                   	nop
  423e3c:	90                   	nop
  423e3d:	90                   	nop
  423e3e:	90                   	nop
  423e3f:	90                   	nop
  423e40:	c6 01 01             	mov    BYTE PTR [ecx],0x1
  423e43:	c3                   	ret
  423e44:	90                   	nop
  423e45:	90                   	nop
  423e46:	90                   	nop
  423e47:	90                   	nop
  423e48:	90                   	nop
  423e49:	90                   	nop
  423e4a:	90                   	nop
  423e4b:	90                   	nop
  423e4c:	90                   	nop
  423e4d:	90                   	nop
  423e4e:	90                   	nop
  423e4f:	90                   	nop
  423e50:	33 c0                	xor    eax,eax
  423e52:	89 41 0c             	mov    DWORD PTR [ecx+0xc],eax
  423e55:	89 41 18             	mov    DWORD PTR [ecx+0x18],eax
  423e58:	89 41 24             	mov    DWORD PTR [ecx+0x24],eax
  423e5b:	c3                   	ret
  423e5c:	90                   	nop
  423e5d:	90                   	nop
  423e5e:	90                   	nop
  423e5f:	90                   	nop
  423e60:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  423e64:	48                   	dec    eax
  423e65:	74 1a                	je     0x423e81
  423e67:	48                   	dec    eax
  423e68:	74 0d                	je     0x423e77
  423e6a:	48                   	dec    eax
  423e6b:	75 1b                	jne    0x423e88
  423e6d:	c7 41 24 00 00 00 00 	mov    DWORD PTR [ecx+0x24],0x0
  423e74:	c2 04 00             	ret    0x4
  423e77:	c7 41 18 00 00 00 00 	mov    DWORD PTR [ecx+0x18],0x0
  423e7e:	c2 04 00             	ret    0x4
  423e81:	c7 41 0c 00 00 00 00 	mov    DWORD PTR [ecx+0xc],0x0
  423e88:	c2 04 00             	ret    0x4
  423e8b:	90                   	nop
  423e8c:	90                   	nop
  423e8d:	90                   	nop
  423e8e:	90                   	nop
  423e8f:	90                   	nop
  423e90:	80 39 00             	cmp    BYTE PTR [ecx],0x0
  423e93:	75 03                	jne    0x423e98
  423e95:	33 c0                	xor    eax,eax
  423e97:	c3                   	ret
  423e98:	8a 41 01             	mov    al,BYTE PTR [ecx+0x1]
  423e9b:	56                   	push   esi
  423e9c:	84 c0                	test   al,al
  423e9e:	0f 84 93 00 00 00    	je     0x423f37
  423ea4:	8b 41 28             	mov    eax,DWORD PTR [ecx+0x28]
  423ea7:	48                   	dec    eax
  423ea8:	74 66                	je     0x423f10
  423eaa:	48                   	dec    eax
  423eab:	74 35                	je     0x423ee2
  423ead:	48                   	dec    eax
  423eae:	0f 85 83 00 00 00    	jne    0x423f37
  423eb4:	8b 71 24             	mov    esi,DWORD PTR [ecx+0x24]
  423eb7:	8b 41 1c             	mov    eax,DWORD PTR [ecx+0x1c]
  423eba:	3b f0                	cmp    esi,eax
  423ebc:	7d 79                	jge    0x423f37
  423ebe:	8b 51 34             	mov    edx,DWORD PTR [ecx+0x34]
  423ec1:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  423ec6:	0f af 51 20          	imul   edx,DWORD PTR [ecx+0x20]
  423eca:	f7 ea                	imul   edx
  423ecc:	c1 fa 05             	sar    edx,0x5
  423ecf:	8b c2                	mov    eax,edx
  423ed1:	c1 e8 1f             	shr    eax,0x1f
  423ed4:	03 d0                	add    edx,eax
  423ed6:	03 d6                	add    edx,esi
  423ed8:	89 51 24             	mov    DWORD PTR [ecx+0x24],edx
  423edb:	e8 00 ff ff ff       	call   0x423de0
  423ee0:	5e                   	pop    esi
  423ee1:	c3                   	ret
  423ee2:	8b 71 18             	mov    esi,DWORD PTR [ecx+0x18]
  423ee5:	8b 41 10             	mov    eax,DWORD PTR [ecx+0x10]
  423ee8:	3b f0                	cmp    esi,eax
  423eea:	7d 4b                	jge    0x423f37
  423eec:	8b 51 30             	mov    edx,DWORD PTR [ecx+0x30]
  423eef:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  423ef4:	0f af 51 14          	imul   edx,DWORD PTR [ecx+0x14]
  423ef8:	f7 ea                	imul   edx
  423efa:	c1 fa 05             	sar    edx,0x5
  423efd:	8b c2                	mov    eax,edx
  423eff:	c1 e8 1f             	shr    eax,0x1f
  423f02:	03 d0                	add    edx,eax
  423f04:	03 d6                	add    edx,esi
  423f06:	89 51 18             	mov    DWORD PTR [ecx+0x18],edx
  423f09:	e8 d2 fe ff ff       	call   0x423de0
  423f0e:	5e                   	pop    esi
  423f0f:	c3                   	ret
  423f10:	8b 71 0c             	mov    esi,DWORD PTR [ecx+0xc]
  423f13:	8b 41 04             	mov    eax,DWORD PTR [ecx+0x4]
  423f16:	3b f0                	cmp    esi,eax
  423f18:	7d 1d                	jge    0x423f37
  423f1a:	8b 51 2c             	mov    edx,DWORD PTR [ecx+0x2c]
  423f1d:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  423f22:	0f af 51 08          	imul   edx,DWORD PTR [ecx+0x8]
  423f26:	f7 ea                	imul   edx
  423f28:	c1 fa 05             	sar    edx,0x5
  423f2b:	8b c2                	mov    eax,edx
  423f2d:	c1 e8 1f             	shr    eax,0x1f
  423f30:	03 d0                	add    edx,eax
  423f32:	03 d6                	add    edx,esi
  423f34:	89 51 0c             	mov    DWORD PTR [ecx+0xc],edx
  423f37:	e8 a4 fe ff ff       	call   0x423de0
  423f3c:	5e                   	pop    esi
  423f3d:	c3                   	ret
; ===== 百分比参数写入 =====

/mnt/data/solid58_inputs/original/RPG.exe:     file format pei-i386


Disassembly of section .text:

00424090 <.text+0x23090>:
  424090:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  424094:	48                   	dec    eax
  424095:	74 1a                	je     0x4240b1
  424097:	48                   	dec    eax
  424098:	74 0d                	je     0x4240a7
  42409a:	48                   	dec    eax
  42409b:	75 1b                	jne    0x4240b8
  42409d:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4240a1:	89 41 34             	mov    DWORD PTR [ecx+0x34],eax
  4240a4:	c2 08 00             	ret    0x8
  4240a7:	8b 54 24 04          	mov    edx,DWORD PTR [esp+0x4]
  4240ab:	89 51 30             	mov    DWORD PTR [ecx+0x30],edx
  4240ae:	c2 08 00             	ret    0x8
  4240b1:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4240b5:	89 41 2c             	mov    DWORD PTR [ecx+0x2c],eax
  4240b8:	c2                   	.byte 0xc2
