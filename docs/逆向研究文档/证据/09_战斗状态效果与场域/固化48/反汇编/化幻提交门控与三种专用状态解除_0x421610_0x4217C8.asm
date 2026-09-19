; 固化48：raw14阻断normal pending提交，以及raw1/raw2/raw11三个专用清槽分支。

/mnt/data/work48/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00421610 <.text+0x20610>:
  421610:	00 00                	add    BYTE PTR [eax],al
  421612:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  421619:	8b 14 01             	mov    edx,DWORD PTR [ecx+eax*1]
  42161c:	85 d2                	test   edx,edx
  42161e:	7e 1f                	jle    0x42163f
  421620:	8b b9 6c 0a 00 00    	mov    edi,DWORD PTR [ecx+0xa6c]
  421626:	85 ff                	test   edi,edi
  421628:	7f 15                	jg     0x42163f
  42162a:	89 94 01 68 09 00 00 	mov    DWORD PTR [ecx+eax*1+0x968],edx
  421631:	8b 14 b5 94 fd 89 00 	mov    edx,DWORD PTR [esi*4+0x89fd94]
  421638:	c7 04 02 00 00 00 00 	mov    DWORD PTR [edx+eax*1],0x0
  42163f:	83 c0 04             	add    eax,0x4
  421642:	3d 68 01 00 00       	cmp    eax,0x168
  421647:	7c c9                	jl     0x421612
  421649:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]


