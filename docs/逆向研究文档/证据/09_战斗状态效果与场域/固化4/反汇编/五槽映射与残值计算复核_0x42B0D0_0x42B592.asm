
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

0042b0d0 <.text+0x2a0d0>:
  42b0d0:	83 ec 1c             	sub    esp,0x1c
  42b0d3:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  42b0d7:	89 4c 24 14          	mov    DWORD PTR [esp+0x14],ecx
  42b0db:	53                   	push   ebx
  42b0dc:	55                   	push   ebp
  42b0dd:	8b 88 28 02 00 00    	mov    ecx,DWORD PTR [eax+0x228]
  42b0e3:	56                   	push   esi
  42b0e4:	85 c9                	test   ecx,ecx
  42b0e6:	57                   	push   edi
  42b0e7:	0f 8f 9e 04 00 00    	jg     0x42b58b
  42b0ed:	8b 4c 24 30          	mov    ecx,DWORD PTR [esp+0x30]
  42b0f1:	33 db                	xor    ebx,ebx
  42b0f3:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  42b0f7:	b8 01 00 00 00       	mov    eax,0x1
  42b0fc:	8b a9 90 00 00 00    	mov    ebp,DWORD PTR [ecx+0x90]
  42b102:	89 6c 24 20          	mov    DWORD PTR [esp+0x20],ebp
  42b106:	eb 04                	jmp    0x42b10c
  42b108:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42b10c:	33 ff                	xor    edi,edi
  42b10e:	3b c7                	cmp    eax,edi
  42b110:	0f 8e 75 04 00 00    	jle    0x42b58b
  42b116:	8b 4c 24 38          	mov    ecx,DWORD PTR [esp+0x38]
  42b11a:	89 7c 24 14          	mov    DWORD PTR [esp+0x14],edi
  42b11e:	3b cf                	cmp    ecx,edi
  42b120:	89 7c 24 10          	mov    DWORD PTR [esp+0x10],edi
  42b124:	74 61                	je     0x42b187
  42b126:	8b 44 99 64          	mov    eax,DWORD PTR [ecx+ebx*4+0x64]
  42b12a:	8b 54 99 78          	mov    edx,DWORD PTR [ecx+ebx*4+0x78]
  42b12e:	3b c7                	cmp    eax,edi
  42b130:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  42b134:	89 54 24 10          	mov    DWORD PTR [esp+0x10],edx
  42b138:	75 49                	jne    0x42b183
  42b13a:	3b ef                	cmp    ebp,edi
  42b13c:	7e 45                	jle    0x42b183
  42b13e:	39 79 18             	cmp    DWORD PTR [ecx+0x18],edi
  42b141:	75 40                	jne    0x42b183
  42b143:	8d 04 ed 00 00 00 00 	lea    eax,[ebp*8+0x0]
  42b14a:	8b 0d b0 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01b0
  42b150:	2b c5                	sub    eax,ebp
  42b152:	8d 44 85 00          	lea    eax,[ebp+eax*4+0x0]
  42b156:	c1 e0 04             	shl    eax,0x4
  42b159:	2b c5                	sub    eax,ebp
  42b15b:	d1 e0                	shl    eax,1
  42b15d:	8d 54 98 50          	lea    edx,[eax+ebx*4+0x50]
  42b161:	8d 44 98 64          	lea    eax,[eax+ebx*4+0x64]
  42b165:	8b 14 0a             	mov    edx,DWORD PTR [edx+ecx*1]
  42b168:	8b 0c 08             	mov    ecx,DWORD PTR [eax+ecx*1]
  42b16b:	85 d2                	test   edx,edx
  42b16d:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  42b171:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  42b175:	7e 0c                	jle    0x42b183
  42b177:	c7 44 24 18 04 00 00 	mov    DWORD PTR [esp+0x18],0x4
  42b17e:	00 
  42b17f:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
  42b183:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  42b187:	8b 44 24 3c          	mov    eax,DWORD PTR [esp+0x3c]
  42b18b:	85 c0                	test   eax,eax
  42b18d:	74 18                	je     0x42b1a7
  42b18f:	8b 94 98 e8 00 00 00 	mov    edx,DWORD PTR [eax+ebx*4+0xe8]
  42b196:	8b 84 98 fc 00 00 00 	mov    eax,DWORD PTR [eax+ebx*4+0xfc]
  42b19d:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  42b1a1:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  42b1a5:	8b f8                	mov    edi,eax
  42b1a7:	85 ff                	test   edi,edi
  42b1a9:	7c 12                	jl     0x42b1bd
  42b1ab:	83 ff 1b             	cmp    edi,0x1b
  42b1ae:	7f 0d                	jg     0x42b1bd
  42b1b0:	8b cf                	mov    ecx,edi
  42b1b2:	c1 e1 04             	shl    ecx,0x4
  42b1b5:	8b b1 0c a0 46 00    	mov    esi,DWORD PTR [ecx+0x46a00c]
  42b1bb:	eb 02                	jmp    0x42b1bf
  42b1bd:	33 f6                	xor    esi,esi
  42b1bf:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  42b1c3:	85 c0                	test   eax,eax
  42b1c5:	0f 8e b2 03 00 00    	jle    0x42b57d
  42b1cb:	83 ff 14             	cmp    edi,0x14
  42b1ce:	0f 8f 3a 01 00 00    	jg     0x42b30e
  42b1d4:	8b 54 24 30          	mov    edx,DWORD PTR [esp+0x30]
  42b1d8:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  42b1dc:	bf 64 00 00 00       	mov    edi,0x64
  42b1e1:	8b 4a 4c             	mov    ecx,DWORD PTR [edx+0x4c]
  42b1e4:	8b 50 4c             	mov    edx,DWORD PTR [eax+0x4c]
  42b1e7:	2b ca                	sub    ecx,edx
  42b1e9:	b8 67 66 66 66       	mov    eax,0x66666667
  42b1ee:	f7 e9                	imul   ecx
  42b1f0:	c1 fa 02             	sar    edx,0x2
  42b1f3:	8b ca                	mov    ecx,edx
  42b1f5:	8b ef                	mov    ebp,edi
  42b1f7:	c1 e9 1f             	shr    ecx,0x1f
  42b1fa:	03 d1                	add    edx,ecx
  42b1fc:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  42b200:	03 d1                	add    edx,ecx
  42b202:	8b df                	mov    ebx,edi
  42b204:	85 f6                	test   esi,esi
  42b206:	89 54 24 28          	mov    DWORD PTR [esp+0x28],edx
  42b20a:	89 7c 24 1c          	mov    DWORD PTR [esp+0x1c],edi
  42b20e:	0f 8e 80 00 00 00    	jle    0x42b294
  42b214:	8b 4c 24 34          	mov    ecx,DWORD PTR [esp+0x34]
  42b218:	8b 81 90 00 00 00    	mov    eax,DWORD PTR [ecx+0x90]
  42b21e:	8b 7c b1 64          	mov    edi,DWORD PTR [ecx+esi*4+0x64]
  42b222:	85 c0                	test   eax,eax
  42b224:	7e 20                	jle    0x42b246
  42b226:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  42b22d:	2b d0                	sub    edx,eax
  42b22f:	8d 2c 90             	lea    ebp,[eax+edx*4]
  42b232:	8b 15 b0 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b0
  42b238:	c1 e5 04             	shl    ebp,0x4
  42b23b:	2b e8                	sub    ebp,eax
  42b23d:	8d 04 6a             	lea    eax,[edx+ebp*2]
  42b240:	8b 6c b0 74          	mov    ebp,DWORD PTR [eax+esi*4+0x74]
  42b244:	eb 06                	jmp    0x42b24c
  42b246:	8b 15 b0 01 8a 00    	mov    edx,DWORD PTR ds:0x8a01b0
  42b24c:	8b 81 94 00 00 00    	mov    eax,DWORD PTR [ecx+0x94]
  42b252:	85 c0                	test   eax,eax
  42b254:	7e 18                	jle    0x42b26e
  42b256:	8d 1c c5 00 00 00 00 	lea    ebx,[eax*8+0x0]
  42b25d:	2b d8                	sub    ebx,eax
  42b25f:	8d 1c 98             	lea    ebx,[eax+ebx*4]
  42b262:	c1 e3 04             	shl    ebx,0x4
  42b265:	2b d8                	sub    ebx,eax
  42b267:	8d 04 5a             	lea    eax,[edx+ebx*2]
  42b26a:	8b 5c b0 74          	mov    ebx,DWORD PTR [eax+esi*4+0x74]
  42b26e:	8b 81 98 00 00 00    	mov    eax,DWORD PTR [ecx+0x98]
  42b274:	85 c0                	test   eax,eax
  42b276:	7e 1c                	jle    0x42b294
  42b278:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  42b27f:	2b c8                	sub    ecx,eax
  42b281:	8d 0c 88             	lea    ecx,[eax+ecx*4]
  42b284:	c1 e1 04             	shl    ecx,0x4
  42b287:	2b c8                	sub    ecx,eax
  42b289:	8d 14 4a             	lea    edx,[edx+ecx*2]
  42b28c:	8b 44 b2 74          	mov    eax,DWORD PTR [edx+esi*4+0x74]
  42b290:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  42b294:	8b c7                	mov    eax,edi
  42b296:	99                   	cdq
  42b297:	8b c8                	mov    ecx,eax
  42b299:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  42b29d:	33 ca                	xor    ecx,edx
  42b29f:	2b ca                	sub    ecx,edx
  42b2a1:	99                   	cdq
  42b2a2:	33 c2                	xor    eax,edx
  42b2a4:	2b c2                	sub    eax,edx
  42b2a6:	0f af c8             	imul   ecx,eax
  42b2a9:	8b c3                	mov    eax,ebx
  42b2ab:	99                   	cdq
  42b2ac:	33 c2                	xor    eax,edx
  42b2ae:	2b c2                	sub    eax,edx
  42b2b0:	0f af c8             	imul   ecx,eax
  42b2b3:	8b c5                	mov    eax,ebp
  42b2b5:	99                   	cdq
  42b2b6:	33 c2                	xor    eax,edx
  42b2b8:	2b c2                	sub    eax,edx
  42b2ba:	0f af c8             	imul   ecx,eax
  42b2bd:	85 f6                	test   esi,esi
  42b2bf:	7e 16                	jle    0x42b2d7
  42b2c1:	85 ff                	test   edi,edi
  42b2c3:	7c 10                	jl     0x42b2d5
  42b2c5:	85 ed                	test   ebp,ebp
  42b2c7:	7c 0c                	jl     0x42b2d5
  42b2c9:	85 db                	test   ebx,ebx
  42b2cb:	7c 08                	jl     0x42b2d5
  42b2cd:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  42b2d1:	85 c0                	test   eax,eax
  42b2d3:	7d 02                	jge    0x42b2d7
  42b2d5:	f7 d9                	neg    ecx
  42b2d7:	b8 83 de 1b 43       	mov    eax,0x431bde83
  42b2dc:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
  42b2e0:	f7 e9                	imul   ecx
  42b2e2:	c1 fa 12             	sar    edx,0x12
  42b2e5:	8b ca                	mov    ecx,edx
  42b2e7:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b2ec:	c1 e9 1f             	shr    ecx,0x1f
  42b2ef:	03 d1                	add    edx,ecx
  42b2f1:	8b 7c 24 10          	mov    edi,DWORD PTR [esp+0x10]
  42b2f5:	0f af 54 24 28       	imul   edx,DWORD PTR [esp+0x28]
  42b2fa:	f7 ea                	imul   edx
  42b2fc:	c1 fa 05             	sar    edx,0x5
  42b2ff:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
  42b303:	8b c2                	mov    eax,edx
  42b305:	c1 e8 1f             	shr    eax,0x1f
  42b308:	03 d0                	add    edx,eax
  42b30a:	8b f2                	mov    esi,edx
  42b30c:	eb 05                	jmp    0x42b313
  42b30e:	be 64 00 00 00       	mov    esi,0x64
  42b313:	83 ff 1b             	cmp    edi,0x1b
  42b316:	7e 15                	jle    0x42b32d
  42b318:	8b 44 24 40          	mov    eax,DWORD PTR [esp+0x40]
  42b31c:	89 7c 98 10          	mov    DWORD PTR [eax+ebx*4+0x10],edi
  42b320:	c7 44 98 24 40 1f 00 	mov    DWORD PTR [eax+ebx*4+0x24],0x1f40
  42b327:	00 
  42b328:	e9 50 02 00 00       	jmp    0x42b57d
  42b32d:	e8 33 62 02 00       	call   0x451565
  42b332:	99                   	cdq
  42b333:	b9 64 00 00 00       	mov    ecx,0x64
  42b338:	f7 f9                	idiv   ecx
  42b33a:	3b d6                	cmp    edx,esi
  42b33c:	7c 0c                	jl     0x42b34a
  42b33e:	8a 44 24 44          	mov    al,BYTE PTR [esp+0x44]
  42b342:	84 c0                	test   al,al
  42b344:	0f 84 33 02 00 00    	je     0x42b57d
  42b34a:	8b 54 24 38          	mov    edx,DWORD PTR [esp+0x38]
  42b34e:	c1 e7 04             	shl    edi,0x4
  42b351:	85 d2                	test   edx,edx
  42b353:	8b 8f 00 a0 46 00    	mov    ecx,DWORD PTR [edi+0x46a000]
  42b359:	74 19                	je     0x42b374
  42b35b:	8b 02                	mov    eax,DWORD PTR [edx]
  42b35d:	3d b7 01 00 00       	cmp    eax,0x1b7
  42b362:	7c 10                	jl     0x42b374
  42b364:	3d be 01 00 00       	cmp    eax,0x1be
  42b369:	7f 09                	jg     0x42b374
  42b36b:	8b 52 4c             	mov    edx,DWORD PTR [edx+0x4c]
  42b36e:	89 54 24 18          	mov    DWORD PTR [esp+0x18],edx
  42b372:	8b ca                	mov    ecx,edx
  42b374:	8b 54 24 30          	mov    edx,DWORD PTR [esp+0x30]
  42b378:	8b 35 b0 01 8a 00    	mov    esi,DWORD PTR ds:0x8a01b0
  42b37e:	8b 82 90 00 00 00    	mov    eax,DWORD PTR [edx+0x90]
  42b384:	85 c0                	test   eax,eax
  42b386:	7e 17                	jle    0x42b39f
  42b388:	8d 2c c5 00 00 00 00 	lea    ebp,[eax*8+0x0]
  42b38f:	2b e8                	sub    ebp,eax
  42b391:	8d 2c a8             	lea    ebp,[eax+ebp*4]
  42b394:	c1 e5 04             	shl    ebp,0x4
  42b397:	2b e8                	sub    ebp,eax
  42b399:	8b 44 6e 48          	mov    eax,DWORD PTR [esi+ebp*2+0x48]
  42b39d:	eb 02                	jmp    0x42b3a1
  42b39f:	33 c0                	xor    eax,eax
  42b3a1:	03 c8                	add    ecx,eax
  42b3a3:	8b 82 94 00 00 00    	mov    eax,DWORD PTR [edx+0x94]
  42b3a9:	85 c0                	test   eax,eax
  42b3ab:	7e 17                	jle    0x42b3c4
  42b3ad:	8d 2c c5 00 00 00 00 	lea    ebp,[eax*8+0x0]
  42b3b4:	2b e8                	sub    ebp,eax
  42b3b6:	8d 2c a8             	lea    ebp,[eax+ebp*4]
  42b3b9:	c1 e5 04             	shl    ebp,0x4
  42b3bc:	2b e8                	sub    ebp,eax
  42b3be:	8b 44 6e 48          	mov    eax,DWORD PTR [esi+ebp*2+0x48]
  42b3c2:	eb 02                	jmp    0x42b3c6
  42b3c4:	33 c0                	xor    eax,eax
  42b3c6:	03 c8                	add    ecx,eax
  42b3c8:	8b 82 98 00 00 00    	mov    eax,DWORD PTR [edx+0x98]
  42b3ce:	85 c0                	test   eax,eax
  42b3d0:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  42b3d4:	7e 17                	jle    0x42b3ed
  42b3d6:	8d 0c c5 00 00 00 00 	lea    ecx,[eax*8+0x0]
  42b3dd:	2b c8                	sub    ecx,eax
  42b3df:	8d 14 88             	lea    edx,[eax+ecx*4]
  42b3e2:	c1 e2 04             	shl    edx,0x4
  42b3e5:	2b d0                	sub    edx,eax
  42b3e7:	8b 6c 56 48          	mov    ebp,DWORD PTR [esi+edx*2+0x48]
  42b3eb:	eb 02                	jmp    0x42b3ef
  42b3ed:	33 ed                	xor    ebp,ebp
  42b3ef:	8b 44 24 30          	mov    eax,DWORD PTR [esp+0x30]
  42b3f3:	8b 4c 24 24          	mov    ecx,DWORD PTR [esp+0x24]
  42b3f7:	50                   	push   eax
  42b3f8:	e8 f3 18 00 00       	call   0x42ccf0
  42b3fd:	8b 4c 24 30          	mov    ecx,DWORD PTR [esp+0x30]
  42b401:	8b f0                	mov    esi,eax
  42b403:	51                   	push   ecx
  42b404:	8b 4c 24 28          	mov    ecx,DWORD PTR [esp+0x28]
  42b408:	e8 e3 18 00 00       	call   0x42ccf0
  42b40d:	0f af f0             	imul   esi,eax
  42b410:	b8 85 91 76 ac       	mov    eax,0xac769185
  42b415:	8b 8f 0c a0 46 00    	mov    ecx,DWORD PTR [edi+0x46a00c]
  42b41b:	f7 ee                	imul   esi
  42b41d:	03 d6                	add    edx,esi
  42b41f:	8b 74 24 18          	mov    esi,DWORD PTR [esp+0x18]
  42b423:	c1 fa 06             	sar    edx,0x6
  42b426:	8b c2                	mov    eax,edx
  42b428:	c1 e8 1f             	shr    eax,0x1f
  42b42b:	03 d0                	add    edx,eax
  42b42d:	8b 87 04 a0 46 00    	mov    eax,DWORD PTR [edi+0x46a004]
  42b433:	8b bf 08 a0 46 00    	mov    edi,DWORD PTR [edi+0x46a008]
  42b439:	03 d5                	add    edx,ebp
  42b43b:	03 f2                	add    esi,edx
  42b43d:	bd 64 00 00 00       	mov    ebp,0x64
  42b442:	89 74 24 18          	mov    DWORD PTR [esp+0x18],esi
  42b446:	83 f8 01             	cmp    eax,0x1
  42b449:	8b f5                	mov    esi,ebp
  42b44b:	7c 0d                	jl     0x42b45a
  42b44d:	83 f8 05             	cmp    eax,0x5
  42b450:	7f 08                	jg     0x42b45a
  42b452:	8b 54 24 34          	mov    edx,DWORD PTR [esp+0x34]
  42b456:	8b 6c 82 38          	mov    ebp,DWORD PTR [edx+eax*4+0x38]
  42b45a:	83 ff 01             	cmp    edi,0x1
  42b45d:	7c 12                	jl     0x42b471
  42b45f:	83 ff 05             	cmp    edi,0x5
  42b462:	7f 0d                	jg     0x42b471
  42b464:	8b c5                	mov    eax,ebp
  42b466:	8b 6c 24 30          	mov    ebp,DWORD PTR [esp+0x30]
  42b46a:	99                   	cdq
  42b46b:	f7 7c bd 38          	idiv   DWORD PTR [ebp+edi*4+0x38]
  42b46f:	8b e8                	mov    ebp,eax
  42b471:	83 f9 01             	cmp    ecx,0x1
  42b474:	7c 29                	jl     0x42b49f
  42b476:	83 f9 0a             	cmp    ecx,0xa
  42b479:	7f 24                	jg     0x42b49f
  42b47b:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  42b47f:	8b 44 88 64          	mov    eax,DWORD PTR [eax+ecx*4+0x64]
  42b483:	8d 04 80             	lea    eax,[eax+eax*4]
  42b486:	8d 0c 80             	lea    ecx,[eax+eax*4]
  42b489:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b48e:	c1 e1 02             	shl    ecx,0x2
  42b491:	f7 e9                	imul   ecx
  42b493:	c1 fa 05             	sar    edx,0x5
  42b496:	8b ca                	mov    ecx,edx
  42b498:	c1 e9 1f             	shr    ecx,0x1f
  42b49b:	03 d1                	add    edx,ecx
  42b49d:	8b f2                	mov    esi,edx
  42b49f:	8b 54 24 34          	mov    edx,DWORD PTR [esp+0x34]
  42b4a3:	b8 67 66 66 66       	mov    eax,0x66666667
  42b4a8:	8b 4a 4c             	mov    ecx,DWORD PTR [edx+0x4c]
  42b4ab:	f7 e9                	imul   ecx
  42b4ad:	c1 fa 02             	sar    edx,0x2
  42b4b0:	8b c2                	mov    eax,edx
  42b4b2:	c1 e8 1f             	shr    eax,0x1f
  42b4b5:	03 d0                	add    edx,eax
  42b4b7:	b8 67 66 66 66       	mov    eax,0x66666667
  42b4bc:	8b fa                	mov    edi,edx
  42b4be:	f7 e9                	imul   ecx
  42b4c0:	d1 fa                	sar    edx,1
  42b4c2:	8b ca                	mov    ecx,edx
  42b4c4:	89 7c 24 28          	mov    DWORD PTR [esp+0x28],edi
  42b4c8:	c1 e9 1f             	shr    ecx,0x1f
  42b4cb:	03 d1                	add    edx,ecx
  42b4cd:	8b fa                	mov    edi,edx
  42b4cf:	8b 54 24 28          	mov    edx,DWORD PTR [esp+0x28]
  42b4d3:	2b fa                	sub    edi,edx
  42b4d5:	83 ff 01             	cmp    edi,0x1
  42b4d8:	7d 04                	jge    0x42b4de
  42b4da:	33 c9                	xor    ecx,ecx
  42b4dc:	eb 24                	jmp    0x42b502
  42b4de:	e8 82 60 02 00       	call   0x451565
  42b4e3:	99                   	cdq
  42b4e4:	f7 ff                	idiv   edi
  42b4e6:	8b 44 24 34          	mov    eax,DWORD PTR [esp+0x34]
  42b4ea:	8b 48 4c             	mov    ecx,DWORD PTR [eax+0x4c]
  42b4ed:	b8 67 66 66 66       	mov    eax,0x66666667
  42b4f2:	8b fa                	mov    edi,edx
  42b4f4:	f7 e9                	imul   ecx
  42b4f6:	c1 fa 02             	sar    edx,0x2
  42b4f9:	8b ca                	mov    ecx,edx
  42b4fb:	03 d7                	add    edx,edi
  42b4fd:	c1 e9 1f             	shr    ecx,0x1f
  42b500:	03 ca                	add    ecx,edx
  42b502:	0f af 74 24 18       	imul   esi,DWORD PTR [esp+0x18]
  42b507:	8b 7c 24 40          	mov    edi,DWORD PTR [esp+0x40]
  42b50b:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  42b50f:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  42b514:	89 54 9f 10          	mov    DWORD PTR [edi+ebx*4+0x10],edx
  42b518:	f7 ee                	imul   esi
  42b51a:	c1 fa 05             	sar    edx,0x5
  42b51d:	8b c2                	mov    eax,edx
  42b51f:	c1 e8 1f             	shr    eax,0x1f
  42b522:	03 d0                	add    edx,eax
  42b524:	2b d1                	sub    edx,ecx
  42b526:	2b d5                	sub    edx,ebp
  42b528:	89 54 9f 24          	mov    DWORD PTR [edi+ebx*4+0x24],edx
  42b52c:	79 08                	jns    0x42b536
  42b52e:	c7 44 9f 24 00 00 00 	mov    DWORD PTR [edi+ebx*4+0x24],0x0
  42b535:	00 
  42b536:	8b 4c 9f 10          	mov    ecx,DWORD PTR [edi+ebx*4+0x10]
  42b53a:	8b 14 cd 68 a6 46 00 	mov    edx,DWORD PTR [ecx*8+0x46a668]
  42b541:	85 d2                	test   edx,edx
  42b543:	7c 24                	jl     0x42b569
  42b545:	8b 74 24 34          	mov    esi,DWORD PTR [esp+0x34]
  42b549:	8b 4c 9f 24          	mov    ecx,DWORD PTR [edi+ebx*4+0x24]
  42b54d:	8b 84 96 e8 01 00 00 	mov    eax,DWORD PTR [esi+edx*4+0x1e8]
  42b554:	3b c1                	cmp    eax,ecx
  42b556:	7e 0b                	jle    0x42b563
  42b558:	2b c1                	sub    eax,ecx
  42b55a:	89 84 96 e8 01 00 00 	mov    DWORD PTR [esi+edx*4+0x1e8],eax
  42b561:	eb 06                	jmp    0x42b569
  42b563:	2b c8                	sub    ecx,eax
  42b565:	89 4c 9f 24          	mov    DWORD PTR [edi+ebx*4+0x24],ecx
  42b569:	8b 44 9f 24          	mov    eax,DWORD PTR [edi+ebx*4+0x24]
  42b56d:	85 c0                	test   eax,eax
  42b56f:	7d 08                	jge    0x42b579
  42b571:	c7 44 9f 24 00 00 00 	mov    DWORD PTR [edi+ebx*4+0x24],0x0
  42b578:	00 
  42b579:	8b 6c 24 20          	mov    ebp,DWORD PTR [esp+0x20]
  42b57d:	43                   	inc    ebx
  42b57e:	83 fb 05             	cmp    ebx,0x5
  42b581:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  42b585:	0f 8c 7d fb ff ff    	jl     0x42b108
  42b58b:	5f                   	pop    edi
  42b58c:	5e                   	pop    esi
  42b58d:	5d                   	pop    ebp
  42b58e:	5b                   	pop    ebx
  42b58f:	83 c4 1c             	add    esp,0x1c
