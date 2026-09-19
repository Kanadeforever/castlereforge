; v0.7D阶段中-固化3 D6 两条五槽调用路径实参来源
; Oracle RPG.exe SHA256: b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; 证据边界: 结合已闭合的 0x42A7D0 第4参数=FightRole+0xCF4=Ail2 record* 与 0x42C680 第4参数=FIRTTECH record*；本文件冻结 push/arg 路径，不赋予 effect ID 业务名。

; ---- Article/Ail2 processor入口与第4参数保存: 0x0042A7D0..0x0042A8D4 ----

/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042a7d0 <.text+0x297d0>:
  42a7d0:	83 ec 10             	sub    esp,0x10
  42a7d3:	53                   	push   ebx
  42a7d4:	55                   	push   ebp
  42a7d5:	56                   	push   esi
  42a7d6:	57                   	push   edi
  42a7d7:	8b 7c 24 2c          	mov    edi,DWORD PTR [esp+0x2c]
  42a7db:	8b e9                	mov    ebp,ecx
  42a7dd:	33 c0                	xor    eax,eax
  42a7df:	33 c9                	xor    ecx,ecx
  42a7e1:	8d 57 10             	lea    edx,[edi+0x10]
  42a7e4:	c6 07 00             	mov    BYTE PTR [edi],0x0
  42a7e7:	89 47 04             	mov    DWORD PTR [edi+0x4],eax
  42a7ea:	89 47 08             	mov    DWORD PTR [edi+0x8],eax
  42a7ed:	88 47 0c             	mov    BYTE PTR [edi+0xc],al
  42a7f0:	89 0a                	mov    DWORD PTR [edx],ecx
  42a7f2:	89 6c 24 18          	mov    DWORD PTR [esp+0x18],ebp
  42a7f6:	89 4a 04             	mov    DWORD PTR [edx+0x4],ecx
  42a7f9:	89 4a 08             	mov    DWORD PTR [edx+0x8],ecx
  42a7fc:	89 4a 0c             	mov    DWORD PTR [edx+0xc],ecx
  42a7ff:	89 4a 10             	mov    DWORD PTR [edx+0x10],ecx
  42a802:	8d 57 24             	lea    edx,[edi+0x24]
  42a805:	89 4f 24             	mov    DWORD PTR [edi+0x24],ecx
  42a808:	89 4a 04             	mov    DWORD PTR [edx+0x4],ecx
  42a80b:	89 4a 08             	mov    DWORD PTR [edx+0x8],ecx
  42a80e:	89 4a 0c             	mov    DWORD PTR [edx+0xc],ecx
  42a811:	89 4a 10             	mov    DWORD PTR [edx+0x10],ecx
  42a814:	88 4f 40             	mov    BYTE PTR [edi+0x40],cl
  42a817:	88 4f 48             	mov    BYTE PTR [edi+0x48],cl
  42a81a:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42a81e:	3b c8                	cmp    ecx,eax
  42a820:	89 47 38             	mov    DWORD PTR [edi+0x38],eax
  42a823:	89 47 3c             	mov    DWORD PTR [edi+0x3c],eax
  42a826:	c7 47 44 01 00 00 00 	mov    DWORD PTR [edi+0x44],0x1
  42a82d:	c7 47 4c ff ff ff ff 	mov    DWORD PTR [edi+0x4c],0xffffffff
  42a834:	75 1f                	jne    0x42a855
  42a836:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  42a83c:	68 30 a8 46 00       	push   0x46a830
  42a841:	68 0c a8 46 00       	push   0x46a80c
  42a846:	e8 05 78 00 00       	call   0x432050
  42a84b:	5f                   	pop    edi
  42a84c:	5e                   	pop    esi
  42a84d:	5d                   	pop    ebp
  42a84e:	5b                   	pop    ebx
  42a84f:	83 c4 10             	add    esp,0x10
  42a852:	c2 14 00             	ret    0x14
  42a855:	8b 74 24 28          	mov    esi,DWORD PTR [esp+0x28]
  42a859:	3b f0                	cmp    esi,eax
  42a85b:	75 1f                	jne    0x42a87c
  42a85d:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  42a863:	68 30 a8 46 00       	push   0x46a830
  42a868:	68 e8 a7 46 00       	push   0x46a7e8
  42a86d:	e8 de 77 00 00       	call   0x432050
  42a872:	5f                   	pop    edi
  42a873:	5e                   	pop    esi
  42a874:	5d                   	pop    ebp
  42a875:	5b                   	pop    ebx
  42a876:	83 c4 10             	add    esp,0x10
  42a879:	c2 14 00             	ret    0x14
  42a87c:	3b f8                	cmp    edi,eax
  42a87e:	75 1f                	jne    0x42a89f
  42a880:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  42a886:	68 30 a8 46 00       	push   0x46a830
  42a88b:	68 c4 a7 46 00       	push   0x46a7c4
  42a890:	e8 bb 77 00 00       	call   0x432050
  42a895:	5f                   	pop    edi
  42a896:	5e                   	pop    esi
  42a897:	5d                   	pop    ebp
  42a898:	5b                   	pop    ebx
  42a899:	83 c4 10             	add    esp,0x10
  42a89c:	c2 14 00             	ret    0x14
  42a89f:	8b 5c 24 30          	mov    ebx,DWORD PTR [esp+0x30]
  42a8a3:	3b d8                	cmp    ebx,eax
  42a8a5:	75 1f                	jne    0x42a8c6
  42a8a7:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  42a8ad:	68 30 a8 46 00       	push   0x46a830
  42a8b2:	68 a0 a7 46 00       	push   0x46a7a0
  42a8b7:	e8 94 77 00 00       	call   0x432050
  42a8bc:	5f                   	pop    edi
  42a8bd:	5e                   	pop    esi
  42a8be:	5d                   	pop    ebp
  42a8bf:	5b                   	pop    ebx
  42a8c0:	83 c4 10             	add    esp,0x10
  42a8c3:	c2 14 00             	ret    0x14
  42a8c6:	56                   	push   esi
  42a8c7:	8b cd                	mov    ecx,ebp
  42a8c9:	89 44 24 2c          	mov    DWORD PTR [esp+0x2c],eax
  42a8cd:	e8 fe 11 00 00       	call   0x42bad0
  42a8d2:	8b                   	.byte 0x8b
  42a8d3:	8b c4                	mov    eax,esp


