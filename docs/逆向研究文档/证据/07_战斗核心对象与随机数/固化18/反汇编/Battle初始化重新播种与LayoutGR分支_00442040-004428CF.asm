  442040:	6a ff                	push   0xffffffff
  442042:	68 9e f9 45 00       	push   0x45f99e
  442047:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  44204d:	50                   	push   eax
  44204e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  442055:	83 ec 70             	sub    esp,0x70
  442058:	53                   	push   ebx
  442059:	55                   	push   ebp
  44205a:	56                   	push   esi
  44205b:	8b e9                	mov    ebp,ecx
  44205d:	33 db                	xor    ebx,ebx
  44205f:	57                   	push   edi
  442060:	53                   	push   ebx
  442061:	8d 4c 24 20          	lea    ecx,[esp+0x20]
  442065:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  442069:	e8 82 f6 ff ff       	call   0x4416f0
  44206e:	53                   	push   ebx
  44206f:	89 9c 24 8c 00 00 00 	mov    DWORD PTR [esp+0x8c],ebx
  442076:	e8 08 f5 00 00       	call   0x451583
  44207b:	50                   	push   eax
  44207c:	e8 da f4 00 00       	call   0x45155b
  442081:	a1 c0 f6 89 00       	mov    eax,ds:0x89f6c0
  442086:	83 c4 08             	add    esp,0x8
  442089:	8b 78 68             	mov    edi,DWORD PTR [eax+0x68]
  44208c:	8b 48 70             	mov    ecx,DWORD PTR [eax+0x70]
  44208f:	8b 50 6c             	mov    edx,DWORD PTR [eax+0x6c]
  442092:	8b 70 28             	mov    esi,DWORD PTR [eax+0x28]
  442095:	8b 40 3c             	mov    eax,DWORD PTR [eax+0x3c]
  442098:	50                   	push   eax
  442099:	8b 46 08             	mov    eax,DWORD PTR [esi+0x8]
  44209c:	51                   	push   ecx
  44209d:	52                   	push   edx
  44209e:	8d 0c 48             	lea    ecx,[eax+ecx*2]
  4420a1:	8b 46 04             	mov    eax,DWORD PTR [esi+0x4]
  4420a4:	51                   	push   ecx
  4420a5:	8d 0c 50             	lea    ecx,[eax+edx*2]
  4420a8:	51                   	push   ecx
  4420a9:	57                   	push   edi
  4420aa:	8b cd                	mov    ecx,ebp
  4420ac:	e8 5f b6 ff ff       	call   0x43d710
  4420b1:	8b cd                	mov    ecx,ebp
  4420b3:	e8 18 08 00 00       	call   0x4428d0
  4420b8:	6a 14                	push   0x14
  4420ba:	e8 a0 f5 00 00       	call   0x45165f
  4420bf:	83 c4 04             	add    esp,0x4
  4420c2:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  4420c6:	85 c0                	test   eax,eax
  4420c8:	c6 84 24 88 00 00 00 	mov    BYTE PTR [esp+0x88],0x1
  4420cf:	01 
  4420d0:	74 0d                	je     0x4420df
  4420d2:	6a 01                	push   0x1
  4420d4:	8b c8                	mov    ecx,eax
  4420d6:	e8 15 f6 ff ff       	call   0x4416f0
  4420db:	8b f0                	mov    esi,eax
  4420dd:	eb 02                	jmp    0x4420e1
  4420df:	33 f6                	xor    esi,esi
  4420e1:	6a 00                	push   0x0
  4420e3:	6a 01                	push   0x1
  4420e5:	68 b8 c4 46 00       	push   0x46c4b8
  4420ea:	8b ce                	mov    ecx,esi
  4420ec:	c6 84 24 94 00 00 00 	mov    BYTE PTR [esp+0x94],0x0
  4420f3:	00 
  4420f4:	e8 c7 f6 ff ff       	call   0x4417c0
  4420f9:	84 c0                	test   al,al
  4420fb:	0f 84 04 01 00 00    	je     0x442205
  442101:	8b ce                	mov    ecx,esi
  442103:	e8 28 ad fe ff       	call   0x42ce30
  442108:	8b f8                	mov    edi,eax
  44210a:	c1 e8 02             	shr    eax,0x2
  44210d:	a3 fc 23 8e 00       	mov    ds:0x8e23fc,eax
  442112:	8d 14 85 00 00 00 00 	lea    edx,[eax*4+0x0]
  442119:	52                   	push   edx
  44211a:	e8 40 f5 00 00       	call   0x45165f
  44211f:	83 c4 04             	add    esp,0x4
  442122:	8b ce                	mov    ecx,esi
  442124:	a3 f8 23 8e 00       	mov    ds:0x8e23f8,eax
  442129:	50                   	push   eax
  44212a:	57                   	push   edi
  44212b:	6a 00                	push   0x0
  44212d:	e8 fe f8 ff ff       	call   0x441a30
  442132:	a1 f8 23 8e 00       	mov    eax,ds:0x8e23f8
  442137:	57                   	push   edi
  442138:	50                   	push   eax
  442139:	e8 92 63 fe ff       	call   0x4284d0
  44213e:	83 c4 08             	add    esp,0x8
  442141:	8b ce                	mov    ecx,esi
  442143:	e8 b8 f8 ff ff       	call   0x441a00
  442148:	6a 00                	push   0x0
  44214a:	6a 01                	push   0x1
  44214c:	68 a4 c4 46 00       	push   0x46c4a4
  442151:	8b ce                	mov    ecx,esi
  442153:	e8 68 f6 ff ff       	call   0x4417c0
  442158:	84 c0                	test   al,al
  44215a:	0f 84 a5 00 00 00    	je     0x442205
  442160:	8b ce                	mov    ecx,esi
  442162:	e8 c9 ac fe ff       	call   0x42ce30
  442167:	8b f8                	mov    edi,eax
  442169:	c1 e8 02             	shr    eax,0x2
  44216c:	a3 04 24 8e 00       	mov    ds:0x8e2404,eax
  442171:	8d 0c 85 00 00 00 00 	lea    ecx,[eax*4+0x0]
  442178:	51                   	push   ecx
  442179:	e8 e1 f4 00 00       	call   0x45165f
  44217e:	83 c4 04             	add    esp,0x4
  442181:	8b ce                	mov    ecx,esi
  442183:	a3 00 24 8e 00       	mov    ds:0x8e2400,eax
  442188:	50                   	push   eax
  442189:	57                   	push   edi
  44218a:	6a 00                	push   0x0
  44218c:	e8 9f f8 ff ff       	call   0x441a30
  442191:	8b 15 00 24 8e 00    	mov    edx,DWORD PTR ds:0x8e2400
  442197:	57                   	push   edi
  442198:	52                   	push   edx
  442199:	e8 32 63 fe ff       	call   0x4284d0
  44219e:	83 c4 08             	add    esp,0x8
  4421a1:	8b ce                	mov    ecx,esi
  4421a3:	e8 58 f8 ff ff       	call   0x441a00
  4421a8:	85 f6                	test   esi,esi
  4421aa:	74 10                	je     0x4421bc
  4421ac:	8b ce                	mov    ecx,esi
  4421ae:	e8 5d f5 ff ff       	call   0x441710
  4421b3:	56                   	push   esi
  4421b4:	e8 97 f3 00 00       	call   0x451550
  4421b9:	83 c4 04             	add    esp,0x4
  4421bc:	68 dc 09 00 00       	push   0x9dc
  4421c1:	e8 99 f4 00 00       	call   0x45165f
  4421c6:	83 c4 04             	add    esp,0x4
  4421c9:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  4421cd:	85 c0                	test   eax,eax
  4421cf:	c6 84 24 88 00 00 00 	mov    BYTE PTR [esp+0x88],0x2
  4421d6:	02 
  4421d7:	74 09                	je     0x4421e2
  4421d9:	8b c8                	mov    ecx,eax
  4421db:	e8 70 49 00 00       	call   0x446b50
  4421e0:	eb 02                	jmp    0x4421e4
  4421e2:	33 c0                	xor    eax,eax
  4421e4:	85 c0                	test   eax,eax
  4421e6:	c6 84 24 88 00 00 00 	mov    BYTE PTR [esp+0x88],0x0
  4421ed:	00 
  4421ee:	a3 14 24 8e 00       	mov    ds:0x8e2414,eax
  4421f3:	75 39                	jne    0x44222e
  4421f5:	68 84 c4 46 00       	push   0x46c484
  4421fa:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  442200:	e8 6b fd fe ff       	call   0x431f70
  442205:	c7 84 24 88 00 00 00 	mov    DWORD PTR [esp+0x88],0xffffffff
  44220c:	ff ff ff ff 
  442210:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  442214:	e8 f7 f4 ff ff       	call   0x441710
  442219:	5f                   	pop    edi
  44221a:	5e                   	pop    esi
  44221b:	5d                   	pop    ebp
  44221c:	32 c0                	xor    al,al
  44221e:	5b                   	pop    ebx
  44221f:	8b 4c 24 70          	mov    ecx,DWORD PTR [esp+0x70]
  442223:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  44222a:	83 c4 7c             	add    esp,0x7c
  44222d:	c3                   	ret
  44222e:	8b cd                	mov    ecx,ebp
  442230:	e8 8b 1e 00 00       	call   0x4440c0
  442235:	84 c0                	test   al,al
  442237:	75 07                	jne    0x442240
  442239:	68 6c c4 46 00       	push   0x46c46c
  44223e:	eb ba                	jmp    0x4421fa
  442240:	6a 01                	push   0x1
  442242:	e8 a9 36 00 00       	call   0x4458f0
  442247:	83 c4 04             	add    esp,0x4
  44224a:	85 c0                	test   eax,eax
  44224c:	74 09                	je     0x442257
  44224e:	bb 01 00 00 00       	mov    ebx,0x1
  442253:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  442257:	6a 03                	push   0x3
  442259:	e8 92 36 00 00       	call   0x4458f0
  44225e:	83 c4 04             	add    esp,0x4
  442261:	85 c0                	test   eax,eax
  442263:	74 09                	je     0x44226e
  442265:	bb 02 00 00 00       	mov    ebx,0x2
  44226a:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  44226e:	6a 04                	push   0x4
  442270:	e8 7b 36 00 00       	call   0x4458f0
  442275:	83 c4 04             	add    esp,0x4
  442278:	85 c0                	test   eax,eax
  44227a:	74 57                	je     0x4422d3
  44227c:	33 db                	xor    ebx,ebx
  44227e:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  442282:	8d 85 a4 03 00 00    	lea    eax,[ebp+0x3a4]
  442288:	b9 10 00 00 00       	mov    ecx,0x10
  44228d:	83 cf ff             	or     edi,0xffffffff
  442290:	89 b8 98 e1 00 00    	mov    DWORD PTR [eax+0xe198],edi
  442296:	89 38                	mov    DWORD PTR [eax],edi
  442298:	83 c0 04             	add    eax,0x4
  44229b:	49                   	dec    ecx
  44229c:	75 f2                	jne    0x442290
  44229e:	83 fb 02             	cmp    ebx,0x2
  4422a1:	0f 85 5c 01 00 00    	jne    0x442403
  4422a7:	33 c0                	xor    eax,eax
  4422a9:	8d 74 24 30          	lea    esi,[esp+0x30]
  4422ad:	8d 8d 3c e5 00 00    	lea    ecx,[ebp+0xe53c]
  4422b3:	8b 16                	mov    edx,DWORD PTR [esi]
  4422b5:	85 d2                	test   edx,edx
  4422b7:	0f 8e e5 00 00 00    	jle    0x4423a2
  4422bd:	83 f8 08             	cmp    eax,0x8
  4422c0:	89 91 68 1e ff ff    	mov    DWORD PTR [ecx-0xe198],edx
  4422c6:	0f 8d d1 00 00 00    	jge    0x44239d
  4422cc:	89 01                	mov    DWORD PTR [ecx],eax
  4422ce:	e9 cf 00 00 00       	jmp    0x4423a2
  4422d3:	6a 00                	push   0x0
  4422d5:	6a 01                	push   0x1
  4422d7:	68 5c c4 46 00       	push   0x46c45c
  4422dc:	8d 4c 24 28          	lea    ecx,[esp+0x28]
  4422e0:	e8 db f4 ff ff       	call   0x4417c0
  4422e5:	84 c0                	test   al,al
  4422e7:	74 93                	je     0x44227c
  4422e9:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  4422ed:	e8 0e f7 ff ff       	call   0x441a00
  4422f2:	85 db                	test   ebx,ebx
  4422f4:	74 8c                	je     0x442282
  4422f6:	83 fb 01             	cmp    ebx,0x1
  4422f9:	75 24                	jne    0x44231f
  4422fb:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  442301:	68 20 c4 46 00       	push   0x46c420
  442306:	e8 65 fc fe ff       	call   0x431f70
  44230b:	6a 02                	push   0x2
  44230d:	e8 de 35 00 00       	call   0x4458f0
  442312:	83 c4 04             	add    esp,0x4
  442315:	a3 08 24 8e 00       	mov    ds:0x8e2408,eax
  44231a:	e9 63 ff ff ff       	jmp    0x442282
  44231f:	83 fb 02             	cmp    ebx,0x2
  442322:	0f 85 5a ff ff ff    	jne    0x442282
  442328:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  44232e:	68 e8 c3 46 00       	push   0x46c3e8
  442333:	e8 38 fc fe ff       	call   0x431f70
  442338:	6a 00                	push   0x0
  44233a:	6a 01                	push   0x1
  44233c:	68 d4 c3 46 00       	push   0x46c3d4
  442341:	8d 4c 24 28          	lea    ecx,[esp+0x28]
  442345:	c6 44 24 38 01       	mov    BYTE PTR [esp+0x38],0x1
  44234a:	e8 71 f4 ff ff       	call   0x4417c0
  44234f:	84 c0                	test   al,al
  442351:	74 30                	je     0x442383
  442353:	8d 44 24 30          	lea    eax,[esp+0x30]
  442357:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  44235b:	50                   	push   eax
  44235c:	6a 40                	push   0x40
  44235e:	6a 00                	push   0x0
  442360:	e8 cb f6 ff ff       	call   0x441a30
  442365:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  442369:	e8 92 f6 ff ff       	call   0x441a00
  44236e:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  442374:	68 a8 c3 46 00       	push   0x46c3a8
  442379:	e8 f2 fb fe ff       	call   0x431f70
  44237e:	e9 ff fe ff ff       	jmp    0x442282
  442383:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  442389:	68 94 c3 46 00       	push   0x46c394
  44238e:	68 6c c3 46 00       	push   0x46c36c
  442393:	e8 b8 fc fe ff       	call   0x432050
  442398:	e9 68 fe ff ff       	jmp    0x442205
  44239d:	8d 50 0c             	lea    edx,[eax+0xc]
  4423a0:	89 11                	mov    DWORD PTR [ecx],edx
  4423a2:	40                   	inc    eax
  4423a3:	83 c6 04             	add    esi,0x4
  4423a6:	83 c1 04             	add    ecx,0x4
  4423a9:	83 f8 10             	cmp    eax,0x10
  4423ac:	0f 8c 01 ff ff ff    	jl     0x4422b3
  4423b2:	8d 85 31 e3 00 00    	lea    eax,[ebp+0xe331]
  4423b8:	50                   	push   eax
  4423b9:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  4423be:	8b 88 d8 00 00 00    	mov    ecx,DWORD PTR [eax+0xd8]
  4423c4:	51                   	push   ecx
  4423c5:	8b 88 e8 00 00 00    	mov    ecx,DWORD PTR [eax+0xe8]
  4423cb:	e8 60 20 fe ff       	call   0x424430
  4423d0:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4423d6:	89 82 dc 00 00 00    	mov    DWORD PTR [edx+0xdc],eax
  4423dc:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  4423e1:	8b 88 e8 00 00 00    	mov    ecx,DWORD PTR [eax+0xe8]
  4423e7:	e8 a4 21 fe ff       	call   0x424590
  4423ec:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  4423f2:	89 81 e4 00 00 00    	mov    DWORD PTR [ecx+0xe4],eax
  4423f8:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4423fe:	e9 24 02 00 00       	jmp    0x442627
  442403:	83 fb 01             	cmp    ebx,0x1
  442406:	0f 85 c2 00 00 00    	jne    0x4424ce
  44240c:	6a 05                	push   0x5
  44240e:	c7 85 b8 03 00 00 fb 	mov    DWORD PTR [ebp+0x3b8],0xfb
  442415:	00 00 00 
  442418:	c7 85 50 e5 00 00 05 	mov    DWORD PTR [ebp+0xe550],0x5
  44241f:	00 00 00 
  442422:	c7 85 c8 03 00 00 fc 	mov    DWORD PTR [ebp+0x3c8],0xfc
  442429:	00 00 00 
  44242c:	c7 85 60 e5 00 00 15 	mov    DWORD PTR [ebp+0xe560],0x15
  442433:	00 00 00 
  442436:	e8 b5 34 00 00       	call   0x4458f0
  44243b:	83 c4 04             	add    esp,0x4
  44243e:	3b c3                	cmp    eax,ebx
  442440:	75 42                	jne    0x442484
  442442:	33 c9                	xor    ecx,ecx
  442444:	b8 a8 23 8e 00       	mov    eax,0x8e23a8
  442449:	8d b5 3c e5 00 00    	lea    esi,[ebp+0xe53c]
  44244f:	83 38 00             	cmp    DWORD PTR [eax],0x0
  442452:	74 22                	je     0x442476
  442454:	33 d2                	xor    edx,edx
  442456:	83 f9 08             	cmp    ecx,0x8
  442459:	0f 9d c2             	setge  dl
  44245c:	81 c2 fb 00 00 00    	add    edx,0xfb
  442462:	3d c8 23 8e 00       	cmp    eax,0x8e23c8
  442467:	89 96 68 1e ff ff    	mov    DWORD PTR [esi-0xe198],edx
  44246d:	8b d1                	mov    edx,ecx
  44246f:	7c 03                	jl     0x442474
  442471:	8d 51 0c             	lea    edx,[ecx+0xc]
  442474:	89 16                	mov    DWORD PTR [esi],edx
  442476:	83 c0 04             	add    eax,0x4
  442479:	41                   	inc    ecx
  44247a:	83 c6 04             	add    esi,0x4
  44247d:	3d e8 23 8e 00       	cmp    eax,0x8e23e8
  442482:	7c cb                	jl     0x44244f
  442484:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  442489:	8d 8d 31 e3 00 00    	lea    ecx,[ebp+0xe331]
  44248f:	51                   	push   ecx
  442490:	c7 80 d8 00 00 00 01 	mov    DWORD PTR [eax+0xd8],0x1
  442497:	00 00 00 
  44249a:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  44249f:	8b 90 d8 00 00 00    	mov    edx,DWORD PTR [eax+0xd8]
  4424a5:	8b 88 e8 00 00 00    	mov    ecx,DWORD PTR [eax+0xe8]
  4424ab:	52                   	push   edx
  4424ac:	e8 7f 1f fe ff       	call   0x424430
  4424b1:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  4424b7:	89 81 dc 00 00 00    	mov    DWORD PTR [ecx+0xdc],eax
  4424bd:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4424c3:	8b 8a e8 00 00 00    	mov    ecx,DWORD PTR [edx+0xe8]
  4424c9:	e9 19 ff ff ff       	jmp    0x4423e7
  4424ce:	6a 06                	push   0x6
  4424d0:	e8 1b 34 00 00       	call   0x4458f0
  4424d5:	83 c4 04             	add    esp,0x4
  4424d8:	85 c0                	test   eax,eax
  4424da:	74 18                	je     0x4424f4
  4424dc:	6a 06                	push   0x6
  4424de:	e8 0d 34 00 00       	call   0x4458f0
  4424e3:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4424e9:	83 c4 04             	add    esp,0x4
  4424ec:	89 82 d8 00 00 00    	mov    DWORD PTR [edx+0xd8],eax
  4424f2:	eb 28                	jmp    0x44251c
  4424f4:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  4424f9:	8b 88 d8 00 00 00    	mov    ecx,DWORD PTR [eax+0xd8]
  4424ff:	85 c9                	test   ecx,ecx
  442501:	75 19                	jne    0x44251c
  442503:	e8 5d f0 00 00       	call   0x451565
  442508:	99                   	cdq
  442509:	b9 06 00 00 00       	mov    ecx,0x6
  44250e:	f7 f9                	idiv   ecx
  442510:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  442515:	42                   	inc    edx
  442516:	89 90 d8 00 00 00    	mov    DWORD PTR [eax+0xd8],edx
  44251c:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  442522:	8d b5 31 e3 00 00    	lea    esi,[ebp+0xe331]
  442528:	56                   	push   esi
  442529:	8b 81 d8 00 00 00    	mov    eax,DWORD PTR [ecx+0xd8]
  44252f:	8b 89 e8 00 00 00    	mov    ecx,DWORD PTR [ecx+0xe8]
  442535:	50                   	push   eax
  442536:	e8 f5 1e fe ff       	call   0x424430
  44253b:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  442541:	89 81 dc 00 00 00    	mov    DWORD PTR [ecx+0xdc],eax
  442547:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  44254d:	8b 81 e0 00 00 00    	mov    eax,DWORD PTR [ecx+0xe0]
  442553:	85 c0                	test   eax,eax
  442555:	7e 0c                	jle    0x442563
  442557:	89 81 dc 00 00 00    	mov    DWORD PTR [ecx+0xdc],eax
  44255d:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  442563:	8b 89 e8 00 00 00    	mov    ecx,DWORD PTR [ecx+0xe8]
  442569:	e8 22 20 fe ff       	call   0x424590
  44256e:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  442574:	89 82 e4 00 00 00    	mov    DWORD PTR [edx+0xe4],eax
  44257a:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  44257f:	8b 88 dc 00 00 00    	mov    ecx,DWORD PTR [eax+0xdc]
  442585:	85 c9                	test   ecx,ecx
  442587:	0f 8e 09 03 00 00    	jle    0x442896
  44258d:	8b 06                	mov    eax,DWORD PTR [esi]
  44258f:	33 d2                	xor    edx,edx
  442591:	85 c0                	test   eax,eax
  442593:	7e 26                	jle    0x4425bb
  442595:	8d 85 55 e3 00 00    	lea    eax,[ebp+0xe355]
  44259b:	8b 08                	mov    ecx,DWORD PTR [eax]
  44259d:	42                   	inc    edx
  44259e:	83 c0 04             	add    eax,0x4
  4425a1:	89 8c 8d 3c e5 00 00 	mov    DWORD PTR [ebp+ecx*4+0xe53c],ecx
  4425a8:	8b 48 fc             	mov    ecx,DWORD PTR [eax-0x4]
  4425ab:	8b 78 dc             	mov    edi,DWORD PTR [eax-0x24]
  4425ae:	89 bc 8d a4 03 00 00 	mov    DWORD PTR [ebp+ecx*4+0x3a4],edi
  4425b5:	8b 0e                	mov    ecx,DWORD PTR [esi]
  4425b7:	3b d1                	cmp    edx,ecx
  4425b9:	7c e0                	jl     0x44259b
  4425bb:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  4425c1:	e8 ba 88 ff ff       	call   0x43ae80
  4425c6:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4425cc:	33 ff                	xor    edi,edi
  4425ce:	8b 42 3c             	mov    eax,DWORD PTR [edx+0x3c]
  4425d1:	85 c0                	test   eax,eax
  4425d3:	7e 52                	jle    0x442627
  4425d5:	be 40 00 00 00       	mov    esi,0x40
  4425da:	8b 44 16 28          	mov    eax,DWORD PTR [esi+edx*1+0x28]
  4425de:	85 c0                	test   eax,eax
  4425e0:	7e 26                	jle    0x442608
  4425e2:	8b 14 16             	mov    edx,DWORD PTR [esi+edx*1]
  4425e5:	85 d2                	test   edx,edx
  4425e7:	89 84 95 c4 03 00 00 	mov    DWORD PTR [ebp+edx*4+0x3c4],eax
  4425ee:	7c 11                	jl     0x442601
  4425f0:	83 fa 08             	cmp    edx,0x8
  4425f3:	7d 0c                	jge    0x442601
  4425f5:	8d 42 14             	lea    eax,[edx+0x14]
  4425f8:	89 84 95 5c e5 00 00 	mov    DWORD PTR [ebp+edx*4+0xe55c],eax
  4425ff:	eb 17                	jmp    0x442618
  442601:	68 3c c3 46 00       	push   0x46c33c
  442606:	eb 05                	jmp    0x44260d
  442608:	68 10 c3 46 00       	push   0x46c310
  44260d:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  442613:	e8 58 f9 fe ff       	call   0x431f70
  442618:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  44261e:	47                   	inc    edi
  44261f:	83 c6 04             	add    esi,0x4
  442622:	3b 7a 3c             	cmp    edi,DWORD PTR [edx+0x3c]
  442625:	7c b3                	jl     0x4425da
  442627:	b8 c4 1a ff ff       	mov    eax,0xffff1ac4
  44262c:	33 db                	xor    ebx,ebx
  44262e:	2b c5                	sub    eax,ebp
  442630:	c6 44 24 13 01       	mov    BYTE PTR [esp+0x13],0x1
  442635:	8d b5 3c e5 00 00    	lea    esi,[ebp+0xe53c]
  44263b:	8d bd 80 0f 00 00    	lea    edi,[ebp+0xf80]
  442641:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  442645:	8a 44 24 13          	mov    al,BYTE PTR [esp+0x13]
  442649:	84 c0                	test   al,al
  44264b:	74 56                	je     0x4426a3
  44264d:	8b 85 4c 02 00 00    	mov    eax,DWORD PTR [ebp+0x24c]
  442653:	8d 8d c4 02 00 00    	lea    ecx,[ebp+0x2c4]
  442659:	51                   	push   ecx
  44265a:	8b 4c 24 1c          	mov    ecx,DWORD PTR [esp+0x1c]
  44265e:	03 c1                	add    eax,ecx
  442660:	8d 95 54 02 00 00    	lea    edx,[ebp+0x254]
  442666:	52                   	push   edx
  442667:	8b 16                	mov    edx,DWORD PTR [esi]
  442669:	8b 0c 30             	mov    ecx,DWORD PTR [eax+esi*1]
  44266c:	8b 86 68 1e ff ff    	mov    eax,DWORD PTR [esi-0xe198]
  442672:	51                   	push   ecx
  442673:	53                   	push   ebx
  442674:	52                   	push   edx
  442675:	50                   	push   eax
  442676:	8d 8f 64 f4 ff ff    	lea    ecx,[edi-0xb9c]
  44267c:	e8 2f cc fd ff       	call   0x41f2b0
  442681:	8b 0e                	mov    ecx,DWORD PTR [esi]
  442683:	43                   	inc    ebx
  442684:	89 0f                	mov    DWORD PTR [edi],ecx
  442686:	8b 16                	mov    edx,DWORD PTR [esi]
  442688:	89 57 04             	mov    DWORD PTR [edi+0x4],edx
  44268b:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  442691:	83 c6 04             	add    esi,0x4
  442694:	81 c7 e4 0d 00 00    	add    edi,0xde4
  44269a:	83 fb 10             	cmp    ebx,0x10
  44269d:	88 44 24 13          	mov    BYTE PTR [esp+0x13],al
  4426a1:	7c a2                	jl     0x442645
  4426a3:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  4426a7:	85 c0                	test   eax,eax
  4426a9:	0f 85 a0 00 00 00    	jne    0x44274f
  4426af:	8b 42 3c             	mov    eax,DWORD PTR [edx+0x3c]
  4426b2:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  4426b9:	00 
  4426ba:	85 c0                	test   eax,eax
  4426bc:	0f 8e 8d 00 00 00    	jle    0x44274f
  4426c2:	bb 68 00 00 00       	mov    ebx,0x68
  4426c7:	8b 04 13             	mov    eax,DWORD PTR [ebx+edx*1]
  4426ca:	85 c0                	test   eax,eax
  4426cc:	7e 41                	jle    0x44270f
  4426ce:	83 f8 09             	cmp    eax,0x9
  4426d1:	7d 3c                	jge    0x44270f
  4426d3:	8b 44 13 d8          	mov    eax,DWORD PTR [ebx+edx*1-0x28]
  4426d7:	8b 74 13 ac          	mov    esi,DWORD PTR [ebx+edx*1-0x54]
  4426db:	8d 0c c0             	lea    ecx,[eax+eax*8]
  4426de:	8d 0c 88             	lea    ecx,[eax+ecx*4]
  4426e1:	8d 0c 49             	lea    ecx,[ecx+ecx*2]
  4426e4:	8d 04 c8             	lea    eax,[eax+ecx*8]
  4426e7:	b9 d4 00 00 00       	mov    ecx,0xd4
  4426ec:	8d 44 85 00          	lea    eax,[ebp+eax*4+0x0]
  4426f0:	8d b8 50 7b 00 00    	lea    edi,[eax+0x7b50]
  4426f6:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  4426f8:	8d 88 04 73 00 00    	lea    ecx,[eax+0x7304]
  4426fe:	c7 80 ec 7b 00 00 00 	mov    DWORD PTR [eax+0x7bec],0x0
  442705:	00 00 00 
  442708:	e8 c3 0a fe ff       	call   0x4231d0
  44270d:	eb 23                	jmp    0x442732
  44270f:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  442715:	50                   	push   eax
  442716:	68 fc c2 46 00       	push   0x46c2fc
  44271b:	e8 f0 f8 fe ff       	call   0x432010
  442720:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  442726:	68 d8 c2 46 00       	push   0x46c2d8
  44272b:	6a 00                	push   0x0
  44272d:	e8 3e f7 fe ff       	call   0x431e70
  442732:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  442738:	8b 44 24 14          	mov    eax,DWORD PTR [esp+0x14]
  44273c:	40                   	inc    eax
  44273d:	83 c3 04             	add    ebx,0x4
  442740:	8b 4a 3c             	mov    ecx,DWORD PTR [edx+0x3c]
  442743:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  442747:	3b c1                	cmp    eax,ecx
  442749:	0f 8c 78 ff ff ff    	jl     0x4426c7
  44274f:	8b 0d 10 24 8e 00    	mov    ecx,DWORD PTR ds:0x8e2410
  442755:	8d b5 e4 03 00 00    	lea    esi,[ebp+0x3e4]
  44275b:	8d bd c4 02 00 00    	lea    edi,[ebp+0x2c4]
  442761:	8d 9d 54 02 00 00    	lea    ebx,[ebp+0x254]
  442767:	56                   	push   esi
  442768:	57                   	push   edi
  442769:	53                   	push   ebx
  44276a:	89 2d 80 fd 89 00    	mov    DWORD PTR ds:0x89fd80,ebp
  442770:	e8 fb 36 00 00       	call   0x445e70
  442775:	8b 0d e4 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01e4
  44277b:	56                   	push   esi
  44277c:	57                   	push   edi
  44277d:	53                   	push   ebx
  44277e:	e8 0d dd fe ff       	call   0x430490
  442783:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  442788:	33 ff                	xor    edi,edi
  44278a:	8b 48 3c             	mov    ecx,DWORD PTR [eax+0x3c]
  44278d:	c7 05 54 fd 89 00 08 	mov    DWORD PTR ds:0x89fd54,0x89fe08
  442794:	fe 89 00 
  442797:	89 0d d8 fc 89 00    	mov    DWORD PTR ds:0x89fcd8,ecx
  44279d:	c7 05 58 fd 89 00 d0 	mov    DWORD PTR ds:0x89fd58,0x89ffd0
  4427a4:	ff 89 00 
  4427a7:	8b 48 3c             	mov    ecx,DWORD PTR [eax+0x3c]
  4427aa:	85 c9                	test   ecx,ecx
  4427ac:	0f 8e 8a 00 00 00    	jle    0x44283c
  4427b2:	be dc fc 89 00       	mov    esi,0x89fcdc
  4427b7:	bb 68 00 00 00       	mov    ebx,0x68
  4427bc:	b9 40 00 00 00       	mov    ecx,0x40
  4427c1:	ba bc 05 00 00       	mov    edx,0x5bc
  4427c6:	2b de                	sub    ebx,esi
  4427c8:	2b ce                	sub    ecx,esi
  4427ca:	2b d6                	sub    edx,esi
  4427cc:	89 4c 24 18          	mov    DWORD PTR [esp+0x18],ecx
  4427d0:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  4427d4:	eb 04                	jmp    0x4427da
  4427d6:	8b 4c 24 18          	mov    ecx,DWORD PTR [esp+0x18]
  4427da:	8d 14 33             	lea    edx,[ebx+esi*1]
  4427dd:	03 ce                	add    ecx,esi
  4427df:	03 c8                	add    ecx,eax
  4427e1:	8b 14 02             	mov    edx,DWORD PTR [edx+eax*1]
  4427e4:	89 16                	mov    DWORD PTR [esi],edx
  4427e6:	8b 01                	mov    eax,DWORD PTR [ecx]
  4427e8:	8d 14 c0             	lea    edx,[eax+eax*8]
  4427eb:	8d 14 90             	lea    edx,[eax+edx*4]
  4427ee:	8d 14 52             	lea    edx,[edx+edx*2]
  4427f1:	8d 04 d0             	lea    eax,[eax+edx*8]
  4427f4:	8d 94 85 50 7b 00 00 	lea    edx,[ebp+eax*4+0x7b50]
  4427fb:	89 56 28             	mov    DWORD PTR [esi+0x28],edx
  4427fe:	8b 01                	mov    eax,DWORD PTR [ecx]
  442800:	8d 0c c0             	lea    ecx,[eax+eax*8]
  442803:	8d 0c 88             	lea    ecx,[eax+ecx*4]
  442806:	8d 14 49             	lea    edx,[ecx+ecx*2]
  442809:	8d 04 d0             	lea    eax,[eax+edx*8]
  44280c:	8b 15 74 fd 89 00    	mov    edx,DWORD PTR ds:0x89fd74
  442812:	8d 8c 85 ac 7f 00 00 	lea    ecx,[ebp+eax*4+0x7fac]
  442819:	89 4e 50             	mov    DWORD PTR [esi+0x50],ecx
  44281c:	8b 82 f0 05 00 00    	mov    eax,DWORD PTR [edx+0x5f0]
  442822:	03 44 24 14          	add    eax,DWORD PTR [esp+0x14]
  442826:	8b 0c 30             	mov    ecx,DWORD PTR [eax+esi*1]
  442829:	e8 42 c5 fd ff       	call   0x41ed70
  44282e:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  442833:	47                   	inc    edi
  442834:	83 c6 04             	add    esi,0x4
  442837:	3b 78 3c             	cmp    edi,DWORD PTR [eax+0x3c]
  44283a:	7c 9a                	jl     0x4427d6
  44283c:	33 f6                	xor    esi,esi
  44283e:	b9 08 fe 89 00       	mov    ecx,0x89fe08
  442843:	89 35 f8 fd 89 00    	mov    DWORD PTR ds:0x89fdf8,esi
  442849:	e8 e2 b1 ff ff       	call   0x43da30
  44284e:	b9 d0 ff 89 00       	mov    ecx,0x89ffd0
  442853:	c7 05 c8 ff 89 00 01 	mov    DWORD PTR ds:0x89ffc8,0x1
  44285a:	00 00 00 
  44285d:	e8 ce b1 ff ff       	call   0x43da30
  442862:	8d 4c 24 1c          	lea    ecx,[esp+0x1c]
  442866:	89 35 90 01 8a 00    	mov    DWORD PTR ds:0x8a0190,esi
  44286c:	c7 84 24 88 00 00 00 	mov    DWORD PTR [esp+0x88],0xffffffff
  442873:	ff ff ff ff 
  442877:	e8 94 ee ff ff       	call   0x441710
  44287c:	8b 8c 24 80 00 00 00 	mov    ecx,DWORD PTR [esp+0x80]
  442883:	8a 44 24 13          	mov    al,BYTE PTR [esp+0x13]
  442887:	5f                   	pop    edi
  442888:	5e                   	pop    esi
  442889:	5d                   	pop    ebp
  44288a:	5b                   	pop    ebx
  44288b:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  442892:	83 c4 7c             	add    esp,0x7c
  442895:	c3                   	ret
  442896:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  44289c:	68 a0 c2 46 00       	push   0x46c2a0
  4428a1:	68 78 c2 46 00       	push   0x46c278
  4428a6:	e8 a5 f7 fe ff       	call   0x432050
  4428ab:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  4428b1:	68 69 01 00 00       	push   0x169
  4428b6:	68 50 c2 46 00       	push   0x46c250
  4428bb:	e8 50 f7 fe ff       	call   0x432010
  4428c0:	89 bc 24 88 00 00 00 	mov    DWORD PTR [esp+0x88],edi
  4428c7:	e9 44 f9 ff ff       	jmp    0x442210
  4428cc:	90                   	nop
  4428cd:	90                   	nop
  4428ce:	90                   	nop
  4428cf:	90                   	nop
