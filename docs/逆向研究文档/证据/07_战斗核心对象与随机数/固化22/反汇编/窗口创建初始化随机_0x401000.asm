
/mnt/data/RPG.exe:     file format pei-i386


Disassembly of section .text:

00401000 <.text>:
  401000:	6a ff                	push   0xffffffff
  401002:	68 91 ce 45 00       	push   0x45ce91
  401007:	64 a1 00 00 00 00    	mov    eax,fs:0x0
  40100d:	50                   	push   eax
  40100e:	64 89 25 00 00 00 00 	mov    DWORD PTR fs:0x0,esp
  401015:	51                   	push   ecx
  401016:	56                   	push   esi
  401017:	68 70 06 00 00       	push   0x670
  40101c:	e8 3e 06 05 00       	call   0x45165f
  401021:	83 c4 04             	add    esp,0x4
  401024:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
  401028:	85 c0                	test   eax,eax
  40102a:	c7 44 24 10 00 00 00 	mov    DWORD PTR [esp+0x10],0x0
  401031:	00 
  401032:	74 2f                	je     0x401063
  401034:	8b 15 c0 f6 89 00    	mov    edx,DWORD PTR ds:0x89f6c0
  40103a:	55                   	push   ebp
  40103b:	57                   	push   edi
  40103c:	8b 72 28             	mov    esi,DWORD PTR [edx+0x28]
  40103f:	8b 4a 70             	mov    ecx,DWORD PTR [edx+0x70]
  401042:	8b 7a 68             	mov    edi,DWORD PTR [edx+0x68]
  401045:	8b 52 6c             	mov    edx,DWORD PTR [edx+0x6c]
  401048:	8b 6e 08             	mov    ebp,DWORD PTR [esi+0x8]
  40104b:	8d 4c 4d 00          	lea    ecx,[ebp+ecx*2+0x0]
  40104f:	51                   	push   ecx
  401050:	8b 4e 04             	mov    ecx,DWORD PTR [esi+0x4]
  401053:	8d 14 51             	lea    edx,[ecx+edx*2]
  401056:	8b c8                	mov    ecx,eax
  401058:	52                   	push   edx
  401059:	57                   	push   edi
  40105a:	e8 e1 0c 03 00       	call   0x431d40
  40105f:	5f                   	pop    edi
  401060:	5d                   	pop    ebp
  401061:	eb 02                	jmp    0x401065
  401063:	33 c0                	xor    eax,eax
  401065:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  40106b:	83 ce ff             	or     esi,0xffffffff
  40106e:	85 c9                	test   ecx,ecx
  401070:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  401074:	a3 ec 01 8b 00       	mov    ds:0x8b01ec,eax
  401079:	75 38                	jne    0x4010b3
  40107b:	68 60 01 00 00       	push   0x160
  401080:	e8 da 05 05 00       	call   0x45165f
  401085:	83 c4 04             	add    esp,0x4
  401088:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
  40108c:	85 c0                	test   eax,eax
  40108e:	c7 44 24 10 01 00 00 	mov    DWORD PTR [esp+0x10],0x1
  401095:	00 
  401096:	74 09                	je     0x4010a1
  401098:	8b c8                	mov    ecx,eax
  40109a:	e8 e1 87 03 00       	call   0x439880
  40109f:	eb 02                	jmp    0x4010a3
  4010a1:	33 c0                	xor    eax,eax
  4010a3:	8b c8                	mov    ecx,eax
  4010a5:	89 74 24 10          	mov    DWORD PTR [esp+0x10],esi
  4010a9:	85 c0                	test   eax,eax
  4010ab:	89 0d 48 1c 8e 00    	mov    DWORD PTR ds:0x8e1c48,ecx
  4010b1:	74 05                	je     0x4010b8
  4010b3:	e8 b8 8c 03 00       	call   0x439d70
  4010b8:	6a 00                	push   0x0
  4010ba:	e8 c4 04 05 00       	call   0x451583
  4010bf:	50                   	push   eax
  4010c0:	e8 96 04 05 00       	call   0x45155b
  4010c5:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  4010ca:	83 c4 08             	add    esp,0x8
  4010cd:	b9 68 00 00 00       	mov    ecx,0x68
  4010d2:	c7 40 3c 05 00 00 00 	mov    DWORD PTR [eax+0x3c],0x5
  4010d9:	b8 02 00 00 00       	mov    eax,0x2
  4010de:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4010e4:	83 c1 04             	add    ecx,0x4
  4010e7:	89 44 11 d4          	mov    DWORD PTR [ecx+edx*1-0x2c],eax
  4010eb:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  4010f1:	89 44 11 fc          	mov    DWORD PTR [ecx+edx*1-0x4],eax
  4010f5:	40                   	inc    eax
  4010f6:	8d 50 fe             	lea    edx,[eax-0x2]
  4010f9:	83 fa 05             	cmp    edx,0x5
  4010fc:	7c e0                	jl     0x4010de
  4010fe:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  401104:	e8 77 9d 03 00       	call   0x43ae80
  401109:	e8 57 04 05 00       	call   0x451565
  40110e:	25 03 00 00 80       	and    eax,0x80000003
  401113:	79 05                	jns    0x40111a
  401115:	48                   	dec    eax
  401116:	83 c8 fc             	or     eax,0xfffffffc
  401119:	40                   	inc    eax
  40111a:	8b 0d 48 1c 8e 00    	mov    ecx,DWORD PTR ds:0x8e1c48
  401120:	be 01 00 00 00       	mov    esi,0x1
  401125:	89 81 bc 00 00 00    	mov    DWORD PTR [ecx+0xbc],eax
  40112b:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  401131:	c7 82 b8 00 00 00 00 	mov    DWORD PTR [edx+0xb8],0x0
  401138:	00 00 00 
  40113b:	6a 00                	push   0x0
  40113d:	e8 23 04 05 00       	call   0x451565
  401142:	99                   	cdq
  401143:	b9 55 00 00 00       	mov    ecx,0x55
  401148:	f7 f9                	idiv   ecx
  40114a:	42                   	inc    edx
  40114b:	52                   	push   edx
  40114c:	8b 15 48 1c 8e 00    	mov    edx,DWORD PTR ds:0x8e1c48
  401152:	56                   	push   esi
  401153:	8b 8a 90 00 00 00    	mov    ecx,DWORD PTR [edx+0x90]
  401159:	e8 c2 7f 03 00       	call   0x439120
  40115e:	46                   	inc    esi
  40115f:	83 fe 1f             	cmp    esi,0x1f
  401162:	7c d7                	jl     0x40113b
  401164:	a1 48 1c 8e 00       	mov    eax,ds:0x8e1c48
  401169:	68 d0 07 00 00       	push   0x7d0
  40116e:	8b 88 90 00 00 00    	mov    ecx,DWORD PTR [eax+0x90]
  401174:	e8 a7 86 03 00       	call   0x439820
  401179:	68 ac 00 00 00       	push   0xac
  40117e:	e8 dc 04 05 00       	call   0x45165f
  401183:	83 c4 04             	add    esp,0x4
  401186:	89 44 24 04          	mov    DWORD PTR [esp+0x4],eax
  40118a:	85 c0                	test   eax,eax
  40118c:	c7 44 24 10 02 00 00 	mov    DWORD PTR [esp+0x10],0x2
  401193:	00 
  401194:	5e                   	pop    esi
  401195:	74 1b                	je     0x4011b2
  401197:	8b c8                	mov    ecx,eax
  401199:	e8 92 71 02 00       	call   0x428330
  40119e:	a3                   	.byte 0xa3
  40119f:	a8                   	.byte 0xa8
