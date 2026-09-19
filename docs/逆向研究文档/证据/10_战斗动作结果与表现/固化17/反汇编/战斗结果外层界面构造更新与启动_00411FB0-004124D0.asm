; Oracle: RPG.exe SHA256 b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; objdump -d -Mintel, VA 0x411fb0..0x4124d0

  411fb0:	6a ff                	push   0xffffffff
  411fb2:	68 19 d5 45 00       	push   0x45d519
  411fb7:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  411fbd:	50                   	push   eax
  411fbe:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  411fc5:	83 ec 20             	sub    esp,0x20
  411fc8:	53                   	push   ebx
  411fc9:	55                   	push   ebp
  411fca:	56                   	push   esi
  411fcb:	8b e9                	mov    ebp,ecx
  411fcd:	57                   	push   edi
  411fce:	89 6c 24 10          	mov    DWORD PTR [esp+0x10],ebp
  411fd2:	e8 39 ef 01 00       	call   0x430f10
  411fd7:	33 db                	xor    ebx,ebx
  411fd9:	8b cd                	mov    ecx,ebp
  411fdb:	53                   	push   ebx
  411fdc:	68 c0 91 21 00       	push   0x2191c0
  411fe1:	55                   	push   ebp
  411fe2:	89 5c 24 44          	mov    DWORD PTR [esp+0x44],ebx
  411fe6:	c7 45 00 90 07 46 00 	mov    DWORD PTR [ebp+0x0],0x460790
  411fed:	e8 3e 24 02 00       	call   0x434430
  411ff2:	8d 8d 18 02 00 00    	lea    ecx,[ebp+0x218]
  411ff8:	e8 33 2d ff ff       	call   0x404d30
  411ffd:	68 64 93 46 00       	push   0x469364
  412002:	53                   	push   ebx
  412003:	53                   	push   ebx
  412004:	8d b5 68 05 00 00    	lea    esi,[ebp+0x568]
  41200a:	8d bd 6c 05 00 00    	lea    edi,[ebp+0x56c]
  412010:	6a 64                	push   0x64
  412012:	6a 5a                	push   0x5a
  412014:	8b cd                	mov    ecx,ebp
  412016:	89 44 24 40          	mov    DWORD PTR [esp+0x40],eax
  41201a:	89 74 24 2c          	mov    DWORD PTR [esp+0x2c],esi
  41201e:	89 7c 24 30          	mov    DWORD PTR [esp+0x30],edi
  412022:	88 9d 79 05 00 00    	mov    BYTE PTR [ebp+0x579],bl
  412028:	e8 b3 f0 01 00       	call   0x4310e0
  41202d:	8b 8d 64 05 00 00    	mov    ecx,DWORD PTR [ebp+0x564]
  412033:	53                   	push   ebx
  412034:	e8 b7 c6 02 00       	call   0x43e6f0
  412039:	68 d0 09 00 00       	push   0x9d0
  41203e:	89 9d 14 06 00 00    	mov    DWORD PTR [ebp+0x614],ebx
  412044:	89 9d 18 06 00 00    	mov    DWORD PTR [ebp+0x618],ebx
  41204a:	89 9d f8 05 00 00    	mov    DWORD PTR [ebp+0x5f8],ebx
  412050:	89 9d f4 05 00 00    	mov    DWORD PTR [ebp+0x5f4],ebx
  412056:	89 9d fc 05 00 00    	mov    DWORD PTR [ebp+0x5fc],ebx
  41205c:	e8 fe f5 03 00       	call   0x45165f
  412061:	83 c4 04             	add    esp,0x4
  412064:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  412068:	3b c3                	cmp    eax,ebx
  41206a:	c6 44 24 38 01       	mov    BYTE PTR [esp+0x38],0x1
  41206f:	74 09                	je     0x41207a
  412071:	8b c8                	mov    ecx,eax
  412073:	e8 f8 04 00 00       	call   0x412570
  412078:	eb 02                	jmp    0x41207c
  41207a:	33 c0                	xor    eax,eax
  41207c:	6a 48                	push   0x48
  41207e:	88 5c 24 3c          	mov    BYTE PTR [esp+0x3c],bl
  412082:	89 85 00 06 00 00    	mov    DWORD PTR [ebp+0x600],eax
  412088:	e8 d2 f5 03 00       	call   0x45165f
  41208d:	83 c4 04             	add    esp,0x4
  412090:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  412094:	3b c3                	cmp    eax,ebx
  412096:	c6 44 24 38 02       	mov    BYTE PTR [esp+0x38],0x2
  41209b:	74 0e                	je     0x4120ab
  41209d:	8d 4c 24 18          	lea    ecx,[esp+0x18]
  4120a1:	51                   	push   ecx
  4120a2:	8b c8                	mov    ecx,eax
  4120a4:	e8 37 f1 01 00       	call   0x4311e0
  4120a9:	eb 02                	jmp    0x4120ad
  4120ab:	33 c0                	xor    eax,eax
  4120ad:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4120af:	8d 95 04 06 00 00    	lea    edx,[ebp+0x604]
  4120b5:	52                   	push   edx
  4120b6:	53                   	push   ebx
  4120b7:	89 02                	mov    DWORD PTR [edx],eax
  4120b9:	8b 17                	mov    edx,DWORD PTR [edi]
  4120bb:	68 e0 01 00 00       	push   0x1e0
  4120c0:	68 80 02 00 00       	push   0x280
  4120c5:	f7 da                	neg    edx
  4120c7:	f7 d9                	neg    ecx
  4120c9:	52                   	push   edx
  4120ca:	51                   	push   ecx
  4120cb:	8b c8                	mov    ecx,eax
  4120cd:	88 5c 24 50          	mov    BYTE PTR [esp+0x50],bl
  4120d1:	e8 ea f1 01 00       	call   0x4312c0
  4120d6:	bf 0d 00 00 00       	mov    edi,0xd
  4120db:	8d b5 08 06 00 00    	lea    esi,[ebp+0x608]
  4120e1:	6a 48                	push   0x48
  4120e3:	e8 77 f5 03 00       	call   0x45165f
  4120e8:	83 c4 04             	add    esp,0x4
  4120eb:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  4120ef:	3b c3                	cmp    eax,ebx
  4120f1:	c6 44 24 38 03       	mov    BYTE PTR [esp+0x38],0x3
  4120f6:	74 0e                	je     0x412106
  4120f8:	8d 54 24 18          	lea    edx,[esp+0x18]
  4120fc:	8b c8                	mov    ecx,eax
  4120fe:	52                   	push   edx
  4120ff:	e8 ec f7 01 00       	call   0x4318f0
  412104:	eb 02                	jmp    0x412108
  412106:	33 c0                	xor    eax,eax
  412108:	56                   	push   esi
  412109:	68 bc 92 46 00       	push   0x4692bc
  41210e:	57                   	push   edi
  41210f:	6a 19                	push   0x19
  412111:	8b c8                	mov    ecx,eax
  412113:	88 5c 24 48          	mov    BYTE PTR [esp+0x48],bl
  412117:	89 06                	mov    DWORD PTR [esi],eax
  412119:	e8 d2 f8 01 00       	call   0x4319f0
  41211e:	8b 0d b0 b2 46 00    	mov    ecx,DWORD PTR ds:0x46b2b0
  412124:	8b 06                	mov    eax,DWORD PTR [esi]
  412126:	6a 30                	push   0x30
  412128:	6a 40                	push   0x40
  41212a:	8b 11                	mov    edx,DWORD PTR [ecx]
  41212c:	89 50 38             	mov    DWORD PTR [eax+0x38],edx
  41212f:	8b 06                	mov    eax,DWORD PTR [esi]
  412131:	c7 40 30 14 00 00 00 	mov    DWORD PTR [eax+0x30],0x14
  412138:	8b 0e                	mov    ecx,DWORD PTR [esi]
  41213a:	e8 51 f9 01 00       	call   0x431a90
  41213f:	8b 0e                	mov    ecx,DWORD PTR [esi]
  412141:	83 c7 1c             	add    edi,0x1c
  412144:	83 c6 04             	add    esi,0x4
  412147:	83 ff 61             	cmp    edi,0x61
  41214a:	c7 41 40 09 00 00 00 	mov    DWORD PTR [ecx+0x40],0x9
  412151:	7c 8e                	jl     0x4120e1
  412153:	8d b5 7c 05 00 00    	lea    esi,[ebp+0x57c]
  412159:	bf 1e 00 00 00       	mov    edi,0x1e
  41215e:	6a 28                	push   0x28
  412160:	e8 fa f4 03 00       	call   0x45165f
  412165:	89 06                	mov    DWORD PTR [esi],eax
  412167:	83 c4 04             	add    esp,0x4
  41216a:	83 c6 04             	add    esi,0x4
  41216d:	4f                   	dec    edi
  41216e:	75 ee                	jne    0x41215e
  412170:	8b 4c 24 30          	mov    ecx,DWORD PTR [esp+0x30]
  412174:	5f                   	pop    edi
  412175:	8b c5                	mov    eax,ebp
  412177:	5e                   	pop    esi
  412178:	5d                   	pop    ebp
  412179:	5b                   	pop    ebx
  41217a:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  412181:	83 c4 2c             	add    esp,0x2c
  412184:	c3                   	ret
  412185:	90                   	nop
  412186:	90                   	nop
  412187:	90                   	nop
  412188:	90                   	nop
  412189:	90                   	nop
  41218a:	90                   	nop
  41218b:	90                   	nop
  41218c:	90                   	nop
  41218d:	90                   	nop
  41218e:	90                   	nop
  41218f:	90                   	nop
  412190:	6a ff                	push   0xffffffff
  412192:	68 38 d5 45 00       	push   0x45d538
  412197:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  41219d:	50                   	push   eax
  41219e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  4121a5:	51                   	push   ecx
  4121a6:	55                   	push   ebp
  4121a7:	8b e9                	mov    ebp,ecx
  4121a9:	56                   	push   esi
  4121aa:	89 6c 24 08          	mov    DWORD PTR [esp+0x8],ebp
  4121ae:	c7 45 00 90 07 46 00 	mov    DWORD PTR [ebp+0x0],0x460790
  4121b5:	8b b5 04 06 00 00    	mov    esi,DWORD PTR [ebp+0x604]
  4121bb:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  4121c2:	00 
  4121c3:	85 f6                	test   esi,esi
  4121c5:	74 1a                	je     0x4121e1
  4121c7:	8b ce                	mov    ecx,esi
  4121c9:	e8 c2 f0 01 00       	call   0x431290
  4121ce:	56                   	push   esi
  4121cf:	e8 7c f3 03 00       	call   0x451550
  4121d4:	83 c4 04             	add    esp,0x4
  4121d7:	c7 85 04 06 00 00 00 	mov    DWORD PTR [ebp+0x604],0x0
  4121de:	00 00 00 
  4121e1:	8b b5 00 06 00 00    	mov    esi,DWORD PTR [ebp+0x600]
  4121e7:	85 f6                	test   esi,esi
  4121e9:	74 1a                	je     0x412205
  4121eb:	8b ce                	mov    ecx,esi
  4121ed:	e8 6e 08 00 00       	call   0x412a60
  4121f2:	56                   	push   esi
  4121f3:	e8 58 f3 03 00       	call   0x451550
  4121f8:	83 c4 04             	add    esp,0x4
  4121fb:	c7 85 00 06 00 00 00 	mov    DWORD PTR [ebp+0x600],0x0
  412202:	00 00 00 
  412205:	53                   	push   ebx
  412206:	57                   	push   edi
  412207:	8d bd 08 06 00 00    	lea    edi,[ebp+0x608]
  41220d:	bb 03 00 00 00       	mov    ebx,0x3
  412212:	8b 37                	mov    esi,DWORD PTR [edi]
  412214:	85 f6                	test   esi,esi
  412216:	74 16                	je     0x41222e
  412218:	8b ce                	mov    ecx,esi
  41221a:	e8 81 f7 01 00       	call   0x4319a0
  41221f:	56                   	push   esi
  412220:	e8 2b f3 03 00       	call   0x451550
  412225:	83 c4 04             	add    esp,0x4
  412228:	c7 07 00 00 00 00    	mov    DWORD PTR [edi],0x0
  41222e:	83 c7 04             	add    edi,0x4
  412231:	4b                   	dec    ebx
  412232:	75 de                	jne    0x412212
  412234:	8d b5 7c 05 00 00    	lea    esi,[ebp+0x57c]
  41223a:	bf 1e 00 00 00       	mov    edi,0x1e
  41223f:	8b 06                	mov    eax,DWORD PTR [esi]
  412241:	85 c0                	test   eax,eax
  412243:	74 0f                	je     0x412254
  412245:	50                   	push   eax
  412246:	e8 05 f3 03 00       	call   0x451550
  41224b:	83 c4 04             	add    esp,0x4
  41224e:	c7 06 00 00 00 00    	mov    DWORD PTR [esi],0x0
  412254:	83 c6 04             	add    esi,0x4
  412257:	4f                   	dec    edi
  412258:	75 e5                	jne    0x41223f
  41225a:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  412260:	5f                   	pop    edi
  412261:	85 c9                	test   ecx,ecx
  412263:	5b                   	pop    ebx
  412264:	74 07                	je     0x41226d
  412266:	6a ff                	push   0xffffffff
  412268:	e8 a3 34 ff ff       	call   0x405710
  41226d:	8b cd                	mov    ecx,ebp
  41226f:	c7 44 24 14 ff ff ff 	mov    DWORD PTR [esp+0x14],0xffffffff
  412276:	ff 
  412277:	e8 c4 ed 01 00       	call   0x431040
  41227c:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  412280:	5e                   	pop    esi
  412281:	5d                   	pop    ebp
  412282:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  412289:	83 c4 10             	add    esp,0x10
  41228c:	c3                   	ret
  41228d:	90                   	nop
  41228e:	90                   	nop
  41228f:	90                   	nop
  412290:	53                   	push   ebx
  412291:	55                   	push   ebp
  412292:	56                   	push   esi
  412293:	8b f1                	mov    esi,ecx
  412295:	33 db                	xor    ebx,ebx
  412297:	8b 86 04 06 00 00    	mov    eax,DWORD PTR [esi+0x604]
  41229d:	8a 8e 79 05 00 00    	mov    cl,BYTE PTR [esi+0x579]
  4122a3:	8d ae 08 06 00 00    	lea    ebp,[esi+0x608]
  4122a9:	88 48 45             	mov    BYTE PTR [eax+0x45],cl
  4122ac:	8a 86 79 05 00 00    	mov    al,BYTE PTR [esi+0x579]
  4122b2:	8b 55 00             	mov    edx,DWORD PTR [ebp+0x0]
  4122b5:	88 42 34             	mov    BYTE PTR [edx+0x34],al
  4122b8:	8b 8e 0c 06 00 00    	mov    ecx,DWORD PTR [esi+0x60c]
  4122be:	8a 96 79 05 00 00    	mov    dl,BYTE PTR [esi+0x579]
  4122c4:	88 51 34             	mov    BYTE PTR [ecx+0x34],dl
  4122c7:	8b 86 10 06 00 00    	mov    eax,DWORD PTR [esi+0x610]
  4122cd:	8a 8e 79 05 00 00    	mov    cl,BYTE PTR [esi+0x579]
  4122d3:	88 48 34             	mov    BYTE PTR [eax+0x34],cl
  4122d6:	8a 86 79 05 00 00    	mov    al,BYTE PTR [esi+0x579]
  4122dc:	3a c3                	cmp    al,bl
  4122de:	0f 84 4a 01 00 00    	je     0x41242e
  4122e4:	8b 86 18 06 00 00    	mov    eax,DWORD PTR [esi+0x618]
  4122ea:	3b c3                	cmp    eax,ebx
  4122ec:	0f 84 8f 00 00 00    	je     0x412381
  4122f2:	ba 01 00 00 00       	mov    edx,0x1
  4122f7:	3b c2                	cmp    eax,edx
  4122f9:	8b 86 14 06 00 00    	mov    eax,DWORD PTR [esi+0x614]
  4122ff:	75 05                	jne    0x412306
  412301:	8d 0c 40             	lea    ecx,[eax+eax*2]
  412304:	eb 0a                	jmp    0x412310
  412306:	8d 04 40             	lea    eax,[eax+eax*2]
  412309:	b9 10 00 00 00       	mov    ecx,0x10
  41230e:	2b c8                	sub    ecx,eax
  412310:	8b 86 64 05 00 00    	mov    eax,DWORD PTR [esi+0x564]
  412316:	89 90 80 00 00 00    	mov    DWORD PTR [eax+0x80],edx
  41231c:	89 88 84 00 00 00    	mov    DWORD PTR [eax+0x84],ecx
  412322:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412328:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  41232e:	8b 8e 14 06 00 00    	mov    ecx,DWORD PTR [esi+0x614]
  412334:	41                   	inc    ecx
  412335:	8b c1                	mov    eax,ecx
  412337:	89 8e 14 06 00 00    	mov    DWORD PTR [esi+0x614],ecx
  41233d:	83 f8 05             	cmp    eax,0x5
  412340:	0f 8c e8 00 00 00    	jl     0x41242e
  412346:	8b 86 64 05 00 00    	mov    eax,DWORD PTR [esi+0x564]
  41234c:	c7 80 80 00 00 00 ff 	mov    DWORD PTR [eax+0x80],0xffffffff
  412353:	ff ff ff 
  412356:	89 98 84 00 00 00    	mov    DWORD PTR [eax+0x84],ebx
  41235c:	89 98 88 00 00 00    	mov    DWORD PTR [eax+0x88],ebx
  412362:	89 98 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],ebx
  412368:	83 be 18 06 00 00 02 	cmp    DWORD PTR [esi+0x618],0x2
  41236f:	75 06                	jne    0x412377
  412371:	88 9e 79 05 00 00    	mov    BYTE PTR [esi+0x579],bl
  412377:	89 9e 18 06 00 00    	mov    DWORD PTR [esi+0x618],ebx
  41237d:	5e                   	pop    esi
  41237e:	5d                   	pop    ebp
  41237f:	5b                   	pop    ebx
  412380:	c3                   	ret
  412381:	8b 8e 00 06 00 00    	mov    ecx,DWORD PTR [esi+0x600]
  412387:	38 99 79 05 00 00    	cmp    BYTE PTR [ecx+0x579],bl
  41238d:	0f 85 9b 00 00 00    	jne    0x41242e
  412393:	8b 8e 04 06 00 00    	mov    ecx,DWORD PTR [esi+0x604]
  412399:	e8 e2 ef 01 00       	call   0x431380
  41239e:	85 c0                	test   eax,eax
  4123a0:	74 48                	je     0x4123ea
  4123a2:	8b 86 f8 05 00 00    	mov    eax,DWORD PTR [esi+0x5f8]
  4123a8:	8b 8e f4 05 00 00    	mov    ecx,DWORD PTR [esi+0x5f4]
  4123ae:	83 c0 03             	add    eax,0x3
  4123b1:	3b c1                	cmp    eax,ecx
  4123b3:	7c 29                	jl     0x4123de
  4123b5:	8b ce                	mov    ecx,esi
  4123b7:	e8 d4 00 00 00       	call   0x412490
  4123bc:	8b 8e 00 06 00 00    	mov    ecx,DWORD PTR [esi+0x600]
  4123c2:	39 99 4c 06 00 00    	cmp    DWORD PTR [ecx+0x64c],ebx
  4123c8:	7e 1a                	jle    0x4123e4
  4123ca:	e8 f1 0f 00 00       	call   0x4133c0
  4123cf:	8b 96 00 06 00 00    	mov    edx,DWORD PTR [esi+0x600]
  4123d5:	c6 82 79 05 00 00 01 	mov    BYTE PTR [edx+0x579],0x1
  4123dc:	eb 06                	jmp    0x4123e4
  4123de:	89 86 f8 05 00 00    	mov    DWORD PTR [esi+0x5f8],eax
  4123e4:	89 9e fc 05 00 00    	mov    DWORD PTR [esi+0x5fc],ebx
  4123ea:	8b 86 fc 05 00 00    	mov    eax,DWORD PTR [esi+0x5fc]
  4123f0:	57                   	push   edi
  4123f1:	40                   	inc    eax
  4123f2:	33 ff                	xor    edi,edi
  4123f4:	89 86 fc 05 00 00    	mov    DWORD PTR [esi+0x5fc],eax
  4123fa:	8b dd                	mov    ebx,ebp
  4123fc:	8b 86 f8 05 00 00    	mov    eax,DWORD PTR [esi+0x5f8]
  412402:	8b 8e f4 05 00 00    	mov    ecx,DWORD PTR [esi+0x5f4]
  412408:	03 c7                	add    eax,edi
  41240a:	3b c1                	cmp    eax,ecx
  41240c:	7d 0a                	jge    0x412418
  41240e:	8b 8c 86 7c 05 00 00 	mov    ecx,DWORD PTR [esi+eax*4+0x57c]
  412415:	51                   	push   ecx
  412416:	eb 05                	jmp    0x41241d
  412418:	68 4c 93 46 00       	push   0x46934c
  41241d:	8b 0b                	mov    ecx,DWORD PTR [ebx]
  41241f:	e8 fc f5 01 00       	call   0x431a20
  412424:	47                   	inc    edi
  412425:	83 c3 04             	add    ebx,0x4
  412428:	83 ff 03             	cmp    edi,0x3
  41242b:	7c cf                	jl     0x4123fc
  41242d:	5f                   	pop    edi
  41242e:	5e                   	pop    esi
  41242f:	5d                   	pop    ebp
  412430:	5b                   	pop    ebx
  412431:	c3                   	ret
  412432:	90                   	nop
  412433:	90                   	nop
  412434:	90                   	nop
  412435:	90                   	nop
  412436:	90                   	nop
  412437:	90                   	nop
  412438:	90                   	nop
  412439:	90                   	nop
  41243a:	90                   	nop
  41243b:	90                   	nop
  41243c:	90                   	nop
  41243d:	90                   	nop
  41243e:	90                   	nop
  41243f:	90                   	nop
  412440:	8b 81 64 05 00 00    	mov    eax,DWORD PTR [ecx+0x564]
  412446:	33 d2                	xor    edx,edx
  412448:	89 91 14 06 00 00    	mov    DWORD PTR [ecx+0x614],edx
  41244e:	c7 80 80 00 00 00 01 	mov    DWORD PTR [eax+0x80],0x1
  412455:	00 00 00 
  412458:	89 90 84 00 00 00    	mov    DWORD PTR [eax+0x84],edx
  41245e:	89 90 88 00 00 00    	mov    DWORD PTR [eax+0x88],edx
  412464:	89 90 8c 00 00 00    	mov    DWORD PTR [eax+0x8c],edx
  41246a:	c6 81 79 05 00 00 01 	mov    BYTE PTR [ecx+0x579],0x1
  412471:	c7 81 18 06 00 00 01 	mov    DWORD PTR [ecx+0x618],0x1
  412478:	00 00 00 
  41247b:	89 91 f8 05 00 00    	mov    DWORD PTR [ecx+0x5f8],edx
  412481:	c3                   	ret
  412482:	90                   	nop
  412483:	90                   	nop
  412484:	90                   	nop
  412485:	90                   	nop
  412486:	90                   	nop
  412487:	90                   	nop
  412488:	90                   	nop
  412489:	90                   	nop
  41248a:	90                   	nop
  41248b:	90                   	nop
  41248c:	90                   	nop
  41248d:	90                   	nop
  41248e:	90                   	nop
  41248f:	90                   	nop
  412490:	c7 81 14 06 00 00 00 	mov    DWORD PTR [ecx+0x614],0x0
  412497:	00 00 00 
  41249a:	c7 81 18 06 00 00 02 	mov    DWORD PTR [ecx+0x618],0x2
  4124a1:	00 00 00 
  4124a4:	c3                   	ret
  4124a5:	90                   	nop
  4124a6:	90                   	nop
  4124a7:	90                   	nop
  4124a8:	90                   	nop
  4124a9:	90                   	nop
  4124aa:	90                   	nop
  4124ab:	90                   	nop
  4124ac:	90                   	nop
  4124ad:	90                   	nop
  4124ae:	90                   	nop
  4124af:	90                   	nop
  4124b0:	c7 81 f4 05 00 00 00 	mov    DWORD PTR [ecx+0x5f4],0x0
  4124b7:	00 00 00 
  4124ba:	c3                   	ret
  4124bb:	90                   	nop
  4124bc:	90                   	nop
  4124bd:	90                   	nop
  4124be:	90                   	nop
  4124bf:	90                   	nop
  4124c0:	53                   	push   ebx
  4124c1:	55                   	push   ebp
  4124c2:	56                   	push   esi
  4124c3:	8b f1                	mov    esi,ecx
  4124c5:	57                   	push   edi
  4124c6:	83 be f4 05 00 00 1e 	cmp    DWORD PTR [esi+0x5f4],0x1e
  4124cd:	0f 8d 93 00 00 00    	jge    0x412566
