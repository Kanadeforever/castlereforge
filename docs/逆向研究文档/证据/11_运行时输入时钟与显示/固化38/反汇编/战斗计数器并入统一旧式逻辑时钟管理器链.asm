; 固化38证据：Battle对象注册到同一管理器，最终vtable[0]=0x442920并递减DataCenter+0xF4。

/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0043d370 <.text+0x3c370>:
  43d370:	56                   	push   esi
  43d371:	8b f1                	mov    esi,ecx
  43d373:	e8 68 70 ff ff       	call   0x4343e0
  43d378:	33 c0                	xor    eax,eax
  43d37a:	c7 06 e4 0f 46 00    	mov    DWORD PTR [esi],0x460fe4
  43d380:	88 86 24 02 00 00    	mov    BYTE PTR [esi+0x224],al
  43d386:	89 86 28 02 00 00    	mov    DWORD PTR [esi+0x228],eax
  43d38c:	89 86 2c 02 00 00    	mov    DWORD PTR [esi+0x22c],eax
  43d392:	89 86 30 02 00 00    	mov    DWORD PTR [esi+0x230],eax
  43d398:	89 86 34 02 00 00    	mov    DWORD PTR [esi+0x234],eax
  43d39e:	89 86 4c 02 00 00    	mov    DWORD PTR [esi+0x24c],eax
  43d3a4:	89 86 38 02 00 00    	mov    DWORD PTR [esi+0x238],eax
  43d3aa:	88 86 40 02 00 00    	mov    BYTE PTR [esi+0x240],al
  43d3b0:	8b c6                	mov    eax,esi
  43d3b2:	5e                   	pop    esi
  43d3b3:	c3                   	ret
  43d3b4:	90                   	nop
  43d3b5:	90                   	nop
  43d3b6:	90                   	nop
  43d3b7:	90                   	nop
  43d3b8:	90                   	nop
  43d3b9:	90                   	nop
  43d3ba:	90                   	nop
  43d3bb:	90                   	nop
  43d3bc:	90                   	nop
  43d3bd:	90                   	nop
  43d3be:	90                   	nop
  43d3bf:	90                   	nop
  43d3c0:	6a ff                	push   0xffffffff
  43d3c2:	68 d8 f5 45 00       	push   0x45f5d8
  43d3c7:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  43d3cd:	50                   	push   eax
  43d3ce:	64 89      	mov    DWORD PTR fs:0x0,esp


/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

