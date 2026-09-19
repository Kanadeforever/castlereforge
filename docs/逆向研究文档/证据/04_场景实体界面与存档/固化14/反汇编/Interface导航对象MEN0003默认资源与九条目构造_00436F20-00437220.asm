
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00436f20 <.text+0x35f20>:
  436f20:	6a ff                	push   0xffffffff
  436f22:	68 1b f0 45 00       	push   0x45f01b
  436f27:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  436f2d:	50                   	push   eax
  436f2e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  436f35:	83 ec 20             	sub    esp,0x20
  436f38:	53                   	push   ebx
  436f39:	55                   	push   ebp
  436f3a:	56                   	push   esi
  436f3b:	8b f1                	mov    esi,ecx
  436f3d:	57                   	push   edi
  436f3e:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  436f42:	e8 c9 9f ff ff       	call   0x430f10
  436f47:	33 db                	xor    ebx,ebx
  436f49:	8b ce                	mov    ecx,esi
  436f4b:	53                   	push   ebx
  436f4c:	68 e4 84 1e 00       	push   0x1e84e4
  436f51:	56                   	push   esi
  436f52:	89 5c 24 44          	mov    DWORD PTR [esp+0x44],ebx
  436f56:	c7 06 44 0e 46 00    	mov    DWORD PTR [esi],0x460e44
  436f5c:	e8 cf d4 ff ff       	call   0x434430
  436f61:	8d 8e 18 02 00 00    	lea    ecx,[esi+0x218]
  436f67:	e8 c4 dd fc ff       	call   0x404d30
  436f6c:	68 bc b0 46 00       	push   0x46b0bc
  436f71:	53                   	push   ebx
  436f72:	53                   	push   ebx
  436f73:	8d be 68 05 00 00    	lea    edi,[esi+0x568]
  436f79:	8d ae 6c 05 00 00    	lea    ebp,[esi+0x56c]
  436f7f:	6a 69                	push   0x69
  436f81:	6a 41                	push   0x41
  436f83:	8b ce                	mov    ecx,esi
  436f85:	89 44 24 40          	mov    DWORD PTR [esp+0x40],eax
  436f89:	89 7c 24 2c          	mov    DWORD PTR [esp+0x2c],edi
  436f8d:	89 6c 24 30          	mov    DWORD PTR [esp+0x30],ebp
  436f91:	e8 4a a1 ff ff       	call   0x4310e0
  436f96:	8b 8e 64 05 00 00    	mov    ecx,DWORD PTR [esi+0x564]
  436f9c:	53                   	push   ebx
  436f9d:	e8 4e 77 00 00       	call   0x43e6f0
  436fa2:	6a 48                	push   0x48
  436fa4:	e8 b6 a6 01 00       	call   0x45165f
  436fa9:	83 c4 04             	add    esp,0x4
  436fac:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  436fb0:	3b c3                	cmp    eax,ebx
  436fb2:	c6 44 24 38 01       	mov    BYTE PTR [esp+0x38],0x1
  436fb7:	74 0e                	je     0x436fc7
  436fb9:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  436fbd:	51                   	push   ecx
  436fbe:	8b c8                	mov    ecx,eax
  436fc0:	e8 1b a2 ff ff       	call   0x4311e0
  436fc5:	eb 02                	jmp    0x436fc9
  436fc7:	33 c0                	xor    eax,eax
  436fc9:	53                   	push   ebx
  436fca:	53                   	push   ebx
  436fcb:	6a 1b                	push   0x1b
  436fcd:	6a 36                	push   0x36
  436fcf:	53                   	push   ebx
  436fd0:	6a 20                	push   0x20
  436fd2:	8b c8                	mov    ecx,eax
  436fd4:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  436fd8:	89 86 7c 05 00 00    	mov    DWORD PTR [esi+0x57c],eax
  436fde:	e8 dd a2 ff ff       	call   0x4312c0
  436fe3:	6a 48                	push   0x48
  436fe5:	e8 75 a6 01 00       	call   0x45165f
  436fea:	83 c4 04             	add    esp,0x4
  436fed:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  436ff1:	3b c3                	cmp    eax,ebx
  436ff3:	c6 44 24 38 02       	mov    BYTE PTR [esp+0x38],0x2
  436ff8:	74 0e                	je     0x437008
  436ffa:	8d 54 24 18          	lea    edx,[esp+0x18]
  436ffe:	8b c8                	mov    ecx,eax
  437000:	52                   	push   edx
  437001:	e8 da a1 ff ff       	call   0x4311e0
  437006:	eb 02                	jmp    0x43700a
  437008:	33 c0                	xor    eax,eax
  43700a:	53                   	push   ebx
  43700b:	53                   	push   ebx
  43700c:	6a 1b                	push   0x1b
  43700e:	6a 36                	push   0x36
  437010:	53                   	push   ebx
  437011:	6a 61                	push   0x61
  437013:	8b c8                	mov    ecx,eax
  437015:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  437019:	89 86 80 05 00 00    	mov    DWORD PTR [esi+0x580],eax
  43701f:	e8 9c a2 ff ff       	call   0x4312c0
  437024:	6a 48                	push   0x48
  437026:	e8 34 a6 01 00       	call   0x45165f
  43702b:	83 c4 04             	add    esp,0x4
  43702e:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  437032:	3b c3                	cmp    eax,ebx
  437034:	c6 44 24 38 03       	mov    BYTE PTR [esp+0x38],0x3
  437039:	74 0e                	je     0x437049
  43703b:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  43703f:	51                   	push   ecx
  437040:	8b c8                	mov    ecx,eax
  437042:	e8 99 a1 ff ff       	call   0x4311e0
  437047:	eb 02                	jmp    0x43704b
  437049:	33 c0                	xor    eax,eax
  43704b:	53                   	push   ebx
  43704c:	53                   	push   ebx
  43704d:	6a 1b                	push   0x1b
  43704f:	6a 36                	push   0x36
  437051:	53                   	push   ebx
  437052:	68 a1 00 00 00       	push   0xa1
  437057:	8b c8                	mov    ecx,eax
  437059:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  43705d:	89 86 84 05 00 00    	mov    DWORD PTR [esi+0x584],eax
  437063:	e8 58 a2 ff ff       	call   0x4312c0
  437068:	6a 48                	push   0x48
  43706a:	e8 f0 a5 01 00       	call   0x45165f
  43706f:	83 c4 04             	add    esp,0x4
  437072:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  437076:	3b c3                	cmp    eax,ebx
  437078:	c6 44 24 38 04       	mov    BYTE PTR [esp+0x38],0x4
  43707d:	74 0e                	je     0x43708d
  43707f:	8d 54 24 18          	lea    edx,[esp+0x18]
  437083:	8b c8                	mov    ecx,eax
  437085:	52                   	push   edx
  437086:	e8 55 a1 ff ff       	call   0x4311e0
  43708b:	eb 02                	jmp    0x43708f
  43708d:	33 c0                	xor    eax,eax
  43708f:	53                   	push   ebx
  437090:	53                   	push   ebx
  437091:	6a 1b                	push   0x1b
  437093:	6a 36                	push   0x36
  437095:	53                   	push   ebx
  437096:	68 e3 00 00 00       	push   0xe3
  43709b:	8b c8                	mov    ecx,eax
  43709d:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  4370a1:	89 86 88 05 00 00    	mov    DWORD PTR [esi+0x588],eax
  4370a7:	e8 14 a2 ff ff       	call   0x4312c0
  4370ac:	6a 48                	push   0x48
  4370ae:	e8 ac a5 01 00       	call   0x45165f
  4370b3:	83 c4 04             	add    esp,0x4
  4370b6:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  4370ba:	3b c3                	cmp    eax,ebx
  4370bc:	c6 44 24 38 05       	mov    BYTE PTR [esp+0x38],0x5
  4370c1:	74 0e                	je     0x4370d1
  4370c3:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  4370c7:	51                   	push   ecx
  4370c8:	8b c8                	mov    ecx,eax
  4370ca:	e8 11 a1 ff ff       	call   0x4311e0
  4370cf:	eb 02                	jmp    0x4370d3
  4370d1:	33 c0                	xor    eax,eax
  4370d3:	53                   	push   ebx
  4370d4:	53                   	push   ebx
  4370d5:	6a 1b                	push   0x1b
  4370d7:	6a 36                	push   0x36
  4370d9:	53                   	push   ebx
  4370da:	68 20 01 00 00       	push   0x120
  4370df:	8b c8                	mov    ecx,eax
  4370e1:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  4370e5:	89 86 8c 05 00 00    	mov    DWORD PTR [esi+0x58c],eax
  4370eb:	e8 d0 a1 ff ff       	call   0x4312c0
  4370f0:	6a 48                	push   0x48
  4370f2:	e8 68 a5 01 00       	call   0x45165f
  4370f7:	83 c4 04             	add    esp,0x4
  4370fa:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  4370fe:	3b c3                	cmp    eax,ebx
  437100:	c6 44 24 38 06       	mov    BYTE PTR [esp+0x38],0x6
  437105:	74 0e                	je     0x437115
  437107:	8d 54 24 18          	lea    edx,[esp+0x18]
  43710b:	8b c8                	mov    ecx,eax
  43710d:	52                   	push   edx
  43710e:	e8 cd a0 ff ff       	call   0x4311e0
  437113:	eb 02                	jmp    0x437117
  437115:	33 c0                	xor    eax,eax
  437117:	53                   	push   ebx
  437118:	53                   	push   ebx
  437119:	6a 1b                	push   0x1b
  43711b:	6a 36                	push   0x36
  43711d:	53                   	push   ebx
  43711e:	68 5f 01 00 00       	push   0x15f
  437123:	8b c8                	mov    ecx,eax
  437125:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  437129:	89 86 90 05 00 00    	mov    DWORD PTR [esi+0x590],eax
  43712f:	e8 8c a1 ff ff       	call   0x4312c0
  437134:	6a 48                	push   0x48
  437136:	e8 24 a5 01 00       	call   0x45165f
  43713b:	83 c4 04             	add    esp,0x4
  43713e:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  437142:	3b c3                	cmp    eax,ebx
  437144:	c6 44 24 38 07       	mov    BYTE PTR [esp+0x38],0x7
  437149:	74 0e                	je     0x437159
  43714b:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  43714f:	51                   	push   ecx
  437150:	8b c8                	mov    ecx,eax
  437152:	e8 89 a0 ff ff       	call   0x4311e0
  437157:	eb 02                	jmp    0x43715b
  437159:	33 c0                	xor    eax,eax
  43715b:	53                   	push   ebx
  43715c:	53                   	push   ebx
  43715d:	6a 1b                	push   0x1b
  43715f:	6a 36                	push   0x36
  437161:	53                   	push   ebx
  437162:	68 a1 01 00 00       	push   0x1a1
  437167:	8b c8                	mov    ecx,eax
  437169:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  43716d:	89 86 94 05 00 00    	mov    DWORD PTR [esi+0x594],eax
  437173:	e8 48 a1 ff ff       	call   0x4312c0
  437178:	6a 48                	push   0x48
  43717a:	e8 e0 a4 01 00       	call   0x45165f
  43717f:	83 c4 04             	add    esp,0x4
  437182:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  437186:	3b c3                	cmp    eax,ebx
  437188:	c6 44 24 38 08       	mov    BYTE PTR [esp+0x38],0x8
  43718d:	74 0e                	je     0x43719d
  43718f:	8d 54 24 18          	lea    edx,[esp+0x18]
  437193:	8b c8                	mov    ecx,eax
  437195:	52                   	push   edx
  437196:	e8 45 a0 ff ff       	call   0x4311e0
  43719b:	eb 02                	jmp    0x43719f
  43719d:	33 c0                	xor    eax,eax
  43719f:	53                   	push   ebx
  4371a0:	53                   	push   ebx
  4371a1:	6a 1b                	push   0x1b
  4371a3:	6a 36                	push   0x36
  4371a5:	53                   	push   ebx
  4371a6:	68 de 01 00 00       	push   0x1de
  4371ab:	8b c8                	mov    ecx,eax
  4371ad:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  4371b1:	89 86 98 05 00 00    	mov    DWORD PTR [esi+0x598],eax
  4371b7:	e8 04 a1 ff ff       	call   0x4312c0
  4371bc:	6a 48                	push   0x48
  4371be:	e8 9c a4 01 00       	call   0x45165f
  4371c3:	83 c4 04             	add    esp,0x4
  4371c6:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  4371ca:	3b c3                	cmp    eax,ebx
  4371cc:	c6 44 24 38 09       	mov    BYTE PTR [esp+0x38],0x9
  4371d1:	74 0e                	je     0x4371e1
  4371d3:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  4371d7:	51                   	push   ecx
  4371d8:	8b c8                	mov    ecx,eax
  4371da:	e8 01 a0 ff ff       	call   0x4311e0
  4371df:	eb 02                	jmp    0x4371e3
  4371e1:	33 c0                	xor    eax,eax
  4371e3:	8b 55 00             	mov    edx,DWORD PTR [ebp+0x0]
  4371e6:	8b 0f                	mov    ecx,DWORD PTR [edi]
  4371e8:	53                   	push   ebx
  4371e9:	53                   	push   ebx
  4371ea:	68 bc 02 00 00       	push   0x2bc
  4371ef:	68 80 02 00 00       	push   0x280
  4371f4:	f7 da                	neg    edx
  4371f6:	f7 d9                	neg    ecx
  4371f8:	52                   	push   edx
  4371f9:	51                   	push   ecx
  4371fa:	8b c8                	mov    ecx,eax
  4371fc:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  437200:	89 86 9c 05 00 00    	mov    DWORD PTR [esi+0x59c],eax
  437206:	e8 b5 a0 ff ff       	call   0x4312c0
  43720b:	8b 4c 24 30          	mov    ecx,DWORD PTR [esp+0x30]
  43720f:	8b c6                	mov    eax,esi
  437211:	5f                   	pop    edi
  437212:	5e                   	pop    esi
  437213:	5d                   	pop    ebp
  437214:	5b                   	pop    ebx
  437215:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  43721c:	83 c4 2c             	add    esp,0x2c
  43721f:	c3                   	ret
