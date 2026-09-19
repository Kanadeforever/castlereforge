
/mnt/data/solid61_inputs/RPG_original.exe:     file format pei-i386


Disassembly of section .text:

0043b170 <.text+0x3a170>:
  43b170:	83 ec 5c             	sub    esp,0x5c
  43b173:	53                   	push   ebx
  43b174:	55                   	push   ebp
  43b175:	8b 6c 24 68          	mov    ebp,DWORD PTR [esp+0x68]
  43b179:	8b d1                	mov    edx,ecx
  43b17b:	56                   	push   esi
  43b17c:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  43b180:	8b 45 00             	mov    eax,DWORD PTR [ebp+0x0]
  43b183:	85 c0                	test   eax,eax
  43b185:	0f 8c 7b 01 00 00    	jl     0x43b306
  43b18b:	3b 82 b0 00 00 00    	cmp    eax,DWORD PTR [edx+0xb0]
  43b191:	0f 8d 6f 01 00 00    	jge    0x43b306
  43b197:	8b b2 b4 00 00 00    	mov    esi,DWORD PTR [edx+0xb4]
  43b19d:	8d 1c c5 00 00 00 00 	lea    ebx,[eax*8+0x0]
  43b1a4:	2b d8                	sub    ebx,eax
  43b1a6:	8d 1c 9b             	lea    ebx,[ebx+ebx*4]
  43b1a9:	c1 e3 04             	shl    ebx,0x4
  43b1ac:	03 de                	add    ebx,esi
  43b1ae:	8b 83 ec 01 00 00    	mov    eax,DWORD PTR [ebx+0x1ec]
  43b1b4:	85 c0                	test   eax,eax
  43b1b6:	7e 20                	jle    0x43b1d8
  43b1b8:	89 82 c4 00 00 00    	mov    DWORD PTR [edx+0xc4],eax
  43b1be:	89 45 08             	mov    DWORD PTR [ebp+0x8],eax
  43b1c1:	8b 83 ec 01 00 00    	mov    eax,DWORD PTR [ebx+0x1ec]
  43b1c7:	5e                   	pop    esi
  43b1c8:	89 82 ec 00 00 00    	mov    DWORD PTR [edx+0xec],eax
  43b1ce:	5d                   	pop    ebp
  43b1cf:	b0 01                	mov    al,0x1
  43b1d1:	5b                   	pop    ebx
  43b1d2:	83 c4 5c             	add    esp,0x5c
  43b1d5:	c2 04 00             	ret    0x4
  43b1d8:	8b 45 0c             	mov    eax,DWORD PTR [ebp+0xc]
  43b1db:	57                   	push   edi
  43b1dc:	85 c0                	test   eax,eax
  43b1de:	7e 0b                	jle    0x43b1eb
  43b1e0:	8d 75 10             	lea    esi,[ebp+0x10]
  43b1e3:	8b c8                	mov    ecx,eax
  43b1e5:	8d 7c 24 1c          	lea    edi,[esp+0x1c]
  43b1e9:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  43b1eb:	83 7b 20 04          	cmp    DWORD PTR [ebx+0x20],0x4
  43b1ef:	75 0c                	jne    0x43b1fd
  43b1f1:	8b 4d 04             	mov    ecx,DWORD PTR [ebp+0x4]
  43b1f4:	b8 01 00 00 00       	mov    eax,0x1
  43b1f9:	89 4c 24 1c          	mov    DWORD PTR [esp+0x1c],ecx
  43b1fd:	83 7b 44 02          	cmp    DWORD PTR [ebx+0x44],0x2
  43b201:	0f 85 c7 00 00 00    	jne    0x43b2ce
  43b207:	85 c0                	test   eax,eax
  43b209:	0f 8e bf 00 00 00    	jle    0x43b2ce
  43b20f:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  43b213:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  43b217:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  43b21b:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  43b21f:	8b 7b 4c             	mov    edi,DWORD PTR [ebx+0x4c]
  43b222:	8b 00                	mov    eax,DWORD PTR [eax]
  43b224:	8d 0c 40             	lea    ecx,[eax+eax*2]
  43b227:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  43b22a:	d1 e1                	shl    ecx,1
  43b22c:	2b c8                	sub    ecx,eax
  43b22e:	8b 82 a4 00 00 00    	mov    eax,DWORD PTR [edx+0xa4]
  43b234:	c1 e1 04             	shl    ecx,0x4
  43b237:	03 c8                	add    ecx,eax
  43b239:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  43b23e:	8b 71 2c             	mov    esi,DWORD PTR [ecx+0x2c]
  43b241:	8b d6                	mov    edx,esi
  43b243:	0f af 53 40          	imul   edx,DWORD PTR [ebx+0x40]
  43b247:	f7 ea                	imul   edx
  43b249:	c1 fa 05             	sar    edx,0x5
  43b24c:	8b c2                	mov    eax,edx
  43b24e:	c1 e8 1f             	shr    eax,0x1f
  43b251:	03 d0                	add    edx,eax
  43b253:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  43b258:	03 d7                	add    edx,edi
  43b25a:	8b fa                	mov    edi,edx
  43b25c:	8b 53 58             	mov    edx,DWORD PTR [ebx+0x58]
  43b25f:	0f af d7             	imul   edx,edi
  43b262:	f7 ea                	imul   edx
  43b264:	c1 fa 05             	sar    edx,0x5
  43b267:	8b c2                	mov    eax,edx
  43b269:	c1 e8 1f             	shr    eax,0x1f
  43b26c:	03 d0                	add    edx,eax
  43b26e:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  43b273:	8b ea                	mov    ebp,edx
  43b275:	8b 53 54             	mov    edx,DWORD PTR [ebx+0x54]
  43b278:	0f af d7             	imul   edx,edi
  43b27b:	f7 ea                	imul   edx
  43b27d:	8b 79 30             	mov    edi,DWORD PTR [ecx+0x30]
  43b280:	c1 fa 05             	sar    edx,0x5
  43b283:	8b c2                	mov    eax,edx
  43b285:	c1 e8 1f             	shr    eax,0x1f
  43b288:	03 d0                	add    edx,eax
  43b28a:	03 fa                	add    edi,edx
  43b28c:	8b c7                	mov    eax,edi
  43b28e:	89 79 30             	mov    DWORD PTR [ecx+0x30],edi
  43b291:	3b c6                	cmp    eax,esi
  43b293:	7e 03                	jle    0x43b298
  43b295:	89 71 30             	mov    DWORD PTR [ecx+0x30],esi
  43b298:	8b 79 38             	mov    edi,DWORD PTR [ecx+0x38]
  43b29b:	8b 41 34             	mov    eax,DWORD PTR [ecx+0x34]
  43b29e:	03 fd                	add    edi,ebp
  43b2a0:	8b d7                	mov    edx,edi
  43b2a2:	89 79 38             	mov    DWORD PTR [ecx+0x38],edi
  43b2a5:	3b d0                	cmp    edx,eax
  43b2a7:	7e 03                	jle    0x43b2ac
  43b2a9:	89 41 38             	mov    DWORD PTR [ecx+0x38],eax
  43b2ac:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  43b2b0:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  43b2b4:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
  43b2b8:	83 c1 04             	add    ecx,0x4
  43b2bb:	48                   	dec    eax
  43b2bc:	89 4c 24 10          	mov    DWORD PTR [esp+0x10],ecx
  43b2c0:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  43b2c4:	0f 85 51 ff ff ff    	jne    0x43b21b
  43b2ca:	8b 6c 24 70          	mov    ebp,DWORD PTR [esp+0x70]
  43b2ce:	8b 4d 04             	mov    ecx,DWORD PTR [ebp+0x4]
  43b2d1:	8b b2 a4 00 00 00    	mov    esi,DWORD PTR [edx+0xa4]
  43b2d7:	5f                   	pop    edi
  43b2d8:	8d 04 49             	lea    eax,[ecx+ecx*2]
  43b2db:	8d 04 c0             	lea    eax,[eax+eax*8]
  43b2de:	d1 e0                	shl    eax,1
  43b2e0:	2b c1                	sub    eax,ecx
  43b2e2:	8b 4b 38             	mov    ecx,DWORD PTR [ebx+0x38]
  43b2e5:	c1 e0 04             	shl    eax,0x4
  43b2e8:	03 c6                	add    eax,esi
  43b2ea:	8b 50 38             	mov    edx,DWORD PTR [eax+0x38]
  43b2ed:	2b d1                	sub    edx,ecx
  43b2ef:	89 50 38             	mov    DWORD PTR [eax+0x38],edx
  43b2f2:	79 07                	jns    0x43b2fb
  43b2f4:	c7 40 38 00 00 00 00 	mov    DWORD PTR [eax+0x38],0x0
  43b2fb:	5e                   	pop    esi
  43b2fc:	5d                   	pop    ebp
  43b2fd:	b0 01                	mov    al,0x1
  43b2ff:	5b                   	pop    ebx
  43b300:	83 c4 5c             	add    esp,0x5c
  43b303:	c2 04 00             	ret    0x4
  43b306:	5e                   	pop    esi
  43b307:	5d                   	pop    ebp
  43b308:	32 c0                	xor    al,al
  43b30a:	5b                   	pop    ebx
  43b30b:	83 c4 5c             	add    esp,0x5c
  43b30e:	c2               	ret    0x4
