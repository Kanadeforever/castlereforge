
/mnt/data/work50/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0041d920 <.text+0x1c920>:
  41d920:	8a 81 79 05 00 00    	mov    al,BYTE PTR [ecx+0x579]
  41d926:	84 c0                	test   al,al
  41d928:	74 05                	je     0x41d92f
  41d92a:	e9 01 00 00 00       	jmp    0x41d930
  41d92f:	c3                   	ret
  41d930:	a0 7c fd 89 00       	mov    al,ds:0x89fd7c
  41d935:	53                   	push   ebx
  41d936:	8b 1d a8 01 46 00    	mov    ebx,DWORD PTR ds:0x4601a8
  41d93c:	55                   	push   ebp
  41d93d:	56                   	push   esi
  41d93e:	57                   	push   edi
  41d93f:	84 c0                	test   al,al
  41d941:	8b e9                	mov    ebp,ecx
  41d943:	75 21                	jne    0x41d966
  41d945:	6a 74                	push   0x74
  41d947:	ff d3                	call   ebx
  41d949:	c1 f8 0f             	sar    eax,0xf
  41d94c:	24 01                	and    al,0x1
  41d94e:	3c 01                	cmp    al,0x1
  41d950:	75 07                	jne    0x41d959
  41d952:	a2 7c fd 89 00       	mov    ds:0x89fd7c,al
  41d957:	eb 0d                	jmp    0x41d966
  41d959:	a0 7c fd 89 00       	mov    al,ds:0x89fd7c
  41d95e:	84 c0                	test   al,al
  41d960:	0f 84 15 02 00 00    	je     0x41db7b
  41d966:	8a 85 84 05 00 00    	mov    al,BYTE PTR [ebp+0x584]
  41d96c:	84 c0                	test   al,al
  41d96e:	75 4b                	jne    0x41d9bb
  41d970:	33 ff                	xor    edi,edi
  41d972:	be f8 98 46 00       	mov    esi,0x4698f8
  41d977:	8a 85 84 05 00 00    	mov    al,BYTE PTR [ebp+0x584]
  41d97d:	84 c0                	test   al,al
  41d97f:	0f 85 f6 01 00 00    	jne    0x41db7b
  41d985:	0f be 06             	movsx  eax,BYTE PTR [esi]
  41d988:	50                   	push   eax
  41d989:	ff d3                	call   ebx
  41d98b:	c1 f8 0f             	sar    eax,0xf
  41d98e:	24 01                	and    al,0x1
  41d990:	3c 01                	cmp    al,0x1
  41d992:	75 16                	jne    0x41d9aa
  41d994:	89 bd 7c 05 00 00    	mov    DWORD PTR [ebp+0x57c],edi
  41d99a:	c7 85 80 05 00 00 01 	mov    DWORD PTR [ebp+0x580],0x1
  41d9a1:	00 00 00 
  41d9a4:	88 85 84 05 00 00    	mov    BYTE PTR [ebp+0x584],al
  41d9aa:	83 c6 1e             	add    esi,0x1e
  41d9ad:	47                   	inc    edi
  41d9ae:	81 fe ac 99 46 00    	cmp    esi,0x4699ac
  41d9b4:	7c c1                	jl     0x41d977
  41d9b6:	5f                   	pop    edi
  41d9b7:	5e                   	pop    esi
  41d9b8:	5d                   	pop    ebp
  41d9b9:	5b                   	pop    ebx
  41d9ba:	c3                   	ret
  41d9bb:	8b 85 7c 05 00 00    	mov    eax,DWORD PTR [ebp+0x57c]
  41d9c1:	8b 95 80 05 00 00    	mov    edx,DWORD PTR [ebp+0x580]
  41d9c7:	8d 04 40             	lea    eax,[eax+eax*2]
  41d9ca:	8d 0c 80             	lea    ecx,[eax+eax*4]
  41d9cd:	0f be 84 4a f8 98 46 	movsx  eax,BYTE PTR [edx+ecx*2+0x4698f8]
  41d9d4:	00 
  41d9d5:	50                   	push   eax
  41d9d6:	ff d3                	call   ebx
  41d9d8:	c1 f8 0f             	sar    eax,0xf
  41d9db:	24 01                	and    al,0x1
  41d9dd:	3c 01                	cmp    al,0x1
  41d9df:	75 37                	jne    0x41da18
  41d9e1:	8b 85 80 05 00 00    	mov    eax,DWORD PTR [ebp+0x580]
  41d9e7:	40                   	inc    eax
  41d9e8:	89 85 80 05 00 00    	mov    DWORD PTR [ebp+0x580],eax
  41d9ee:	8b d0                	mov    edx,eax
  41d9f0:	8b 85 7c 05 00 00    	mov    eax,DWORD PTR [ebp+0x57c]
  41d9f6:	8d 04 40             	lea    eax,[eax+eax*2]
  41d9f9:	8d 0c 80             	lea    ecx,[eax+eax*4]
  41d9fc:	33 c0                	xor    eax,eax
  41d9fe:	8d 3c 4d f8 98 46 00 	lea    edi,[ecx*2+0x4698f8]
  41da05:	83 c9 ff             	or     ecx,0xffffffff
  41da08:	f2 ae                	repnz scas al,BYTE PTR es:[edi]
  41da0a:	f7 d1                	not    ecx
  41da0c:	49                   	dec    ecx
  41da0d:	3b d1                	cmp    edx,ecx
  41da0f:	75 07                	jne    0x41da18
  41da11:	c6 85 85 05 00 00 01 	mov    BYTE PTR [ebp+0x585],0x1
  41da18:	68 a1 00 00 00       	push   0xa1
  41da1d:	ff d3                	call   ebx
  41da1f:	c1 f8 0f             	sar    eax,0xf
  41da22:	24 01                	and    al,0x1
  41da24:	3c 01                	cmp    al,0x1
  41da26:	0f 85 4f 01 00 00    	jne    0x41db7b
  41da2c:	8a 8d 85 05 00 00    	mov    cl,BYTE PTR [ebp+0x585]
  41da32:	a1 d8 fc 89 00       	mov    eax,ds:0x89fcd8
  41da37:	84 c9                	test   cl,cl
  41da39:	0f 84 2e 01 00 00    	je     0x41db6d
  41da3f:	8b 8d 7c 05 00 00    	mov    ecx,DWORD PTR [ebp+0x57c]
  41da45:	83 f9 05             	cmp    ecx,0x5
  41da48:	0f 87 1f 01 00 00    	ja     0x41db6d
  41da4e:	ff 24 8d 80 db 41 00 	jmp    DWORD PTR [ecx*4+0x41db80]
  41da55:	85 c0                	test   eax,eax
  41da57:	0f 8e 10 01 00 00    	jle    0x41db6d
  41da5d:	b9 04 fd 89 00       	mov    ecx,0x89fd04
  41da62:	ba 9f 86 01 00       	mov    edx,0x1869f
  41da67:	8b 31                	mov    esi,DWORD PTR [ecx]
  41da69:	83 c1 04             	add    ecx,0x4
  41da6c:	48                   	dec    eax
  41da6d:	89 96 20 02 00 00    	mov    DWORD PTR [esi+0x220],edx
  41da73:	75 f2                	jne    0x41da67
  41da75:	5f                   	pop    edi
  41da76:	c6 85 84 05 00 00 00 	mov    BYTE PTR [ebp+0x584],0x0
  41da7d:	5e                   	pop    esi
  41da7e:	5d                   	pop    ebp
  41da7f:	c6 05 7c fd 89 00 00 	mov    BYTE PTR ds:0x89fd7c,0x0
  41da86:	5b                   	pop    ebx
  41da87:	c3                   	ret
  41da88:	85 c0                	test   eax,eax
  41da8a:	0f 8e dd 00 00 00    	jle    0x41db6d
  41da90:	b9 04 fd 89 00       	mov    ecx,0x89fd04
  41da95:	8b d0                	mov    edx,eax
  41da97:	8b 01                	mov    eax,DWORD PTR [ecx]
  41da99:	83 c1 04             	add    ecx,0x4
  41da9c:	4a                   	dec    edx
  41da9d:	8b 70 2c             	mov    esi,DWORD PTR [eax+0x2c]
  41daa0:	89 70 30             	mov    DWORD PTR [eax+0x30],esi
  41daa3:	8b 70 34             	mov    esi,DWORD PTR [eax+0x34]
  41daa6:	89 70 38             	mov    DWORD PTR [eax+0x38],esi
  41daa9:	75 ec                	jne    0x41da97
  41daab:	5f                   	pop    edi
  41daac:	c6 85 84 05 00 00 00 	mov    BYTE PTR [ebp+0x584],0x0
  41dab3:	5e                   	pop    esi
  41dab4:	5d                   	pop    ebp
  41dab5:	c6 05 7c fd 89 00 00 	mov    BYTE PTR ds:0x89fd7c,0x0
  41dabc:	5b                   	pop    ebx
  41dabd:	c3                   	ret
  41dabe:	85 c0                	test   eax,eax
  41dac0:	0f 8e a7 00 00 00    	jle    0x41db6d
  41dac6:	b9 04 fd 89 00       	mov    ecx,0x89fd04
  41dacb:	ba 9f 86 01 00       	mov    edx,0x1869f
  41dad0:	8b 31                	mov    esi,DWORD PTR [ecx]
  41dad2:	83 c1 04             	add    ecx,0x4
  41dad5:	48                   	dec    eax
  41dad6:	89 96 44 02 00 00    	mov    DWORD PTR [esi+0x244],edx
  41dadc:	75 f2                	jne    0x41dad0
  41dade:	5f                   	pop    edi
  41dadf:	c6 85 84 05 00 00 00 	mov    BYTE PTR [ebp+0x584],0x0
  41dae6:	5e                   	pop    esi
  41dae7:	5d                   	pop    ebp
  41dae8:	c6 05 7c fd 89 00 00 	mov    BYTE PTR ds:0x89fd7c,0x0
  41daef:	5b                   	pop    ebx
  41daf0:	c3                   	ret
  41daf1:	85 c0                	test   eax,eax
  41daf3:	7e 78                	jle    0x41db6d
  41daf5:	ba 04 fd 89 00       	mov    edx,0x89fd04
  41dafa:	be a0 86 01 00       	mov    esi,0x186a0
  41daff:	8b 0a                	mov    ecx,DWORD PTR [edx]
  41db01:	83 c2 04             	add    edx,0x4
  41db04:	8b 59 24             	mov    ebx,DWORD PTR [ecx+0x24]
  41db07:	03 de                	add    ebx,esi
  41db09:	48                   	dec    eax
  41db0a:	89 59 24             	mov    DWORD PTR [ecx+0x24],ebx
  41db0d:	75 f0                	jne    0x41daff
  41db0f:	5f                   	pop    edi
  41db10:	c6 85 84 05 00 00 00 	mov    BYTE PTR [ebp+0x584],0x0
  41db17:	5e                   	pop    esi
  41db18:	5d                   	pop    ebp
  41db19:	c6 05 7c fd 89 00 00 	mov    BYTE PTR ds:0x89fd7c,0x0
  41db20:	5b                   	pop    ebx
  41db21:	c3                   	ret
  41db22:	85 c0                	test   eax,eax
  41db24:	7e 47                	jle    0x41db6d
  41db26:	b9 04 fd 89 00       	mov    ecx,0x89fd04
  41db2b:	ba 9f 86 01 00       	mov    edx,0x1869f
  41db30:	8b 31                	mov    esi,DWORD PTR [ecx]
  41db32:	83 c1 04             	add    ecx,0x4
  41db35:	48                   	dec    eax
  41db36:	89 96 3c 02 00 00    	mov    DWORD PTR [esi+0x23c],edx
  41db3c:	75 f2                	jne    0x41db30
  41db3e:	5f                   	pop    edi
  41db3f:	c6 85 84 05 00 00 00 	mov    BYTE PTR [ebp+0x584],0x0
  41db46:	5e                   	pop    esi
  41db47:	5d                   	pop    ebp
  41db48:	c6 05 7c fd 89 00 00 	mov    BYTE PTR ds:0x89fd7c,0x0
  41db4f:	5b                   	pop    ebx
  41db50:	c3                   	ret
  41db51:	85 c0                	test   eax,eax
  41db53:	7e 18                	jle    0x41db6d
  41db55:	b9 04 fd 89 00       	mov    ecx,0x89fd04
  41db5a:	ba 9f 86 01 00       	mov    edx,0x1869f
  41db5f:	8b 31                	mov    esi,DWORD PTR [ecx]
  41db61:	83 c1 04             	add    ecx,0x4
  41db64:	48                   	dec    eax
  41db65:	89 96 40 02 00 00    	mov    DWORD PTR [esi+0x240],edx
  41db6b:	75 f2                	jne    0x41db5f
  41db6d:	c6 85 84 05 00 00 00 	mov    BYTE PTR [ebp+0x584],0x0
  41db74:	c6 05 7c fd 89 00 00 	mov    BYTE PTR ds:0x89fd7c,0x0
  41db7b:	5f                   	pop    edi
  41db7c:	5e                   	pop    esi
  41db7d:	5d                   	pop    ebp
  41db7e:	5b                   	pop    ebx
  41db7f:	c3                   	ret
