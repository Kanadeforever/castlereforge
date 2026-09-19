; 幽城幻剑录 v0.7D阶段中-固化3 静态证据
; Oracle RPG.exe SHA256: b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; VA范围: 0x0040E6C0..0x0040E71D
; 用途: EVE opcode 0x7C 路径把两项参数交给 0x43B8F0 的队伍资源调整模式。
; 边界: 结构语义可冻结；作者资源术语仍需字符串/UI/其他直接证据。

  40e6c0:	83 ec 10             	sub    esp,0x10
  40e6c3:	56                   	push   esi
  40e6c4:	8b f1                	mov    esi,ecx
  40e6c6:	83 ec 10             	sub    esp,0x10
  40e6c9:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  40e6cc:	8b d4                	mov    edx,esp
  40e6ce:	89 46 08             	mov    DWORD PTR [esi+0x8],eax
  40e6d1:	6a 01                	push   0x1
  40e6d3:	8b 48 04             	mov    ecx,DWORD PTR [eax+0x4]
  40e6d6:	8b 40 08             	mov    eax,DWORD PTR [eax+0x8]
  40e6d9:	89 0a                	mov    DWORD PTR [edx],ecx
  40e6db:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  40e6df:	89 42 04             	mov    DWORD PTR [edx+0x4],eax
  40e6e2:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
  40e6e6:	89 42 08             	mov    DWORD PTR [edx+0x8],eax
  40e6e9:	89 4a 0c             	mov    DWORD PTR [edx+0xc],ecx
  40e6ec:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  40e6f2:	e8 f9 d1 02 00       	call   0x43b8f0
  40e6f7:	8b 15 10 f8 89 00    	mov    edx,DWORD PTR ds:0x89f810
  40e6fd:	56                   	push   esi
  40e6fe:	42                   	inc    edx
  40e6ff:	c7 05 18 f8 89 00 00 	mov    DWORD PTR ds:0x89f818,0x0
  40e706:	00 00 00 
  40e709:	89 15 10 f8 89 00    	mov    DWORD PTR ds:0x89f810,edx
  40e70f:	e8 3c 2e 04 00       	call   0x451550
  40e714:	83 c4 04             	add    esp,0x4
  40e717:	33 c0                	xor    eax,eax
  40e719:	5e                   	pop    esi
  40e71a:	83 c4 10             	add    esp,0x10
  40e71d:	c3                   	ret
