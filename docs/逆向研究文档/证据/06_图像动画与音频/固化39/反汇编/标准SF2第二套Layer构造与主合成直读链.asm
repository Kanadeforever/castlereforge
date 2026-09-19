; 固化39证据：标准SF2第二套Layer构造与主合成直读链
; 来源：用户本轮 RPG.exe.org 解包所得 RPG.exe。
; 目的：证明 0x428ED0 同样以 Section0Record+0x5E+layer_index*29 构造Layer指针并存入对象+0x3C；
;       0x4290A0 canonical主compositor读取 +0x00/+0x02/+0x04/+0x0A/+0x0B/+0x0D/+0x0F，未直接读取 +0x06..+0x09。
; 注意：不得把此负证据扩大成“该字段永远无语义/可以丢弃”。


/mnt/data/work39/oracle/RPG.exe:     file format pei-i386


Disassembly of section .text:

00428e70 <.text+0x27e70>:
  428e70:	56                   	push   esi
  428e71:	8b f1                	mov    esi,ecx
  428e73:	57                   	push   edi
  428e74:	80 be 2d 01 00 00 01 	cmp    BYTE PTR [esi+0x12d],0x1
  428e7b:	75 4b                	jne    0x428ec8
  428e7d:	8b 46 20             	mov    eax,DWORD PTR [esi+0x20]
  428e80:	8b 7c 24 0c          	mov    edi,DWORD PTR [esp+0xc]
  428e84:	33 c9                	xor    ecx,ecx
  428e86:	66 8b 48 14          	mov    cx,WORD PTR [eax+0x14]
  428e8a:	3b f9                	cmp    edi,ecx
  428e8c:	7d 3a                	jge    0x428ec8
  428e8e:	85 ff                	test   edi,edi
  428e90:	7c 36                	jl     0x428ec8
  428e92:	8b 56 24             	mov    edx,DWORD PTR [esi+0x24]
  428e95:	8b 0c ba             	mov    ecx,DWORD PTR [edx+edi*4]
  428e98:	03 c1                	add    eax,ecx
  428e9a:	89 46 34             	mov    DWORD PTR [esi+0x34],eax
  428e9d:	66 83 78 54 00       	cmp    WORD PTR [eax+0x54],0x0
  428ea2:	75 11                	jne    0x428eb5
  428ea4:	89 7e 38             	mov    DWORD PTR [esi+0x38],edi
  428ea7:	c7 46 40 ff ff ff ff 	mov    DWORD PTR [esi+0x40],0xffffffff
  428eae:	5f                   	pop    edi
  428eaf:	b0 01                	mov    al,0x1
  428eb1:	5e                   	pop    esi
  428eb2:	c2 04 00             	ret    0x4
  428eb5:	6a 00                	push   0x0
  428eb7:	8b ce                	mov    ecx,esi
  428eb9:	e8 12 00 00 00       	call   0x428ed0
  428ebe:	89 7e 38             	mov    DWORD PTR [esi+0x38],edi
  428ec1:	5f                   	pop    edi
  428ec2:	b0 01                	mov    al,0x1
  428ec4:	5e                   	pop    esi
  428ec5:	c2 04 00             	ret    0x4
  428ec8:	5f                   	pop    edi
  428ec9:	32 c0                	xor    al,al
  428ecb:	5e                   	pop    esi
  428ecc:	c2 04 00             	ret    0x4
  428ecf:	90                   	nop
  428ed0:	8a 81 2d 01 00 00    	mov    al,BYTE PTR [ecx+0x12d]
  428ed6:	56                   	push   esi
  428ed7:	84 c0                	test   al,al
  428ed9:	75 06                	jne    0x428ee1
  428edb:	32 c0                	xor    al,al
  428edd:	5e                   	pop    esi
  428ede:	c2 04 00             	ret    0x4
  428ee1:	8b 51 34             	mov    edx,DWORD PTR [ecx+0x34]
  428ee4:	8b 44 24 08          	mov    eax,DWORD PTR [esp+0x8]
  428ee8:	33 f6                	xor    esi,esi
  428eea:	66 8b 72 54          	mov    si,WORD PTR [edx+0x54]
  428eee:	3b c6                	cmp    eax,esi
  428ef0:	7c 06                	jl     0x428ef8
  428ef2:	32 c0                	xor    al,al
  428ef4:	5e                   	pop    esi
  428ef5:	c2 04 00             	ret    0x4
  428ef8:	8d 34 c5 00 00 00 00 	lea    esi,[eax*8+0x0]
  428eff:	03 d0                	add    edx,eax
  428f01:	2b f0                	sub    esi,eax
  428f03:	89 41 40             	mov    DWORD PTR [ecx+0x40],eax
  428f06:	b0 01                	mov    al,0x1
  428f08:	8d 54 b2 5e          	lea    edx,[edx+esi*4+0x5e]
  428f0c:	5e                   	pop    esi
  428f0d:	89 51 3c             	mov    DWORD PTR [ecx+0x3c],edx
  428f10:	c2 04 00             	ret    0x4
  428f13:	90                   	nop
  428f14:	90                   	nop
  428f15:	90                   	nop
  428f16:	90                   	nop
  428f17:	90                   	nop
  428f18:	90                   	nop
  428f19:	90                   	nop
  428f1a:	90                   	nop
  428f1b:	90                   	nop
  428f1c:	90                   	nop
  428f1d:	90                   	nop
  428f1e:	90                   	nop
  428f1f:	90                   	nop
  428f20:	8a 81 2d 01 00 00    	mov    al,BYTE PTR [ecx+0x12d]
  428f26:	84 c0                	test   al,al
  428f28:	75 05                	jne    0x428f2f
  428f2a:	32 c0                	xor    al,al
  428f2c:	c2 04 00             	ret    0x4
  428f2f:	8b                   	.byte 0x8b