0043d530 <.text+0x3c530>:
  43d530:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  43d536:	6a ff                	push   0xffffffff
  43d538:	68 fb f5 45 00       	push   0x45f5fb
  43d53d:	50                   	push   eax
  43d53e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  43d545:	56                   	push   esi
  43d546:	8b f1                	mov    esi,ecx
  43d548:	57                   	push   edi
  43d549:	8a 86 24 02 00 00    	mov    al,BYTE PTR [esi+0x224]
  43d54f:	84 c0                	test   al,al
  43d551:	0f 85 6f 01 00 00    	jne    0x43d6c6
  43d557:	8b 44 24 1c          	mov    eax,DWORD PTR [esp+0x1c]
  43d55b:	8b 54 24 18          	mov    edx,DWORD PTR [esp+0x18]
  43d55f:	8b 4c 24 20          	mov    ecx,DWORD PTR [esp+0x20]
  43d563:	89 86 1c 02 00 00    	mov    DWORD PTR [esi+0x21c],eax
  43d569:	8d 42 01             	lea    eax,[edx+0x1]
  43d56c:	89 8e 20 02 00 00    	mov    DWORD PTR [esi+0x220],ecx
  43d572:	89 86 18 02 00 00    	mov    DWORD PTR [esi+0x218],eax
  43d578:	c1 e0 04             	shl    eax,0x4
  43d57b:	50                   	push   eax
  43d57c:	e8 de 40 01 00       	call   0x45165f
  43d581:	89 86 28 02 00 00    	mov    DWORD PTR [esi+0x228],eax
  43d587:	8b 86 18 02 00 00    	mov    eax,DWORD PTR [esi+0x218]
  43d58d:	c1 e0 02             	shl    eax,0x2
  43d590:	50                   	push   eax
  43d591:	e8 c9 40 01 00       	call   0x45165f
  43d596:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  43d59c:	89 86 2c 02 00 00    	mov    DWORD PTR [esi+0x22c],eax
  43d5a2:	51                   	push   ecx
  43d5a3:	e8 b7 40 01 00       	call   0x45165f
  43d5a8:	8b 96 18 02 00 00    	mov    edx,DWORD PTR [esi+0x218]
  43d5ae:	89 86 30 02 00 00    	mov    DWORD PTR [esi+0x230],eax
  43d5b4:	c1 e2 02             	shl    edx,0x2
  43d5b7:	52                   	push   edx
  43d5b8:	e8 a2 40 01 00       	call   0x45165f
  43d5bd:	89 86 34 02 00 00    	mov    DWORD PTR [esi+0x234],eax
  43d5c3:	8b 86 18 02 00 00    	mov    eax,DWORD PTR [esi+0x218]
  43d5c9:	c1 e0 02             	shl    eax,0x2
  43d5cc:	50                   	push   eax
  43d5cd:	e8 8d 40 01 00       	call   0x45165f
  43d5d2:	8b 8e 18 02 00 00    	mov    ecx,DWORD PTR [esi+0x218]
  43d5d8:	89 86 4c 02 00 00    	mov    DWORD PTR [esi+0x24c],eax
  43d5de:	51                   	push   ecx
  43d5df:	e8 7b 40 01 00       	call   0x45165f
  43d5e4:	8b 96 18 02 00 00    	mov    edx,DWORD PTR [esi+0x218]
  43d5ea:	89 86 44 02 00 00    	mov    DWORD PTR [esi+0x244],eax
  43d5f0:	c1 e2 02             	shl    edx,0x2
  43d5f3:	52                   	push   edx
  43d5f4:	e8 66 40 01 00       	call   0x45165f
  43d5f9:	89 86 48 02 00 00    	mov    DWORD PTR [esi+0x248],eax
  43d5ff:	8b 86 18 02 00 00    	mov    eax,DWORD PTR [esi+0x218]
  43d605:	83 c4 1c             	add    esp,0x1c
  43d608:	33 ff                	xor    edi,edi
  43d60a:	85 c0                	test   eax,eax
  43d60c:	0f 8e 9c 00 00 00    	jle    0x43d6ae
  43d612:	8b 86 44 02 00 00    	mov    eax,DWORD PTR [esi+0x244]
  43d618:	6a 24                	push   0x24
  43d61a:	c6 04 07 00          	mov    BYTE PTR [edi+eax*1],0x0
  43d61e:	8b 8e 48 02 00 00    	mov    ecx,DWORD PTR [esi+0x248]
  43d624:	c7 04 b9 00 00 00 00 	mov    DWORD PTR [ecx+edi*4],0x0
  43d62b:	8b 96 4c 02 00 00    	mov    edx,DWORD PTR [esi+0x24c]
  43d631:	c7 04 ba 00 00 00 00 	mov    DWORD PTR [edx+edi*4],0x0
  43d638:	e8 22 40 01 00       	call   0x45165f
  43d63d:	83 c4 04             	add    esp,0x4
  43d640:	89 44 24 1c          	mov    DWORD PTR [esp+0x1c],eax
  43d644:	85 c0                	test   eax,eax
  43d646:	c7 44 24 10 00 00 00 	mov    DWORD PTR [esp+0x10],0x0
  43d64d:	00 
  43d64e:	74 09                	je     0x43d659
  43d650:	8b c8                	mov    ecx,eax
  43d652:	e8 b9 fa ff ff       	call   0x43d110
  43d657:	eb 02                	jmp    0x43d65b
  43d659:	33 c0                	xor    eax,eax
  43d65b:	8b 8e 4c 02 00 00    	mov    ecx,DWORD PTR [esi+0x24c]
  43d661:	c7 44 24 10 ff ff ff 	mov    DWORD PTR [esp+0x10],0xffffffff
  43d668:	ff 
  43d669:	89 04 b9             	mov    DWORD PTR [ecx+edi*4],eax
  43d66c:	8b 96 4c 02 00 00    	mov    edx,DWORD PTR [esi+0x24c]
  43d672:	83 3c ba 00          	cmp    DWORD PTR [edx+edi*4],0x0
  43d676:	74 1d                	je     0x43d695
  43d678:	8b 86 30 02 00 00    	mov    eax,DWORD PTR [esi+0x230]
  43d67e:	c6 04 38 01          	mov    BYTE PTR [eax+edi*1],0x1
  43d682:	8b 8e 4c 02 00 00    	mov    ecx,DWORD PTR [esi+0x24c]
  43d688:	8b 86 20 02 00 00    	mov    eax,DWORD PTR [esi+0x220]
  43d68e:	8b 14 b9             	mov    edx,DWORD PTR [ecx+edi*4]
  43d691:	89 02                	mov    DWORD PTR [edx],eax
  43d693:	eb 0a                	jmp    0x43d69f
  43d695:	8b 8e 30 02 00 00    	mov    ecx,DWORD PTR [esi+0x230]
  43d69b:	c6 04 39 00          	mov    BYTE PTR [ecx+edi*1],0x0
  43d69f:	8b 86 18 02 00 00    	mov    eax,DWORD PTR [esi+0x218]
  43d6a5:	47                   	inc    edi
  43d6a6:	3b f8                	cmp    edi,eax
  43d6a8:	0f 8c 64 ff ff ff    	jl     0x43d612
  43d6ae:	8b 96 1c 02 00 00    	mov    edx,DWORD PTR [esi+0x21c]
  43d6b4:	6a 00                	push   0x0
  43d6b6:	52                   	push   edx
  43d6b7:	56                   	push   esi
  43d6b8:	8b ce                	mov    ecx,esi
  43d6ba:	e8 71 6d ff ff       	call   0x434430
  43d6bf:	c6 86 24 02 00 00 01 	mov    BYTE PTR [esi+0x224],0x1
  43d6c6:	8b 4c 24 08          	mov    ecx,DWORD PTR [esp+0x8]
  43d6ca:	5f                   	pop    edi
  43d6cb:	64 89 0d 00 00   	mov    DWORD PTR fs:0x0,ecx


