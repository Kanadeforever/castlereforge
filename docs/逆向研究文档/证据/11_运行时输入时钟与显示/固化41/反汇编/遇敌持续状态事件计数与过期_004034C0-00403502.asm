; 固化41：world encounter 1500事件计数器与过期逻辑
  4034bb:	90                   	nop
  4034bc:	90                   	nop
  4034bd:	90                   	nop
  4034be:	90                   	nop
  4034bf:	90                   	nop
  4034c0:	a1 14 f6 46 00       	mov    eax,ds:0x46f614
  4034c5:	33 d2                	xor    edx,edx
  4034c7:	3b c2                	cmp    eax,edx
  4034c9:	74 12                	je     0x4034dd
  4034cb:	8b 44 24 04          	mov    eax,DWORD PTR [esp+0x4]
  4034cf:	8b 0d 18 f6 46 00    	mov    ecx,DWORD PTR ds:0x46f618
  4034d5:	03 c8                	add    ecx,eax
  4034d7:	89 0d 18 f6 46 00    	mov    DWORD PTR ds:0x46f618,ecx
  4034dd:	8b 0d 1c f6 46 00    	mov    ecx,DWORD PTR ds:0x46f61c
  4034e3:	3b ca                	cmp    ecx,edx
  4034e5:	74 1b                	je     0x403502
  4034e7:	a1 20 f6 46 00       	mov    eax,ds:0x46f620
  4034ec:	40                   	inc    eax
  4034ed:	3b c1                	cmp    eax,ecx
  4034ef:	a3 20 f6 46 00       	mov    ds:0x46f620,eax
  4034f4:	7e 0c                	jle    0x403502
  4034f6:	89 15 1c f6 46 00    	mov    DWORD PTR ds:0x46f61c,edx
  4034fc:	89 15 20 f6 46 00    	mov    DWORD PTR ds:0x46f620,edx
  403502:	c3                   	ret
  403503:	90                   	nop
  403504:	90                   	nop
  403505:	90                   	nop
  403506:	90                   	nop
