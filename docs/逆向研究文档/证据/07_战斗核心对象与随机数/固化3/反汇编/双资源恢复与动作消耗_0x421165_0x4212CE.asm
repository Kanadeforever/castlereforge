; 幽城幻剑录 v0.7D阶段中-固化3 静态证据
; Oracle RPG.exe SHA256: b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; VA范围: 0x00421165..0x004212CD
; 用途: 证明两类恢复聚合、FIRTTECH+0x34/+0x38 动作资源扣除与 Article/Ail2 +0x118/+0x11C 扣除。
; 边界: 仅证明当前 Oracle 指令结构；作者变量名/业务名需额外 consumer 或动态证据。

  421165:	8b 87 68 01 00 00    	mov    eax,DWORD PTR [edi+0x168]
  42116b:	3b c5                	cmp    eax,ebp
  42116d:	7e 26                	jle    0x421195
  42116f:	8b b7 7c 08 00 00    	mov    esi,DWORD PTR [edi+0x87c]
  421175:	03 f0                	add    esi,eax
  421177:	8b 87 78 08 00 00    	mov    eax,DWORD PTR [edi+0x878]
  42117d:	8b ce                	mov    ecx,esi
  42117f:	89 b7 7c 08 00 00    	mov    DWORD PTR [edi+0x87c],esi
  421185:	3b c8                	cmp    ecx,eax
  421187:	7e 06                	jle    0x42118f
  421189:	89 87 7c 08 00 00    	mov    DWORD PTR [edi+0x87c],eax
  42118f:	89 af 68 01 00 00    	mov    DWORD PTR [edi+0x168],ebp
  421195:	8b 87 6c 01 00 00    	mov    eax,DWORD PTR [edi+0x16c]
  42119b:	3b c5                	cmp    eax,ebp
  42119d:	7e 26                	jle    0x4211c5
  42119f:	8b b7 84 08 00 00    	mov    esi,DWORD PTR [edi+0x884]
  4211a5:	03 f0                	add    esi,eax
  4211a7:	8b 87 80 08 00 00    	mov    eax,DWORD PTR [edi+0x880]
  4211ad:	8b ce                	mov    ecx,esi
  4211af:	89 b7 84 08 00 00    	mov    DWORD PTR [edi+0x884],esi
  4211b5:	3b c8                	cmp    ecx,eax
  4211b7:	7e 06                	jle    0x4211bf
  4211b9:	89 87 84 08 00 00    	mov    DWORD PTR [edi+0x884],eax
  4211bf:	89 af 6c 01 00 00    	mov    DWORD PTR [edi+0x16c],ebp
  4211c5:	39 2d 84 fd 89 00    	cmp    DWORD PTR ds:0x89fd84,ebp
  4211cb:	7e 69                	jle    0x421236
  4211cd:	55                   	push   ebp
  4211ce:	6a 03                	push   0x3
  4211d0:	8b cf                	mov    ecx,edi
  4211d2:	e8 d9 1e 00 00       	call   0x4230b0
  4211d7:	8b f0                	mov    esi,eax
  4211d9:	3b f5                	cmp    esi,ebp
  4211db:	7c 59                	jl     0x421236
  4211dd:	8b 1d 84 fd 89 00    	mov    ebx,DWORD PTR ds:0x89fd84
  4211e3:	b8 77 77 77 77       	mov    eax,0x77777777
  4211e8:	f7 eb                	imul   ebx
  4211ea:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  4211f1:	2b d3                	sub    edx,ebx
  4211f3:	c1 fa 03             	sar    edx,0x3
  4211f6:	8b 99 84 08 00 00    	mov    ebx,DWORD PTR [ecx+0x884]
  4211fc:	8b c2                	mov    eax,edx
  4211fe:	c1 e8 1f             	shr    eax,0x1f
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
  42124c:	8b 97 84 08 00 00    	mov    edx,DWORD PTR [edi+0x884]
  421252:	2b d1                	sub    edx,ecx
  421254:	89 97 84 08 00 00    	mov    DWORD PTR [edi+0x884],edx
  42125a:	8b 08                	mov    ecx,DWORD PTR [eax]
  42125c:	81 f9 2f 01 00 00    	cmp    ecx,0x12f
  421262:	74 57                	je     0x4212bb
  421264:	8b 50 34             	mov    edx,DWORD PTR [eax+0x34]
  421267:	8b 87 7c 08 00 00    	mov    eax,DWORD PTR [edi+0x87c]
  42126d:	2b c2                	sub    eax,edx
  42126f:	6a 03                	push   0x3
  421271:	8b cf                	mov    ecx,edi
  421273:	89 87 7c 08 00 00    	mov    DWORD PTR [edi+0x87c],eax
  421279:	e8 82 07 00 00       	call   0x421a00
  42127e:	5f                   	pop    edi
  42127f:	5e                   	pop    esi
  421280:	5d                   	pop    ebp
  421281:	5b                   	pop    ebx
  421282:	83 c4 10             	add    esp,0x10
  421285:	c2 04 00             	ret    0x4
  421288:	8b 87 f4 0c 00 00    	mov    eax,DWORD PTR [edi+0xcf4]
  42128e:	3b c5                	cmp    eax,ebp
  421290:	74 29                	je     0x4212bb
  421292:	83 bf bc 01 00 00 ff 	cmp    DWORD PTR [edi+0x1bc],0xffffffff
  421299:	75 20                	jne    0x4212bb
  42129b:	8b 88 18 01 00 00    	mov    ecx,DWORD PTR [eax+0x118]
  4212a1:	8b 97 7c 08 00 00    	mov    edx,DWORD PTR [edi+0x87c]
  4212a7:	2b d1                	sub    edx,ecx
  4212a9:	89 97 7c 08 00 00    	mov    DWORD PTR [edi+0x87c],edx
  4212af:	8b 90 1c 01 00 00    	mov    edx,DWORD PTR [eax+0x11c]
  4212b5:	29 97 84 08 00 00    	sub    DWORD PTR [edi+0x884],edx
  4212bb:	6a 03                	push   0x3
  4212bd:	8b cf                	mov    ecx,edi
  4212bf:	e8 3c 07 00 00       	call   0x421a00
  4212c4:	5f                   	pop    edi
  4212c5:	5e                   	pop    esi
  4212c6:	5d                   	pop    ebp
  4212c7:	5b                   	pop    ebx
  4212c8:	83 c4 10             	add    esp,0x10
  4212cb:	c2 04 00             	ret    0x4