; ---- Article路径调用0x42B0D0: 0x0042B014..0x0042B06E ----

/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042b014 <.text+0x2a014>:
  42b014:	8b 83 d4 00 00 00    	mov    eax,DWORD PTR [ebx+0xd4]
  42b01a:	83 f8 0a             	cmp    eax,0xa
  42b01d:	74 23                	je     0x42b042
  42b01f:	83 f8 0b             	cmp    eax,0xb
  42b022:	74 1e                	je     0x42b042
  42b024:	8b 83 c8 00 00 00    	mov    eax,DWORD PTR [ebx+0xc8]
  42b02a:	83 f8 02             	cmp    eax,0x2
  42b02d:	74 13                	je     0x42b042
  42b02f:	83 f8 03             	cmp    eax,0x3
  42b032:	74 0e                	je     0x42b042
  42b034:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
  42b038:	6a 00                	push   0x0
  42b03a:	57                   	push   edi
  42b03b:	53                   	push   ebx
  42b03c:	6a 00                	push   0x0
  42b03e:	56                   	push   esi
  42b03f:	52                   	push   edx
  42b040:	eb 0c                	jmp    0x42b04e
  42b042:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  42b046:	6a 01                	push   0x1
  42b048:	57                   	push   edi
  42b049:	53                   	push   ebx
  42b04a:	6a 00                	push   0x0
  42b04c:	56                   	push   esi
  42b04d:	50                   	push   eax
  42b04e:	8b cd                	mov    ecx,ebp
  42b050:	e8 7b 00 00 00       	call   0x42b0d0
  42b055:	81 7b 20 08 01 00 00 	cmp    DWORD PTR [ebx+0x20],0x108
  42b05c:	75 10                	jne    0x42b06e
  42b05e:	8b 4e 2c             	mov    ecx,DWORD PTR [esi+0x2c]
  42b061:	f7 d9                	neg    ecx
  42b063:	89 4f 04             	mov    DWORD PTR [edi+0x4],ecx
  42b066:	8b 56 34             	mov    edx,DWORD PTR [esi+0x34]
  42b069:	f7 da                	neg    edx
  42b06b:	89 57 08             	mov    DWORD PTR [edi+0x8],edx
  42b06e:	8b                   	.byte 0x8b


; ---- FIRTTECH路径调用0x42B0D0: 0x0042CBB4..0x0042CBFF ----

/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042cbb4 <.text+0x2bbb4>:
  42cbb4:	8b 47 3c             	mov    eax,DWORD PTR [edi+0x3c]
  42cbb7:	83 f8 0a             	cmp    eax,0xa
  42cbba:	74 1f                	je     0x42cbdb
  42cbbc:	83 f8 0b             	cmp    eax,0xb
  42cbbf:	74 1a                	je     0x42cbdb
  42cbc1:	8b 47 44             	mov    eax,DWORD PTR [edi+0x44]
  42cbc4:	83 f8 02             	cmp    eax,0x2
  42cbc7:	74 12                	je     0x42cbdb
  42cbc9:	83 f8 03             	cmp    eax,0x3
  42cbcc:	74 0d                	je     0x42cbdb
  42cbce:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42cbd2:	6a 00                	push   0x0
  42cbd4:	56                   	push   esi
  42cbd5:	6a 00                	push   0x0
  42cbd7:	57                   	push   edi
  42cbd8:	51                   	push   ecx
  42cbd9:	eb 0b                	jmp    0x42cbe6
  42cbdb:	8b 54 24 24          	mov    edx,DWORD PTR [esp+0x24]
  42cbdf:	6a 01                	push   0x1
  42cbe1:	56                   	push   esi
  42cbe2:	6a 00                	push   0x0
  42cbe4:	57                   	push   edi
  42cbe5:	52                   	push   edx
  42cbe6:	53                   	push   ebx
  42cbe7:	8b cd                	mov    ecx,ebp
  42cbe9:	e8 e2 e4 ff ff       	call   0x42b0d0
  42cbee:	8b 07                	mov    eax,DWORD PTR [edi]
  42cbf0:	3d 97 01 00 00       	cmp    eax,0x197
  42cbf5:	74 07                	je     0x42cbfe
  42cbf7:	3d d0 01 00 00       	cmp    eax,0x1d0
  42cbfc:	75 0f                	jne    0x42cc0d
  42cbfe:	8b                   	.byte 0x8b
  42cbff:	44                   	inc    esp