/mnt/data/work48/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00421690 <.text+0x20690>:
  421690:	8b 81 d0 0d 00 00    	mov    eax,DWORD PTR [ecx+0xdd0]
  421696:	33 d2                	xor    edx,edx
  421698:	53                   	push   ebx
  421699:	56                   	push   esi
  42169a:	3b c2                	cmp    eax,edx
  42169c:	57                   	push   edi
  42169d:	0f 8e c1 02 00 00    	jle    0x421964
  4216a3:	8b b1 f4 0c 00 00    	mov    esi,DWORD PTR [ecx+0xcf4]
  4216a9:	3b f2                	cmp    esi,edx
  4216ab:	0f 84 b3 02 00 00    	je     0x421964
  4216b1:	8b 44 24 10          	mov    eax,DWORD PTR [esp+0x10]
  4216b5:	2b c2                	sub    eax,edx
  4216b7:	74 0e                	je     0x4216c7
  4216b9:	48                   	dec    eax
  4216ba:	0f 84 a4 02 00 00    	je     0x421964
  4216c0:	48                   	dec    eax
  4216c1:	0f 85 9d 02 00 00    	jne    0x421964
  4216c7:	8b 7e 20             	mov    edi,DWORD PTR [esi+0x20]
  4216ca:	8d b7 f8 fe ff ff    	lea    esi,[edi-0x108]
  4216d0:	83 fe 64             	cmp    esi,0x64
  4216d3:	0f 87 8b 02 00 00    	ja     0x421964
  4216d9:	33 c0                	xor    eax,eax
  4216db:	8a 86 90 19 42 00    	mov    al,BYTE PTR [esi+0x421990]
  4216e1:	ff 24 85 6c 19 42 00 	jmp    DWORD PTR [eax*4+0x42196c]
  4216e8:	8b b1 c8 00 00 00    	mov    esi,DWORD PTR [ecx+0xc8]
  4216ee:	33 c0                	xor    eax,eax
  4216f0:	3b f2                	cmp    esi,edx
  4216f2:	0f 8e 6c 02 00 00    	jle    0x421964
  4216f8:	8d b1 88 00 00 00    	lea    esi,[ecx+0x88]
  4216fe:	8b 3e                	mov    edi,DWORD PTR [esi]
  421700:	40                   	inc    eax
  421701:	83 c6 04             	add    esi,0x4
  421704:	8b 3c bd 94 fd 89 00 	mov    edi,DWORD PTR [edi*4+0x89fd94]
  42170b:	89 97 34 0a 00 00    	mov    DWORD PTR [edi+0xa34],edx
  421711:	8b b9 c8 00 00 00    	mov    edi,DWORD PTR [ecx+0xc8]
  421717:	3b c7                	cmp    eax,edi
  421719:	7c e3                	jl     0x4216fe
  42171b:	5f                   	pop    edi
  42171c:	5e                   	pop    esi
  42171d:	5b                   	pop    ebx
  42171e:	c2 04 00             	ret    0x4
  421721:	8b b1 c8 00 00 00    	mov    esi,DWORD PTR [ecx+0xc8]
  421727:	33 c0                	xor    eax,eax
  421729:	3b f2                	cmp    esi,edx
  42172b:	0f 8e 33 02 00 00    	jle    0x421964
  421731:	8d b1 88 00 00 00    	lea    esi,[ecx+0x88]
  421737:	8b 3e                	mov    edi,DWORD PTR [esi]
  421739:	40                   	inc    eax
  42173a:	83 c6 04             	add    esi,0x4
  42173d:	8b 3c bd 94 fd 89 00 	mov    edi,DWORD PTR [edi*4+0x89fd94]
  421744:	89 97 38 0a 00 00    	mov    DWORD PTR [edi+0xa38],edx
  42174a:	8b b9 c8 00 00 00    	mov    edi,DWORD PTR [ecx+0xc8]
  421750:	3b c7                	cmp    eax,edi
  421752:	7c e3                	jl     0x421737
  421754:	5f                   	pop    edi
  421755:	5e                   	pop    esi
  421756:	5b                   	pop    ebx
  421757:	c2 04 00             	ret    0x4
  42175a:	8b b1 c8 00 00 00    	mov    esi,DWORD PTR [ecx+0xc8]
  421760:	33 c0                	xor    eax,eax
  421762:	3b f2                	cmp    esi,edx
  421764:	0f 8e fa 01 00 00    	jle    0x421964
  42176a:	8d b1 88 00 00 00    	lea    esi,[ecx+0x88]
  421770:	8b 3e                	mov    edi,DWORD PTR [esi]
  421772:	40                   	inc    eax
  421773:	83 c6 04             	add    esi,0x4
  421776:	8b 3c bd 94 fd 89 00 	mov    edi,DWORD PTR [edi*4+0x89fd94]
  42177d:	89 97 3c 0a 00 00    	mov    DWORD PTR [edi+0xa3c],edx
  421783:	8b b9 c8 00 00 00    	mov    edi,DWORD PTR [ecx+0xc8]
  421789:	3b c7                	cmp    eax,edi
  42178b:	7c e3                	jl     0x421770
  42178d:	5f                   	pop    edi
  42178e:	5e                   	pop    esi
  42178f:	5b                   	pop    ebx
  421790:	c2 04 00             	ret    0x4
  421793:	8b b1 c8 00 00 00    	mov    esi,DWORD PTR [ecx+0xc8]
  421799:	33 c0                	xor    eax,eax
  42179b:	3b f2                	cmp    esi,edx
  42179d:	0f 8e c1 01 00 00    	jle    0x421964
  4217a3:	8d b1 88 00 00 00    	lea    esi,[ecx+0x88]
  4217a9:	8b 3e                	mov    edi,DWORD PTR [esi]
  4217ab:	40                   	inc    eax
  4217ac:	83 c6 04             	add    esi,0x4
  4217af:	8b 3c bd 94 fd 89 00 	mov    edi,DWORD PTR [edi*4+0x89fd94]
  4217b6:	89 97 60 0a 00 00    	mov    DWORD PTR [edi+0xa60],edx
  4217bc:	8b b9 c8 00 00 00    	mov    edi,DWORD PTR [ecx+0xc8]
  4217c2:	3b c7                	cmp    eax,edi
  4217c4:	7c e3                	jl     0x4217a9
  4217c6:	5f                   	pop    edi
  4217c7:	5e                   	pop    esi
