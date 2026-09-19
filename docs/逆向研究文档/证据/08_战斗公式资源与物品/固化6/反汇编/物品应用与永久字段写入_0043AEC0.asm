  43aec0:	83 ec 68             	sub    esp,0x68
  43aec3:	53                   	push   ebx
  43aec4:	8b d9                	mov    ebx,ecx
  43aec6:	8b 4c 24 70          	mov    ecx,DWORD PTR [esp+0x70]
  43aeca:	55                   	push   ebp
  43aecb:	56                   	push   esi
  43aecc:	57                   	push   edi
  43aecd:	8b 01                	mov    eax,DWORD PTR [ecx]
  43aecf:	33 f6                	xor    esi,esi
  43aed1:	89 71 08             	mov    DWORD PTR [ecx+0x8],esi
  43aed4:	33 d2                	xor    edx,edx
  43aed6:	8d 3c c5 00 00 00 00 	lea    edi,[eax*8+0x0]
  43aedd:	89 5c 24 20          	mov    DWORD PTR [esp+0x20],ebx
  43aee1:	2b f8                	sub    edi,eax
  43aee3:	c6 44 24 13 01       	mov    BYTE PTR [esp+0x13],0x1
  43aee8:	89 74 24 14          	mov    DWORD PTR [esp+0x14],esi
  43aeec:	89 74 24 18          	mov    DWORD PTR [esp+0x18],esi
  43aef0:	8d 3c b8             	lea    edi,[eax+edi*4]
  43aef3:	89 74 24 1c          	mov    DWORD PTR [esp+0x1c],esi
  43aef7:	c1 e7 04             	shl    edi,0x4
  43aefa:	2b f8                	sub    edi,eax
  43aefc:	8b 83 ac 00 00 00    	mov    eax,DWORD PTR [ebx+0xac]
  43af02:	8d 2c 78             	lea    ebp,[eax+edi*2]
  43af05:	8b 44 78 28          	mov    eax,DWORD PTR [eax+edi*2+0x28]
  43af09:	3b c6                	cmp    eax,esi
  43af0b:	7e 1d                	jle    0x43af2a
  43af0d:	89 41 08             	mov    DWORD PTR [ecx+0x8],eax
  43af10:	89 83 c4 00 00 00    	mov    DWORD PTR [ebx+0xc4],eax
  43af16:	8b 09                	mov    ecx,DWORD PTR [ecx]
  43af18:	5f                   	pop    edi
  43af19:	5e                   	pop    esi
  43af1a:	89 8b ec 00 00 00    	mov    DWORD PTR [ebx+0xec],ecx
  43af20:	5d                   	pop    ebp
  43af21:	b0 01                	mov    al,0x1
  43af23:	5b                   	pop    ebx
  43af24:	83 c4 68             	add    esp,0x68
  43af27:	c2 04 00             	ret    0x4
  43af2a:	8b 85 c8 00 00 00    	mov    eax,DWORD PTR [ebp+0xc8]
  43af30:	83 f8 02             	cmp    eax,0x2
  43af33:	74 05                	je     0x43af3a
  43af35:	83 f8 03             	cmp    eax,0x3
  43af38:	75 1e                	jne    0x43af58
  43af3a:	8b 85 c4 00 00 00    	mov    eax,DWORD PTR [ebp+0xc4]
  43af40:	89 44 24 14          	mov    DWORD PTR [esp+0x14],eax
  43af44:	8b 85 d8 00 00 00    	mov    eax,DWORD PTR [ebp+0xd8]
  43af4a:	89 44 24 18          	mov    DWORD PTR [esp+0x18],eax
  43af4e:	8b 85 dc 00 00 00    	mov    eax,DWORD PTR [ebp+0xdc]
  43af54:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  43af58:	8b 85 b8 00 00 00    	mov    eax,DWORD PTR [ebp+0xb8]
  43af5e:	83 f8 01             	cmp    eax,0x1
  43af61:	74 18                	je     0x43af7b
  43af63:	83 f8 02             	cmp    eax,0x2
  43af66:	74 13                	je     0x43af7b
  43af68:	83 f8 04             	cmp    eax,0x4
  43af6b:	75 3b                	jne    0x43afa8
  43af6d:	8b 49 04             	mov    ecx,DWORD PTR [ecx+0x4]
  43af70:	ba 01 00 00 00       	mov    edx,0x1
  43af75:	89 4c 24 28          	mov    DWORD PTR [esp+0x28],ecx
  43af79:	eb 2d                	jmp    0x43afa8
  43af7b:	8b 85 bc 00 00 00    	mov    eax,DWORD PTR [ebp+0xbc]
  43af81:	3b c6                	cmp    eax,esi
  43af83:	75 0c                	jne    0x43af91
  43af85:	8b 41 10             	mov    eax,DWORD PTR [ecx+0x10]
  43af88:	8b 51 0c             	mov    edx,DWORD PTR [ecx+0xc]
  43af8b:	89 44 24 28          	mov    DWORD PTR [esp+0x28],eax
  43af8f:	eb 17                	jmp    0x43afa8
  43af91:	83 f8 03             	cmp    eax,0x3
  43af94:	75 12                	jne    0x43afa8
  43af96:	8b 51 0c             	mov    edx,DWORD PTR [ecx+0xc]
  43af99:	3b d6                	cmp    edx,esi
  43af9b:	7e 0b                	jle    0x43afa8
  43af9d:	8d 71 10             	lea    esi,[ecx+0x10]
  43afa0:	8b ca                	mov    ecx,edx
  43afa2:	8d 7c 24 28          	lea    edi,[esp+0x28]
  43afa6:	f3 a5                	rep movs DWORD PTR es:[edi],DWORD PTR ds:[esi]
  43afa8:	85 d2                	test   edx,edx
  43afaa:	0f 8e 69 01 00 00    	jle    0x43b119
  43afb0:	8d 4c 24 28          	lea    ecx,[esp+0x28]
  43afb4:	89 54 24 24          	mov    DWORD PTR [esp+0x24],edx
  43afb8:	89 4c 24 7c          	mov    DWORD PTR [esp+0x7c],ecx
  43afbc:	8b 54 24 7c          	mov    edx,DWORD PTR [esp+0x7c]
  43afc0:	8b 02                	mov    eax,DWORD PTR [edx]
  43afc2:	85 c0                	test   eax,eax
  43afc4:	0f 8e 5d 01 00 00    	jle    0x43b127
  43afca:	83 f8 09             	cmp    eax,0x9
  43afcd:	0f 8d 54 01 00 00    	jge    0x43b127
  43afd3:	8d 0c 40             	lea    ecx,[eax+eax*2]
  43afd6:	8d bd fc 00 00 00    	lea    edi,[ebp+0xfc]
  43afdc:	8d 0c c9             	lea    ecx,[ecx+ecx*8]
  43afdf:	d1 e1                	shl    ecx,1
  43afe1:	2b c8                	sub    ecx,eax
  43afe3:	8b 83 a4 00 00 00    	mov    eax,DWORD PTR [ebx+0xa4]
  43afe9:	c1 e1 04             	shl    ecx,0x4
  43afec:	03 c8                	add    ecx,eax
  43afee:	bb 05 00 00 00       	mov    ebx,0x5
  43aff3:	8b 57 ec             	mov    edx,DWORD PTR [edi-0x14]
  43aff6:	8b 85 d0 00 00 00    	mov    eax,DWORD PTR [ebp+0xd0]
  43affc:	85 d2                	test   edx,edx
  43affe:	7e 44                	jle    0x43b044
  43b000:	8b 17                	mov    edx,DWORD PTR [edi]
  43b002:	8d 72 d5             	lea    esi,[edx-0x2b]
  43b005:	83 fe 11             	cmp    esi,0x11
  43b008:	77 3a                	ja     0x43b044
  43b00a:	33 d2                	xor    edx,edx
  43b00c:	8a 96 54 b1 43 00    	mov    dl,BYTE PTR [esi+0x43b154]
  43b012:	ff 24 95 30 b1 43 00 	jmp    DWORD PTR [edx*4+0x43b130]
  43b019:	c7 81 9c 00 00 00 01 	mov    DWORD PTR [ecx+0x9c],0x1
  43b020:	00 00 00 
  43b023:	01 41 3c             	add    DWORD PTR [ecx+0x3c],eax
  43b026:	eb 1c                	jmp    0x43b044
  43b028:	01 41 40             	add    DWORD PTR [ecx+0x40],eax
  43b02b:	eb 17                	jmp    0x43b044
  43b02d:	01 41 48             	add    DWORD PTR [ecx+0x48],eax
  43b030:	eb 12                	jmp    0x43b044
  43b032:	01 41 4c             	add    DWORD PTR [ecx+0x4c],eax
  43b035:	eb 0d                	jmp    0x43b044
  43b037:	01 41 44             	add    DWORD PTR [ecx+0x44],eax
  43b03a:	eb 08                	jmp    0x43b044
  43b03c:	01 41 2c             	add    DWORD PTR [ecx+0x2c],eax
  43b03f:	eb 03                	jmp    0x43b044
  43b041:	01 41 34             	add    DWORD PTR [ecx+0x34],eax
  43b044:	83 c7 04             	add    edi,0x4
  43b047:	4b                   	dec    ebx
  43b048:	75 a9                	jne    0x43aff3
  43b04a:	8b 7c 24 18          	mov    edi,DWORD PTR [esp+0x18]
  43b04e:	85 ff                	test   edi,edi
  43b050:	7e 48                	jle    0x43b09a
  43b052:	8b 71 2c             	mov    esi,DWORD PTR [ecx+0x2c]
  43b055:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  43b05a:	8b d6                	mov    edx,esi
  43b05c:	0f af 54 24 14       	imul   edx,DWORD PTR [esp+0x14]
  43b061:	f7 ea                	imul   edx
  43b063:	c1 fa 05             	sar    edx,0x5
  43b066:	8b c2                	mov    eax,edx
  43b068:	c1 e8 1f             	shr    eax,0x1f
  43b06b:	03 d0                	add    edx,eax
  43b06d:	8b 85 d0 00 00 00    	mov    eax,DWORD PTR [ebp+0xd0]
  43b073:	03 d0                	add    edx,eax
  43b075:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  43b07a:	0f af d7             	imul   edx,edi
  43b07d:	f7 ea                	imul   edx
  43b07f:	8b 79 30             	mov    edi,DWORD PTR [ecx+0x30]
  43b082:	c1 fa 05             	sar    edx,0x5
  43b085:	8b c2                	mov    eax,edx
  43b087:	c1 e8 1f             	shr    eax,0x1f
  43b08a:	03 d0                	add    edx,eax
  43b08c:	03 fa                	add    edi,edx
  43b08e:	8b c7                	mov    eax,edi
  43b090:	89 79 30             	mov    DWORD PTR [ecx+0x30],edi
  43b093:	3b c6                	cmp    eax,esi
  43b095:	7e 03                	jle    0x43b09a
  43b097:	89 71 30             	mov    DWORD PTR [ecx+0x30],esi
  43b09a:	8b 7c 24 1c          	mov    edi,DWORD PTR [esp+0x1c]
  43b09e:	85 ff                	test   edi,edi
  43b0a0:	7e 48                	jle    0x43b0ea
  43b0a2:	8b 71 34             	mov    esi,DWORD PTR [ecx+0x34]
  43b0a5:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  43b0aa:	8b d6                	mov    edx,esi
  43b0ac:	0f af 54 24 14       	imul   edx,DWORD PTR [esp+0x14]
  43b0b1:	f7 ea                	imul   edx
  43b0b3:	c1 fa 05             	sar    edx,0x5
  43b0b6:	8b c2                	mov    eax,edx
  43b0b8:	c1 e8 1f             	shr    eax,0x1f
  43b0bb:	03 d0                	add    edx,eax
  43b0bd:	8b 85 d0 00 00 00    	mov    eax,DWORD PTR [ebp+0xd0]
  43b0c3:	03 d0                	add    edx,eax
  43b0c5:	b8 1f 85 eb 51       	mov    eax,0x51eb851f
  43b0ca:	0f af d7             	imul   edx,edi
  43b0cd:	f7 ea                	imul   edx
  43b0cf:	8b 79 38             	mov    edi,DWORD PTR [ecx+0x38]
  43b0d2:	c1 fa 05             	sar    edx,0x5
  43b0d5:	8b c2                	mov    eax,edx
  43b0d7:	c1 e8 1f             	shr    eax,0x1f
  43b0da:	03 d0                	add    edx,eax
  43b0dc:	03 fa                	add    edi,edx
  43b0de:	8b c7                	mov    eax,edi
  43b0e0:	89 79 38             	mov    DWORD PTR [ecx+0x38],edi
  43b0e3:	3b c6                	cmp    eax,esi
  43b0e5:	7e 03                	jle    0x43b0ea
  43b0e7:	89 71 38             	mov    DWORD PTR [ecx+0x38],esi
  43b0ea:	8b 45 28             	mov    eax,DWORD PTR [ebp+0x28]
  43b0ed:	85 c0                	test   eax,eax
  43b0ef:	7e 0a                	jle    0x43b0fb
  43b0f1:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  43b0f5:	89 81 c4 00 00 00    	mov    DWORD PTR [ecx+0xc4],eax
  43b0fb:	8b 5c 24 20          	mov    ebx,DWORD PTR [esp+0x20]
  43b0ff:	8b 4c 24 7c          	mov    ecx,DWORD PTR [esp+0x7c]
  43b103:	8b 44 24 24          	mov    eax,DWORD PTR [esp+0x24]
  43b107:	83 c1 04             	add    ecx,0x4
  43b10a:	48                   	dec    eax
  43b10b:	89 4c 24 7c          	mov    DWORD PTR [esp+0x7c],ecx
  43b10f:	89 44 24 24          	mov    DWORD PTR [esp+0x24],eax
  43b113:	0f 85 a3 fe ff ff    	jne    0x43afbc
  43b119:	8a 44 24 13          	mov    al,BYTE PTR [esp+0x13]
  43b11d:	5f                   	pop    edi
  43b11e:	5e                   	pop    esi
  43b11f:	5d                   	pop    ebp
  43b120:	5b                   	pop    ebx
  43b121:	83 c4 68             	add    esp,0x68
  43b124:	c2 04 00             	ret    0x4
  43b127:	c6 44 24 13 00       	mov    BYTE PTR [esp+0x13],0x0
  43b12c:	eb d1                	jmp    0x43b0ff
  43b12e:	8b ff                	mov    edi,edi
  43b130:	19 b0 43 00 23 b0    	sbb    DWORD PTR [eax-0x4fdcffbd],esi
  43b136:	43                   	inc    ebx
  43b137:	00 28                	add    BYTE PTR [eax],ch
  43b139:	b0 43                	mov    al,0x43
  43b13b:	00 2d b0 43 00 32    	add    BYTE PTR ds:0x320043b0,ch
  43b141:	b0 43                	mov    al,0x43
  43b143:	00 37                	add    BYTE PTR [edi],dh
  43b145:	b0 43                	mov    al,0x43
  43b147:	00 3c b0             	add    BYTE PTR [eax+esi*4],bh
  43b14a:	43                   	inc    ebx
  43b14b:	00 41 b0             	add    BYTE PTR [ecx-0x50],al
  43b14e:	43                   	inc    ebx
  43b14f:	00 44 b0 43          	add    BYTE PTR [eax+esi*4+0x43],al
  43b153:	00 00                	add    BYTE PTR [eax],al
  43b155:	08 08                	or     BYTE PTR [eax],cl
  43b157:	08 08                	or     BYTE PTR [eax],cl
  43b159:	08 08                	or     BYTE PTR [eax],cl
  43b15b:	08 08                	or     BYTE PTR [eax],cl
  43b15d:	08 08                	or     BYTE PTR [eax],cl
  43b15f:	01 02                	add    DWORD PTR [edx],eax
  43b161:	03 04 05 06 07 90 90 	add    eax,DWORD PTR [eax*1-0x6f6ff8fa]
  43b168:	90                   	nop
  43b169:	90                   	nop
  43b16a:	90                   	nop
  43b16b:	90                   	nop
  43b16c:	90                   	nop
  43b16d:	90                   	nop
  43b16e:	90                   	nop
  43b16f:	90                   	nop
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
