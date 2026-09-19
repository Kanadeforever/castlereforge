; Oracle: RPG.exe SHA256 b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; objdump -d -Mintel, VA 0x432090..0x432a20

  432090:	6a ff                	push   0xffffffff
  432092:	68 c8 ec 45 00       	push   0x45ecc8
  432097:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  43209d:	50                   	push   eax
  43209e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  4320a5:	51                   	push   ecx
  4320a6:	53                   	push   ebx
  4320a7:	56                   	push   esi
  4320a8:	8b f1                	mov    esi,ecx
  4320aa:	89 74 24 08          	mov    DWORD PTR [esp+0x8],esi
  4320ae:	e8 2d 23 00 00       	call   0x4343e0
  4320b3:	33 db                	xor    ebx,ebx
  4320b5:	6a 01                	push   0x1
  4320b7:	8d 8e 2c 02 00 00    	lea    ecx,[esi+0x22c]
  4320bd:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  4320c1:	e8 2a f6 00 00       	call   0x4416f0
  4320c6:	c7 06 c4 0d 46 00    	mov    DWORD PTR [esi],0x460dc4
  4320cc:	88 9e 20 02 00 00    	mov    BYTE PTR [esi+0x220],bl
  4320d2:	88 9e 40 02 00 00    	mov    BYTE PTR [esi+0x240],bl
  4320d8:	a1 4c 1c 8e 00       	mov    eax,ds:0x8e1c4c
  4320dd:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  4320e1:	c6 80 48 02 00 00 01 	mov    BYTE PTR [eax+0x248],0x1
  4320e8:	89 1d dc 01 8b 00    	mov    DWORD PTR ds:0x8b01dc,ebx
  4320ee:	89 9e 18 02 00 00    	mov    DWORD PTR [esi+0x218],ebx
  4320f4:	89 1d a4 01 8a 00    	mov    DWORD PTR ds:0x8a01a4,ebx
  4320fa:	c7 86 24 02 00 00 19 	mov    DWORD PTR [esi+0x224],0x19
  432101:	00 00 00 
  432104:	89 1d ac 01 8a 00    	mov    DWORD PTR ds:0x8a01ac,ebx
  43210a:	89 1d b0 01 8a 00    	mov    DWORD PTR ds:0x8a01b0,ebx
  432110:	c7 86 44 02 00 00 0f 	mov    DWORD PTR [esi+0x244],0xf
  432117:	00 00 00 
  43211a:	88 9e 48 02 00 00    	mov    BYTE PTR [esi+0x248],bl
  432120:	8b c6                	mov    eax,esi
  432122:	5e                   	pop    esi
  432123:	5b                   	pop    ebx
  432124:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  43212b:	83 c4 10             	add    esp,0x10
  43212e:	c3                   	ret
  43212f:	90                   	nop
  432130:	6a ff                	push   0xffffffff
  432132:	68 f6 ec 45 00       	push   0x45ecf6
  432137:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  43213d:	50                   	push   eax
  43213e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  432145:	51                   	push   ecx
  432146:	56                   	push   esi
  432147:	8b f1                	mov    esi,ecx
  432149:	89 74 24 04          	mov    DWORD PTR [esp+0x4],esi
  43214d:	c7 06 c4 0d 46 00    	mov    DWORD PTR [esi],0x460dc4
  432153:	a1 4c 1c 8e 00       	mov    eax,ds:0x8e1c4c
  432158:	56                   	push   esi
  432159:	c7 44 24 14 01 00 00 	mov    DWORD PTR [esp+0x14],0x1
  432160:	00 
  432161:	c6 80 48 02 00 00 00 	mov    BYTE PTR [eax+0x248],0x0
  432168:	e8 23 26 00 00       	call   0x434790
  43216d:	8b ce                	mov    ecx,esi
  43216f:	e8 1c 06 00 00       	call   0x432790
  432174:	8b ce                	mov    ecx,esi
  432176:	e8 a5 26 00 00       	call   0x434820
  43217b:	8d 8e 2c 02 00 00    	lea    ecx,[esi+0x22c]
  432181:	c7 05 f0 01 8b 00 00 	mov    DWORD PTR ds:0x8b01f0,0x0
  432188:	00 00 00 
  43218b:	c6 44 24 10 00       	mov    BYTE PTR [esp+0x10],0x0
  432190:	e8 7b f5 00 00       	call   0x441710
  432195:	8b ce                	mov    ecx,esi
  432197:	c7 44 24 10 ff ff ff 	mov    DWORD PTR [esp+0x10],0xffffffff
  43219e:	ff 
  43219f:	e8 6c 22 00 00       	call   0x434410
  4321a4:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  4321a8:	5e                   	pop    esi
  4321a9:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  4321b0:	83 c4 10             	add    esp,0x10
  4321b3:	c3                   	ret
  4321b4:	90                   	nop
  4321b5:	90                   	nop
  4321b6:	90                   	nop
  4321b7:	90                   	nop
  4321b8:	90                   	nop
  4321b9:	90                   	nop
  4321ba:	90                   	nop
  4321bb:	90                   	nop
  4321bc:	90                   	nop
  4321bd:	90                   	nop
  4321be:	90                   	nop
  4321bf:	90                   	nop
  4321c0:	56                   	push   esi
  4321c1:	8b f1                	mov    esi,ecx
  4321c3:	8b 0d 4c 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c4c
  4321c9:	e8 22 c0 00 00       	call   0x43e1f0
  4321ce:	85 c0                	test   eax,eax
  4321d0:	89 86 28 02 00 00    	mov    DWORD PTR [esi+0x228],eax
  4321d6:	5e                   	pop    esi
  4321d7:	0f 9d c0             	setge  al
  4321da:	c3                   	ret
  4321db:	90                   	nop
  4321dc:	90                   	nop
  4321dd:	90                   	nop
  4321de:	90                   	nop
  4321df:	90                   	nop
  4321e0:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  4321e5:	8b 88 b4 00 00 00    	mov    ecx,DWORD PTR [eax+0xb4]
  4321eb:	89 0d ac 01 8a 00    	mov    DWORD PTR ds:0x8a01ac,ecx
  4321f1:	8b 90 ac 00 00 00    	mov    edx,DWORD PTR [eax+0xac]
  4321f7:	89 15 b0 01 8a 00    	mov    DWORD PTR ds:0x8a01b0,edx
  4321fd:	b0 01                	mov    al,0x1
  4321ff:	c3                   	ret
  432200:	33 c0                	xor    eax,eax
  432202:	a3 ac 01 8a 00       	mov    ds:0x8a01ac,eax
  432207:	a3 b0 01 8a 00       	mov    ds:0x8a01b0,eax
  43220c:	c3                   	ret
  43220d:	90                   	nop
  43220e:	90                   	nop
  43220f:	90                   	nop
  432210:	6a ff                	push   0xffffffff
  432212:	68 0b ed 45 00       	push   0x45ed0b
  432217:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  43221d:	50                   	push   eax
  43221e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  432225:	51                   	push   ecx
  432226:	56                   	push   esi
  432227:	8b 35 a4 01 8a 00    	mov    esi,DWORD PTR ds:0x8a01a4
  43222d:	85 f6                	test   esi,esi
  43222f:	74 1a                	je     0x43224b
  432231:	8b ce                	mov    ecx,esi
  432233:	e8 c8 46 ff ff       	call   0x426900
  432238:	56                   	push   esi
  432239:	e8 12 f3 01 00       	call   0x451550
  43223e:	83 c4 04             	add    esp,0x4
  432241:	c7 05 a4 01 8a 00 00 	mov    DWORD PTR ds:0x8a01a4,0x0
  432248:	00 00 00 
  43224b:	68 94 02 00 00       	push   0x294
  432250:	e8 0a f4 01 00       	call   0x45165f
  432255:	83 c4 04             	add    esp,0x4
  432258:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
  43225c:	85 c0                	test   eax,eax
  43225e:	c7 44 24 10 00 00 00 	mov    DWORD PTR [esp+0x10],0x0
  432265:	00 
  432266:	74 2f                	je     0x432297
  432268:	8b 15 c0 f6 89 00    	mov    edx,DWORD PTR ds:0x89f6c0
  43226e:	55                   	push   ebp
  43226f:	57                   	push   edi
  432270:	8b 72 28             	mov    esi,DWORD PTR [edx+0x28]
  432273:	8b 4a 70             	mov    ecx,DWORD PTR [edx+0x70]
  432276:	8b 7a 68             	mov    edi,DWORD PTR [edx+0x68]
  432279:	8b 52 6c             	mov    edx,DWORD PTR [edx+0x6c]
  43227c:	8b 6e 08             	mov    ebp,DWORD PTR [esi+0x8]
  43227f:	8d 4c 4d 00          	lea    ecx,[ebp+ecx*2+0x0]
  432283:	51                   	push   ecx
  432284:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  432287:	8d 14 51             	lea    edx,[ecx+edx*2]
  43228a:	8b c8                	mov    ecx,eax
  43228c:	52                   	push   edx
  43228d:	57                   	push   edi
  43228e:	e8 0d 45 ff ff       	call   0x4267a0
  432293:	5f                   	pop    edi
  432294:	5d                   	pop    ebp
  432295:	eb 02                	jmp    0x432299
  432297:	33 c0                	xor    eax,eax
  432299:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  43229d:	a3 a4 01 8a 00       	mov    ds:0x8a01a4,eax
  4322a2:	85 c0                	test   eax,eax
  4322a4:	0f 95 c0             	setne  al
  4322a7:	5e                   	pop    esi
  4322a8:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  4322af:	83 c4 10             	add    esp,0x10
  4322b2:	c3                   	ret
  4322b3:	90                   	nop
  4322b4:	90                   	nop
  4322b5:	90                   	nop
  4322b6:	90                   	nop
  4322b7:	90                   	nop
  4322b8:	90                   	nop
  4322b9:	90                   	nop
  4322ba:	90                   	nop
  4322bb:	90                   	nop
  4322bc:	90                   	nop
  4322bd:	90                   	nop
  4322be:	90                   	nop
  4322bf:	90                   	nop
  4322c0:	6a ff                	push   0xffffffff
  4322c2:	68 62 ed 45 00       	push   0x45ed62
  4322c7:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  4322cd:	50                   	push   eax
  4322ce:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  4322d5:	51                   	push   ecx
  4322d6:	56                   	push   esi
  4322d7:	57                   	push   edi
  4322d8:	8b f9                	mov    edi,ecx
  4322da:	68 1c 06 00 00       	push   0x61c
  4322df:	e8 7b f3 01 00       	call   0x45165f
  4322e4:	83 c4 04             	add    esp,0x4
  4322e7:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  4322eb:	85 c0                	test   eax,eax
  4322ed:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  4322f4:	00 
  4322f5:	74 09                	je     0x432300
  4322f7:	8b c8                	mov    ecx,eax
  4322f9:	e8 b2 fc fd ff       	call   0x411fb0
  4322fe:	eb 02                	jmp    0x432302
  432300:	33 c0                	xor    eax,eax
  432302:	8b 0d a8 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01a8
  432308:	83 ce ff             	or     esi,0xffffffff
  43230b:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  43230f:	a3 c8 fc 89 00       	mov    ds:0x89fcc8,eax
  432314:	e8 97 60 ff ff       	call   0x4283b0
  432319:	6a 00                	push   0x0
  43231b:	68 50 c3 00 00       	push   0xc350
  432320:	57                   	push   edi
  432321:	8b cf                	mov    ecx,edi
  432323:	e8 08 21 00 00       	call   0x434430
  432328:	8b cf                	mov    ecx,edi
  43232a:	e8 b1 fe ff ff       	call   0x4321e0
  43232f:	84 c0                	test   al,al
  432331:	75 23                	jne    0x432356
  432333:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432339:	68 44 ac 46 00       	push   0x46ac44
  43233e:	6a 00                	push   0x0
  432340:	e8 2b fb ff ff       	call   0x431e70
  432345:	5f                   	pop    edi
  432346:	5e                   	pop    esi
  432347:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  43234b:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  432352:	83 c4 10             	add    esp,0x10
  432355:	c3                   	ret
  432356:	8b cf                	mov    ecx,edi
  432358:	e8 b3 fe ff ff       	call   0x432210
  43235d:	84 c0                	test   al,al
  43235f:	75 23                	jne    0x432384
  432361:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432367:	68 34 ac 46 00       	push   0x46ac34
  43236c:	6a 00                	push   0x0
  43236e:	e8 fd fa ff ff       	call   0x431e70
  432373:	5f                   	pop    edi
  432374:	5e                   	pop    esi
  432375:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  432379:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  432380:	83 c4 10             	add    esp,0x10
  432383:	c3                   	ret
  432384:	8b cf                	mov    ecx,edi
  432386:	e8 35 fe ff ff       	call   0x4321c0
  43238b:	84 c0                	test   al,al
  43238d:	75 23                	jne    0x4323b2
  43238f:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432395:	68 24 ac 46 00       	push   0x46ac24
  43239a:	6a 00                	push   0x0
  43239c:	e8 cf fa ff ff       	call   0x431e70
  4323a1:	5f                   	pop    edi
  4323a2:	5e                   	pop    esi
  4323a3:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  4323a7:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  4323ae:	83 c4 10             	add    esp,0x10
  4323b1:	c3                   	ret
  4323b2:	a1 4c 1c 8e 00       	mov    eax,ds:0x8e1c4c
  4323b7:	53                   	push   ebx
  4323b8:	55                   	push   ebp
  4323b9:	bb 01 00 00 00       	mov    ebx,0x1
  4323be:	68 3c 02 00 00       	push   0x23c
  4323c3:	88 98 48 02 00 00    	mov    BYTE PTR [eax+0x248],bl
  4323c9:	e8 91 f2 01 00       	call   0x45165f
  4323ce:	83 c4 04             	add    esp,0x4
  4323d1:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  4323d5:	85 c0                	test   eax,eax
  4323d7:	89 5c 24 1c          	mov    DWORD PTR [esp+0x1c],ebx
  4323db:	74 09                	je     0x4323e6
  4323dd:	8b c8                	mov    ecx,eax
  4323df:	e8 2c 7d 01 00       	call   0x44a110
  4323e4:	eb 02                	jmp    0x4323e8
  4323e6:	33 c0                	xor    eax,eax
  4323e8:	8b 15 c0 f6 89 00    	mov    edx,DWORD PTR ds:0x89f6c0
  4323ee:	89 74 24 1c          	mov    DWORD PTR [esp+0x1c],esi
  4323f2:	a3 28 24 8e 00       	mov    ds:0x8e2428,eax
  4323f7:	8b 72 28             	mov    esi,DWORD PTR [edx+0x28]
  4323fa:	8b 4a 68             	mov    ecx,DWORD PTR [edx+0x68]
  4323fd:	8b 5a 70             	mov    ebx,DWORD PTR [edx+0x70]
  432400:	8b 52 6c             	mov    edx,DWORD PTR [edx+0x6c]
  432403:	8b 6e 08             	mov    ebp,DWORD PTR [esi+0x8]
  432406:	8b 76 04             	mov    esi,DWORD PTR [esi+0x4]
  432409:	8d 5c 5d 00          	lea    ebx,[ebp+ebx*2+0x0]
  43240d:	8d 14 56             	lea    edx,[esi+edx*2]
  432410:	53                   	push   ebx
  432411:	52                   	push   edx
  432412:	51                   	push   ecx
  432413:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  432419:	68 e0 01 00 00       	push   0x1e0
  43241e:	68 80 02 00 00       	push   0x280
  432423:	8b 91 c0 00 00 00    	mov    edx,DWORD PTR [ecx+0xc0]
  432429:	8b c8                	mov    ecx,eax
  43242b:	52                   	push   edx
  43242c:	e8 bf 7d 01 00       	call   0x44a1f0
  432431:	5d                   	pop    ebp
  432432:	5b                   	pop    ebx
  432433:	84 c0                	test   al,al
  432435:	75 23                	jne    0x43245a
  432437:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  43243d:	68 10 ac 46 00       	push   0x46ac10
  432442:	6a 00                	push   0x0
  432444:	e8 27 fa ff ff       	call   0x431e70
  432449:	5f                   	pop    edi
  43244a:	5e                   	pop    esi
  43244b:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  43244f:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  432456:	83 c4 10             	add    esp,0x10
  432459:	c3                   	ret
  43245a:	68 00 06 00 00       	push   0x600
  43245f:	e8 fb f1 01 00       	call   0x45165f
  432464:	83 c4 04             	add    esp,0x4
  432467:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  43246b:	85 c0                	test   eax,eax
  43246d:	c7 44 24 14 02 00 00 	mov    DWORD PTR [esp+0x14],0x2
  432474:	00 
  432475:	74 09                	je     0x432480
  432477:	8b c8                	mov    ecx,eax
  432479:	e8 32 8d fe ff       	call   0x41b1b0
  43247e:	eb 02                	jmp    0x432482
  432480:	33 c0                	xor    eax,eax
  432482:	83 ce ff             	or     esi,0xffffffff
  432485:	a3 74 fd 89 00       	mov    ds:0x89fd74,eax
  43248a:	85 c0                	test   eax,eax
  43248c:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  432490:	75 22                	jne    0x4324b4
  432492:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432498:	68 fc ab 46 00       	push   0x46abfc
  43249d:	50                   	push   eax
  43249e:	e8 cd f9 ff ff       	call   0x431e70
  4324a3:	5f                   	pop    edi
  4324a4:	5e                   	pop    esi
  4324a5:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  4324a9:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  4324b0:	83 c4 10             	add    esp,0x10
  4324b3:	c3                   	ret
  4324b4:	68 7c e5 00 00       	push   0xe57c
  4324b9:	e8 a1 f1 01 00       	call   0x45165f
  4324be:	83 c4 04             	add    esp,0x4
  4324c1:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  4324c5:	85 c0                	test   eax,eax
  4324c7:	c7 44 24 14 03 00 00 	mov    DWORD PTR [esp+0x14],0x3
  4324ce:	00 
  4324cf:	74 09                	je     0x4324da
  4324d1:	8b c8                	mov    ecx,eax
  4324d3:	e8 d8 f7 00 00       	call   0x441cb0
  4324d8:	eb 02                	jmp    0x4324dc
  4324da:	33 c0                	xor    eax,eax
  4324dc:	85 c0                	test   eax,eax
  4324de:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  4324e2:	89 87 18 02 00 00    	mov    DWORD PTR [edi+0x218],eax
  4324e8:	74 0b                	je     0x4324f5
  4324ea:	8b c8                	mov    ecx,eax
  4324ec:	e8 4f fb 00 00       	call   0x442040
  4324f1:	84 c0                	test   al,al
  4324f3:	75 23                	jne    0x432518
  4324f5:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  4324fb:	68 e0 ab 46 00       	push   0x46abe0
  432500:	6a 00                	push   0x0
  432502:	e8 69 f9 ff ff       	call   0x431e70
  432507:	5f                   	pop    edi
  432508:	5e                   	pop    esi
  432509:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  43250d:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  432514:	83 c4 10             	add    esp,0x10
  432517:	c3                   	ret
  432518:	8b cf                	mov    ecx,edi
  43251a:	e8 51 01 00 00       	call   0x432670
  43251f:	68 a4 03 00 00       	push   0x3a4
  432524:	e8 36 f1 01 00       	call   0x45165f
  432529:	83 c4 04             	add    esp,0x4
  43252c:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  432530:	85 c0                	test   eax,eax
  432532:	c7 44 24 14 04 00 00 	mov    DWORD PTR [esp+0x14],0x4
  432539:	00 
  43253a:	74 09                	je     0x432545
  43253c:	8b c8                	mov    ecx,eax
  43253e:	e8 fd 16 00 00       	call   0x433c40
  432543:	eb 02                	jmp    0x432547
  432545:	33 c0                	xor    eax,eax
  432547:	6a fe                	push   0xfffffffe
  432549:	6a 00                	push   0x0
  43254b:	8b c8                	mov    ecx,eax
  43254d:	89 74 24 1c          	mov    DWORD PTR [esp+0x1c],esi
  432551:	a3 c8 40 8c 00       	mov    ds:0x8c40c8,eax
  432556:	e8 05 18 00 00       	call   0x433d60
  43255b:	84 c0                	test   al,al
  43255d:	75 23                	jne    0x432582
  43255f:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432565:	68 cc ab 46 00       	push   0x46abcc
  43256a:	6a 00                	push   0x0
  43256c:	e8 ff f8 ff ff       	call   0x431e70
  432571:	5f                   	pop    edi
  432572:	5e                   	pop    esi
  432573:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  432577:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  43257e:	83 c4 10             	add    esp,0x10
  432581:	c3                   	ret
  432582:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  432587:	6a 00                	push   0x0
  432589:	8b 88 d8 00 00 00    	mov    ecx,DWORD PTR [eax+0xd8]
  43258f:	51                   	push   ecx
  432590:	8b 88 e8 00 00 00    	mov    ecx,DWORD PTR [eax+0xe8]
  432596:	e8 95 1e ff ff       	call   0x424430
  43259b:	85 c0                	test   eax,eax
  43259d:	7d 1a                	jge    0x4325b9
  43259f:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  4325a5:	68 98 ab 46 00       	push   0x46ab98
  4325aa:	68 74 ab 46 00       	push   0x46ab74
  4325af:	e8 9c fa ff ff       	call   0x432050
  4325b4:	b8 01 00 00 00       	mov    eax,0x1
  4325b9:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  4325bf:	50                   	push   eax
  4325c0:	e8 4b 1b 00 00       	call   0x434110
  4325c5:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  4325cb:	68 68 ab 46 00       	push   0x46ab68
  4325d0:	e8 ab 1a 00 00       	call   0x434080
  4325d5:	8b 0d c8 40 8c 00    	mov    ecx,DWORD PTR ds:0x8c40c8
  4325db:	68 68 ab 46 00       	push   0x46ab68
  4325e0:	e8 eb 1a 00 00       	call   0x4340d0
  4325e5:	68 a0 83 00 00       	push   0x83a0
  4325ea:	e8 70 f0 01 00       	call   0x45165f
  4325ef:	83 c4 04             	add    esp,0x4
  4325f2:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  4325f6:	85 c0                	test   eax,eax
  4325f8:	c7 44 24 14 05 00 00 	mov    DWORD PTR [esp+0x14],0x5
  4325ff:	00 
  432600:	74 09                	je     0x43260b
  432602:	8b c8                	mov    ecx,eax
  432604:	e8 27 ac ff ff       	call   0x42d230
  432609:	eb 02                	jmp    0x43260d
  43260b:	33 c0                	xor    eax,eax
  43260d:	8b c8                	mov    ecx,eax
  43260f:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  432613:	a3 dc 01 8b 00       	mov    ds:0x8b01dc,eax
  432618:	e8 23 ad ff ff       	call   0x42d340
  43261d:	84 c0                	test   al,al
  43261f:	75 23                	jne    0x432644
  432621:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432627:	68 54 ab 46 00       	push   0x46ab54
  43262c:	6a 00                	push   0x0
  43262e:	e8 3d f8 ff ff       	call   0x431e70
  432633:	5f                   	pop    edi
  432634:	5e                   	pop    esi
  432635:	8b 4c 24 04          	mov    ecx,DWORD PTR [esp+0x4]
  432639:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  432640:	83 c4 10             	add    esp,0x10
  432643:	c3                   	ret
  432644:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  43264a:	8b 0d d8 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01d8
  432650:	8b 82 c0 00 00 00    	mov    eax,DWORD PTR [edx+0xc0]
  432656:	50                   	push   eax
  432657:	e8 d4 a9 ff ff       	call   0x42d030
  43265c:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  432660:	5f                   	pop    edi
  432661:	b0 01                	mov    al,0x1
  432663:	5e                   	pop    esi
  432664:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  43266b:	83 c4 10             	add    esp,0x10
  43266e:	c3                   	ret
  43266f:	90                   	nop
  432670:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  432676:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  43267c:	6a ff                	push   0xffffffff
  43267e:	68 7e ed 45 00       	push   0x45ed7e
  432683:	50                   	push   eax
  432684:	33 c0                	xor    eax,eax
  432686:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  43268d:	81 ec f8 01 00 00    	sub    esp,0x1f8
  432693:	85 c9                	test   ecx,ecx
  432695:	56                   	push   esi
  432696:	0f 84 d6 00 00 00    	je     0x432772
  43269c:	8b 0d b4 01 8a 00    	mov    ecx,DWORD PTR ds:0x8a01b4
  4326a2:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4326a8:	8b 49 04             	mov    ecx,DWORD PTR [ecx+0x4]
  4326ab:	8b 92 e4 00 00 00    	mov    edx,DWORD PTR [edx+0xe4]
  4326b1:	3b 91 f8 01 00 00    	cmp    edx,DWORD PTR [ecx+0x1f8]
  4326b7:	7d 06                	jge    0x4326bf
  4326b9:	85 d2                	test   edx,edx
  4326bb:	7c 02                	jl     0x4326bf
  4326bd:	8b c2                	mov    eax,edx
  4326bf:	8d 04 80             	lea    eax,[eax+eax*4]
  4326c2:	57                   	push   edi
  4326c3:	6a 14                	push   0x14
  4326c5:	8d 04 80             	lea    eax,[eax+eax*4]
  4326c8:	8d bc 41 fc 01 00 00 	lea    edi,[ecx+eax*2+0x1fc]
  4326cf:	e8 8b ef 01 00       	call   0x45165f
  4326d4:	83 c4 04             	add    esp,0x4
  4326d7:	89 44 24 08          	mov    DWORD PTR [esp+0x8],eax
  4326db:	85 c0                	test   eax,eax
  4326dd:	c7 84 24 08 02 00 00 	mov    DWORD PTR [esp+0x208],0x0
  4326e4:	00 00 00 00 
  4326e8:	74 0d                	je     0x4326f7
  4326ea:	6a 01                	push   0x1
  4326ec:	8b c8                	mov    ecx,eax
  4326ee:	e8 fd ef 00 00       	call   0x4416f0
  4326f3:	8b f0                	mov    esi,eax
  4326f5:	eb 02                	jmp    0x4326f9
  4326f7:	33 f6                	xor    esi,esi
  4326f9:	8d 4c 24 0c          	lea    ecx,[esp+0xc]
  4326fd:	c7 84 24 08 02 00 00 	mov    DWORD PTR [esp+0x208],0xffffffff
  432704:	ff ff ff ff 
  432708:	51                   	push   ecx
  432709:	57                   	push   edi
  43270a:	b9 90 1c 8e 00       	mov    ecx,0x8e1c90
  43270f:	e8 dc f4 00 00       	call   0x441bf0
  432714:	84 c0                	test   al,al
  432716:	5f                   	pop    edi
  432717:	74 35                	je     0x43274e
  432719:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  43271f:	e8 8c 2f fd ff       	call   0x4056b0
  432724:	8b 0d bc f6 46 00    	mov    ecx,DWORD PTR ds:0x46f6bc
  43272a:	8d 54 24 08          	lea    edx,[esp+0x8]
  43272e:	6a 01                	push   0x1
  432730:	52                   	push   edx
  432731:	e8 3a 2f fd ff       	call   0x405670
  432736:	84 c0                	test   al,al
  432738:	75 24                	jne    0x43275e
  43273a:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432740:	68 88 ac 46 00       	push   0x46ac88
  432745:	6a 00                	push   0x0
  432747:	e8 24 f7 ff ff       	call   0x431e70
  43274c:	eb 10                	jmp    0x43275e
  43274e:	8b 0d ec 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01ec
  432754:	68 60 ac 46 00       	push   0x46ac60
  432759:	e8 12 f8 ff ff       	call   0x431f70
  43275e:	85 f6                	test   esi,esi
  432760:	74 10                	je     0x432772
  432762:	8b ce                	mov    ecx,esi
  432764:	e8 a7 ef 00 00       	call   0x441710
  432769:	56                   	push   esi
  43276a:	e8 e1 ed 01 00       	call   0x451550
  43276f:	83 c4 04             	add    esp,0x4
  432772:	8b 8c 24 fc 01 00 00 	mov    ecx,DWORD PTR [esp+0x1fc]
  432779:	5e                   	pop    esi
  43277a:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  432781:	81 c4 04 02 00 00    	add    esp,0x204
  432787:	c3                   	ret
  432788:	90                   	nop
  432789:	90                   	nop
  43278a:	90                   	nop
  43278b:	90                   	nop
  43278c:	90                   	nop
  43278d:	90                   	nop
  43278e:	90                   	nop
  43278f:	90                   	nop
  432790:	53                   	push   ebx
  432791:	56                   	push   esi
  432792:	8b 35 c8 40 8c 00    	mov    esi,DWORD PTR ds:0x8c40c8
  432798:	33 db                	xor    ebx,ebx
  43279a:	57                   	push   edi
  43279b:	3b f3                	cmp    esi,ebx
  43279d:	8b f9                	mov    edi,ecx
  43279f:	74 16                	je     0x4327b7
  4327a1:	8b ce                	mov    ecx,esi
  4327a3:	e8 28 15 00 00       	call   0x433cd0
  4327a8:	56                   	push   esi
  4327a9:	e8 a2 ed 01 00       	call   0x451550
  4327ae:	83 c4 04             	add    esp,0x4
  4327b1:	89 1d c8 40 8c 00    	mov    DWORD PTR ds:0x8c40c8,ebx
  4327b7:	8b b7 18 02 00 00    	mov    esi,DWORD PTR [edi+0x218]
  4327bd:	3b f3                	cmp    esi,ebx
  4327bf:	74 16                	je     0x4327d7
  4327c1:	8b ce                	mov    ecx,esi
  4327c3:	e8 48 f6 00 00       	call   0x441e10
  4327c8:	56                   	push   esi
  4327c9:	e8 82 ed 01 00       	call   0x451550
  4327ce:	83 c4 04             	add    esp,0x4
  4327d1:	89 9f 18 02 00 00    	mov    DWORD PTR [edi+0x218],ebx
  4327d7:	8b 35 dc 01 8b 00    	mov    esi,DWORD PTR ds:0x8b01dc
  4327dd:	3b f3                	cmp    esi,ebx
  4327df:	74 16                	je     0x4327f7
  4327e1:	8b ce                	mov    ecx,esi
  4327e3:	e8 e8 aa ff ff       	call   0x42d2d0
  4327e8:	56                   	push   esi
  4327e9:	e8 62 ed 01 00       	call   0x451550
  4327ee:	83 c4 04             	add    esp,0x4
  4327f1:	89 1d dc 01 8b 00    	mov    DWORD PTR ds:0x8b01dc,ebx
  4327f7:	8b 35 74 fd 89 00    	mov    esi,DWORD PTR ds:0x89fd74
  4327fd:	3b f3                	cmp    esi,ebx
  4327ff:	74 16                	je     0x432817
  432801:	8b ce                	mov    ecx,esi
  432803:	e8 28 8d fe ff       	call   0x41b530
  432808:	56                   	push   esi
  432809:	e8 42 ed 01 00       	call   0x451550
  43280e:	83 c4 04             	add    esp,0x4
  432811:	89 1d 74 fd 89 00    	mov    DWORD PTR ds:0x89fd74,ebx
  432817:	8b 35 a4 01 8a 00    	mov    esi,DWORD PTR ds:0x8a01a4
  43281d:	3b f3                	cmp    esi,ebx
  43281f:	74 16                	je     0x432837
  432821:	8b ce                	mov    ecx,esi
  432823:	e8 d8 40 ff ff       	call   0x426900
  432828:	56                   	push   esi
  432829:	e8 22 ed 01 00       	call   0x451550
  43282e:	83 c4 04             	add    esp,0x4
  432831:	89 1d a4 01 8a 00    	mov    DWORD PTR ds:0x8a01a4,ebx
  432837:	8b 35 28 24 8e 00    	mov    esi,DWORD PTR ds:0x8e2428
  43283d:	3b f3                	cmp    esi,ebx
  43283f:	74 16                	je     0x432857
  432841:	8b ce                	mov    ecx,esi
  432843:	e8 28 79 01 00       	call   0x44a170
  432848:	56                   	push   esi
  432849:	e8 02 ed 01 00       	call   0x451550
  43284e:	83 c4 04             	add    esp,0x4
  432851:	89 1d 28 24 8e 00    	mov    DWORD PTR ds:0x8e2428,ebx
  432857:	8b 0d 4c 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c4c
  43285d:	e8 de b9 00 00       	call   0x43e240
  432862:	8b cf                	mov    ecx,edi
  432864:	e8 97 f9 ff ff       	call   0x432200
  432869:	8b 35 c8 fc 89 00    	mov    esi,DWORD PTR ds:0x89fcc8
  43286f:	3b f3                	cmp    esi,ebx
  432871:	74 16                	je     0x432889
  432873:	8b ce                	mov    ecx,esi
  432875:	e8 16 f9 fd ff       	call   0x412190
  43287a:	56                   	push   esi
  43287b:	e8 d0 ec 01 00       	call   0x451550
  432880:	83 c4 04             	add    esp,0x4
  432883:	89 1d c8 fc 89 00    	mov    DWORD PTR ds:0x89fcc8,ebx
  432889:	88 9f 20 02 00 00    	mov    BYTE PTR [edi+0x220],bl
  43288f:	5f                   	pop    edi
  432890:	5e                   	pop    esi
  432891:	5b                   	pop    ebx
  432892:	c3                   	ret
  432893:	90                   	nop
  432894:	90                   	nop
  432895:	90                   	nop
  432896:	90                   	nop
  432897:	90                   	nop
  432898:	90                   	nop
  432899:	90                   	nop
  43289a:	90                   	nop
  43289b:	90                   	nop
  43289c:	90                   	nop
  43289d:	90                   	nop
  43289e:	90                   	nop
  43289f:	90                   	nop
  4328a0:	56                   	push   esi
  4328a1:	57                   	push   edi
  4328a2:	8b f1                	mov    esi,ecx
  4328a4:	6a 64                	push   0x64
  4328a6:	56                   	push   esi
  4328a7:	e8 54 1c 00 00       	call   0x434500
  4328ac:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  4328b2:	e8 d9 0c 01 00       	call   0x443590
  4328b7:	8b f8                	mov    edi,eax
  4328b9:	a1 74 fd 89 00       	mov    eax,ds:0x89fd74
  4328be:	8a 88 fc 05 00 00    	mov    cl,BYTE PTR [eax+0x5fc]
  4328c4:	84 c9                	test   cl,cl
  4328c6:	74 05                	je     0x4328cd
  4328c8:	bf 03 00 00 00       	mov    edi,0x3
  4328cd:	8a 86 40 02 00 00    	mov    al,BYTE PTR [esi+0x240]
  4328d3:	84 c0                	test   al,al
  4328d5:	0f 85 9d 00 00 00    	jne    0x432978
  4328db:	83 ff 01             	cmp    edi,0x1
  4328de:	74 0f                	je     0x4328ef
  4328e0:	83 ff 02             	cmp    edi,0x2
  4328e3:	74 0a                	je     0x4328ef
  4328e5:	83 ff 04             	cmp    edi,0x4
  4328e8:	74 05                	je     0x4328ef
  4328ea:	83 ff 03             	cmp    edi,0x3
  4328ed:	75 7b                	jne    0x43296a
  4328ef:	83 ff 01             	cmp    edi,0x1
  4328f2:	c6 86 40 02 00 00 01 	mov    BYTE PTR [esi+0x240],0x1
  4328f9:	74 05                	je     0x432900
  4328fb:	83 ff 04             	cmp    edi,0x4
  4328fe:	75 1d                	jne    0x43291d
  432900:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  432906:	6a 01                	push   0x1
  432908:	e8 53 0d 01 00       	call   0x443660
  43290d:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  432913:	c7 81 18 01 00 00 01 	mov    DWORD PTR [ecx+0x118],0x1
  43291a:	00 00 00 
  43291d:	83 ff 02             	cmp    edi,0x2
  432920:	75 1a                	jne    0x43293c
  432922:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  432928:	57                   	push   edi
  432929:	e8 32 0d 01 00       	call   0x443660
  43292e:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  432934:	89 ba 18 01 00 00    	mov    DWORD PTR [edx+0x118],edi
  43293a:	eb 23                	jmp    0x43295f
  43293c:	83 ff 03             	cmp    edi,0x3
  43293f:	75 19                	jne    0x43295a
  432941:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  432947:	57                   	push   edi
  432948:	e8 13 0d 01 00       	call   0x443660
  43294d:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  432952:	89 b8 18 01 00 00    	mov    DWORD PTR [eax+0x118],edi
  432958:	eb 05                	jmp    0x43295f
  43295a:	83 ff 04             	cmp    edi,0x4
  43295d:	74 0b                	je     0x43296a
  43295f:	8b 0d c8 fc 89 00    	mov    ecx,DWORD PTR ds:0x89fcc8
  432965:	e8 d6 fa fd ff       	call   0x412440
  43296a:	8a 86 40 02 00 00    	mov    al,BYTE PTR [esi+0x240]
  432970:	84 c0                	test   al,al
  432972:	0f 84 a2 00 00 00    	je     0x432a1a
  432978:	a1 c8 fc 89 00       	mov    eax,ds:0x89fcc8
  43297d:	8a 88 79 05 00 00    	mov    cl,BYTE PTR [eax+0x579]
  432983:	84 c9                	test   cl,cl
  432985:	0f 85 8f 00 00 00    	jne    0x432a1a
  43298b:	8b 88 00 06 00 00    	mov    ecx,DWORD PTR [eax+0x600]
  432991:	8a 81 79 05 00 00    	mov    al,BYTE PTR [ecx+0x579]
  432997:	84 c0                	test   al,al
  432999:	75 7f                	jne    0x432a1a
  43299b:	8a 86 48 02 00 00    	mov    al,BYTE PTR [esi+0x248]
  4329a1:	84 c0                	test   al,al
  4329a3:	75 42                	jne    0x4329e7
  4329a5:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  4329ab:	6a 01                	push   0x1
  4329ad:	6a 00                	push   0x0
  4329af:	68 40 ac 27 00       	push   0x27ac40
  4329b4:	68 f6 00 00 00       	push   0xf6
  4329b9:	68 25 01 00 00       	push   0x125
  4329be:	68 f6 00 00 00       	push   0xf6
  4329c3:	68 25 01 00 00       	push   0x125
  4329c8:	68 98 a4 46 00       	push   0x46a498
  4329cd:	e8 fe aa ff ff       	call   0x42d4d0
  4329d2:	8b 0d dc 01 8b 00    	mov    ecx,DWORD PTR ds:0x8b01dc
  4329d8:	6a 00                	push   0x0
  4329da:	50                   	push   eax
  4329db:	e8 00 ad ff ff       	call   0x42d6e0
  4329e0:	c6 86 48 02 00 00 01 	mov    BYTE PTR [esi+0x248],0x1
  4329e7:	8b 86 44 02 00 00    	mov    eax,DWORD PTR [esi+0x244]
  4329ed:	85 c0                	test   eax,eax
  4329ef:	7f 22                	jg     0x432a13
  4329f1:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  4329f7:	e8 64 f5 00 00       	call   0x441f60
  4329fc:	85 f6                	test   esi,esi
  4329fe:	74 1a                	je     0x432a1a
  432a00:	8b ce                	mov    ecx,esi
  432a02:	e8 29 f7 ff ff       	call   0x432130
  432a07:	56                   	push   esi
  432a08:	e8 43 eb 01 00       	call   0x451550
  432a0d:	83 c4 04             	add    esp,0x4
  432a10:	5f                   	pop    edi
  432a11:	5e                   	pop    esi
  432a12:	c3                   	ret
  432a13:	48                   	dec    eax
  432a14:	89 86 44 02 00 00    	mov    DWORD PTR [esi+0x244],eax
  432a1a:	5f                   	pop    edi
  432a1b:	5e                   	pop    esi
  432a1c:	c3                   	ret
  432a1d:	90                   	nop
  432a1e:	90                   	nop
  432a1f:	90                   	nop
