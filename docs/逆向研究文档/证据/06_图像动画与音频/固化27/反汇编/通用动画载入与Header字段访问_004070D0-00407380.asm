
upload/RPG.exe:     file format pei-i386


Disassembly of section .text:

004070d0 <.text+0x60d0>:
  4070d0:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  4070d4:	56                   	push   esi
  4070d5:	8b f1                	mov    esi,ecx
  4070d7:	57                   	push   edi
  4070d8:	8b 4c 24 0c          	mov    ecx,DWORD PTR [esp+0xc]
  4070dc:	50                   	push   eax
  4070dd:	8d 7e 30             	lea    edi,[esi+0x30]
  4070e0:	57                   	push   edi
  4070e1:	51                   	push   ecx
  4070e2:	e8 c9 b4 ff ff       	call   0x4025b0
  4070e7:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
  4070eb:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  4070ef:	89 46 04             	mov    DWORD PTR [esi+0x4],eax
  4070f2:	8b 07                	mov    eax,DWORD PTR [edi]
  4070f4:	33 c9                	xor    ecx,ecx
  4070f6:	89 16                	mov    DWORD PTR [esi],edx
  4070f8:	8b 54 24 2c          	mov    edx,DWORD PTR [esp+0x2c]
  4070fc:	89 46 34             	mov    DWORD PTR [esi+0x34],eax
  4070ff:	66 8b 48 14          	mov    cx,WORD PTR [eax+0x14]
  407103:	33 ff                	xor    edi,edi
  407105:	89 4e 08             	mov    DWORD PTR [esi+0x8],ecx
  407108:	8a 4c 24 28          	mov    cl,BYTE PTR [esp+0x28]
  40710c:	89 7e 10             	mov    DWORD PTR [esi+0x10],edi
  40710f:	c7 46 14 01 00 00 00 	mov    DWORD PTR [esi+0x14],0x1
  407116:	89 7e 18             	mov    DWORD PTR [esi+0x18],edi
  407119:	89 7e 1c             	mov    DWORD PTR [esi+0x1c],edi
  40711c:	89 56 20             	mov    DWORD PTR [esi+0x20],edx
  40711f:	89 7e 24             	mov    DWORD PTR [esi+0x24],edi
  407122:	89 7e 28             	mov    DWORD PTR [esi+0x28],edi
  407125:	88 4e 2c             	mov    BYTE PTR [esi+0x2c],cl
  407128:	8b 50 16             	mov    edx,DWORD PTR [eax+0x16]
  40712b:	83 c4 0c             	add    esp,0xc
  40712e:	03 d0                	add    edx,eax
  407130:	89 56 38             	mov    DWORD PTR [esi+0x38],edx
  407133:	8b 48 20             	mov    ecx,DWORD PTR [eax+0x20]
  407136:	03 c8                	add    ecx,eax
  407138:	89 4e 3c             	mov    DWORD PTR [esi+0x3c],ecx
  40713b:	8b 50 2a             	mov    edx,DWORD PTR [eax+0x2a]
  40713e:	03 d0                	add    edx,eax
  407140:	89 56 40             	mov    DWORD PTR [esi+0x40],edx
  407143:	8b 48 34             	mov    ecx,DWORD PTR [eax+0x34]
  407146:	03 c8                	add    ecx,eax
  407148:	89 4e 44             	mov    DWORD PTR [esi+0x44],ecx
  40714b:	66 39 78 14          	cmp    WORD PTR [eax+0x14],di
  40714f:	75 09                	jne    0x40715a
  407151:	c7 46 4c ff ff ff ff 	mov    DWORD PTR [esi+0x4c],0xffffffff
  407158:	eb 08                	jmp    0x407162
  40715a:	57                   	push   edi
  40715b:	8b ce                	mov    ecx,esi
  40715d:	e8 9e 00 00 00       	call   0x407200
  407162:	8b 56 34             	mov    edx,DWORD PTR [esi+0x34]
  407165:	66 39 7a 1e          	cmp    WORD PTR [edx+0x1e],di
  407169:	75 09                	jne    0x407174
  40716b:	c7 46 5c ff ff ff ff 	mov    DWORD PTR [esi+0x5c],0xffffffff
  407172:	eb 08                	jmp    0x40717c
  407174:	57                   	push   edi
  407175:	8b ce                	mov    ecx,esi
  407177:	e8 54 01 00 00       	call   0x4072d0
  40717c:	8b 46 34             	mov    eax,DWORD PTR [esi+0x34]
  40717f:	66 39 78 32          	cmp    WORD PTR [eax+0x32],di
  407183:	75 09                	jne    0x40718e
  407185:	c7 46 64 ff ff ff ff 	mov    DWORD PTR [esi+0x64],0xffffffff
  40718c:	eb 08                	jmp    0x407196
  40718e:	57                   	push   edi
  40718f:	8b ce                	mov    ecx,esi
  407191:	e8 8a 01 00 00       	call   0x407320
  407196:	8b 0d c0 f6 89 00    	mov    ecx,DWORD PTR ds:0x89f6c0
  40719c:	83 79 3c 10          	cmp    DWORD PTR [ecx+0x3c],0x10
  4071a0:	75 07                	jne    0x4071a9
  4071a2:	8b ce                	mov    ecx,esi
  4071a4:	e8 07 00 00 00       	call   0x4071b0
  4071a9:	5f                   	pop    edi
  4071aa:	5e                   	pop    esi
  4071ab:	c2 18 00             	ret    0x18
  4071ae:	90                   	nop
  4071af:	90                   	nop
  4071b0:	56                   	push   esi
  4071b1:	8b f1                	mov    esi,ecx
  4071b3:	57                   	push   edi
  4071b4:	33 ff                	xor    edi,edi
  4071b6:	8b 46 34             	mov    eax,DWORD PTR [esi+0x34]
  4071b9:	66 39 78 28          	cmp    WORD PTR [eax+0x28],di
  4071bd:	76 30                	jbe    0x4071ef
  4071bf:	8b 4e 40             	mov    ecx,DWORD PTR [esi+0x40]
  4071c2:	8b 14 b9             	mov    edx,DWORD PTR [ecx+edi*4]
  4071c5:	8b 4e 30             	mov    ecx,DWORD PTR [esi+0x30]
  4071c8:	03 d1                	add    edx,ecx
  4071ca:	33 c9                	xor    ecx,ecx
  4071cc:	66 8b 48 09          	mov    cx,WORD PTR [eax+0x9]
  4071d0:	52                   	push   edx
  4071d1:	33 d2                	xor    edx,edx
  4071d3:	51                   	push   ecx
  4071d4:	66 8b 50 07          	mov    dx,WORD PTR [eax+0x7]
  4071d8:	52                   	push   edx
  4071d9:	e8 9d 5b 04 00       	call   0x44cd7b
  4071de:	8b 46 34             	mov    eax,DWORD PTR [esi+0x34]
  4071e1:	33 c9                	xor    ecx,ecx
  4071e3:	83 c4 0c             	add    esp,0xc
  4071e6:	47                   	inc    edi
  4071e7:	66 8b 48 28          	mov    cx,WORD PTR [eax+0x28]
  4071eb:	3b f9                	cmp    edi,ecx
  4071ed:	7c d0                	jl     0x4071bf
  4071ef:	5f                   	pop    edi
  4071f0:	5e                   	pop    esi
  4071f1:	c3                   	ret
  4071f2:	90                   	nop
  4071f3:	90                   	nop
  4071f4:	90                   	nop
  4071f5:	90                   	nop
  4071f6:	90                   	nop
  4071f7:	90                   	nop
  4071f8:	90                   	nop
  4071f9:	90                   	nop
  4071fa:	90                   	nop
  4071fb:	90                   	nop
  4071fc:	90                   	nop
  4071fd:	90                   	nop
  4071fe:	90                   	nop
  4071ff:	90                   	nop
  407200:	56                   	push   esi
  407201:	57                   	push   edi
  407202:	8b 7c 24 0c          	mov    edi,DWORD PTR [esp+0xc]
  407206:	8b f1                	mov    esi,ecx
  407208:	85 ff                	test   edi,edi
  40720a:	7c 3f                	jl     0x40724b
  40720c:	8b 46 34             	mov    eax,DWORD PTR [esi+0x34]
  40720f:	33 c9                	xor    ecx,ecx
  407211:	66 8b 48 14          	mov    cx,WORD PTR [eax+0x14]
  407215:	3b f9                	cmp    edi,ecx
  407217:	7d 32                	jge    0x40724b
  407219:	8b 56 38             	mov    edx,DWORD PTR [esi+0x38]
  40721c:	8b 0c ba             	mov    ecx,DWORD PTR [edx+edi*4]
  40721f:	03 c1                	add    eax,ecx
  407221:	89 46 48             	mov    DWORD PTR [esi+0x48],eax
  407224:	66 83 78 54 00       	cmp    WORD PTR [eax+0x54],0x0
  407229:	75 0f                	jne    0x40723a
  40722b:	89 7e 4c             	mov    DWORD PTR [esi+0x4c],edi
  40722e:	c7 46 54 ff ff ff ff 	mov    DWORD PTR [esi+0x54],0xffffffff
  407235:	5f                   	pop    edi
  407236:	5e                   	pop    esi
  407237:	c2 04 00             	ret    0x4
  40723a:	6a 00                	push   0x0
  40723c:	8b ce                	mov    ecx,esi
  40723e:	e8 2d 00 00 00       	call   0x407270
  407243:	89 7e 4c             	mov    DWORD PTR [esi+0x4c],edi
  407246:	5f                   	pop    edi
  407247:	5e                   	pop    esi
  407248:	c2 04 00             	ret    0x4
  40724b:	8d 54 24 0c          	lea    edx,[esp+0xc]
  40724f:	68 d8 37 46 00       	push   0x4637d8
  407254:	52                   	push   edx
  407255:	c7 44 24 14 14 8b 46 	mov    DWORD PTR [esp+0x14],0x468b14
  40725c:	00 
  40725d:	e8 65 a9 04 00       	call   0x451bc7
  407262:	90                   	nop
  407263:	90                   	nop
  407264:	90                   	nop
  407265:	90                   	nop
  407266:	90                   	nop
  407267:	90                   	nop
  407268:	90                   	nop
  407269:	90                   	nop
  40726a:	90                   	nop
  40726b:	90                   	nop
  40726c:	90                   	nop
  40726d:	90                   	nop
  40726e:	90                   	nop
  40726f:	90                   	nop
  407270:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  407274:	56                   	push   esi
  407275:	85 c0                	test   eax,eax
  407277:	7c 31                	jl     0x4072aa
  407279:	8b 51 48             	mov    edx,DWORD PTR [ecx+0x48]
  40727c:	33 f6                	xor    esi,esi
  40727e:	66 8b 72 54          	mov    si,WORD PTR [edx+0x54]
  407282:	3b c6                	cmp    eax,esi
  407284:	7d 24                	jge    0x4072aa
  407286:	8d 34 c5 00 00 00 00 	lea    esi,[eax*8+0x0]
  40728d:	03 d0                	add    edx,eax
  40728f:	2b f0                	sub    esi,eax
  407291:	89 41 54             	mov    DWORD PTR [ecx+0x54],eax
  407294:	33 c0                	xor    eax,eax
  407296:	8d 54 b2 5e          	lea    edx,[edx+esi*4+0x5e]
  40729a:	89 51 50             	mov    DWORD PTR [ecx+0x50],edx
  40729d:	66 8b 02             	mov    ax,WORD PTR [edx]
  4072a0:	50                   	push   eax
  4072a1:	e8 2a 00 00 00       	call   0x4072d0
  4072a6:	5e                   	pop    esi
  4072a7:	c2 04 00             	ret    0x4
  4072aa:	8d 4c 24 08          	lea    ecx,[esp+0x8]
  4072ae:	68 d8 37 46 00       	push   0x4637d8
  4072b3:	51                   	push   ecx
  4072b4:	c7 44 24 10 40 8b 46 	mov    DWORD PTR [esp+0x10],0x468b40
  4072bb:	00 
  4072bc:	e8 06 a9 04 00       	call   0x451bc7
  4072c1:	90                   	nop
  4072c2:	90                   	nop
  4072c3:	90                   	nop
  4072c4:	90                   	nop
  4072c5:	90                   	nop
  4072c6:	90                   	nop
  4072c7:	90                   	nop
  4072c8:	90                   	nop
  4072c9:	90                   	nop
  4072ca:	90                   	nop
  4072cb:	90                   	nop
  4072cc:	90                   	nop
  4072cd:	90                   	nop
  4072ce:	90                   	nop
  4072cf:	90                   	nop
  4072d0:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4072d4:	56                   	push   esi
  4072d5:	85 c0                	test   eax,eax
  4072d7:	7c 22                	jl     0x4072fb
  4072d9:	8b 51 34             	mov    edx,DWORD PTR [ecx+0x34]
  4072dc:	33 f6                	xor    esi,esi
  4072de:	66 8b 72 1e          	mov    si,WORD PTR [edx+0x1e]
  4072e2:	3b c6                	cmp    eax,esi
  4072e4:	7d 15                	jge    0x4072fb
  4072e6:	8b 51 3c             	mov    edx,DWORD PTR [ecx+0x3c]
  4072e9:	8b 71 30             	mov    esi,DWORD PTR [ecx+0x30]
  4072ec:	8b 14 82             	mov    edx,DWORD PTR [edx+eax*4]
  4072ef:	89 41 5c             	mov    DWORD PTR [ecx+0x5c],eax
  4072f2:	03 d6                	add    edx,esi
  4072f4:	5e                   	pop    esi
  4072f5:	89 51 58             	mov    DWORD PTR [ecx+0x58],edx
  4072f8:	c2 04 00             	ret    0x4
  4072fb:	8d 44 24 08          	lea    eax,[esp+0x8]
  4072ff:	68 d8 37 46 00       	push   0x4637d8
  407304:	50                   	push   eax
  407305:	c7 44 24 10 6c 8b 46 	mov    DWORD PTR [esp+0x10],0x468b6c
  40730c:	00 
  40730d:	e8 b5 a8 04 00       	call   0x451bc7
  407312:	90                   	nop
  407313:	90                   	nop
  407314:	90                   	nop
  407315:	90                   	nop
  407316:	90                   	nop
  407317:	90                   	nop
  407318:	90                   	nop
  407319:	90                   	nop
  40731a:	90                   	nop
  40731b:	90                   	nop
  40731c:	90                   	nop
  40731d:	90                   	nop
  40731e:	90                   	nop
  40731f:	90                   	nop
  407320:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  407324:	56                   	push   esi
  407325:	83 f8 ff             	cmp    eax,0xffffffff
  407328:	74 22                	je     0x40734c
  40732a:	8b 51 34             	mov    edx,DWORD PTR [ecx+0x34]
  40732d:	33 f6                	xor    esi,esi
  40732f:	66 8b 72 32          	mov    si,WORD PTR [edx+0x32]
  407333:	3b c6                	cmp    eax,esi
  407335:	7d 15                	jge    0x40734c
  407337:	8b 51 44             	mov    edx,DWORD PTR [ecx+0x44]
  40733a:	8b 71 30             	mov    esi,DWORD PTR [ecx+0x30]
  40733d:	8b 14 82             	mov    edx,DWORD PTR [edx+eax*4]
  407340:	89 41 64             	mov    DWORD PTR [ecx+0x64],eax
  407343:	03 d6                	add    edx,esi
  407345:	5e                   	pop    esi
  407346:	89 51 60             	mov    DWORD PTR [ecx+0x60],edx
  407349:	c2 04 00             	ret    0x4
  40734c:	c7 41 64 ff ff ff ff 	mov    DWORD PTR [ecx+0x64],0xffffffff
  407353:	5e                   	pop    esi
  407354:	c2 04 00             	ret    0x4
  407357:	90                   	nop
  407358:	90                   	nop
  407359:	90                   	nop
  40735a:	90                   	nop
  40735b:	90                   	nop
  40735c:	90                   	nop
  40735d:	90                   	nop
  40735e:	90                   	nop
  40735f:	90                   	nop
  407360:	8b 41 64             	mov    eax,DWORD PTR [ecx+0x64]
  407363:	83 f8 ff             	cmp    eax,0xffffffff
  407366:	75 03                	jne    0x40736b
  407368:	33 c0                	xor    eax,eax
  40736a:	c3                   	ret
  40736b:	8b 51 44             	mov    edx,DWORD PTR [ecx+0x44]
  40736e:	8b 49 30             	mov    ecx,DWORD PTR [ecx+0x30]
  407371:	8b 04 82             	mov    eax,DWORD PTR [edx+eax*4]
  407374:	8d 44 08 10          	lea    eax,[eax+ecx*1+0x10]
  407378:	c3                   	ret
  407379:	90                   	nop
  40737a:	90                   	nop
  40737b:	90                   	nop
  40737c:	90                   	nop
  40737d:	90                   	nop
  40737e:	90                   	nop
  40737f:	90                   	nop