/mnt/data/work39/oracle/RPG.exe:     file format pei-i386


Disassembly of section .text:

004290a0 <.text+0x280a0>:
  4290a0:	83 ec 1c             	sub    esp,0x1c
  4290a3:	55                   	push   ebp
  4290a4:	8b 6c 24 2c          	mov    ebp,DWORD PTR [esp+0x2c]
  4290a8:	56                   	push   esi
  4290a9:	8b f1                	mov    esi,ecx
  4290ab:	57                   	push   edi
  4290ac:	8b 7c 24 30          	mov    edi,DWORD PTR [esp+0x30]
  4290b0:	8a 8e 2d 01 00 00    	mov    cl,BYTE PTR [esi+0x12d]
  4290b6:	33 c0                	xor    eax,eax
  4290b8:	84 c9                	test   cl,cl
  4290ba:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  4290be:	89 7e 10             	mov    DWORD PTR [esi+0x10],edi
  4290c1:	89 6e 14             	mov    DWORD PTR [esi+0x14],ebp
  4290c4:	0f 84 25 03 00 00    	je     0x4293ef
  4290ca:	8b 4c 24 2c          	mov    ecx,DWORD PTR [esp+0x2c]
  4290ce:	39 41 10             	cmp    DWORD PTR [ecx+0x10],eax
  4290d1:	0f 84 18 03 00 00    	je     0x4293ef
  4290d7:	39 46 1c             	cmp    DWORD PTR [esi+0x1c],eax
  4290da:	0f 84 0f 03 00 00    	je     0x4293ef
  4290e0:	83 7e 38 ff          	cmp    DWORD PTR [esi+0x38],0xffffffff
  4290e4:	0f 84 05 03 00 00    	je     0x4293ef
  4290ea:	8b 56 34             	mov    edx,DWORD PTR [esi+0x34]
  4290ed:	89 46 18             	mov    DWORD PTR [esi+0x18],eax
  4290f0:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  4290f4:	66 39 42 54          	cmp    WORD PTR [edx+0x54],ax
  4290f8:	0f 86 df 02 00 00    	jbe    0x4293dd
  4290fe:	53                   	push   ebx
  4290ff:	50                   	push   eax
  429100:	8b ce                	mov    ecx,esi
  429102:	e8 c9 fd ff ff       	call   0x428ed0
  429107:	8b 46 3c             	mov    eax,DWORD PTR [esi+0x3c]
  42910a:	33 c9                	xor    ecx,ecx
  42910c:	66 8b 08             	mov    cx,WORD PTR [eax]
  42910f:	51                   	push   ecx
  429110:	8b ce                	mov    ecx,esi
  429112:	e8 09 fe ff ff       	call   0x428f20
  429117:	8b 4e 44             	mov    ecx,DWORD PTR [esi+0x44]
  42911a:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  42911e:	85 d2                	test   edx,edx
  429120:	8b 41 0c             	mov    eax,DWORD PTR [ecx+0xc]
  429123:	89 46 18             	mov    DWORD PTR [esi+0x18],eax
  429126:	75 04                	jne    0x42912c
  429128:	89 44 24 10          	mov    DWORD PTR [esp+0x10],eax
  42912c:	85 c0                	test   eax,eax
  42912e:	0f 8e 0b 02 00 00    	jle    0x42933f
  429134:	8b 4e 54             	mov    ecx,DWORD PTR [esi+0x54]
  429137:	85 c9                	test   ecx,ecx
  429139:	0f 84 83 02 00 00    	je     0x4293c2
  42913f:	50                   	push   eax
  429140:	e8 2b fd ff ff       	call   0x428e70
  429145:	8b 4e 54             	mov    ecx,DWORD PTR [esi+0x54]
  429148:	33 db                	xor    ebx,ebx
  42914a:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  42914e:	8b 51 34             	mov    edx,DWORD PTR [ecx+0x34]
  429151:	66 39 5a 54          	cmp    WORD PTR [edx+0x54],bx
  429155:	0f 86 67 02 00 00    	jbe    0x4293c2
  42915b:	8d 44 24 24          	lea    eax,[esp+0x24]
  42915f:	8d 54 24 28          	lea    edx,[esp+0x28]
  429163:	50                   	push   eax
  429164:	52                   	push   edx
  429165:	e8 c6 11 00 00       	call   0x42a330
  42916a:	8b 4e 54             	mov    ecx,DWORD PTR [esi+0x54]
  42916d:	53                   	push   ebx
  42916e:	e8 5d fd ff ff       	call   0x428ed0
  429173:	8b 4e 54             	mov    ecx,DWORD PTR [esi+0x54]
  429176:	8a 81 2d 01 00 00    	mov    al,BYTE PTR [ecx+0x12d]
  42917c:	84 c0                	test   al,al
  42917e:	75 04                	jne    0x429184
  429180:	33 c0                	xor    eax,eax
  429182:	eb 08                	jmp    0x42918c
  429184:	8b 51 3c             	mov    edx,DWORD PTR [ecx+0x3c]
  429187:	33 c0                	xor    eax,eax
  429189:	66 8b 02             	mov    ax,WORD PTR [edx]
  42918c:	50                   	push   eax
  42918d:	e8 8e fd ff ff       	call   0x428f20
  429192:	8b 44 24 3c          	mov    eax,DWORD PTR [esp+0x3c]
  429196:	8b 4e 54             	mov    ecx,DWORD PTR [esi+0x54]
  429199:	83 f8 ff             	cmp    eax,0xffffffff
  42919c:	0f 85 be 00 00 00    	jne    0x429260
  4291a2:	8a 81 2d 01 00 00    	mov    al,BYTE PTR [ecx+0x12d]
  4291a8:	84 c0                	test   al,al
  4291aa:	75 04                	jne    0x4291b0
  4291ac:	33 ed                	xor    ebp,ebp
  4291ae:	eb 15                	jmp    0x4291c5
  4291b0:	8b 41 44             	mov    eax,DWORD PTR [ecx+0x44]
  4291b3:	8b 51 20             	mov    edx,DWORD PTR [ecx+0x20]
  4291b6:	33 ed                	xor    ebp,ebp
  4291b8:	66 8b 68 02          	mov    bp,WORD PTR [eax+0x2]
  4291bc:	33 c0                	xor    eax,eax
  4291be:	66 8b 42 09          	mov    ax,WORD PTR [edx+0x9]
  4291c2:	0f af e8             	imul   ebp,eax
  4291c5:	8a 81 2d 01 00 00    	mov    al,BYTE PTR [ecx+0x12d]
  4291cb:	84 c0                	test   al,al
  4291cd:	75 0a                	jne    0x4291d9
  4291cf:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  4291d6:	00 
  4291d7:	eb 18                	jmp    0x4291f1
  4291d9:	8b 51 44             	mov    edx,DWORD PTR [ecx+0x44]
  4291dc:	33 c0                	xor    eax,eax
  4291de:	33 ff                	xor    edi,edi
  4291e0:	66 8b 02             	mov    ax,WORD PTR [edx]
  4291e3:	8b 51 20             	mov    edx,DWORD PTR [ecx+0x20]
  4291e6:	66 8b 7a 07          	mov    di,WORD PTR [edx+0x7]
  4291ea:	0f af c7             	imul   eax,edi
  4291ed:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  4291f1:	8b 7e 3c             	mov    edi,DWORD PTR [esi+0x3c]
  4291f4:	8b 5e 44             	mov    ebx,DWORD PTR [esi+0x44]
  4291f7:	0f bf 47 0f          	movsx  eax,WORD PTR [edi+0xf]
  4291fb:	0f bf 57 0d          	movsx  edx,WORD PTR [edi+0xd]
  4291ff:	50                   	push   eax
  429200:	52                   	push   edx
  429201:	0f bf 47 0b          	movsx  eax,WORD PTR [edi+0xb]
  429205:	33 d2                	xor    edx,edx
  429207:	50                   	push   eax
  429208:	8a 57 0a             	mov    dl,BYTE PTR [edi+0xa]
  42920b:	8b 43 08             	mov    eax,DWORD PTR [ebx+0x8]
  42920e:	52                   	push   edx
  42920f:	99                   	cdq
  429210:	2b c2                	sub    eax,edx
  429212:	8b d0                	mov    edx,eax
  429214:	0f bf 47 04          	movsx  eax,WORD PTR [edi+0x4]
  429218:	d1 fa                	sar    edx,1
  42921a:	03 d0                	add    edx,eax
  42921c:	8b c5                	mov    eax,ebp
  42921e:	89 54 24 30          	mov    DWORD PTR [esp+0x30],edx
  429222:	8b 6c 24 48          	mov    ebp,DWORD PTR [esp+0x48]
  429226:	99                   	cdq
  429227:	2b c2                	sub    eax,edx
  429229:	8b 54 24 30          	mov    edx,DWORD PTR [esp+0x30]
  42922d:	d1 f8                	sar    eax,1
  42922f:	2b d0                	sub    edx,eax
  429231:	8b 43 04             	mov    eax,DWORD PTR [ebx+0x4]
  429234:	03 d5                	add    edx,ebp
  429236:	52                   	push   edx
  429237:	99                   	cdq
  429238:	2b c2                	sub    eax,edx
  42923a:	8b d8                	mov    ebx,eax
  42923c:	0f bf 47 02          	movsx  eax,WORD PTR [edi+0x2]
  429240:	d1 fb                	sar    ebx,1
  429242:	03 d8                	add    ebx,eax
  429244:	8b 44 24 28          	mov    eax,DWORD PTR [esp+0x28]
  429248:	8b 7c 24 48          	mov    edi,DWORD PTR [esp+0x48]
  42924c:	99                   	cdq
  42924d:	2b c2                	sub    eax,edx
  42924f:	8b 54 24 44          	mov    edx,DWORD PTR [esp+0x44]
  429253:	d1 f8                	sar    eax,1
  429255:	2b d8                	sub    ebx,eax
  429257:	03 df                	add    ebx,edi
  429259:	53                   	push   ebx
  42925a:	52                   	push   edx
  42925b:	e9 b8 00 00 00       	jmp    0x429318
  429260:	8a 91 2d 01 00 00    	mov    dl,BYTE PTR [ecx+0x12d]
  429266:	84 d2                	test   dl,dl
  429268:	75 04                	jne    0x42926e
  42926a:	33 ed                	xor    ebp,ebp
  42926c:	eb 15                	jmp    0x429283
  42926e:	8b 51 44             	mov    edx,DWORD PTR [ecx+0x44]
  429271:	33 ed                	xor    ebp,ebp
  429273:	33 ff                	xor    edi,edi
  429275:	66 8b 6a 02          	mov    bp,WORD PTR [edx+0x2]
  429279:	8b 51 20             	mov    edx,DWORD PTR [ecx+0x20]
  42927c:	66 8b 7a 09          	mov    di,WORD PTR [edx+0x9]
  429280:	0f af ef             	imul   ebp,edi
  429283:	8a 91 2d 01 00 00    	mov    dl,BYTE PTR [ecx+0x12d]
  429289:	84 d2                	test   dl,dl
  42928b:	75 0a                	jne    0x429297
  42928d:	c7 44 24 14 00 00 00 	mov    DWORD PTR [esp+0x14],0x0
  429294:	00 
  429295:	eb 1a                	jmp    0x4292b1
  429297:	8b 51 44             	mov    edx,DWORD PTR [ecx+0x44]
  42929a:	33 ff                	xor    edi,edi
  42929c:	33 db                	xor    ebx,ebx
  42929e:	66 8b 3a             	mov    di,WORD PTR [edx]
  4292a1:	8b d7                	mov    edx,edi
  4292a3:	8b 79 20             	mov    edi,DWORD PTR [ecx+0x20]
  4292a6:	66 8b 5f 07          	mov    bx,WORD PTR [edi+0x7]
  4292aa:	0f af d3             	imul   edx,ebx
  4292ad:	89 54 24 14          	mov    DWORD PTR [esp+0x14],edx
  4292b1:	8b 54 24 48          	mov    edx,DWORD PTR [esp+0x48]
  4292b5:	8b 7e 44             	mov    edi,DWORD PTR [esi+0x44]
  4292b8:	52                   	push   edx
  4292b9:	8b 54 24 48          	mov    edx,DWORD PTR [esp+0x48]
  4292bd:	52                   	push   edx
  4292be:	8b 54 24 48          	mov    edx,DWORD PTR [esp+0x48]
  4292c2:	8b 5e 3c             	mov    ebx,DWORD PTR [esi+0x3c]
  4292c5:	52                   	push   edx
  4292c6:	50                   	push   eax
  4292c7:	8b 47 08             	mov    eax,DWORD PTR [edi+0x8]
  4292ca:	99                   	cdq
  4292cb:	2b c2                	sub    eax,edx
  4292cd:	8b d0                	mov    edx,eax
  4292cf:	0f bf 43 04          	movsx  eax,WORD PTR [ebx+0x4]
  4292d3:	d1 fa                	sar    edx,1
  4292d5:	03 d0                	add    edx,eax
  4292d7:	8b c5                	mov    eax,ebp
  4292d9:	89 54 24 30          	mov    DWORD PTR [esp+0x30],edx
  4292dd:	8b 6c 24 48          	mov    ebp,DWORD PTR [esp+0x48]
  4292e1:	99                   	cdq
  4292e2:	2b c2                	sub    eax,edx
  4292e4:	8b 54 24 30          	mov    edx,DWORD PTR [esp+0x30]
  4292e8:	d1 f8                	sar    eax,1
  4292ea:	2b d0                	sub    edx,eax
  4292ec:	8b 47 04             	mov    eax,DWORD PTR [edi+0x4]
  4292ef:	03 d5                	add    edx,ebp
  4292f1:	52                   	push   edx
  4292f2:	99                   	cdq
  4292f3:	2b c2                	sub    eax,edx
  4292f5:	8b f8                	mov    edi,eax
  4292f7:	0f bf 43 02          	movsx  eax,WORD PTR [ebx+0x2]
  4292fb:	d1 ff                	sar    edi,1
  4292fd:	03 f8                	add    edi,eax
  4292ff:	8b 44 24 28          	mov    eax,DWORD PTR [esp+0x28]
  429303:	99                   	cdq
  429304:	2b c2                	sub    eax,edx
  429306:	d1 f8                	sar    eax,1
  429308:	2b f8                	sub    edi,eax
  42930a:	8b 44 24 44          	mov    eax,DWORD PTR [esp+0x44]
  42930e:	8b d7                	mov    edx,edi
  429310:	8b 7c 24 48          	mov    edi,DWORD PTR [esp+0x48]
  429314:	03 d7                	add    edx,edi
  429316:	52                   	push   edx
  429317:	50                   	push   eax
  429318:	e8 63 01 00 00       	call   0x429480
  42931d:	8b 4e 54             	mov    ecx,DWORD PTR [esi+0x54]
  429320:	8b 5c 24 18          	mov    ebx,DWORD PTR [esp+0x18]
  429324:	33 c0                	xor    eax,eax
  429326:	43                   	inc    ebx
  429327:	8b 51 34             	mov    edx,DWORD PTR [ecx+0x34]
  42932a:	89 5c 24 18          	mov    DWORD PTR [esp+0x18],ebx
  42932e:	66 8b 42 54          	mov    ax,WORD PTR [edx+0x54]
  429332:	3b d8                	cmp    ebx,eax
  429334:	0f 8c 21 fe ff ff    	jl     0x42915b
  42933a:	e9 83 00 00 00       	jmp    0x4293c2
  42933f:	8b 46 3c             	mov    eax,DWORD PTR [esi+0x3c]
  429342:	33 db                	xor    ebx,ebx
  429344:	0f bf 50 02          	movsx  edx,WORD PTR [eax+0x2]
  429348:	89 16                	mov    DWORD PTR [esi],edx
  42934a:	0f bf 50 04          	movsx  edx,WORD PTR [eax+0x4]
  42934e:	89 56 04             	mov    DWORD PTR [esi+0x4],edx
  429351:	8b 56 20             	mov    edx,DWORD PTR [esi+0x20]
  429354:	66 8b 5a 07          	mov    bx,WORD PTR [edx+0x7]
  429358:	33 d2                	xor    edx,edx
  42935a:	66 8b 11             	mov    dx,WORD PTR [ecx]
  42935d:	0f af da             	imul   ebx,edx
  429360:	8b 56 20             	mov    edx,DWORD PTR [esi+0x20]
  429363:	89 5e 08             	mov    DWORD PTR [esi+0x8],ebx
  429366:	33 db                	xor    ebx,ebx
  429368:	66 8b 5a 09          	mov    bx,WORD PTR [edx+0x9]
  42936c:	33 d2                	xor    edx,edx
  42936e:	66 8b 51 02          	mov    dx,WORD PTR [ecx+0x2]
  429372:	8b 4c 24 3c          	mov    ecx,DWORD PTR [esp+0x3c]
  429376:	0f af da             	imul   ebx,edx
  429379:	83 f9 ff             	cmp    ecx,0xffffffff
  42937c:	89 5e 0c             	mov    DWORD PTR [esi+0xc],ebx
  42937f:	75 17                	jne    0x429398
  429381:	0f bf 48 0f          	movsx  ecx,WORD PTR [eax+0xf]
  429385:	0f bf 50 0d          	movsx  edx,WORD PTR [eax+0xd]
  429389:	51                   	push   ecx
  42938a:	52                   	push   edx
  42938b:	0f bf 48 0b          	movsx  ecx,WORD PTR [eax+0xb]
  42938f:	33 d2                	xor    edx,edx
  429391:	51                   	push   ecx
  429392:	8a 50 0a             	mov    dl,BYTE PTR [eax+0xa]
  429395:	52                   	push   edx
  429396:	eb 10                	jmp    0x4293a8
  429398:	8b 54 24 48          	mov    edx,DWORD PTR [esp+0x48]
  42939c:	52                   	push   edx
  42939d:	8b 54 24 48          	mov    edx,DWORD PTR [esp+0x48]
  4293a1:	52                   	push   edx
  4293a2:	8b 54 24 48          	mov    edx,DWORD PTR [esp+0x48]
  4293a6:	52                   	push   edx
  4293a7:	51                   	push   ecx
  4293a8:	0f bf 48 04          	movsx  ecx,WORD PTR [eax+0x4]
  4293ac:	0f bf 50 02          	movsx  edx,WORD PTR [eax+0x2]
  4293b0:	8b 44 24 40          	mov    eax,DWORD PTR [esp+0x40]
  4293b4:	03 cd                	add    ecx,ebp
  4293b6:	03 d7                	add    edx,edi
  4293b8:	51                   	push   ecx
  4293b9:	52                   	push   edx
  4293ba:	50                   	push   eax
  4293bb:	8b ce                	mov    ecx,esi
  4293bd:	e8 be 00 00 00       	call   0x429480
  4293c2:	8b 4e 34             	mov    ecx,DWORD PTR [esi+0x34]
  4293c5:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  4293c9:	33 d2                	xor    edx,edx
  4293cb:	40                   	inc    eax
  4293cc:	66 8b 51 54          	mov    dx,WORD PTR [ecx+0x54]
