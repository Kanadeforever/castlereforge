; 固化48：raw13布尔打包以及raw14/raw9/raw16结果处理。

/mnt/data/work48/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00420810 <.text+0x1f810>:
  420810:	7c b4                	jl     0x4207c6
  420812:	c6 05 fc fd 89 00 01 	mov    BYTE PTR ds:0x89fdfc,0x1
  420819:	8b 86 58 0a 00 00    	mov    eax,DWORD PTR [esi+0xa58]
  42081f:	8b ae 6c 0a 00 00    	mov    ebp,DWORD PTR [esi+0xa6c]
  420825:	8b be 34 0a 00 00    	mov    edi,DWORD PTR [esi+0xa34]
  42082b:	33 db                	xor    ebx,ebx
  42082d:	3b c3                	cmp    eax,ebx
  42082f:	0f 9f c0             	setg   al
  420832:	3b eb                	cmp    ebp,ebx
  420834:	8b ae 38 0a 00 00    	mov    ebp,DWORD PTR [esi+0xa38]
  42083a:	0f 9f c1             	setg   cl
  42083d:	3b fb                	cmp    edi,ebx
  42083f:	88 44 24 1c          	mov    BYTE PTR [esp+0x1c],al
  420843:	0f 9f c2             	setg   dl
  420846:	88 54 24 1e          	mov    BYTE PTR [esp+0x1e],dl
  42084a:	8b 96 88 0a 00 00    	mov    edx,DWORD PTR [esi+0xa88]
  420850:	3b d3                	cmp    edx,ebx
  420852:	88 4c 24 1d          	mov    BYTE PTR [esp+0x1d],cl
  420856:	8b 8e 70 0a 00 00    	mov    ecx,DWORD PTR [esi+0xa70]
  42085c:	8b be 3c 0a 00 00    	mov    edi,DWORD PTR [esi+0xa3c]
  420862:	0f 9f c0             	setg   al
  420865:	88 44 24 1f          	mov    BYTE PTR [esp+0x1f],al
  420869:	8b 86 5c 0a 00 00    	mov    eax,DWORD PTR [esi+0xa5c]
  42086f:	3b cb                	cmp    ecx,ebx
  420871:	0f 9f c1             	setg   cl
  420874:	3b c3                	cmp    eax,ebx
  420876:	88 4c 24 20          	mov    BYTE PTR [esp+0x20],cl
  42087a:	0f 9f c2             	setg   dl
  42087d:	3b eb                	cmp    ebp,ebx
  42087f:	88 54 24 21          	mov    BYTE PTR [esp+0x21],dl
  420883:	8b 96 60 0a 00 00    	mov    edx,DWORD PTR [esi+0xa60]
  420889:	0f 9f c0             	setg   al
  42088c:	3b fb                	cmp    edi,ebx
  42088e:	88 44 24 22          	mov    BYTE PTR [esp+0x22],al
  420892:	0f 9f c1             	setg   cl
  420895:	88 4c 24 23          	mov    BYTE PTR [esp+0x23],cl
  420899:	8b 8e 68 0a 00 00    	mov    ecx,DWORD PTR [esi+0xa68]
  42089f:	3b d3                	cmp    edx,ebx
  4208a1:	0f 9f c2             	setg   dl
  4208a4:	3b cb                	cmp    ecx,ebx
  4208a6:	88 54 24 24          	mov    BYTE PTR [esp+0x24],dl
  4208aa:	0f 9f c0             	setg   al
  4208ad:	88 44 24 25          	mov    BYTE PTR [esp+0x25],al
  4208b1:	8b 86 80 0a 00 00    	mov    eax,DWORD PTR [esi+0xa80]
  4208b7:	3b c3                	cmp    eax,ebx
  4208b9:	0f 9f c1             	setg   cl
  4208bc:	88 4c 24 26          	mov    BYTE PTR [esp+0x26],cl


/mnt/data/work48/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00421360 <.text+0x20360>:
  421360:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  421367:	39 a8 6c 0a 00 00    	cmp    DWORD PTR [eax+0xa6c],ebp
  42136d:	7e 05                	jle    0x421374
  42136f:	89 2b                	mov    DWORD PTR [ebx],ebp
  421371:	89 6b 04             	mov    DWORD PTR [ebx+0x4],ebp
  421374:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  42137b:	39 a8 80 0a 00 00    	cmp    DWORD PTR [eax+0xa80],ebp
  421381:	7e 14                	jle    0x421397
  421383:	39 2b                	cmp    DWORD PTR [ebx],ebp
  421385:	7f 05                	jg     0x42138c
  421387:	39 6b 04             	cmp    DWORD PTR [ebx+0x4],ebp
  42138a:	7e 0b                	jle    0x421397
  42138c:	89 a8 80 0a 00 00    	mov    DWORD PTR [eax+0xa80],ebp
  421392:	89 2b                	mov    DWORD PTR [ebx],ebp
  421394:	89 6b 04             	mov    DWORD PTR [ebx+0x4],ebp
  421397:	8b 04 b5 94 fd 89 00 	mov    eax,DWORD PTR [esi*4+0x89fd94]
  42139e:	39 a8 58 0a 00 00    	cmp    DWORD PTR [eax+0xa58],ebp
  4213a4:	7e 0f                	jle    0x4213b5
  4213a6:	39 2b                	cmp    DWORD PTR [ebx],ebp
  4213a8:	7d 05                	jge    0x4213af
  4213aa:	39 6b 04             	cmp    DWORD PTR [ebx+0x4],ebp
  4213ad:	7c 06                	jl     0x4213b5
  4213af:	89 a8 58 0a 00 00    	mov    DWORD PTR [eax+0xa58],ebp
  4213b5:	8b 0c b5 94 fd 89 00 	mov    ecx,DWORD PTR [esi*4+0x89fd94]
  4213bc:	39 a9 74 0a 00 00    	cmp    DWORD PTR [ecx+0xa74],ebp
  4213c2:	7e 1d                	jle    0x4213e1
  4213c4:	8b 03                	mov    eax,DWORD PTR [ebx]
  4213c6:	3b c5                	cmp    eax,ebp
  4213c8:	7e 04                	jle    0x4213ce
  4213ca:	89 2b                	mov    DWORD PTR [ebx],ebp
  4213cc:	eb 13                	jmp    0x4213e1
  4213ce:	7d 11                	jge    0x4213e1
  4213d0:	f7 d8                	neg    eax
  4213d2:	89 03                	mov    DWORD PTR [ebx],eax
  4213d4:	8b 14 b5 94 fd 89 00 	mov    edx,DWORD PTR [esi*4+0x89fd94]
  4213db:	89 aa 74 0a 00 00    	mov    DWORD PTR [edx+0xa74],ebp
  4213e1:	8a 43 3c             	mov    al,BYTE PTR [ebx+0x3c]
  4213e4:	84                 	test   al,al
