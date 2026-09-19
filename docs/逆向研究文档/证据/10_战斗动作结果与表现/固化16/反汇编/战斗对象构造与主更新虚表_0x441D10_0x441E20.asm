
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00441d10 <.text+0x40d10>:
  441d10:	00 00                	add    BYTE PTR [eax],al
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
  441d70:	7c ec                	jl     0x441d5e
  441d72:	68 94 05 00 00       	push   0x594
  441d77:	e8 e3 f8 00 00       	call   0x45165f
  441d7c:	83 c4 04             	add    esp,0x4
  441d7f:	89 44 24 0c          	mov    DWORD PTR [esp+0xc],eax
  441d83:	3b c3                	cmp    eax,ebx
  441d85:	c6 44 24 18 05       	mov    BYTE PTR [esp+0x18],0x5
  441d8a:	74 09                	je     0x441d95
  441d8c:	8b c8                	mov    ecx,eax
  441d8e:	e8 7d 53 00 00       	call   0x447110
  441d93:	eb 02                	jmp    0x441d97
  441d95:	33 c0                	xor    eax,eax
  441d97:	a3 18 24 8e 00       	mov    ds:0x8e2418,eax
  441d9c:	c7 86 24 e2 00 00 ff 	mov    DWORD PTR [esi+0xe224],0xffffffff
  441da3:	ff ff ff 
  441da6:	c7 86 ac e2 00 00 ff 	mov    DWORD PTR [esi+0xe2ac],0xffffffff
  441dad:	ff ff ff 
  441db0:	33 c9                	xor    ecx,ecx
  441db2:	c6 05 dc 96 46 00 01 	mov    BYTE PTR ds:0x4696dc,0x1
  441db9:	89 8e 1c e3 00 00    	mov    DWORD PTR [esi+0xe31c],ecx
  441dbf:	89 8e 20 e3 00 00    	mov    DWORD PTR [esi+0xe320],ecx
  441dc5:	8b c6                	mov    eax,esi
  441dc7:	89 8e 24 e3 00 00    	mov    DWORD PTR [esi+0xe324],ecx
  441dcd:	89 8e 28 e3 00 00    	mov    DWORD PTR [esi+0xe328],ecx
  441dd3:	8b 4c 24 10          	mov    ecx,DWORD PTR [esp+0x10]
  441dd7:	88 9e a8 e2 00 00    	mov    BYTE PTR [esi+0xe2a8],bl
  441ddd:	89 9e 2c e3 00 00    	mov    DWORD PTR [esi+0xe32c],ebx
  441de3:	88 9e 30 e3 00 00    	mov    BYTE PTR [esi+0xe330],bl
  441de9:	88 9e 2d e5 00 00    	mov    BYTE PTR [esi+0xe52d],bl
  441def:	c7 86 30 e5 00 00 ff 	mov    DWORD PTR [esi+0xe530],0xffffffff
  441df6:	ff ff ff 
  441df9:	5e                   	pop    esi
  441dfa:	c6 05 0c 24 8e 00 01 	mov    BYTE PTR ds:0x8e240c,0x1
  441e01:	5b                   	pop    ebx
  441e02:	64 89 0d 00 00 00 00 	mov    DWORD PTR fs:0x0,ecx
  441e09:	83 c4 14             	add    esp,0x14
  441e0c:	c3                   	ret
  441e0d:	90                   	nop
  441e0e:	90                   	nop
  441e0f:	90                   	nop
  441e10:	6a ff                	push   0xffffffff
  441e12:	68 6d f9 45 00       	push   0x45f96d
  441e17:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  441e1d:	50                   	push   eax
  441e1e:	64 89      	mov    DWORD PTR fs:0x0,esp
