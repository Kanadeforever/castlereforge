; 幽城幻剑录 v0.7D阶段中-固化3 静态证据
; Oracle RPG.exe SHA256: b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; VA范围: 0x004231F0..0x004232C3
; 用途: 证明主资源<=0 时的失效/移出战斗类状态转移：+0x72C=-1、主资源清零、关联对象禁用。
; 边界: 仅证明当前 Oracle 指令结构；作者变量名/业务名需额外 consumer 或动态证据。

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
