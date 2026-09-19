; 固化40直接证据：FightRole+0xA34起39个DWORD runtime槽的固定减10循环。
; 关键边界：正值每次合格调用减10，<=0夹零；循环索引0..38，共39槽。


/mnt/data/work40/origexe/RPG.exe:     file format pei-i386


Disassembly of section .text:

00420650 <.text+0x1f650>:
  420650:	d0 6a 03             	shr    BYTE PTR [edx+0x3],1
  420653:	52                   	push   edx
  420654:	8b cb                	mov    ecx,ebx
  420656:	e8 35 3a 00 00       	call   0x424090
  42065b:	8b cb                	mov    ecx,ebx
  42065d:	e8 2e 38 00 00       	call   0x423e90
  420662:	8d be 34 0a 00 00    	lea    edi,[esi+0xa34]
  420668:	33 d2                	xor    edx,edx
  42066a:	8b c7                	mov    eax,edi
  42066c:	5b                   	pop    ebx
  42066d:	8b 08                	mov    ecx,DWORD PTR [eax]
  42066f:	85 c9                	test   ecx,ecx
  420671:	7e 1d                	jle    0x420690
  420673:	83 fa 10             	cmp    edx,0x10
  420676:	75 09                	jne    0x420681
  420678:	85 c9                	test   ecx,ecx
  42067a:	7e 14                	jle    0x420690
  42067c:	83 fa 13             	cmp    edx,0x13
  42067f:	74 0f                	je     0x420690
  420681:	83 c1 f6             	add    ecx,0xfffffff6
  420684:	85 c9                	test   ecx,ecx
  420686:	89 08                	mov    DWORD PTR [eax],ecx
  420688:	7f 06                	jg     0x420690
  42068a:	c7 00 00 00 00 00    	mov    DWORD PTR [eax],0x0
  420690:	42                   	inc    edx
  420691:	83 c0 04             	add    eax,0x4
  420694:	83 fa 27             	cmp    edx,0x27
  420697:	7c d4                	jl     0x42066d
  420699:	8b 3f                	mov    edi,DWORD PTR [edi]
  42069b:	b8 b5 81 4e 1b       	mov    eax,0x1b4e81b5
