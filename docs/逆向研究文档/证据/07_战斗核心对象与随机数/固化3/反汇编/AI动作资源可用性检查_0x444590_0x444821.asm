; 幽城幻剑录 v0.7D阶段中-固化3 静态证据
; Oracle RPG.exe SHA256: b10c65f56051e5a625b6c34857bcb73bd002efe3c158b6bd0cc2bb17fa871dcf
; VA范围: 0x00444590..0x00444820
; 用途: 证明 FIRTTECH+0x34/+0x38 在 AI 候选可用性检查中分别与当前主/次资源比较。
; 边界: 仅证明当前 Oracle 指令结构；作者变量名/业务名需额外 consumer 或动态证据。

  444590:	83 ec 08             	sub    esp,0x8
  444593:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  444597:	53                   	push   ebx
  444598:	8b d9                	mov    ebx,ecx
  44459a:	55                   	push   ebp
  44459b:	8d 14 c5 00 00 00 00 	lea    edx,[eax*8+0x0]
  4445a2:	56                   	push   esi
  4445a3:	2b d0                	sub    edx,eax
  4445a5:	57                   	push   edi
  4445a6:	89 5c 24 14          	mov    DWORD PTR [esp+0x14],ebx
  4445aa:	c7 44 24 10 ff ff ff 	mov    DWORD PTR [esp+0x10],0xffffffff
  4445b1:	ff 
  4445b2:	8d 0c 92             	lea    ecx,[edx+edx*4]
  4445b5:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4445bb:	c1 e1 04             	shl    ecx,0x4
  4445be:	8b 92 b4 00 00 00    	mov    edx,DWORD PTR [edx+0xb4]
  4445c4:	8d 3c 0a             	lea    edi,[edx+ecx*1]
  4445c7:	8b 54 24 1c          	mov    edx,DWORD PTR [esp+0x1c]
  4445cb:	8d 0c d2             	lea    ecx,[edx+edx*8]
  4445ce:	8d 34 8a             	lea    esi,[edx+ecx*4]
  4445d1:	8d 0c 76             	lea    ecx,[esi+esi*2]
  4445d4:	8b 77 38             	mov    esi,DWORD PTR [edi+0x38]
  4445d7:	8d 14 ca             	lea    edx,[edx+ecx*8]
  4445da:	8b 8c 93 68 0c 00 00 	mov    ecx,DWORD PTR [ebx+edx*4+0xc68]
  4445e1:	3b ce                	cmp    ecx,esi
  4445e3:	8d 14 93             	lea    edx,[ebx+edx*4]
  4445e6:	0f 8c 35 02 00 00    	jl     0x444821
  4445ec:	8b 92 60 0c 00 00    	mov    edx,DWORD PTR [edx+0xc60]
  4445f2:	8b 4f 34             	mov    ecx,DWORD PTR [edi+0x34]
  4445f5:	3b d1                	cmp    edx,ecx
  4445f7:	0f 8c 24 02 00 00    	jl     0x444821
  4445fd:	8b 8b 38 e5 00 00    	mov    ecx,DWORD PTR [ebx+0xe538]
  444603:	c1 e0 06             	shl    eax,0x6
  444606:	03 c8                	add    ecx,eax
  444608:	8b 41 2c             	mov    eax,DWORD PTR [ecx+0x2c]
  44460b:	85 c0                	test   eax,eax
  44460d:	74 09                	je     0x444618
  44460f:	83 f8 01             	cmp    eax,0x1
  444612:	0f 85 f6 00 00 00    	jne    0x44470e
  444618:	8b 51 30             	mov    edx,DWORD PTR [ecx+0x30]
  44461b:	8b f8                	mov    edi,eax
  44461d:	f7 df                	neg    edi
  44461f:	1b ff                	sbb    edi,edi
  444621:	83 e7 f8             	and    edi,0xfffffff8
  444624:	83 c7 10             	add    edi,0x10
  444627:	85 d2                	test   edx,edx
  444629:	0f 8e df 00 00 00    	jle    0x44470e
  44462f:	f7 d8                	neg    eax
  444631:	1b c0                	sbb    eax,eax
  444633:	24 f8                	and    al,0xf8
  444635:	83 c0 08             	add    eax,0x8
  444638:	8b f0                	mov    esi,eax
  44463a:	3b f7                	cmp    esi,edi
  44463c:	0f 8d cc 00 00 00    	jge    0x44470e
  444642:	8d 04 f6             	lea    eax,[esi+esi*8]
  444645:	03 d6                	add    edx,esi
  444647:	8d 04 86             	lea    eax,[esi+eax*4]
  44464a:	8d 04 40             	lea    eax,[eax+eax*2]
  44464d:	8d 04 c6             	lea    eax,[esi+eax*8]
  444650:	8d ac 83 60 0c 00 00 	lea    ebp,[ebx+eax*4+0xc60]
  444657:	8d 04 f6             	lea    eax,[esi+esi*8]
  44465a:	8d 04 86             	lea    eax,[esi+eax*4]
  44465d:	8d 04 40             	lea    eax,[eax+eax*2]
  444660:	8d 14 c2             	lea    edx,[edx+eax*8]
  444663:	8d 84 93 18 0e 00 00 	lea    eax,[ebx+edx*4+0xe18]
  44466a:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  44466e:	83 7c 24 10 ff       	cmp    DWORD PTR [esp+0x10],0xffffffff
  444673:	75 78                	jne    0x4446ed
  444675:	8b 41 30             	mov    eax,DWORD PTR [ecx+0x30]
  444678:	83 f8 1c             	cmp    eax,0x1c
  44467b:	75 26                	jne    0x4446a3
  44467d:	8b 45 fc             	mov    eax,DWORD PTR [ebp-0x4]
  444680:	8d 14 40             	lea    edx,[eax+eax*2]
  444683:	b8 67 66 66 66       	mov    eax,0x66666667
  444688:	d1 e2                	shl    edx,1
  44468a:	f7 ea                	imul   edx
  44468c:	c1 fa 02             	sar    edx,0x2
  44468f:	8b c2                	mov    eax,edx
  444691:	c1 e8 1f             	shr    eax,0x1f
  444694:	03 d0                	add    edx,eax
  444696:	8b 45 00             	mov    eax,DWORD PTR [ebp+0x0]
  444699:	3b c2                	cmp    eax,edx
  44469b:	7d 37                	jge    0x4446d4
  44469d:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  4446a1:	eb 31                	jmp    0x4446d4
  4446a3:	83 f8 1d             	cmp    eax,0x1d
  4446a6:	75 57                	jne    0x4446ff
  4446a8:	8b 54 24 14          	mov    edx,DWORD PTR [esp+0x14]
  4446ac:	33 db                	xor    ebx,ebx
  4446ae:	8d 82 60 0c 00 00    	lea    eax,[edx+0xc60]
  4446b4:	ba 08 00 00 00       	mov    edx,0x8
  4446b9:	83 38 00             	cmp    DWORD PTR [eax],0x0
  4446bc:	7e 01                	jle    0x4446bf
  4446be:	43                   	inc    ebx
  4446bf:	05 e4 0d 00 00       	add    eax,0xde4
  4446c4:	4a                   	dec    edx
  4446c5:	75 f2                	jne    0x4446b9
  4446c7:	83 fb 06             	cmp    ebx,0x6
  4446ca:	7d 04                	jge    0x4446d0
  4446cc:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  4446d0:	8b 5c 24 14          	mov    ebx,DWORD PTR [esp+0x14]
  4446d4:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
  4446d8:	46                   	inc    esi
  4446d9:	81 c5 e4 0d 00 00    	add    ebp,0xde4
  4446df:	81 c2 e4 0d 00 00    	add    edx,0xde4
  4446e5:	3b f7                	cmp    esi,edi
  4446e7:	89 54 24 20          	mov    DWORD PTR [esp+0x20],edx
  4446eb:	7c 81                	jl     0x44466e
  4446ed:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  4446f1:	85 c0                	test   eax,eax
  4446f3:	7c 19                	jl     0x44470e
  4446f5:	5f                   	pop    edi
  4446f6:	5e                   	pop    esi
  4446f7:	5d                   	pop    ebp
  4446f8:	5b                   	pop    ebx
  4446f9:	83 c4 08             	add    esp,0x8
  4446fc:	c2 08 00             	ret    0x8
  4446ff:	8b 44 24 20          	mov    eax,DWORD PTR [esp+0x20]
  444703:	83 38 00             	cmp    DWORD PTR [eax],0x0
  444706:	7e cc                	jle    0x4446d4
  444708:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  44470c:	eb c6                	jmp    0x4446d4
  44470e:	8b 71 34             	mov    esi,DWORD PTR [ecx+0x34]
  444711:	85 f6                	test   esi,esi
  444713:	74 09                	je     0x44471e
  444715:	83 fe 01             	cmp    esi,0x1
  444718:	0f 85 d9 00 00 00    	jne    0x4447f7
  44471e:	8b 69 38             	mov    ebp,DWORD PTR [ecx+0x38]
  444721:	8b fe                	mov    edi,esi
  444723:	f7 df                	neg    edi
  444725:	1b ff                	sbb    edi,edi
  444727:	89 6c 24 1c          	mov    DWORD PTR [esp+0x1c],ebp
  44472b:	83 e7 f8             	and    edi,0xfffffff8
  44472e:	83 c7 10             	add    edi,0x10
  444731:	85 ed                	test   ebp,ebp
  444733:	0f 8e be 00 00 00    	jle    0x4447f7
  444739:	f7 de                	neg    esi
  44473b:	1b f6                	sbb    esi,esi
  44473d:	83 e6 f8             	and    esi,0xfffffff8
  444740:	83 c6 08             	add    esi,0x8
  444743:	3b f7                	cmp    esi,edi
  444745:	0f 8d ac 00 00 00    	jge    0x4447f7
  44474b:	8d 0c f6             	lea    ecx,[esi+esi*8]
  44474e:	8d 04 8e             	lea    eax,[esi+ecx*4]
  444751:	8d 0c f6             	lea    ecx,[esi+esi*8]
  444754:	8d 14 40             	lea    edx,[eax+eax*2]
  444757:	8d 04 d6             	lea    eax,[esi+edx*8]
  44475a:	8d 9c 83 60 0c 00 00 	lea    ebx,[ebx+eax*4+0xc60]
  444761:	8d 04 8e             	lea    eax,[esi+ecx*4]
  444764:	8b 4c 24 14          	mov    ecx,DWORD PTR [esp+0x14]
  444768:	8d 14 40             	lea    edx,[eax+eax*2]
  44476b:	8d 04 2e             	lea    eax,[esi+ebp*1]
  44476e:	8d 14 d0             	lea    edx,[eax+edx*8]
  444771:	8d 84 91 18 0e 00 00 	lea    eax,[ecx+edx*4+0xe18]
  444778:	89 44 24 20          	mov    DWORD PTR [esp+0x20],eax
  44477c:	8b 54 24 10          	mov    edx,DWORD PTR [esp+0x10]
  444780:	83 c8 ff             	or     eax,0xffffffff
  444783:	3b d0                	cmp    edx,eax
  444785:	75 70                	jne    0x4447f7
  444787:	83 fd 1c             	cmp    ebp,0x1c
  44478a:	75 25                	jne    0x4447b1
  44478c:	8b 43 fc             	mov    eax,DWORD PTR [ebx-0x4]
  44478f:	8d 14 40             	lea    edx,[eax+eax*2]
  444792:	b8 67 66 66 66       	mov    eax,0x66666667
  444797:	d1 e2                	shl    edx,1
  444799:	f7 ea                	imul   edx
  44479b:	c1 fa 02             	sar    edx,0x2
  44479e:	8b c2                	mov    eax,edx
  4447a0:	c1 e8 1f             	shr    eax,0x1f
  4447a3:	03 d0                	add    edx,eax
  4447a5:	8b 03                	mov    eax,DWORD PTR [ebx]
  4447a7:	3b c2                	cmp    eax,edx
  4447a9:	7d 33                	jge    0x4447de
  4447ab:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  4447af:	eb 2d                	jmp    0x4447de
  4447b1:	83 fd 1d             	cmp    ebp,0x1d
  4447b4:	75 4f                	jne    0x444805
  4447b6:	33 ed                	xor    ebp,ebp
  4447b8:	8d 81 60 0c 00 00    	lea    eax,[ecx+0xc60]
  4447be:	ba 08 00 00 00       	mov    edx,0x8
  4447c3:	83 38 00             	cmp    DWORD PTR [eax],0x0
  4447c6:	7e 01                	jle    0x4447c9
  4447c8:	45                   	inc    ebp
  4447c9:	05 e4 0d 00 00       	add    eax,0xde4
  4447ce:	4a                   	dec    edx
  4447cf:	75 f2                	jne    0x4447c3
  4447d1:	83 fd 06             	cmp    ebp,0x6
  4447d4:	7d 04                	jge    0x4447da
  4447d6:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  4447da:	8b 6c 24 1c          	mov    ebp,DWORD PTR [esp+0x1c]
  4447de:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
  4447e2:	46                   	inc    esi
  4447e3:	81 c3 e4 0d 00 00    	add    ebx,0xde4
  4447e9:	81 c2 e4 0d 00 00    	add    edx,0xde4
  4447ef:	3b f7                	cmp    esi,edi
  4447f1:	89 54 24 20          	mov    DWORD PTR [esp+0x20],edx
  4447f5:	7c 85                	jl     0x44477c
  4447f7:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  4447fb:	5f                   	pop    edi
  4447fc:	5e                   	pop    esi
  4447fd:	5d                   	pop    ebp
  4447fe:	5b                   	pop    ebx
  4447ff:	83 c4 08             	add    esp,0x8
  444802:	c2 08 00             	ret    0x8
  444805:	83 3b 00             	cmp    DWORD PTR [ebx],0x0
  444808:	7e d4                	jle    0x4447de
  44480a:	39 83 b0 fe ff ff    	cmp    DWORD PTR [ebx-0x150],eax
  444810:	74 cc                	je     0x4447de
  444812:	8b 54 24 20          	mov    edx,DWORD PTR [esp+0x20]
  444816:	83 3a 00             	cmp    DWORD PTR [edx],0x0
  444819:	7f c3                	jg     0x4447de
  44481b:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  44481f:	eb bd                	jmp    0x4447de