/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00441cb0 <.text+0x40cb0>:
  441cb0:	6a ff                	push   0xffffffff
  441cb2:	68 08 f9 45 00       	push   0x45f908
  441cb7:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  441cbd:	50                   	push   eax
  441cbe:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  441cc5:	83 ec 08             	sub    esp,0x8
  441cc8:	53                   	push   ebx
  441cc9:	56                   	push   esi
  441cca:	8b f1                	mov    esi,ecx
  441ccc:	89 74 24 08          	mov    DWORD PTR [esp+0x8],esi
  441cd0:	e8 9b b6 ff ff       	call   0x43d370
  441cd5:	68 a0 f1 41 00       	push   0x41f1a0
  441cda:	68 70 f0 41 00       	push   0x41f070
  441cdf:	6a 10                	push   0x10
  441ce1:	8d 86 e4 03 00 00    	lea    eax,[esi+0x3e4]
  441ce7:	33 db                	xor    ebx,ebx
  441ce9:	68 e4 0d 00 00       	push   0xde4
  441cee:	50                   	push   eax
  441cef:	89 5c 24 2c          	mov    DWORD PTR [esp+0x2c],ebx
  441cf3:	e8 d7 04 01 00       	call   0x4521cf
  441cf8:	6a 32                	push   0x32
  441cfa:	8d 8e b0 e2 00 00    	lea    ecx,[esi+0xe2b0]
  441d00:	c6 44 24 1c 01       	mov    BYTE PTR [esp+0x1c],0x1
  441d05:	e8 b6 23 fe ff       	call   0x4240c0
  441d0a:	6a 32                	push   0x32
  441d0c:	8d 8e d4 e2 00 00    	lea    ecx,[esi+0xe2d4]
  441d12:	c6 44 24 1c 02       	mov    BYTE PTR [esp+0x1c],0x2
  441d17:	e8 a4 23 fe ff       	call   0x4240c0
  441d1c:	6a 32                	push   0x32
  441d1e:	8d 8e f8 e2 00 00    	lea    ecx,[esi+0xe2f8]
  441d24:	c6 44 24 1c 03       	mov    BYTE PTR [esp+0x1c],0x3
  441d29:	e8 92 23 fe ff       	call   0x4240c0
  441d2e:	68 10 4a 0f 00       	push   0xf4a10
  441d33:	68 10 4a 0f 00       	push   0xf4a10
  441d38:	6a 10                	push   0x10
  441d3a:	8b ce                	mov    ecx,esi
  441d3c:	c6 44 24 24 04       	mov    BYTE PTR [esp+0x24],0x4
  441d41:	c7 06 d4 10 46 00    	mov    DWORD PTR [esi],0x4610d4
  441d47:	e8 e4 b7 ff ff       	call   0x43d530
  441d4c:	c7 86 50 02 00 00 1c 	mov    DWORD PTR [esi+0x250],0x1c
  441d53:	00 00 00 
  441d56:	33 c0                	xor    eax,eax
  441d58:	8d 8e 34 03 00 00    	lea    ecx,[esi+0x334]
  441d5e:	c7 01 ff ff ff ff    	mov    DWORD PTR [ecx],0xffffffff
  441d64:	8b 96 50 02 00 00    	mov    edx,DWORD PTR [esi+0x250]
  441d6a:	40                   	inc    eax
  441d6b:	83 c1 04             	add    ecx,0x4
  441d6e:	3b c2                	cmp    eax,edx


/mnt/data/work37/input/RPG.exe:     file format pei-i386


Disassembly of section .text:

00442920 <.text+0x41920>:
  442920:	a0 0c 24 8e 00       	mov    al,ds:0x8e240c
  442925:	57                   	push   edi
  442926:	84 c0                	test   al,al
  442928:	8b f9                	mov    edi,ecx
  44292a:	0f 84 af 00 00 00    	je     0x4429df
  442930:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  442936:	53                   	push   ebx
  442937:	bb 01 00 00 00       	mov    ebx,0x1
  44293c:	56                   	push   esi
  44293d:	8b 81 f4 00 00 00    	mov    eax,DWORD PTR [ecx+0xf4]
  442943:	3b c3                	cmp    eax,ebx
  442945:	7e 11                	jle    0x442958
  442947:	8a 15 dc 96 46 00    	mov    dl,BYTE PTR ds:0x4696dc
  44294d:	84 d2                	test   dl,dl
  44294f:	74 07                	je     0x442958
  442951:	48                   	dec    eax
  442952:	89 81 f4 00 00 00    	mov    DWORD PTR [ecx+0xf4],eax
  442958:	a1 74 fd 89 00       	mov    eax,ds:0x89fd74
  44295d:	8a 88 fc 05 00 00    	mov    cl,BYTE PTR [eax+0x5fc]
  442963:	84 c9                	test   cl,cl
  442965:	74 3c                	je     0x4429a3
  442967:	c6 05 dc 96 46 00 00 	mov    BYTE PTR ds:0x4696dc,0x0
  44296e:	8a 87        	mov    al,BYTE PTR [edi+0xe330]
